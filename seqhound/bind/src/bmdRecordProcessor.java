/*
 * Created on 16-Jun-2004
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

import java.sql.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class bmdRecordProcessor {
	
	Connection conn = null;
	//String dburl = "jdbc:mysql://marge:33306/bind_module2";
	ArrayList unresolvedQueue = new ArrayList();
	HashMap bmd_object2objid = null;
	bmdDB bmddb = null; 
	
	public bmdRecordProcessor() {
		bmddb = new bmdDB();
		conn = bmddb.getConn();
//		try {
//			Class.forName("com.mysql.jdbc.Driver");
//		} catch (ClassNotFoundException e) {
//			System.err.println("bmdRecordProcessor: Fatal Error: Unable to find specified jdbc connector class");
//			System.err.println(e.getMessage());
//			e.printStackTrace();
//			System.exit(1);
//		}
//		
//		try {
//			conn = DriverManager.getConnection(dburl,"mmatan", "asdf1234");
//		} catch (SQLException e1) {
//			System.err.println("bmdRecordProcessor: Fatal Error: Unable to establish database connection using specified server/login");
//			System.err.println("SQLException: " + e1.getMessage());
//			e1.printStackTrace();
//			System.exit(1);
//		}
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
	
	public boolean processRecord(bmdRecord bmd) {
		bmd_object2objid = new HashMap();
		Statement stmt = null;
		Integer uid = null;
		
		//Process in a record type specific manner
		if (bmd.bmd_source.type.compareTo("interaction")==0) {
			processInteractionRecord(bmd);
		}
		else if (bmd.bmd_source.type.compareTo("complex")==0) {

				processComplexRecord(bmd);

		}
		else if (bmd.bmd_source.type.compareTo("pathway")==0) {
				processComplexRecord(bmd);
		}
	
		return true;
	}
	
	//This method will attempt to resolve interaction id references within
	//the complex2ints_rec records stored in the unresolveQueue ArrayList
	//attribute of this this object.  Generally, these will have been
	//placed there when the complex or pathway record was first processed
	//by a call to processRecord(), when the unresolved references were
	//first encountered.  Since it will be possible for there to be 
	//references across files, we will need either a way to have the 
	//program parse multiple files in a single execution(Easiest) or
	//create a method of storing and then reloading complex2ints_rec 
	//records between executions of the program(slightly more difficult,
	//but more general, and shouldn't be that hard utilizing java 1.4's 
	//Long Term Persistence (LTP) mechanisms for java beans.
	public boolean postProcess() {
		boolean postProcessSuccessful=true;
		Iterator unresolvedQueueI = unresolvedQueue.iterator();
		while (unresolvedQueueI.hasNext()) {
			bmd_complex2ints_rec unresolvedComplex2Int = (bmd_complex2ints_rec) unresolvedQueueI.next();
			Integer compid = unresolvedComplex2Int.compid;
			String intSourceDB = unresolvedComplex2Int.intSourceDB;
			Iterator intSourceDBIntIdI = unresolvedComplex2Int.int_ids.iterator();
			while (intSourceDBIntIdI.hasNext()) {
				Integer intSourceDBIntId = (Integer) intSourceDBIntIdI.next();
				Integer intid = getIntIDKey(intSourceDB,intSourceDBIntId);
				if (intid == null) {
					System.err.println("postProcess:Error: unable to resolve interaction reference in complex record.  This suggests a data integrity issue with your source data, perhaps you missed an input file?  This entry will not be inserted into bmd_complex2ints.  Continuing with other enqueued unresolved interaction references.  The unresolved interaction corresponds to source Interaction Database \"" + intSourceDB + "\", interaction ID \"" + intSourceDBIntId + "\" which is references by the complex with internal complex id(compid) \"" + compid + "\".");
					
					postProcessSuccessful=false;
				}
				else {
					insertComplex2SubunitsIntoDB(compid, intid);
				}
				
			}
		}
		if (postProcessSuccessful) {
			return true;
		}
		else {
			return false;
		}
	}
	
	private Integer resolveCompIDKey(bmd_complex2subunits_rec comp2sub, bmd_complex2ints_rec comp2int, String intSourceDB) throws SQLException {
//		Statement stmt = conn.createStatement();
//		stmt.execute("INSERT INTO bmd_ints (uid,objAid,objBid,descr) VALUES(" +  convertToSQLType(uid) + "," + convertToSQLType(objAid) + "," + convertToSQLType(objBid) + ",\"" + convertToSQLType(descr) + "\")");
//		return ;
		Iterator complexSubunitI = comp2sub.subunitObjects.iterator();
		ArrayList objIDList = new ArrayList();
		while (complexSubunitI.hasNext()) {
			bmd_objects_rec curSubunitObj = (bmd_objects_rec) complexSubunitI.next();
			Integer objID = resolveObjIDKey(curSubunitObj);
			if (objID == null) {
				System.err.println("resolveCompIDKey: Error: Failed to resolve ObjectA into database.  Ignoring rest of record.");
				return null;
			}
			else {
				objIDList.add(objID);
			}

		}
		int numSubunits = objIDList.size();
		Integer compid = insertComplexIntoDB(numSubunits);
		
		Iterator objIDListI = objIDList.iterator();
		while (objIDListI.hasNext()) {
			Integer curObjID = (Integer) objIDListI.next();
			insertComplex2SubunitsIntoDB(compid,curObjID);
			
		}
		
		bmd_complex2ints_rec unresolvedComp2int = null;
		Iterator intIDListI = comp2int.int_ids.iterator();
		while (intIDListI.hasNext()) {
			Integer intSourceDBID = (Integer) intIDListI.next();
			Integer intid = getIntIDKey(intSourceDB,intSourceDBID);
			if (intid == null) { //This should only occur if the complex record references interaction id's not in the database.
				if (unresolvedComp2int == null) {
					unresolvedComp2int = new bmd_complex2ints_rec();
					unresolvedComp2int.int_ids = new ArrayList();
					unresolvedComp2int.compid = compid;
					unresolvedComp2int.intSourceDB = intSourceDB;
				}
				unresolvedComp2int.int_ids.add(intSourceDBID);
				
			}
			else {
				insertComplex2IntsIntoDB(compid,intid);
			}
		}
		
		if (unresolvedComp2int != null) {  //if we populated the unresolveComp2int object, then enqueue it for post processing
			if (unresolvedComp2int.int_ids.size() >0) {
				unresolvedQueue.add(unresolvedComp2int);  //This won't work, I need to record it's association with the current compid, which has ... wait, I could have generated it by now!
				System.err.println("resolveCompIDKey: Warning, this complex references interactions which are not in the database.  Enqueueing them for processing after the other records have been processed, and continuing with the rest of the records");
			}
		}
		
		
		return compid;
	}
	
	private void insertComplex2IntsIntoDB(Integer compid,Integer intid) throws SQLException {
		Statement stmt = conn.createStatement();
		stmt.execute("INSERT INTO bmd_complex2ints (compid,intid) VALUES(" +  convertToSQLType(compid) + "," + convertToSQLType(intid) + ");");
		return;
		
	}
	
	private Integer getIntIDKey(String intSourceDB, Integer intid) {
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
	
	private boolean processComplexRecord(bmdRecord bmd) {
		try {
			
			Integer compid = resolveCompIDKey(bmd.bmd_complex2subunits,bmd.bmd_complex2ints,bmd.bmd_source.db);

			
			Integer uid = insertSourceIntoDB(bmd.bmd_source,compid);
			
			//System.out.println(bmd_object2objid.toString());
			Iterator subunitI = bmd.bmd_complex2subunits.subunitObjects.iterator();
			while(subunitI.hasNext()) {
				bmd_objects_rec curBmdObject = (bmd_objects_rec) subunitI.next();
				Integer objId = (Integer) bmd_object2objid.get(curBmdObject);
				//System.out.println("objId: " + objId);
				if (objId == null) {
				    System.err.println("Error:no object id from this complexe's object");
				}
				if (curBmdObject.label == null) {
				    System.err.println("Error:no label for this complexe's subunit object");
				}
				insertObjLabelIntoDB(uid,objId,curBmdObject.label);
			}
			
			
			if (bmd.bmd_ref != null) {
				bmd.bmd_refs.add(bmd.bmd_ref);
				bmd.bmd_ref = null;
				System.err.println("processInteractionRecord: Warning: the bmd_ref object was not null, there may be an error in your parser.");
			}
			insertRefsIntoDB(uid,bmd.bmd_refs);
			
			//handle object labels
			//select objid from bmd_complex2subunits where compid=compid
			//for each objid, create label
			
//			ArrayList objidList = getObjIDList(uid);
//			Iterator objidListI = objidList.iterator();
//			while (objidListI.hasNext()) {
//				Integer objid = (Integer) objidListI.next();
//			}
			
			
			
			return true;
			
			
		} catch (SQLException e) {
			System.err.println("processComplexRecord: Fatal Error: Unable to create SQLstatement");
			System.err.println("SQLException: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
		return false;
	}
	
	private Integer insertComplexIntoDB(int numSubunits) throws SQLException {
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
	
	private boolean processInteractionRecord(bmdRecord bmd) {
		try {
			
			Integer intid = resolveIntIDKey(bmd.bmd_objects__A, bmd.bmd_objects__B);
			if (intid == null) {
				System.err.println("processInteractionRecord: Error: Failed to resolve record to an interaction id.  Ignoring rest of record.");
				return false;
			}
			
			Integer uid = insertSourceIntoDB(bmd.bmd_source,intid);
			//System.out.println(bmd_object2objid.toString());
			Integer objAId = (Integer) bmd_object2objid.get(bmd.bmd_objects__A);
			Integer objBId = (Integer) bmd_object2objid.get(bmd.bmd_objects__B);
//			Integer objAId = getObjID(bmd.bmd_objects__A);
//			Integer objBId = getObjID(bmd.bmd_objects__B);
			//System.out.println("objAId: " + objAId + " objBId: " + objBId);
			insertObjLabelIntoDB(uid,objAId,bmd.bmd_objects__A.label);
			insertObjLabelIntoDB(uid,objBId,bmd.bmd_objects__B.label);
			
			if (bmd.bmd_ref != null) {
				bmd.bmd_refs.add(bmd.bmd_ref);
				bmd.bmd_ref = null;
				System.err.println("processInteractionRecord: Warning: the bmd_ref object was not null, there may be an error in your parser.");
			}
			insertRefsIntoDB(uid,bmd.bmd_refs);
		
			
		} catch (SQLException e) {
			System.err.println("processInteractionRecord: Fatal Error: Unable to create SQLstatement");
			System.err.println("SQLException: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
		
	
		return true;
	}
	
	private Integer getObjID(bmd_objects_rec bmd_object) throws SQLException {

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
	
	
	private void insertObjLabelIntoDB(Integer uid,Integer objid, StringBuffer label) {
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
	
	private void insertRefsIntoDB(Integer uid,ArrayList refs) {
		Iterator refsI = refs.iterator();
		while (refsI.hasNext()) {
			bmd_refs_rec curRef = (bmd_refs_rec) refsI.next();
			if (!insertRefIntoDB(uid,curRef)) {
				System.err.println("failed to insert ref into db.");
			}
		}
	}
	
	private void insertComplex2SubunitsIntoDB(Integer compid,Integer objID)  {
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
	
	private Integer resolveIntIDKey(bmd_objects_rec objA, bmd_objects_rec objB) throws SQLException {
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
	

	
	private boolean insertRefIntoDB(Integer uid,bmd_refs_rec ref) {
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
	
	private Integer insertSourceIntoDB(bmd_source_rec bmd_source, Integer intcompID) throws SQLException {
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
	
	
	private Integer resolveObjIDKey(bmd_objects_rec bmd_object) throws SQLException {

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
		private int resolveObjDBKey(StringBuffer db_name) throws SQLException {
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
		
		private int resolveMolTypeKey(String type_name) throws SQLException {
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
		
		private int resolveRefDBKey(String db_name) throws SQLException {
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
		
		private int resolveExpDBKey(String method_descr) throws SQLException {
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
		
		private int resolveIntDBKey(String db_name) throws SQLException {
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
		
		private int resolveRecTypeKey(String type_name) throws SQLException {
			
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
