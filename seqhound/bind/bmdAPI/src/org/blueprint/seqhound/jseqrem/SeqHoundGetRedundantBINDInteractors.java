package org.blueprint.seqhound.jseqrem;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
/*
 * Created on 5-Jul-2004
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
public class SeqHoundGetRedundantBINDInteractors extends SeqHoundQuery {
    public final static String[] ARGLIST={"iid"};
	public SeqHoundGetRedundantBINDInteractors(Connection conn2setup) {
		super(conn2setup);
		m_argumentList=ARGLIST;
	}
	

	
	/**
	 * Returns a string specifying GIs for the interaction id (iid) given
	 * protein GI.
	 * @param gi Protein GI to query.
	 * @return Encoding of an Ontoglyph for the given GI.
	 * @throws SQLException On any database problems.
	 * @throws SeqHoundLogicException On logical errors in the query.
	 */
	public String getData(String intid) throws SQLException, SeqHoundLogicException {
	   
	    
	    
	   StringBuffer dataBuf=new StringBuffer();
	   
	   //First retrieve the interaction ids of redundant interactions
	   Statement sourceidstmt = m_connection.createStatement();
	   Statement intidstmt = m_connection.createStatement();
	   String sourceidsql = "SELECT sourceR.id FROM bind_module.bmd_source as sourceR,bind_module.bmd_source as sourceP,bind_module.bmd_ints as intsR, bind_module.bmd_ints as intsP WHERE sourceP.intcompid = intsP.intid and intsP.rig = intsR.rig and intsR.intid = sourceR.intcompid and sourceR.type=1 and sourceR.db=1 and sourceP.type=1 and sourceR.db=1 and sourceP.id=" + intid;
	   System.out.println(sourceidsql);
	   sourceidstmt.execute(sourceidsql);
	   ResultSet sourceidR = sourceidstmt.getResultSet();
   
	   //Now retrive the objects for those redundant interactions
	   StringBuffer intidsql = new StringBuffer("SELECT distinct objectsA.id,mol_typeA.type_name,obj_dbasesA.db_name,objectsA.tax,labelsA.label,objectsB.id,mol_typeB.type_name,obj_dbasesB.db_name,objectsB.tax,labelsB.label,source.id FROM bind_module.bmd_obj_labels as labelsA, bind_module.bmd_objects as objectsA, bind_module.bmd_source as source, bind_module.bmd_ints as ints,bind_module.bmd_mol_type as mol_typeA,bind_module.bmd_obj_dbases as obj_dbasesA,bind_module.bmd_obj_labels as labelsB, bind_module.bmd_objects as objectsB, bind_module.bmd_mol_type as mol_typeB,bind_module.bmd_obj_dbases as obj_dbasesB WHERE ( ");
	   if (!sourceidR.first()) {
	       System.err.println("No interactions with this interaction id");
	       return null;
	   }
	   sourceidR.beforeFirst();
	   while(sourceidR.next()) {
	       intidsql.append("source.id=" + sourceidR.getInt(1));
	       if (!sourceidR.isLast()) {
	           intidsql.append(" or ");
	       }
	       
	   }
	   intidsql.append(") and source.intcompid = ints.intid and ((ints.objAid = objectsA.objid and ints.objAid=labelsA.objid) and (ints.objBid = objectsB.objid and ints.objBid=labelsB.objid)) and source.uid=labelsA.uid and source.uid=labelsB.uid and objectsA.db = obj_dbasesA.db and objectsA.type=mol_typeA.type and objectsB.db = obj_dbasesB.db and objectsB.type=mol_typeB.type");
	   System.out.println(intidsql);
	   intidstmt.execute(intidsql.toString());
	   ResultSet objR = intidstmt.getResultSet();
	   if (!objR.first()) {
	   	System.out.println("No interactor molecular objects found.");
	   	return null;
	   }
	   objR.beforeFirst();
	   while (objR.next()) {
	       //TODO include wasNull test for null taxid's and GIs
	       dataBuf.append(objR.getInt(1));objR.wasNull();
	       dataBuf.append("," + objR.getString(2));
	       dataBuf.append("," + objR.getString(3));
	       dataBuf.append("," + objR.getInt(4));
	       dataBuf.append("," + objR.getString(5));
	       dataBuf.append("," + objR.getInt(6));
	       dataBuf.append("," + objR.getString(7));
	       dataBuf.append("," + objR.getString(8));
	       dataBuf.append("," + objR.getInt(9));
	       dataBuf.append("," + objR.getString(10));
	       dataBuf.append("," + objR.getString(11));
	       
	       if (!objR.isLast()) {
	           dataBuf.append("<BR>\n");
	       }

	   }
	   return dataBuf.toString();

	}
	
	public static void main (String[] args) {
		Connection conn = null;
		
		try {
			Class.forName("com.mysql.jdbc.Driver");
		} catch (ClassNotFoundException e) {
			System.err.println("SeqHoundGetInteractors: Fatal Error: Unable to find specified jdbc connector class");
			System.err.println(e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
		
		String dburl = "jdbc:mysql://marge:33306/bind_module2";
		
		try {
			conn = DriverManager.getConnection(dburl,"mmatan", "asdf1234");
		} catch (SQLException e1) {
			System.err.println("SeqHoundGetInteractors: Fatal Error: Unable to establish database connection using specified server/login");
			System.err.println("SQLException: " + e1.getMessage());
			e1.printStackTrace();
			System.exit(1);
		}
		
		SeqHoundGetRedundantBINDInteractors shgi = new SeqHoundGetRedundantBINDInteractors(conn);
		try {
            System.out.println(shgi.getData("79184"));
        } catch (SQLException e2) {
            // TODO Auto-generated catch block
            e2.printStackTrace();
        } catch (SeqHoundLogicException e2) {
            // TODO Auto-generated catch block
            e2.printStackTrace();
        }
		
	}
}
