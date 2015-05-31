
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
public class SeqHoundGetInteractors extends SeqHoundQuery {
    public final static String[] ARGLIST={"iid"};
	public SeqHoundGetInteractors(Connection conn2setup) {
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
	   
	   Statement stmt = m_connection.createStatement();
	   StringBuffer sql = new StringBuffer("SELECT objects.id FROM bind_module.bmd_objects as objects, bind_module.bmd_source as source, bind_module.bmd_ints as ints WHERE source.id = ");
	   sql.append(intid);
	   sql.append(" and source.intcompid = ints.intid and (ints.objAid = objects.objid or ints.objBid = objects.objid)");
	   stmt.execute(sql.toString());
	   ResultSet objR = stmt.getResultSet();
	   if (!objR.first()) {
	   	System.out.println("No interactor molecular objects found.");
	   	return null;
	   }
	   objR.beforeFirst();
	   while (objR.next()) {
	       dataBuf.append(objR.getInt(1));
	       if (!objR.isLast()) {
	           dataBuf.append(',');
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
		
		SeqHoundGetInteractors shgi = new SeqHoundGetInteractors(conn);
		try {
            System.out.println(shgi.getData("64945"));
        } catch (SQLException e2) {
            // TODO Auto-generated catch block
            e2.printStackTrace();
        } catch (SeqHoundLogicException e2) {
            // TODO Auto-generated catch block
            e2.printStackTrace();
        }
		
	}
}
