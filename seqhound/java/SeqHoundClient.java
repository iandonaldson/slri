/*
 * Created on May 27, 2004
 *
 * TODO To change the template for this generated file go to
 * Window - Preferences - Java - Code Generation - Code and Comments
 */

import org.blueprint.seqhound.SeqHound;
import java.io.IOException;

/**
 * @author lieu
 *
 * TODO To change the template for thi
 * 
 * 
 * 
 * 
 * s generated type comment go to
 * Window - Preferences - Java - Code Generation - Code and Comments
 */
public class SeqHoundClient 
{
	public static void main(String[] args) 
	{
	   int gi = 4557225;
	   try {
	   	  SeqHound seqhound = new SeqHound();
	   	  seqhound.SHoundInit(true, "app");
	   	  int[] i = seqhound.SHoundLLIDFromGi(4557225);
	   	  
	   	  System.out.print ( "The locuslink id for gi " + gi + " is: " );
	   	  System.out.println ( i[0] );
	   } catch ( IOException io ) {
	   	  io.printStackTrace();
	   }
	}
}
