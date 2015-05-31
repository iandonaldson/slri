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
public class SeqHoundGetBINDComplex extends SeqHoundQuery {
    public final static String[] ARGLIST={"compid"};
	public SeqHoundGetBINDComplex(Connection conn2setup) {
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
	public String getData(String compid) throws SQLException, SeqHoundLogicException {
	   
	    
	    
	   StringBuffer dataBuf=new StringBuffer();
	   
	   Statement stmt = m_connection.createStatement();
	   StringBuffer sql = new StringBuffer("SELECT distinct objAid,objAtype,objAdb,objAtax,objAlabel,objBid,objBtype,objBdb,objBtax,objBlabel,intid FROM bind_module.complexes2ints WHERE compid = ");
	   sql.append(compid);
	   //sql.append(" and source.intcompid=comp2ints.compid and comp2ints.intid=ints.intid and ((ints.objAid = objectsA.objid and ints.objAid=labelsA.objid) and (ints.objBid = objectsB.objid and ints.objBid=labelsB.objid)) and sourceI.uid=labelsA.uid and sourceI.uid=labelsB.uid and objectsA.db = obj_dbasesA.db and objectsA.type=mol_typeA.type and objectsB.db = obj_dbasesB.db and objectsB.type=mol_typeB.type and sourceI.intcompid = ints.intid");
	   System.out.println(sql);
	   stmt.execute(sql.toString());
	   ResultSet objR = stmt.getResultSet();
	   if (!objR.first()) {
	   	System.out.println("No interactor molecular objects found.");
	   	return null;
	   }
	   objR.beforeFirst();
	   while (objR.next()) {
	       dataBuf.append(objR.getInt(1));
	       dataBuf.append("," + objR.getString(2));
	       dataBuf.append("," + objR.getString(3));
	       dataBuf.append("," + objR.getInt(4));
	       dataBuf.append("," + objR.getString(5));
	       dataBuf.append("," + objR.getInt(6));
	       dataBuf.append("," + objR.getString(7));
	       dataBuf.append("," + objR.getString(8));
	       dataBuf.append("," + objR.getInt(9));
	       dataBuf.append("," + objR.getString(10));
	       dataBuf.append("," + objR.getInt(11));
	       
	       if (!objR.isLast()) {
	           dataBuf.append('\n');
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
		
		String dburl = "jdbc:mysql://marge:33306/bind_module";
		
		try {
			conn = DriverManager.getConnection(dburl,"mmatan", "asdf1234");
		} catch (SQLException e1) {
			System.err.println("SeqHoundGetInteractors: Fatal Error: Unable to establish database connection using specified server/login");
			System.err.println("SQLException: " + e1.getMessage());
			e1.printStackTrace();
			System.exit(1);
		}
		
		SeqHoundGetBINDComplex shgi = new SeqHoundGetBINDComplex(conn);
		try {
            System.out.println(shgi.getData("133358"));
        } catch (SQLException e2) {
            // TODO Auto-generated catch block
            e2.printStackTrace();
        } catch (SeqHoundLogicException e2) {
            // TODO Auto-generated catch block
            e2.printStackTrace();
        }
		
	}
}
