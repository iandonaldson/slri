import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

/*
 * Created on 30-Jun-2004
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
public class SeqHound2 {
	Connection conn = null;
	
	public SeqHound2() {
		
	}
	
	public SeqHound2(Connection conn) {
		this.conn = conn;
	}
	
	public boolean SHoundInit(boolean entrez,String programName) {
		return true;
	}
	
	public int[] SHoundRedundantGroup(int gi) {
		Statement stmt;
		int[] redundantGIs = null;
		try {
			stmt = conn.createStatement();
			stmt.execute("SELECT rgroup from seqhound.redund where gi=" + gi);
			ResultSet rgroupR = stmt.getResultSet();
			if (!rgroupR.first()) {
				//System.err.println("SHoundRedundantGroup:Warning: no results for gi");
				return null;
			}
			rgroupR.first();
			int rgroup = rgroupR.getInt(1);
			
			stmt.execute("SELECT gi from seqhound.redund where rgroup=" + rgroup);
			ResultSet giR = stmt.getResultSet();
			if (!giR.first()) {
				//System.err.println("SHoundRedundantGroup:Warning: no results for rgroup");
				return null;
			}
			int numGIs = 0;
			giR.beforeFirst();
			while (giR.next()) {
				numGIs++;
			}
			redundantGIs = new int[numGIs];
			int giIndex=0;
			while (giR.next()) {
				redundantGIs[giIndex] = giR.getInt(1);
				giIndex++;
			}			
			return redundantGIs;
		
		} catch (SQLException e) {
			System.err.println("getGenbankProteinObjectGIs: Error: Unable to get genbankproteingi values");
			System.err.println("SQLException: " + e.getMessage());
			e.printStackTrace();
			return null;
		}
		
		
	}
}
		
		
	

