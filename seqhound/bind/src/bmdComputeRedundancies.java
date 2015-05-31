//import com.mysql.jdbc.Connection;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Iterator;
import org.blueprint.seqhound.SeqHound;
/*
 * Created on 25-Jun-2004
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
public class bmdComputeRedundancies {
    SeqHound seq = null; 
	bmdDB bmddb = null;
	
	//This internal class will be used to
	//represent pairs of rogs to be mapped 
	//to rigs

    
	public bmdComputeRedundancies() {
		bmddb = new bmdDB();
		
		try {
			seq = new SeqHound();
		} catch (IOException e) {
			System.err.println("bmdComputeRedundancies: Fatal Error, failed to initialize SeqHound. Exiting.");
			e.printStackTrace();
			System.exit(1);
		}
		
		
	}
	
	public void computeRedundancies() {
		
		computeObjectRedundancies();
		computeInteractionRedundancies();
	}
	
	private void computeObjectRedundancies() {
		Integer maxRogI = bmddb.getMaxROG();
		int rogIndex = 1;
//		if (maxRogI != null) {
//			rogIndex = maxRogI.intValue();
//		}
//		else {
//			rogIndex = 1;
//		}
//		
		//First create a new internal hash including all the gi2objid mappings.
		bmddb.createGenbankProteinGI2ObjidInternalMap();
		
		//Now get the genbank protein GIs to use in redund queries
		HashSet gpGIs = bmddb.getGenbankProteinGIs();
		
		//This hashset will contain those GIs which have already been placed into a redundant group
		HashSet placedGIs = new HashSet();
		try {
			bmddb.clearROGs();  //blank the rogs to zero
			Iterator gpGIsI = gpGIs.iterator();
			while (gpGIsI.hasNext()) {
				Integer curGI = (Integer) gpGIsI.next();
				if (!placedGIs.contains(curGI)) {
					int[] curRedundGIs = seq.SHoundRedundantGroup(curGI.intValue());
					//int[] curRedundGIs = {6325291,14318476};
					ArrayList RedundantGIList = new ArrayList();
					if (curRedundGIs != null) {
						for (int i = 0; i<curRedundGIs.length; i++) {
							Integer curRedundantGI = new Integer(curRedundGIs[i]);
							if (gpGIs.contains(curRedundantGI) && !placedGIs.contains(curRedundantGI)) {
								RedundantGIList.add(new Integer(curRedundGIs[i]));
								
							}
						}
					}
					else {  //no redundant group returned, simply add current GI to redundant group
						RedundantGIList.add(curGI);
					}
					if (RedundantGIList.size() > 0) {
						bmddb.setRogForGenbankGIList(rogIndex,RedundantGIList);
						//placedGIs.add(curGI);  //TODO decide whether to make this and the last conditional upon success of setting the rog properly.
						placedGIs.addAll(RedundantGIList);
						rogIndex++;
					}
				}
			}
			bmddb.AssignRemainingROGs();
			

		} catch (IOException e) {
			// TODO Auto-generated catch block
			System.err.println("computeObjectRedundancies: Error: IOException, seqhound wasn't able to retrieve redundant group information.  Redund computation failed.");
			e.printStackTrace();
		}
		
	}
	
	private void computeInteractionRedundancies() {
//		Integer maxRig = bmddb.getMaxRIG();
//		int rigIndex = 1;
//		bmddb.createRog2ObjidInternalMap();
		bmddb.computeRIGs();
		
	}
	
	// Main Method
    public static void main (String[] args) {
    	bmdComputeRedundancies bmdCR = new bmdComputeRedundancies();
    	
    	try {
			if (bmdCR.seq.SHoundInit(false,"bmdComputeRedundancies") == false) {
				System.err.println("bmdComputeRedundancies: Fatal error: unable to initialize Seqhound");
				
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			System.err.println("bmdComputeRedundancies: Fatal error: unable to initialize Seqhound due to io exception");
			e.printStackTrace();
		}
    	bmdCR.computeRedundancies();
        
        
    }
}
