//import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Connection;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;


/*
 * Created on 28-Jun-2004
 *
 * TODO To change the template for this generated file go to
 * Window - Preferences - Java - Code Generation - Code and Comments
 */
/**
 * @author mmatan
 *
 * TODO To change the template for this generated type comment go to
 * Window - Preferences - Java - Code Generation - Code and Comments
 */
public class bmdDB {
	
	private Connection conn = null;
	private HashMap bmd_object2objid = null;
	String dburl = "jdbc:mysql://marge:33306/bind_module2";
	HashMap gi2objid = null;
	HashMap objid2rog = null;
	
	public bmdDB() {
		try {
			Class.forName("com.mysql.jdbc.Driver");
		} catch (ClassNotFoundException e) {
			System.err.println("bmdRecordProcessor: Fatal Error: Unable to find specified jdbc connector class");
			System.err.println(e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
		
		try {
			conn = DriverManager.getConnection(dburl,"mmatan", "asdf1234");
		} catch (SQLException e1) {
			System.err.println("bmdRecordProcessor: Fatal Error: Unable to establish database connection using specified server/login");
			System.err.println("SQLException: " + e1.getMessage());
			e1.printStackTrace();
			System.exit(1);
		}
	}
	
	public Connection getConn() {
		return conn;
	}
	
	//This internal class will be used to
	//represent pairs of rogs to be mapped 
	//to rigs
	private class Rogpair {
		public Integer rogA = null;
		public Integer rogB = null;
		public Rogpair(Integer rogA,Integer rogB) {
			this.rogA = rogA;
			this.rogB = rogB;
		}
		
		public Rogpair getReverse() {
			return new Rogpair(this.rogB,this.rogA);
			
		}
		
		public boolean equals(Object obj) {
			if (!this.rogA.equals(((Rogpair) obj).rogA)) {
				return false;
			}
			if (!this.rogB.equals(((Rogpair) obj).rogB)) {
				return false;
			}
			return true;
		}
		
		public int hashCode() {
			int hash = 1;
			hash = hash*31 + rogA.hashCode();
			hash = hash*31 + rogB.hashCode();
			return hash;
		}
	}

	public void computeRIGs() {
		createObjid2ROGInternalMap();
		Statement stmt = null;
		HashMap rogpair2rig = new HashMap();
		int rigIndex = 1;
		try {
			stmt = conn.createStatement(ResultSet.TYPE_SCROLL_INSENSITIVE,
					ResultSet.CONCUR_UPDATABLE);
			stmt.execute("SELECT objAid,objBid,rig,intid from bmd_ints");
			ResultSet intR = stmt.getResultSet();
			if (!intR.first()) {
				System.err.println("computeRIGs: Error: Select from bmd_ints returned no values.  Exiting.");
				return;
			}
			intR.beforeFirst();
			while (intR.next()) {
				Integer objAid = new Integer(intR.getInt(1));
				Integer objBid = new Integer(intR.getInt(2));
				Rogpair curRogPair = new Rogpair((Integer) objid2rog.get(objAid),(Integer) objid2rog.get(objBid));
				Integer curRig = null;
				if (rogpair2rig.containsKey(curRogPair)) {
					curRig = (Integer) rogpair2rig.get(curRogPair);
				}
				else {
					curRig = new Integer(rigIndex);
					rogpair2rig.put(curRogPair,curRig);
//					//So that the symetrical interaction is mapped to the same redundant interaction group
					rogpair2rig.put(curRogPair.getReverse(),curRig);  
					rigIndex++;
				}
				intR.updateInt(3,curRig.intValue());
				intR.updateRow();
			}
		}
		catch(SQLException e) {
			System.err.println("computeRIGs: Error: Unable to query and/or update objAid,objBid,rig");
			System.err.println("SQLException: " + e.getMessage());
			e.printStackTrace();
			return;
		}
	}

	public void clearROGs() {
		Statement stmt = null;
		try {
			stmt = conn.createStatement();
			stmt.executeUpdate("UPDATE bmd_objects SET rog=0");
			
		}
		catch(SQLException e) {
			System.err.println("clearROGs(): Error: failed to set rog field to 0");
			System.err.println("SQLException: " + e.getMessage());
			e.printStackTrace();
			return;
		}
	}
	public void AssignRemainingROGs() {
		Statement stmt = null;
		//HashMap rogpair2rig = new HashMap();
		int maxRog = (getMaxROG()).intValue();
		int rogIndex = maxRog + 1;
		try {
			
			stmt = conn.createStatement(ResultSet.TYPE_SCROLL_INSENSITIVE,
					ResultSet.CONCUR_UPDATABLE);
			stmt.execute("SELECT objid,rog FROM bmd_objects WHERE rog=0");
			ResultSet objR = stmt.getResultSet();
			if (!objR.first()) {
				System.err.println("AssignRemainingROGs: Error: Select from bmd_objects returned no values.  Exiting.");
				return;
			}
			objR.beforeFirst();
			while (objR.next()) {
				objR.updateInt(2,rogIndex);
				objR.updateRow();
				rogIndex++;
			}
		}
		catch(SQLException e) {
			System.err.println("AssignRemainingROGs: Error: Unable to query and/or update rog");
			System.err.println("SQLException: " + e.getMessage());
			e.printStackTrace();
			return;
		}
	}
	
	public void createObjid2ROGInternalMap() {
		Statement stmt;
		objid2rog = new HashMap();
		try {
			stmt = conn.createStatement();
			stmt.execute("SELECT objid,rog from bmd_objects");
			ResultSet objidNrogR = stmt.getResultSet();
			if (!objidNrogR.first()) {
				System.err.println("createObjid2ROGInternalMap:Warning: no results");
				return;
			}
			objidNrogR.beforeFirst();
			while (objidNrogR.next()) {
				objid2rog.put(new Integer(objidNrogR.getInt(1)), new Integer(objidNrogR.getInt(2)));
			}
			return;
		}
		catch(SQLException e) {
			System.err.println("createObjid2ROGInternalMap: Error: Unable to get objid,rog values");
			System.err.println("SQLException: " + e.getMessage());
			e.printStackTrace();
			return;
		}
	}
	
	public void createGenbankProteinGI2ObjidInternalMap() {
		Statement stmt;
		gi2objid = new HashMap();
		try {
			stmt = conn.createStatement();
			stmt.execute("SELECT objid,id from bmd_objects where " + mysqlEquivalenceInteger("db",new Integer(resolveObjDBKey(new StringBuffer("Genbank")))) + " and " + mysqlEquivalenceInteger("type",new Integer(resolveMolTypeKey("protein"))));
			ResultSet gpobjectsR = stmt.getResultSet();
			if (!gpobjectsR.first()) {
				System.err.println("createGenbankProteinGI2ObjidInternalHash:Warning: no results");
				return;
			}
			gpobjectsR.beforeFirst();
			while (gpobjectsR.next()) {
				Integer curGI = new Integer(gpobjectsR.getInt(2));
				ArrayList objidList = null;
				if (gi2objid.containsKey(curGI)) {
					objidList = (ArrayList) gi2objid.get(curGI);
					
				}
				else {
					objidList = new ArrayList();
				}
				objidList.add(new Integer(gpobjectsR.getInt(1)));
				gi2objid.put( curGI,objidList);
			}
			return;
			
			
		} catch (SQLException e) {
			System.err.println("createGenbankProteinGI2ObjidInternalHash: Error: Unable to get objid,id values");
			System.err.println("SQLException: " + e.getMessage());
			e.printStackTrace();
			return;
		}
	}
	
	public void setRogForGenbankGIList(int rog,List GIList) {
		Statement stmt = null;
		if (gi2objid == null) {
			createGenbankProteinGI2ObjidInternalMap();
		}
		if (GIList.size() >0) {
			try {
				stmt = conn.createStatement();
				StringBuffer sql = new StringBuffer("UPDATE bmd_objects SET rog= " + rog + " WHERE ( ");
				Iterator GIListI = GIList.iterator();
				while (GIListI.hasNext()) {
					Integer curGI = (Integer) GIListI.next();
					ArrayList objidList = (ArrayList) gi2objid.get(curGI);
					Iterator objidListI = objidList.iterator();
					while (objidListI.hasNext()) {
						Integer curObjid = (Integer) objidListI.next();
						sql.append(mysqlEquivalenceInteger("objid",curObjid));
						if (objidListI.hasNext() || GIListI.hasNext()) {
							sql.append(" or ");
						}
						else {
							sql.append(")");
						}
					}
				}
				System.out.println(sql);
				stmt.executeUpdate(sql.toString());
			
			} catch (SQLException e) {
				System.err.println("setRogForGenbankGIList: Error: Unable to set rog for GI values");
				System.err.println("SQLException: " + e.getMessage());
				e.printStackTrace();
				return;
			}
		}
	}
	
	public HashSet getGenbankProteinGIs() {
		Statement stmt;
		HashSet gpGIs = new HashSet();
		try {
			stmt = conn.createStatement();
			stmt.execute("SELECT objid,id,rog from bmd_objects where " + mysqlEquivalenceInteger("db",new Integer(resolveObjDBKey(new StringBuffer("Genbank")))) + " and " + mysqlEquivalenceInteger("type",new Integer(resolveMolTypeKey("protein"))));
			ResultSet gpobjectsR = stmt.getResultSet();
			if (!gpobjectsR.first()) {
				System.err.println("getGenbankProteinObjectGIs:Warning: no results");
				return null;
			}
			gpobjectsR.beforeFirst();
			while (gpobjectsR.next()) {
				gpGIs.add( new Integer(gpobjectsR.getInt(2)));
			}
			return gpGIs;
			
			
		} catch (SQLException e) {
			System.err.println("getGenbankProteinObjectGIs: Error: Unable to get genbankproteingi values");
			System.err.println("SQLException: " + e.getMessage());
			e.printStackTrace();
			return null;
		}
		
		
	}
	
	public Integer getMaxROG() {
		Statement stmt;
		try {
			stmt = conn.createStatement();
			stmt.execute("SELECT max(rog) from bmd_objects");
			ResultSet maxRogR = stmt.getResultSet();
			if (!maxRogR.first()) {
				System.err.println("getMaxROG:failed to retrieve maximum rog in bmd_objects table.");
				return null;
			}
			Integer maxRog = new Integer(maxRogR.getInt(1));
			return maxRog;
		} catch (SQLException e) {
			System.err.println("getMaxROG: Error: Unable to get maxRog value");
			System.err.println("SQLException: " + e.getMessage());
			e.printStackTrace();
			return null;
		}
		
	}
	
	public Integer getMaxRIG() {
		Statement stmt;
		try {
			stmt = conn.createStatement();
			stmt.execute("SELECT max(rig) from bmd_ints");
			ResultSet maxRigR = stmt.getResultSet();
			if (!maxRigR.first()) {
				System.err.println("getMaxRIG:failed to retrieve maximum rig from bmd_ints table.");
				return null;
			}
			Integer maxRig = new Integer(maxRigR.getInt(1));
			return maxRig;
		} catch (SQLException e) {
			System.err.println("getMaxRIG: Error: Unable to get maxRig value");
			System.err.println("SQLException: " + e.getMessage());
			e.printStackTrace();
			return null;
		}
		
	}
	
	private String mysqlEquivalenceInteger(String fieldname, Integer value) {
		if (value != null) {
			return fieldname + "=" + value;
		}
		else {
			return fieldname + " IS NULL";
		}
	}
	
	private String mysqlEquivalenceString(String fieldname, String value) {
		if (value != null) {
			return fieldname + "=" + convertToSQLType(value);
		}
		else {
			return fieldname + " IS NULL";
		}
	}
	
	private void insertComplex2IntsIntoDB(Integer compid,Integer intid) throws SQLException {
		Statement stmt = conn.createStatement();
		stmt.execute("INSERT INTO bmd_complex2ints (compid,intid) VALUES(" +  convertToSQLType(compid) + "," + convertToSQLType(intid) + ");");
		return;
		
	}
	
	public Integer getIntIDKey(String intSourceDB, Integer intid) {
		try {
			Statement stmt = conn.createStatement();
			Integer intSourceDBKey = new Integer(resolveIntDBKey(intSourceDB));  //note that this might not be desirable, since it will insert the value if it is not found.  Maybe better to create a getter, like this method, but for DBKey
			//System.out.println("SELECT intcompid FROM bmd_source WHERE " + mysqlEquivalenceInteger("id",intid) + " and " + mysqlEquivalenceInteger("db",intSourceDBKey) + " and type=" + resolveRecTypeKey("interaction") + ";");
			stmt.execute("SELECT intcompid FROM bmd_source WHERE " + mysqlEquivalenceInteger("id",intid) + " and " + mysqlEquivalenceInteger("db",intSourceDBKey) + " and type=" + resolveRecTypeKey("interaction") + ";");
			ResultSet intcompidR = stmt.getResultSet();
			if (!intcompidR.first()) {
				System.err.println("getIntIDKey: no matching interaction found, returning null.");
				return null;
			}
			else {
				Integer intcompid = new Integer(intcompidR.getInt(1));
				System.out.println("intcompid: " + intcompid);
				return intcompid;
			}
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			System.err.println("getIntIDKey: SQLException, unable to select for mathcing interaction.  Returning null.");
			System.err.println("SQLException: " + e.getMessage());
			e.printStackTrace();
			return null;
		}
		
	}
	
	public Integer insertComplexIntoDB(int numSubunits) throws SQLException {
		Statement stmt = conn.createStatement();
		stmt.execute("INSERT INTO bmd_complexes (numsubunits) VALUES(" +  numSubunits + ")", Statement.RETURN_GENERATED_KEYS);
		ResultSet compidR = stmt.getGeneratedKeys();
		if (!compidR.first()) {
			System.err.println("insertComplexIntoDB: Failed on insert of source entry into bmd_source table");
			return null;
		}
		else {
			Integer compid = new Integer(compidR.getInt(1)); 
			return compid;
		}
		
	}
	
	public Integer getObjID(bmd_objects_rec bmd_object) throws SQLException {

		Statement stmt = conn.createStatement();
		stmt.execute("SELECT objid FROM bmd_objects where type=" + resolveMolTypeKey(bmd_object.type) + " and db=" + resolveObjDBKey(bmd_object.db) + " and " + mysqlEquivalenceString("acc",bmd_object.acc) + " and " + mysqlEquivalenceInteger("id",new Integer(bmd_object.id.toString())) + " and " + mysqlEquivalenceInteger("tax",bmd_object.tax) + " ;");
		ResultSet objidR = stmt.getResultSet();
		if (!objidR.first()) {
			return null;
		}
		
		int objid = objidR.getInt(1);
		//System.out.println("New index for object in bmd_objects:" + objid);
		//bmd_object2objid.put(bmd_object,new Integer(objid));  //Note:this means that the
		return new Integer(objid);
	}
	
	
	public void insertObjLabelIntoDB(Integer uid,Integer objid, StringBuffer label) {
		try {
			Statement stmt = conn.createStatement();
			stmt.execute("INSERT INTO bmd_obj_labels (uid,objid,label) VALUES(" +  convertToSQLType(uid) + "," + convertToSQLType(objid) + "," + convertToSQLType(label) + ");");
		} catch (SQLException e) {
			System.err.println("insertObjLabelIntoDB: Error: Failed to insert entry into bmd_obj_labels table.");
			System.err.println("SQLException: " + e.getMessage());
			e.printStackTrace();
		}
		return;
		
	}
	
	public void insertRefsIntoDB(Integer uid,ArrayList refs) {
		Iterator refsI = refs.iterator();
		while (refsI.hasNext()) {
			bmd_refs_rec curRef = (bmd_refs_rec) refsI.next();
			if (!insertRefIntoDB(uid,curRef)) {
				System.err.println("failed to insert ref into db.");
			}
		}
	}
	
	public void insertComplex2SubunitsIntoDB(Integer compid,Integer objID)  {
		try {
			Statement stmt = conn.createStatement();
			stmt.execute("INSERT INTO bmd_complex2subunits (compid,objid) VALUES(" +  convertToSQLType(compid) + "," + convertToSQLType(objID) + ");");
		} catch (SQLException e) {
			System.err.println("insertComplex2SubunitsIntoDB: Error: Failed to insert entry into bmd_complex2ints table.");
			System.err.println("SQLException: " + e.getMessage());
			e.printStackTrace();
		}
		return;
		
	}
	
	public Integer resolveIntIDKey(bmd_objects_rec objA, bmd_objects_rec objB) throws SQLException {
//		Statement stmt = conn.createStatement();
//		stmt.execute("INSERT INTO bmd_ints (uid,objAid,objBid,descr) VALUES(" +  convertToSQLType(uid) + "," + convertToSQLType(objAid) + "," + convertToSQLType(objBid) + ",\"" + convertToSQLType(descr) + "\")");
//		return ;
		Statement stmt = conn.createStatement();
		//TODO optimize this method so that resolveObjIDKey isn't called twice for the same object.  This could be combined with insertion into the HashMap.
		Integer objAid = resolveObjIDKey(objA);
		Integer objBid = resolveObjIDKey(objB);
		stmt.execute("SELECT intid FROM bmd_ints where objAid=" + objAid + " and objBid=" + objBid + ";");
		ResultSet intidR = stmt.getResultSet();
		if (!intidR.first()) {
			stmt.execute("INSERT INTO bmd_ints (objAid,objBid) VALUES(" + objAid + " ," + objBid + ");");
			intidR = stmt.getGeneratedKeys();
			intidR.first();
		}
		int intid = intidR.getInt(1);
		System.out.println("Index for interaction in bmd_ints:" + intid);
		
		//objdb_name2objdb.put(db_name,new Integer(db));
		
		return new Integer(intid);
	}
	

	
	public boolean insertRefIntoDB(Integer uid,bmd_refs_rec ref) {
		Statement stmt;
		try {
			stmt = conn.createStatement();
			stmt.execute("INSERT INTO bmd_refs (uid,db,acc,id,method) VALUES(" +  convertToSQLType(uid) + "," + resolveRefDBKey(ref.db) + "," + convertToSQLType(ref.acc) + "," + convertToSQLType(ref.id) + "," + resolveExpDBKey(ref.method) + ")");
		} catch (SQLException e) {
			System.err.println("insertRefIntoDB: Error: Unable to insert reference into bmd_refs table.");
			System.err.println("SQLException: " + e.getMessage());
			e.printStackTrace();
			return false;
		}
		return true;
	}
	
	public Integer insertSourceIntoDB(bmd_source_rec bmd_source, Integer intcompID) throws SQLException {
		Statement stmt = conn.createStatement();
		stmt.execute("INSERT INTO bmd_source (intcompid,db,acc,id,type,descr) VALUES(" + convertToSQLType(intcompID) + "," +  resolveIntDBKey(bmd_source.db) + "," + convertToSQLType(bmd_source.acc) + "," + convertToSQLType(new Integer(bmd_source.id.toString())) + "," + resolveRecTypeKey(bmd_source.type) + "," + convertToSQLType(bmd_source.descr) + ")", Statement.RETURN_GENERATED_KEYS);
		ResultSet uidR = stmt.getGeneratedKeys();
		if (!uidR.first()) {
			System.err.println("insertSourceIntoDB: Failed on insert of source entry into bmd_source table");
			return null;
		}
		else {
			Integer uid = new Integer(uidR.getInt(1)); 
			return uid;
		}
		
	}
	
	
	public Integer resolveObjIDKey(bmd_objects_rec bmd_object) throws SQLException {

		Statement stmt = conn.createStatement();
		Integer bmd_object_id;
		if (bmd_object.id != null) { 
		   bmd_object_id = new Integer(bmd_object.id.toString());
		}
		else {
		    bmd_object_id = null;
		}
		stmt.execute("SELECT objid FROM bmd_objects where type=" + resolveMolTypeKey(bmd_object.type) + " and db=" + resolveObjDBKey(bmd_object.db) + " and " + mysqlEquivalenceString("acc",bmd_object.acc) + " and " + mysqlEquivalenceInteger("id",bmd_object_id) + " and " + mysqlEquivalenceInteger("tax",bmd_object.tax) + " ;");
		ResultSet objidR = stmt.getResultSet();
		if (!objidR.first()) {
			stmt.execute("INSERT INTO bmd_objects (type,db,acc,id,tax) VALUES(" + resolveMolTypeKey(bmd_object.type) + "," + resolveObjDBKey(bmd_object.db) + "," + convertToSQLType(bmd_object.acc) + "," + convertToSQLType(bmd_object_id) + " ," + convertToSQLType(bmd_object.tax) + ");",Statement.RETURN_GENERATED_KEYS );
			objidR = stmt.getGeneratedKeys();
			objidR.first();
		}
		
		int objid = objidR.getInt(1);
		System.out.println("Index for object in bmd_objects:" + objid);
		bmd_object2objid.put(bmd_object,new Integer(objid));  //Note:this means that the
		return new Integer(objid);
	}
	
		
	private String convertToSQLType(Integer I) {
		if (I == null) {
			return "NULL";
		}
		else {
			return I.toString();
		}
	}
	
	private String convertToSQLType(StringBuffer S) {
		if (S != null) {
			return convertToSQLType(S.toString());
		}
		else {
				return convertToSQLType((String) null);
		}
	}
	
	private String convertToSQLType(String S) {
		if (S == null) {
			return "NULL";
		}
		else if (S.compareTo("") == 0) {
			return "NULL";
		}
		else {
			Pattern doublequoteP = Pattern.compile("\"");
			Matcher m = doublequoteP.matcher(S);
			S = m.replaceAll("\\\\\"");
			System.out.println("escaped string:" + S);
			return "\"" + S + "\"";
		}
	}
	
		HashMap objdb_name2objdb = new HashMap();
		public int resolveObjDBKey(StringBuffer db_name) throws SQLException {
			if (db_name == null) {
				db_name = new StringBuffer("not-specified");
			}
			if (objdb_name2objdb.containsKey(db_name)) {
				return ((Integer) objdb_name2objdb.get(db_name)).intValue(); 
			}
			else {
				Statement stmt = conn.createStatement();
				stmt.execute("SELECT db FROM bmd_obj_dbases where db_name=\"" + db_name + "\";");
				ResultSet dbR = stmt.getResultSet();
				if (!dbR.first()) {
					stmt.execute("INSERT INTO bmd_obj_dbases (db_name) VALUES(\"" + db_name + "\");",Statement.RETURN_GENERATED_KEYS );
					dbR = stmt.getGeneratedKeys();
					dbR.first();
				}
				int db = dbR.getInt(1);
				System.out.println("Index for object database name in bmd_obj_dbases:" + db);
				
				objdb_name2objdb.put(db_name,new Integer(db));
				
				return db;
			}
		}
		
		HashMap moltype_name2moltype = new HashMap();
		
		public int resolveMolTypeKey(String type_name) throws SQLException {
			if (type_name == null) {
				type_name = "not-specified";
			}
			if (moltype_name2moltype.containsKey(type_name)) {
				return ((Integer) moltype_name2moltype.get(type_name)).intValue(); 
			}
			else {
				Statement stmt = conn.createStatement();
				stmt.execute("SELECT type FROM bmd_mol_type where type_name=\"" + type_name + "\";");
				ResultSet typeR = stmt.getResultSet();
				if (!typeR.first()) {  //in case there is not entry for this type yet, create one
					stmt.execute("INSERT INTO bmd_mol_type (type_name) VALUES(\"" + type_name + "\");",Statement.RETURN_GENERATED_KEYS );
					typeR = stmt.getGeneratedKeys();
					typeR.first();
				}
				int type = typeR.getInt(1);
				System.out.println("Index for molecule type name in bmd_mol_type:" + type);
				
				moltype_name2moltype.put(type_name, new Integer(type));
				
				return type;
			}
		}
		
		HashMap refdb_name2refdb = new HashMap();
		
		public int resolveRefDBKey(String db_name) throws SQLException {
			if (db_name == null) {
				db_name = "not-specified";
			}
			if (refdb_name2refdb.containsKey(db_name)) {
				return ((Integer) refdb_name2refdb.get(db_name)).intValue(); 
			}
			else {
				Statement stmt = conn.createStatement();
				stmt.execute("SELECT db FROM bmd_refs_db where db_name=\"" + db_name + "\";");
				ResultSet dbR = stmt.getResultSet();
				if (!dbR.first()) {
					stmt.execute("INSERT INTO bmd_refs_db (db_name) VALUES(\"" + db_name + "\");",Statement.RETURN_GENERATED_KEYS );
					dbR = stmt.getGeneratedKeys();
					dbR.first();
				}
				int db = dbR.getInt(1);
				System.out.println("Index for references database name \"" + db_name + "\" in bmd_obj_dbases:" + db);
				
				refdb_name2refdb.put(db_name,new Integer(db));
				
				return db;
			}
		}
		
		
		HashMap method_descr2method = new HashMap();
		
		public int resolveExpDBKey(String method_descr) throws SQLException {
			if (method_descr == null) {
				method_descr = "not-specified";
			}
			
			if (method_descr2method.containsKey(method_descr)) {
				return ((Integer) method_descr2method.get(method_descr)).intValue();
			}
			else {
				Statement stmt = conn.createStatement();
				stmt.execute("SELECT method FROM bmd_exp_methods where method_descr=\"" + method_descr + "\";");
				ResultSet methodR = stmt.getResultSet();
				if (!methodR.first()) {
					stmt.execute("INSERT INTO bmd_exp_methods (method_descr) VALUES(\"" + method_descr + "\");",Statement.RETURN_GENERATED_KEYS );
					methodR = stmt.getGeneratedKeys();
					methodR.first();
				}
				int method = methodR.getInt(1);
				System.out.println("Index for method database name in bmd_exp_methods:" + method);
				
				method_descr2method.put(method_descr,new Integer(method));
				
				return method;
			}
		}	
		
		HashMap intdb_name2intdb = new HashMap();
		
		public int resolveIntDBKey(String db_name) throws SQLException {
			if (db_name==null) {
				db_name = "not-specified";
			}
			if (intdb_name2intdb.containsKey(db_name)) {
				return ((Integer) intdb_name2intdb.get(db_name)).intValue(); 
			}
			else {
				Statement stmt = conn.createStatement();
				stmt.execute("SELECT db FROM bmd_dbases where db_name=\"" + db_name + "\";");
				ResultSet dbR = stmt.getResultSet();
				if (!dbR.first()) {
					stmt.execute("INSERT INTO bmd_dbases (db_name) VALUES(\"" + db_name + "\");",Statement.RETURN_GENERATED_KEYS );
					dbR = stmt.getGeneratedKeys();
					dbR.first();
				}
				int db = dbR.getInt(1);
				System.out.println("Index for interaction database name \"" + db_name + "\" in bmd_dbases:" + db);
				
				intdb_name2intdb.put(db_name,new Integer(db));
				
				return db;
			}
		}
		
		HashMap rectype_name2rectype = new HashMap();
		
		public int resolveRecTypeKey(String type_name) throws SQLException {
			
			if (rectype_name2rectype.containsKey(type_name)) {
				return ((Integer) rectype_name2rectype.get(type_name)).intValue(); 
			}
			else {
				Statement stmt = conn.createStatement();
				stmt.execute("SELECT type FROM bmd_record_types where type_name=\"" + type_name + "\";");
				ResultSet typeR = stmt.getResultSet();
				if (!typeR.first()) {  //in case there is not entry for this type yet, create one
					stmt.execute("INSERT INTO bmd_record_types (type_name) VALUES(\"" + type_name + "\");",Statement.RETURN_GENERATED_KEYS );
					typeR = stmt.getGeneratedKeys();
					typeR.first();
				}
				int type = typeR.getInt(1);
				System.out.println("Index for record type name in bmd_mol_type:" + type);
				
				rectype_name2rectype.put(type_name, new Integer(type));
				
				return type;
			}
		}
	
}
