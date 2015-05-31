/* $Id: SeqHoundTest.java,v 1.17 2005/03/14 20:39:23 ryao Exp $ */
import java.io.*;
import java.lang.Object;
import java.lang.String;
import java.util.*;
import org.blueprint.seqhound.SeqHound;
import org.blueprint.seqhound.GoAnnotation;
import org.blueprint.seqhound.SLRIMisc;
import junit.framework.TestCase;
import java.util.logging.*;


/**
  * File :    SeqhoundTest.java
  *           a unit test driver for seqhound remote java API testing
  * class:    SeqhoundTest.java
  *           a class provides unit testing functions to the SeqHound
  *           APIs by calling methods in SeqHound.java.
  * 
  * @author   Rong Yao (ryao@blueprint.org)
  *
  */
public class SeqHoundTest extends TestCase
{
    private static SeqHound sh = null;
    private static boolean boolTrue = true, boolFalse = false;
    private static String emptyStr = "", nullStr= null;
    private static int neg1 = -1;
    private static int[] negList = {neg1, neg1, neg1};
    private static int[] nullIntList = null;
    private static String[] nullList = null;
    private static String[] emList = {emptyStr, emptyStr, emptyStr};
    private static int[] emIntList = new int[0];
    private FileOutputStream out;
    private static StreamHandler slog;
    private String logfile = "java_test.log";
 
  
    public SeqHoundTest (String name) throws IOException
    {
        super(name);
    }
    public SeqHound createInstance () throws Exception
    {
        return new SeqHound();
    }
    protected void setUp () throws Exception
    {
        super.setUp();
        sh = createInstance();
        sh.SHoundInit(true, "test");
	try{
	   out = new FileOutputStream (new File(logfile), true);
	   slog = new StreamHandler(out, new SimpleFormatter());
	}catch (FileNotFoundException e)
	{
	}
    }
    protected void tearDown () throws Exception
    {
        sh.SHoundFini();
        sh = null;
        super.tearDown();
    }
    
    protected static String pad(String funcname, int length) throws Exception
    {
	StringBuffer buf = new StringBuffer(funcname);
        while (buf.length() < length)
	{
	    buf.insert(buf.length(), ' ');
	}
        return buf.toString();
    }
    
    protected static void writeLog (String funcname, String status, String input) throws Exception
    {
        funcname = pad(funcname, 45);
	status = pad(status, 10);
        String msg = funcname+"    "+status+"    "+input;
        LogRecord line = new LogRecord(Level.parse("INFO"), msg);
	line.setSourceClassName ("SeqHoundTest");
	slog.publish(line);
	slog.flush();
    }
    
    public static void testInit ()  throws Exception
    {
        String funcname = "SHoundInit";
        System.out.println(funcname);
	try{
            assertEquals(boolTrue, sh.SHoundInit(false, "Test"));
            assertEquals(boolTrue, sh.SHoundInit(true, "Test"));
	    writeLog(funcname, "OK", "false & Test");
	}catch (Error e)
	{
	    writeLog(funcname, "FAIL", "false & Test");
	}
	
	funcname = "SHoundIsInited";
	System.out.println(funcname);
	try{
	    assertEquals(boolTrue, sh.SHoundIsInited());
	    writeLog(funcname, "OK", "");
	}catch (Error e)
	{
	    writeLog(funcname, "FAIL", "");
	}
	
	funcname = "SHoundIsNetEntrezOn";
	System.out.println(funcname);
	try{
	    assertEquals(boolTrue, sh.SHoundIsNetEntrezOn());
	    writeLog(funcname, "OK", "");
	}catch (Error e)
	{
	    writeLog(funcname, "FAIL", "");
	}
	
	funcname = "SHoundNetEntrezInit";
	System.out.println(funcname);
	try{
	    assertEquals(boolTrue, sh.SHoundNetEntrezInit("Test"));
	    writeLog(funcname, "OK", "Test");
	}catch (Error e)
	{
	    writeLog(funcname, "FAIL", "Test");
	}

    }
    
   
    public static void testCoreFunctions () throws Exception
    {
        String funcname = "SHoundFindAcc";
        System.out.println(funcname);
        String accession = "X88853";
	int gi = sh.SHoundFindAcc(accession);
	try {
            assertTrue(gi>0);
            assertEquals(-1, sh.SHoundFindAcc(emptyStr));
            assertEquals(-1, sh.SHoundFindAcc(nullStr));
	    writeLog(funcname, "OK", accession);
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", accession);
	}
  
        funcname = "SHoundFindAccList";
  	System.out.println(funcname);
	String[] accList = {"X88853", "Y136070"};
	int[] retList = sh.SHoundFindAccList(accList);
        try{
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    retList = sh.SHoundFindAccList(emList);
	    assertTrue(retList.length==0);
            assertNull (sh.SHoundFindAccList(nullList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(accList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(accList));
	}
   
	
	funcname = "SHoundFindName";
	System.out.println(funcname);
	String pdbId = "3TS1";
	gi = sh.SHoundFindName(pdbId);
	try{
	    assertTrue(gi>0);
            assertEquals(-1, sh.SHoundFindName(emptyStr));
            assertEquals(-1, sh.SHoundFindName(nullStr));
	    writeLog(funcname, "OK", pdbId);
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", pdbId);
	}
	
	funcname = "SHoundFindNameList";	
	System.out.println(funcname);
	String[] pdbList = {"3TS1", "AD060234"};
	try{
	    retList = sh.SHoundFindNameList(pdbList);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    retList = sh.SHoundFindNameList(emList);
            assertTrue(retList.length==0);
            assertNull(sh.SHoundFindNameList(nullList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(pdbList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(pdbList));
	}
        
	funcname = "SHoundAccFromGi";    
        System.out.println(funcname);
	gi = 4646;
	String retStr = sh.SHoundAccFromGi(gi);
	try{
            assertNotNull(retStr);
            assertNull(sh.SHoundAccFromGi(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	}


	funcname = "SHoundAccFromGiList";	
	System.out.println(funcname);
	int[] giList2 = {4646, 230779};
        String[] retAccList = sh.SHoundAccFromGiList(giList2);
	try{
	    assertNotNull(retAccList);
            assertTrue(retAccList.length>0);
            retAccList = sh.SHoundAccFromGiList(negList);
	    assertTrue(retAccList.length == 0);
	    assertNull(sh.SHoundAccFromGiList(emIntList));
	    assertNull(sh.SHoundAccFromGiList(nullIntList));
            writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList2));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList2));
	}
	
	funcname = "SHoundTitleFromGi";    
        System.out.println(funcname);
	gi = 230779;
	retStr = sh.SHoundTitleFromGi(gi);
	try{
            assertNotNull(retStr);
            assertNull(sh.SHoundTitleFromGi(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	}
	
	funcname = "SHoundGetAccFromRetiredGi";
        System.out.println(funcname);
        gi = 800;
	try{
            assertNotNull (sh.SHoundGetAccFromRetiredGi(gi));
	    assertNull(sh.SHoundGetAccFromRetiredGi(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	}
        
	funcname = "SHoundMoleculeType";
	System.out.println(funcname);
	gi = 4646;
	String type = "dna";
	retStr = sh.SHoundMoleculeType(gi);
        try{
	    assertNotNull(retStr);
            assertEquals(type, retStr);
            assertNull(sh.SHoundAccFromGi(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	}
	
	funcname = "SHoundIsProtein";	
	System.out.println(funcname);
	gi = 4001824;
	try{
	    assertTrue(sh.SHoundIsProtein(gi));
    	    assertFalse(sh.SHoundIsProtein(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	}
	
	funcname = "SHoundSequenceLength";
	System.out.println(funcname);
	int len = 985;
	gi = 4646;
	try{
	    assertEquals(len, sh.SHoundSequenceLength(gi));
	    assertEquals(-1, sh.SHoundSequenceLength(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	}
	
	funcname = "SHoundIsNRFirst";
	System.out.println(funcname);
	int nrFirstGi = 4557225;
	try {
	    assertTrue(sh.SHoundIsNRFirst(nrFirstGi));
            assertFalse(sh.SHoundIsNRFirst(gi));
            assertFalse(sh.SHoundIsNRFirst(neg1));	
	    writeLog(funcname, "OK", String.valueOf(nrFirstGi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(nrFirstGi));
	}
	
	funcname = "SHoundExists";
	System.out.println(funcname);
	gi = 4646;
	try{
	    assertTrue(sh.SHoundExists(gi));
            assertFalse(sh.SHoundExists(neg1));
            writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	}
	
	funcname = "SHoundGetReferenceIDFromGi";
	System.out.println(funcname);
	gi = 4646;
	int refId = 89083486;
	int refType = 2;
        int[] refList = sh.SHoundGetReferenceIDFromGi(gi, refType);
	try{
            assertNotNull(refList);
	    assertEquals(refId, refList[0]); 
	    assertNull(sh.SHoundGetReferenceIDFromGi(neg1,neg1));
	    writeLog(funcname, "OK", (String.valueOf(gi)) + " & " + (String.valueOf(refType)));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", (String.valueOf(gi)) + " & " + (String.valueOf(refType)));
	}

	funcname = "SHoundGetReferenceIDFromGiList";
	System.out.println(funcname);
	try{
            refList = sh.SHoundGetReferenceIDFromGiList(giList2, refType);
	    assertNotNull (refList);
	    assertTrue(refList.length>0);
	    refList =  sh.SHoundGetReferenceIDFromGiList(negList, refType) ;
	    assertTrue(refList.length==0);
	    assertNull (sh.SHoundGetReferenceIDFromGiList(emIntList, refType));
	    assertNull (sh.SHoundGetReferenceIDFromGiList(giList2, neg1));
	    assertNull (sh.SHoundGetReferenceIDFromGiList(nullIntList, refType));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList2)+" & "+ (String.valueOf(refType)));
        }catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList2)+" & "+ (String.valueOf(refType)));
	}
	
	funcname = "SHoundGiFromReferenceID";	
	System.out.println(funcname);
	refId = 85190481;
	try{
	    retList = sh.SHoundGiFromReferenceID(refId, refType);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull(sh.SHoundGiFromReferenceID(neg1,neg1));
	    writeLog(funcname, "OK", String.valueOf(refId));
        }catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(refId));
	}    
      
        funcname = "SHoundGiFromReferenceList";
        System.out.println(funcname);
	int[] refList2 = {85190481, 97002444};
	try{
	   retList = sh.SHoundGiFromReferenceList(refList2, refType);
	   assertNotNull(retList);
	   assertTrue(retList.length>0);  
           retList = sh.SHoundGiFromReferenceList(negList, refType);  
	   assertTrue(retList.length==0);
	   assertNull (sh.SHoundGiFromReferenceList(refList2, neg1));
	   assertNull (sh.SHoundGiFromReferenceList(nullIntList, refType));
	   assertNull (sh.SHoundGiFromReferenceList(emIntList, refType));
	   writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(refList2));
        }catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(refList2));
	}      
	
	funcname = "SHoundGetDefline";	
	System.out.println(funcname);
        gi = 6324856;
	try{
            retStr = sh.SHoundGetDefline(gi);
	    assertNotNull(retStr);
	    assertNull(sh.SHoundGetDefline(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
        }catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	}      
	 
	funcname = "SHoundGetDeflineList";	
	System.out.println(funcname);
	int[] giList4 = {6324856, 230779};
	String[] retStrList = sh.SHoundGetDeflineList(giList4);
	try{
	    assertNotNull(retStrList);
	    assertTrue(retStrList.length>0);
	    retStrList = sh.SHoundGetDeflineList(negList);
	    assertTrue(retStrList.length == 0);
	    assertNull(sh.SHoundGetDeflineList(emIntList));
	    assertNull(sh.SHoundGetDeflineList(nullIntList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList4));
        }catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList4));
	}      
	
	funcname = "SHoundGetFasta";			
	System.out.println(funcname);
	gi = 6324856;
	try{
	    retStr = sh.SHoundGetFasta(gi);
	    assertNotNull(retStr);
	    assertNull(sh.SHoundGetFasta(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	}      
	
	funcname = "SHoundGetFastaList";	
	System.out.println(funcname);
	try{
	    retStrList = sh.SHoundGetFastaList(giList4);
	    assertNotNull(retStrList);
	    assertTrue(retStrList.length>0);
	    assertNull(sh.SHoundGetFastaList(negList));
            assertNull(sh.SHoundGetFastaList(emIntList));
	    assertNull(sh.SHoundGetFastaList(nullIntList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList4));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList4));
	} 
	
	funcname = "SHoundGetFastaFromRedundantGroupID";     
	System.out.println(funcname);
	int gid = 11;
	try{
	    retStr = sh.SHoundGetFastaFromRedundantGroupID(gid);
	    assertNotNull(retStr);
	    assertNull( sh.SHoundGetFastaFromRedundantGroupID(neg1));
	    writeLog(funcname, "OK", String.valueOf(gid));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gid));
	} 
		
        funcname = "SHoundGetGenBankff";
        System.out.println(funcname);
	gi  = 230779;
	try{
	    retStr = sh.SHoundGetGenBankff(gi);
	    assertNotNull(retStr);
	    assertNull(sh.SHoundGetGenBankff(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	} 
	
	funcname = "SHoundGetGenBankffList";	
	System.out.println(funcname);
	int[] giList5  = {230779, 32, 666};
	try{
	    retStrList = sh.SHoundGetGenBankffList(giList5);
	    assertNotNull(retStrList);
	    assertTrue(retStrList.length>0);
	    retStrList = sh.SHoundGetGenBankffList(negList);
	    //according to seqrem return, each retStrList element contains
		//string: Could not generate genbankff for this gi...
		assertTrue(retStrList.length == 3);
	    assertNull( sh.SHoundGetGenBankffList(emIntList));
	    assertNull( sh.SHoundGetGenBankffList(nullIntList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList5));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList5));
	} 
	
	funcname = "SHoundDNAFromProtein";	
	System.out.println(funcname);
	int proteinGi = 32;
	try{
	    gi = sh.SHoundDNAFromProtein(proteinGi);
	    assertTrue(gi>0);
	    assertEquals(-1, sh.SHoundDNAFromProtein(neg1));
	    writeLog(funcname, "OK", String.valueOf(proteinGi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(proteinGi));
	} 
	
	funcname = "SHoundDNAFromProteinList";	
	System.out.println(funcname);
	int[] proteinGiList = {32, 325, 4546};
	try{
	    retList = sh.SHoundDNAFromProteinList(proteinGiList);
	    assertNotNull(retList);
	    assertTrue (retList.length>0);
	    retList = sh.SHoundDNAFromProteinList(negList);
	    assertTrue(retList.length == 0);
	    assertNull (sh.SHoundDNAFromProteinList(emIntList));
	    assertNull (sh.SHoundDNAFromProteinList(nullIntList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(proteinGiList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(proteinGiList));
	} 
	
	funcname = "SHoundProteinFromDNA";	
	System.out.println(funcname);
	int DNAGi = 152776;
	try{
	    retList = sh.SHoundProteinFromDNA(DNAGi);
	    assertNotNull(retList);
	    assertTrue(retList.length>0); 
	    assertNull(sh.SHoundProteinFromDNA(neg1));
	    writeLog(funcname, "OK", String.valueOf(DNAGi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(DNAGi));
	}     
     
        funcname = "SHoundProteinFromDNAList";
        System.out.println(funcname);
	int[] DNAGiList = {152776,2463028};
	try{
	    retList = sh.SHoundProteinFromDNAList(DNAGiList);
	    assertNotNull(retList);
	    assertTrue(retList.length>0); 
	    retList = sh.SHoundProteinFromDNAList(negList);  
	    assertTrue(retList.length==0); 
	    assertNull(sh.SHoundProteinFromDNAList(emIntList));
	    assertNull(sh.SHoundProteinFromDNAList(nullIntList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(DNAGiList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(DNAGiList));
	}     
	
	funcname = "SHoundMuidFromGi";
	System.out.println(funcname);
	gi = 6322016;
	try{
	    retList = sh.SHoundMuidFromGi(gi);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull(sh.SHoundMuidFromGi(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	}      
	
	funcname = "SHoundMuidFromGiList";	
	System.out.println(funcname);
	int[] giList6 = {6322016, 32};
	try{
	    retList = sh.SHoundMuidFromGiList(giList6);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);   
	    retList = sh.SHoundMuidFromGiList(negList);  
	    assertTrue(retList.length==0); 
	    assertNull (sh.SHoundMuidFromGiList(emIntList)); 
	    assertNull (sh.SHoundMuidFromGiList(nullIntList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList6));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList6));
	}      
	    
	funcname = "SHoundMuidFrom3D";
	System.out.println(funcname);
	int mmdbId = 11;
	try{
	    retList = sh.SHoundMuidFrom3D(mmdbId);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull(sh.SHoundMuidFromGi(neg1));  
            writeLog(funcname, "OK", String.valueOf(mmdbId));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(mmdbId));
	}      
	
	funcname = "SHoundMuidFrom3DList";
	System.out.println(funcname);
	int[] mmdbIdList = {11, 12};
	try{
	    retList = sh.SHoundMuidFrom3DList(mmdbIdList);
	    assertNotNull(retList);
	    assertTrue(retList.length >0);
	    retList = sh.SHoundMuidFrom3DList(negList);  
	    assertTrue(retList.length==0); 
	    assertNull (sh.SHoundMuidFrom3DList(emIntList));
	    assertNull (sh.SHoundMuidFrom3DList(nullIntList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(mmdbIdList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(mmdbIdList));
	}    
	
	 
	/*this function does not completely work
	  format=1 (XML format) does not work
	 */
	funcname = "SHoundProteinReportSetDescription";	
	System.out.println(funcname);
        gi = 4557225;
	int format = 2;
	try{
	    retStr = sh.SHoundProteinReportSetDescription(gi, format);
	    assertNotNull(retStr);
	    retStr = sh.SHoundProteinReportSetDescription(neg1, neg1);
	    assertNull (retStr);
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	} 
	
    } 
    

    public static void testStructureFunctions () throws Exception
    {
    	String funcname = "SHound3DbWhat";
	System.out.println(funcname);
	int molFlag = 1;
	int mmdbId = 3436;
	try{
	    assertEquals(molFlag, sh.SHound3DbWhat(mmdbId));
	    assertEquals(-1, sh.SHound3DbWhat(neg1));
	    writeLog(funcname, "OK", String.valueOf(mmdbId));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(mmdbId));
	}    
	
	funcname = "SHound3DExists";
	System.out.println(funcname);
	int gi = 4646;
	try{
	    assertTrue(sh.SHound3DExists(gi));
            assertFalse(sh.SHound3DExists(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	}    
	
	funcname =  "SHoundGetLargestMMDBID";       
	System.out.println(funcname);
	try{
	    assertTrue(sh.SHoundGetLargestMMDBID()>0);
	    writeLog(funcname, "OK", "");
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", "");
	}    
	
	funcname = "SHoundGet3D";
	System.out.println(funcname);
	mmdbId = 11;
	String retStr = sh.SHoundGet3D(mmdbId);
	try{
	    assertNotNull(retStr);
	    assertNull(sh.SHoundGet3D(neg1));
	    writeLog(funcname, "OK", String.valueOf(mmdbId));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(mmdbId));
	}    
	
	funcname = "SHoundGet3DFromPdbId";
	System.out.println(funcname);
	String pdbId = "3INS";
	try{
	    retStr = sh.SHoundGet3DfromPdbId(pdbId);
	    assertNotNull(retStr);
	    assertNull(sh.SHoundGet3DfromPdbId(emptyStr));
            assertNull(sh.SHoundGet3DfromPdbId(nullStr));
            writeLog(funcname, "OK", pdbId);
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", pdbId);
	}    
	
	funcname = "SHoundGetXML3D";
	System.out.println(funcname);
	mmdbId = 11;
	try{	
	    retStr = sh.SHoundGetXML3D(mmdbId);
	    assertNotNull(retStr);
	    assertNull(sh.SHoundGetXML3D(neg1));
	    writeLog(funcname, "OK", String.valueOf(mmdbId));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(mmdbId));
	}    
	
	funcname = "SHoundGetPDB3D";
	System.out.println(funcname);
	mmdbId = 11;
	try{
	    retStr = sh.SHoundGetPDB3D(mmdbId);
	    assertNotNull(retStr);
	    assertNull(sh.SHoundGetXML3D(neg1));
	    writeLog(funcname, "OK", String.valueOf(mmdbId));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(mmdbId));
	}  
	
	funcname = "SHoundGiFrom3D";
	System.out.println(funcname);
	mmdbId = 11;
	boolean proteinOnly = false;
	int[] retList = sh.SHoundGiFrom3D(mmdbId, proteinOnly);
	try{
	    assertNotNull(retList);
	    assertNull(sh.SHoundGiFrom3D(neg1, proteinOnly));
	    writeLog(funcname, "OK", String.valueOf(mmdbId)+ " & false");
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(mmdbId)+" & false");
	}  
	
	funcname = "SHoundGiFrom3DList";
	System.out.println(funcname);
	int[] mmdbIdList = {11, 12};
	retList = sh.SHoundGiFrom3DList(mmdbIdList, proteinOnly);
	try{
	    assertNotNull(retList);
	    retList = sh.SHoundGiFrom3DList(negList, proteinOnly);
	    assertTrue(retList.length == 0);
	    assertNull (sh.SHoundGiFrom3DList(emIntList, proteinOnly));
	    assertNull (sh.SHoundGiFrom3DList(nullIntList, proteinOnly));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(mmdbIdList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(mmdbIdList));
	}  
	
	funcname = "SHoundGiFromPDBchain";
	System.out.println(funcname);
	String id = "9XIM";
	String chain = "A";
	try{
	    gi = sh.SHoundGiFromPDBchain(id, chain);
	    assertTrue(gi>0);
	    assertEquals (-1, sh.SHoundGiFromPDBchain(emptyStr, emptyStr));
	    assertEquals (-1, sh.SHoundGiFromPDBchain(nullStr, nullStr));
	    writeLog(funcname, "OK", id+" & " + chain);
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", id+" & "+chain);
	}  
	
	funcname = "SHound3DFromGi";
	System.out.println(funcname);
        gi = 443580;
	try{
	     mmdbId = sh.SHound3DFromGi(gi);
	     assertTrue(mmdbId>0);
	     assertEquals(-1, sh.SHound3DFromGi(neg1));
	     writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	}  
	
	funcname = "SHound3DFromGiList";
	System.out.println(funcname);
	int[] giList = {443580, 230779, 4557225};
	try{
	    retList = sh.SHound3DFromGiList(giList);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    retList = sh.SHound3DFromGiList(negList);
	    assertTrue(retList.length == 0);
	    assertNull ( sh.SHound3DFromGiList(emIntList));
	    assertNull ( sh.SHound3DFromGiList(nullIntList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList));
	}  
   }
  
   
    public static void testASNFunctions () throws Exception
    {
        String funcname = "SHoundSeqIdFromGi";
        System.out.println(funcname);
	int gi = 230779;
	String retStr = sh.SHoundSeqIdFromGi(gi);
	try{
	    assertNotNull(retStr);
	    assertNull(sh.SHoundSeqIdFromGi(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	}  
   
	funcname = "SHoundSeqIdFromGiList";
	System.out.println(funcname);
	int[] giList = {230779, 480117, 230940};
	try{
            retStr = sh.SHoundSeqIdFromGiList(giList);
	    assertNotNull(retStr);
	    assertNull(sh.SHoundSeqIdFromGiList(negList));
	    assertNull(sh.SHoundSeqIdFromGiList(emIntList)); 
	    assertNull(sh.SHoundSeqIdFromGiList(nullIntList)); 
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList));
	}  
	
	funcname = "SHoundGetBioseq";
	System.out.println(funcname);
	try{
	    retStr = sh.SHoundGetBioseq(gi);
	    assertNotNull(retStr);
	    assertNull(sh.SHoundGetBioseq(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	}  
	
	funcname = "SHoundGetBioseqList";
	System.out.println(funcname);
	try{	
	    retStr = sh.SHoundGetBioseqList(giList);
	    assertNotNull(retStr);
 	    assertNull(sh.SHoundGetBioseqList(negList)); 
	    assertNull(sh.SHoundGetBioseqList(emIntList));
	    assertNull(sh.SHoundGetBioseqList(nullIntList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList));
	}  
	
	funcname = "SHoundGetXMLBioseq";
	System.out.println(funcname);
	try{	
	    retStr = sh.SHoundGetXMLBioseq(gi);
	    assertNotNull(retStr);
	    assertNull(sh.SHoundGetXMLBioseq(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	}  
	
	funcname = "SHoundXMLBioseq";
	System.out.println(funcname);
	gi = 230779;
	try{
	    retStr = sh.SHoundXMLBioseq(gi);
	    assertNotNull(retStr);
	    assertNull(sh.SHoundXMLBioseq(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	}  
	
	funcname = "SHoundGetSeqEntry";
	System.out.println(funcname);
	gi = 230779;
	try{
	    retStr = sh.SHoundGetSeqEntry(gi);
	    assertNotNull(retStr);
	    assertNull(sh.SHoundGetSeqEntry(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	}  
	
	funcname = "SHoundGetSeqEntryList";
	System.out.println(funcname);
	try{
	    retStr = sh.SHoundGetSeqEntryList(giList);
	    assertNotNull(retStr);
	    assertNull(sh.SHoundGetSeqEntryList(negList)); 
	    assertNull(sh.SHoundGetSeqEntryList(emIntList));
	    assertNull(sh.SHoundGetSeqEntryList(nullIntList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList));
	} 
	
	funcname = "SHoundGetXMLSeqEntry";
	System.out.println(funcname);
	gi = 230779;
	try{
	    retStr = sh.SHoundGetXMLSeqEntry(gi);
	    assertNotNull(retStr);
	    assertNull(sh.SHoundGetXMLSeqEntry(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	} 
	
	funcname = "SHoundGetBioseqSet";
	System.out.println(funcname);
	gi = 2791836;
	try{
	    retStr = sh.SHoundGetBioseqSet(gi);
	    assertNotNull(retStr);
	    assertNull(sh.SHoundGetBioseqSet(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	} 
	
	funcname = "SHoundGetBioseqSetList";
	System.out.println("SHoundGetBioseqSetList");
	int[] giList1 = {2791826, 2791835};
	try{
	    retStr = sh.SHoundGetBioseqSetList(giList1);
	    assertNotNull(retStr);
	    assertNull(sh.SHoundGetBioseqSetList(negList));
	    assertNull(sh.SHoundGetBioseqSetList(emIntList));
	    assertNull(sh.SHoundGetBioseqSetList(nullIntList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList1));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList1));
	} 
    }
    
   
    public static void testLocusLinkFunctions () throws Exception
    {
    	
	String funcname = "SHoundGOIDFromLLID";
 	System.out.println(funcname);
    	int llid = 1;
    	int[] retList = sh.SHoundGOIDFromLLID(llid);
	try{
    	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    writeLog(funcname, "OK", String.valueOf(llid));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(llid));
	} 
	
        funcname = "SHoundOMIMFromLLID";
        System.out.println(funcname);
    	llid =1;
	int omimid = sh.SHoundOMIMFromLLID(llid);
    	try{
    	    assertTrue(omimid>0);
    	    assertEquals(-1, sh.SHoundOMIMFromLLID(neg1));
	    writeLog(funcname, "OK", String.valueOf(llid));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(llid));
	} 
	
	funcname = "SHoundCDDIDFromLLID";
    	System.out.println(funcname);
    	llid = 174;
	String[] retStrList = sh.SHoundCDDIDFromLLID(llid);
	try{
	    assertNotNull(retStrList);
    	    assertTrue(retStrList.length>0); 
    	    assertNull(sh.SHoundCDDIDFromLLID(neg1));
	    writeLog(funcname, "OK", String.valueOf(llid));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(llid));
	} 
    	
    	funcname = "SHoundLLIDFromGOIDAndECode";
	System.out.println(funcname);
    	int goID = 6810;
    	String ecode = "IEA";
	try{
    	    retList = sh.SHoundLLIDFromGOIDAndECode(goID, ecode);
	    assertNotNull(retList);
    	    assertTrue (retList.length>0); 
	    assertNull (sh.SHoundLLIDFromGOIDAndECode(neg1, ecode));
	    assertNull (sh.SHoundLLIDFromGOIDAndECode(goID, emptyStr));
	    assertNull (sh.SHoundLLIDFromGOIDAndECode(goID, nullStr));
	    writeLog(funcname, "OK", String.valueOf(goID)+" & "+ecode);
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(goID)+" & "+ecode);
	} 
	
	funcname = "SHoundLLIDFromOMIM";
	System.out.println(funcname);
	omimid = 103950;
	try{
	    retList = sh.SHoundLLIDFromOMIM(omimid);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull(sh.SHoundLLIDFromOMIM(neg1));
	    writeLog(funcname, "OK", String.valueOf(omimid));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(omimid));
	} 
	
	funcname = "SHoundLLIDFromCDDID";
	System.out.println(funcname);
	String cddid = "pfam00664";
	try{
	    retList = sh.SHoundLLIDFromCDDID(cddid);
	    assertNotNull(retList);
	    assertTrue(retList.length >0); 
	    assertNull(sh.SHoundLLIDFromCDDID(emptyStr));
	    assertNull(sh.SHoundLLIDFromCDDID(nullStr));
	    writeLog(funcname, "OK", cddid);
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", cddid);
	} 
	
	funcname = "SHoundGOPMIDFromGiAndGOID";
    	System.out.println(funcname);
	int gi = 21071030;
	goID = 5554;
	try{
            retList = sh.SHoundGOPMIDFromGiAndGOID(gi, goID);
	    assertNotNull(retList);
	    assertTrue(retList.length>0); 
	    assertNull(sh.SHoundGOPMIDFromGiAndGOID(neg1,neg1));
	    writeLog(funcname, "OK", String.valueOf(gi) + " & " + String.valueOf(goID));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi) + " & " + String.valueOf(goID));
	} 
	
	funcname = "SHoundGOECodeFromGiAndGOID";
	System.out.println(funcname);
	String[] ecodeList = sh.SHoundGOECodeFromGiAndGOID(gi, goID);
	try{
	     assertNotNull(ecodeList);
       	     assertTrue(ecodeList.length>0);
	     assertNull(sh.SHoundGOPMIDFromGiAndGOID(neg1,neg1));
	     writeLog(funcname, "OK", String.valueOf(gi) + " & " + String.valueOf(goID) );
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi) + " & " + String.valueOf(goID));
	} 
	
	funcname = "SHoundGiFromGOID";
	System.out.println("SHoundGiFromGOID");
	goID = 8320;
	try{
	    retList = sh.SHoundGiFromGOID(goID);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull(sh.SHoundGiFromGOID(neg1));
	    writeLog(funcname, "OK", String.valueOf(goID));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(goID));
	} 
	
	funcname = "SHoundGiFromGOIDList";
   	System.out.println("SHoundGiFromGOIDList ... takes a while");
	int[] goIDList = {8320, 4009};
	try{
	    retList = sh.SHoundGiFromGOIDList(goIDList);
	    assertNotNull(retList);
	    assertTrue(retList.length >0);
	    retList = sh.SHoundGiFromGOIDList(negList);
	    assertTrue(retList.length == 0);
	    assertNull(sh.SHoundGiFromGOIDList(emIntList));
	    assertNull(sh.SHoundGiFromGOIDList(nullIntList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(goIDList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(goIDList));
	} 
	
	funcname = "SHoundGiFromGOIDListAndECode";
	System.out.println(funcname);
	ecode = "NR";
	try{
	    retList = sh.SHoundGiFromGOIDListAndECode(goIDList, ecode);
	    assertNotNull(retList);
	    assertTrue(retList.length >0);
	    assertNull(sh.SHoundGiFromGOIDListAndECode(emIntList, emptyStr));
	    assertNull(sh.SHoundGiFromGOIDListAndECode(goIDList, nullStr));
	    retList = sh.SHoundGiFromGOIDListAndECode(negList, emptyStr);
	    assertTrue(retList.length == 0);
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(goIDList)+" & "+ecode);
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(goIDList)+" & "+ecode);
	} 
	
	funcname = "SHoundGOIDFromGi";
	System.out.println(funcname);
	gi = 4557225;
	try{
	    retList = sh.SHoundGOIDFromGi(gi);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);	
	    assertNull(sh.SHoundGOIDFromGi(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	} 
	
	funcname = "SHoundGOIDFromGiList";
	System.out.println(funcname);
	gi = 4557225;
	int[] giList = {4501841, 4557225};
	try{
	    retList = sh.SHoundGOIDFromGiList(giList);
	    assertNotNull(retList);
	    assertTrue(retList.length >0);
	    retList = sh.SHoundGOIDFromGiList(negList);
	    assertTrue(retList.length == 0);	
	    assertNull(sh.SHoundGOIDFromGiList(emIntList));
	    assertNull(sh.SHoundGOIDFromGiList(nullIntList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList));
	} 
	    
	funcname = "SHoundGiFromGOIDAndECode";	
	System.out.println(funcname);
	goID = 5554;
	ecode = "ND";
	try{
	    retList = sh.SHoundGiFromGOIDAndECode(goID, ecode);
	    assertNotNull(retList);
	    assertTrue (retList.length >0);
	    assertNull (sh.SHoundGiFromGOIDAndECode(neg1, ecode));
	    assertNull (sh.SHoundGiFromGOIDAndECode(goID, nullStr));
	    assertNull (sh.SHoundGiFromGOIDAndECode(goID, emptyStr));
	    writeLog(funcname, "OK", String.valueOf(goID)+" & "+ecode);
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(goID)+" & " +ecode);
	} 
	
	funcname = "SHoundGiFromLLID";
	System.out.println(funcname);
	int llID = 9;
	try{
	    retList = sh.SHoundGiFromLLID(llID);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull(sh.SHoundGiFromLLID(neg1));
	    writeLog(funcname, "OK", String.valueOf(llID) );
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(llID));
	} 
	
	funcname = "SHoundGiFromLLIDList";
	System.out.println(funcname);
	int[] llIDList = {1, 9, 13};
	try{
	    retList = sh.SHoundGiFromLLIDList(llIDList);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    retList = sh.SHoundGiFromLLIDList(negList);
	    assertTrue(retList.length ==0);
	    assertNull(sh.SHoundGiFromLLIDList(nullIntList));
	    assertNull(sh.SHoundGiFromLLIDList(emIntList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(llIDList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(llIDList));
	} 
	
	funcname = "SHoundLLIDFromGi";
	System.out.println(funcname);
	gi = 42741671;
	try{
	    retList = sh.SHoundLLIDFromGi(gi);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull(sh.SHoundLLIDFromGi(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	} 
	
	funcname = "SHoundLLIDFromGiList";
	System.out.println(funcname);
	int[] giList3 = {42741671, 4501843};
	try{
	    retList = sh.SHoundLLIDFromGiList(giList3);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull(sh.SHoundLLIDFromGiList(nullIntList));
	    assertNull(sh.SHoundLLIDFromGiList(emIntList));
	    retList = sh.SHoundLLIDFromGiList(negList);
	    assertTrue(retList.length == 0);
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList3) );
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList3));
	} 
	
	funcname = "SHoundLocusFromGi";	
	System.out.println(funcname);
	gi = 42741671;
	try{
	    assertNotNull(sh.SHoundLocusFromGi(gi));
	    assertNull(sh.SHoundLocusFromGi(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	} 
   }
   
   	
    public static void testTaxonomyFunctions() throws Exception
    {
	String funcname = "SHoundTaxIDFromGi";
	System.out.println(funcname);
	int gi = 230779;
	try{
	    assertTrue(sh.SHoundTaxIDFromGi(gi)>0);
	    assertEquals(-1, sh.SHoundTaxIDFromGi(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	} 
	
	funcname = "SHoundTaxIDFromGiList";
	System.out.println(funcname);
	int[] giList = {230779, 324, 32};
	int[] retList = sh.SHoundTaxIDFromGiList(giList);
	try{
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    retList = sh.SHoundTaxIDFromGiList(negList);
	    assertTrue(retList.length == 0);
	    assertNull (sh.SHoundTaxIDFromGiList(emIntList));
	    assertNull (sh.SHoundTaxIDFromGiList(nullIntList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList));
	} 
	
	funcname = "SHoundTaxIDFrom3D";
	System.out.println(funcname);
	int mmdbId = 11;
	try{
	    assertTrue(sh.SHoundTaxIDFrom3D(mmdbId) >0);
	    assertEquals(-1, sh.SHoundTaxIDFrom3D(neg1));
	    writeLog(funcname, "OK", String.valueOf(mmdbId));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(mmdbId));
	} 
	
	funcname = "SHoundTaxIDFrom3DList";
	System.out.println(funcname);
	int[] mmdbIdList = {9, 11, 12};
	try{
	    retList = sh.SHoundTaxIDFrom3DList(mmdbIdList);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    retList = sh.SHoundTaxIDFrom3DList(negList);
	    assertTrue(retList.length == 0);
	    assertNull (sh.SHoundTaxIDFrom3DList(emIntList));
	    assertNull (sh.SHoundTaxIDFrom3DList(nullIntList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(mmdbIdList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(mmdbIdList));
	} 
		
	funcname = "SHoundProteinsFromTaxID";	
	System.out.println(funcname);
	int taxID = 56636;
	try{
	    retList = sh.SHoundProteinsFromTaxID(taxID);
	    assertNotNull(retList);
	    assertTrue (retList.length >0);
	    assertNull (sh.SHoundProteinsFromTaxID(neg1));
	    writeLog(funcname, "OK", String.valueOf(taxID));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(taxID));
	} 
	
	funcname = "SHoundProteinsFromTaxIDList";
	System.out.println(funcname);
	int[] taxIDList = {56636, 10665, 82};
	try{
            retList = sh.SHoundProteinsFromTaxIDList(taxIDList);
	    assertNotNull(retList);
	    assertTrue(retList.length >0);
	    retList =  sh.SHoundProteinsFromTaxIDList(negList);
	    assertTrue(retList.length == 0);
	    assertNull (sh.SHoundProteinsFromTaxIDList(emIntList));
	    assertNull (sh.SHoundProteinsFromTaxIDList(nullIntList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(taxIDList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(taxIDList));
	} 
	
	funcname = "SHoundProteinsFromTaxIDIII";
	System.out.println(funcname);
	taxID = 3055;
	try{
	    retList = sh.SHoundProteinsFromTaxIDIII(taxID);
	    assertNotNull(retList);
	    assertTrue (retList.length >0);
	    assertNull(sh.SHoundProteinsFromTaxIDIII(neg1));
	    writeLog(funcname, "OK", String.valueOf(taxID));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(taxID));
	} 
	
	funcname = "SHoundDNAFromTaxID";
	System.out.println(funcname);
	taxID = 56636;
	try{
	    retList = sh.SHoundDNAFromTaxID(taxID);
	    assertNotNull(retList);
	    assertTrue (retList.length >0);
	    assertNull (sh.SHoundDNAFromTaxID(neg1));
            writeLog(funcname, "OK", String.valueOf(taxID));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(taxID));
	} 
	
	funcname = "SHoundDNAFromTaxIDList";
	System.out.println(funcname);
	int[] taxIDList2= {56636, 10665, 82};
	try{
	    retList = sh.SHoundDNAFromTaxIDList(taxIDList2);
	    assertNotNull(retList);
	    assertTrue(retList.length >0);
	    retList =  sh.SHoundDNAFromTaxIDList(negList);
	    assertTrue (retList.length == 0);
	    assertNull (sh.SHoundDNAFromTaxIDList(emIntList));
	    assertNull (sh.SHoundDNAFromTaxIDList(nullIntList));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(taxIDList2));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(taxIDList2));
	} 
	
	funcname = "SHoundDNAFromTaxIDIII";
	System.out.println(funcname);
	taxID = 3055;
	try{
	    retList = sh.SHoundDNAFromTaxIDIII(taxID);
	    assertNotNull(retList);
	    assertTrue(retList.length >0);
	    assertNull(sh.SHoundDNAFromTaxIDIII(neg1));
	    writeLog(funcname, "OK", String.valueOf(taxID));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(taxID));
	} 
	
	funcname = "SHoundProteinsFromOrganism";
	System.out.println(funcname);
	taxID = 9606;
	int chromflags = 255;
	try{
	    retList = sh.SHoundProteinsFromOrganism(taxID, chromflags);
	    assertNotNull(retList);
	    assertTrue (retList.length >0);
	    assertNull (sh.SHoundProteinsFromOrganism( neg1, neg1));
	    writeLog(funcname, "OK", String.valueOf(taxID)+" & "+String.valueOf(chromflags));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(taxID)+" & "+String.valueOf(chromflags));
	} 
	
	funcname = "SHoundProteinsFromOrganismList";
	System.out.println(funcname);
	int[] taxIDList3 = {56636, 9606, 2101};
	try{
	    retList = sh.SHoundProteinsFromOrganismList(taxIDList3, chromflags);
	    assertNotNull(retList);
	    assertTrue(retList.length >0);
	    assertNull (sh.SHoundProteinsFromOrganismList(emIntList, chromflags));
	    assertNull (sh.SHoundProteinsFromOrganismList(nullIntList, chromflags));
	    assertNull(sh.SHoundProteinsFromOrganismList( negList, neg1));
	    retList = sh.SHoundProteinsFromOrganismList( negList, chromflags);
	    assertTrue(retList.length == 0);
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(taxIDList3)+" & "+String.valueOf(chromflags));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(taxIDList3)+" & "+String.valueOf(chromflags));
	} 
	
	funcname = "SHoundProteinsFromChromosome";
	System.out.println(funcname);
	int chromID = 10;
	try{
	    retList = sh.SHoundProteinsFromChromosome(chromID);
	    assertNotNull(retList);
	    assertTrue (retList.length>0);
	    assertNull (sh.SHoundProteinsFromChromosome(neg1));
	    writeLog(funcname, "OK", String.valueOf(chromID));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(chromID));
	} 
	
	funcname = "SHoundProteinsFromChromosomeList";
	System.out.println(funcname);
	int[] chromList = {173, 169, 10};
	try{
	    retList = sh.SHoundProteinsFromChromosomeList(chromList);
	    assertNotNull(retList);
	    assertTrue (retList.length >0);
	    assertNull (sh.SHoundProteinsFromChromosomeList(emIntList));
	    assertNull (sh.SHoundProteinsFromChromosomeList(nullIntList));
	    retList = sh.SHoundProteinsFromChromosomeList( negList);
	    assertTrue(retList.length == 0);
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(chromList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(chromList));
	} 
	
	funcname = "SHoundAllGenomes";
	System.out.println(funcname);
	try{
	    retList = sh.SHoundAllGenomes();
	    assertNotNull(retList);
	    assertTrue (retList.length>0);
	    writeLog(funcname, "OK", "");
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", "");
	} 

	funcname = "SHoundAllGenomesWithScientificNames";
	System.out.println(funcname);
	String[] alist = sh.SHoundAllGenomesWithScientificNames();
	try{
		assertNotNull(alist);
		assertTrue(alist.length >0);
		writeLog(funcname, "OK", "");
	}catch(Error e)
	{
		writeLog(funcname, "FAIL", "");
	}
		
        funcname = "SHoundAllGenomesWithNumProteins";
	System.out.println(funcname);
	try{
		alist = sh.SHoundAllGenomesWithNumProteins();
		assertNotNull(alist);
		assertTrue(alist.length >0);
		writeLog(funcname, "OK", "");
	}catch(Error e)
	{
		writeLog(funcname, "FAIL", "");
	}
	
	funcname = "SHoundChromosomeFromGenome";
	System.out.println(funcname);
	taxID = 9606;
	chromflags = 255;
	try{
	    retList = sh.SHoundChromosomeFromGenome(taxID, chromflags);
	    assertNotNull(retList);
	    assertTrue (retList.length>0);
	    assertNull (sh.SHoundChromosomeFromGenome( neg1, neg1));
	    writeLog(funcname, "OK", String.valueOf(taxID) + " & "+String.valueOf(chromflags));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(taxID) + " & "+String.valueOf(chromflags));
	} 
	
	funcname = "SHoundChromosomeFromGenomeList";
	System.out.println(funcname);
	int[] genomeList= {6239, 155864, 562};
	try{
	    retList = sh.SHoundChromosomeFromGenomeList(genomeList, chromflags);
	    assertNotNull(retList);
	    assertTrue (retList.length >0);
	    assertNull (sh.SHoundChromosomeFromGenomeList(emIntList, chromflags));
	    assertNull (sh.SHoundChromosomeFromGenomeList(nullIntList, chromflags));
	    assertNull (sh.SHoundChromosomeFromGenomeList(negList, neg1));
	    retList = sh.SHoundChromosomeFromGenomeList (negList, chromflags);
	    assertTrue (retList.length == 0);
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(genomeList)+" & "+String.valueOf(chromflags));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(genomeList)+" & "+String.valueOf(chromflags));
	} 
	    
	funcname = "SHoundDNAFromOrganism";	
	System.out.println(funcname);
	taxID = 9606;
	chromflags = 255;
	try{
	    retList = sh.SHoundDNAFromOrganism(taxID, chromflags);
	    assertNotNull(retList);
	    assertTrue (retList.length >0);
	    assertNull (sh.SHoundDNAFromOrganism( neg1, neg1));
	    writeLog(funcname, "OK", String.valueOf(taxID) + " & "+String.valueOf(chromflags));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(taxID) + " & "+String.valueOf(chromflags));
	} 
	
	funcname = "SHoundDNAFromOrganismList";
	System.out.println(funcname);
	int[] orgList = {6239, 56636, 562};
	try{
	    retList = sh.SHoundDNAFromOrganismList(orgList, chromflags);
	    assertNotNull(retList);
	    assertTrue (retList.length >0);
	    assertNull (sh.SHoundDNAFromOrganismList(emIntList, chromflags));
	    assertNull (sh.SHoundDNAFromOrganismList(nullIntList, chromflags));
	    assertNull (sh.SHoundDNAFromOrganismList( negList, neg1));
	    retList = sh.SHoundDNAFromOrganismList (negList, chromflags);
	    assertTrue (retList.length == 0);
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(orgList)+" & "+String.valueOf(chromflags));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(orgList)+" & "+String.valueOf(chromflags));
	} 
	
	funcname = "SHoundDNAFromChromosome";
	System.out.println(funcname);
	chromID = 10;
	try{
	    retList = sh.SHoundDNAFromChromosome(chromID);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull (sh.SHoundDNAFromChromosome(neg1));
	    writeLog(funcname, "OK", String.valueOf(chromID));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(chromID));
	} 
	
	funcname = "SHoundDNAFromChromosomeList";
	System.out.println(funcname);
	int[] chromList2 = {171,173,10};
	try{
	    retList = sh.SHoundDNAFromChromosomeList(chromList2);
	    assertNotNull(retList);
	    assertTrue (retList.length>0);
	    assertNull (sh.SHoundDNAFromChromosomeList(emIntList));
	    assertNull (sh.SHoundDNAFromChromosomeList(nullIntList));
	    retList = sh.SHoundDNAFromChromosomeList (negList);
	    assertTrue (retList.length == 0);
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(chromList2));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(chromList2));
	} 
	
	funcname = "SHoundGetTaxChildNodes";
        System.out.println (funcname);
	taxID = 9606;
	try{
	    retList = sh.SHoundGetTaxChildNodes(taxID);
	    assertNotNull(retList);
	    assertTrue (retList.length>0);
	    assertNull(sh.SHoundGetTaxChildNodes(neg1));
	    writeLog(funcname, "OK", String.valueOf(taxID));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(taxID));
	} 
	
	funcname = "SHoundGetTaxChildNodesList";
	System.out.println (funcname);
	int[] taxIDList4 = {9606, 10665, 10066};
	try{
	    retList = sh.SHoundGetTaxChildNodesList(taxIDList4);
	    assertNotNull(retList);
	    assertTrue (retList.length>0);
	    assertNull(sh.SHoundGetTaxChildNodesList(emIntList));
	    assertNull(sh.SHoundGetTaxChildNodesList(nullIntList));
	    retList = sh.SHoundGetTaxChildNodesList (negList);
	    assertTrue(retList.length == 0);
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(taxIDList4));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(taxIDList4));
	} 
	
	funcname = "SHoundGetAllTaxProgeny";
	System.out.println (funcname);
	taxID = 9606;
	try{
	    retList = sh.SHoundGetAllTaxProgeny(taxID);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull(sh.SHoundGetAllTaxProgeny(neg1));
	    writeLog(funcname, "OK", String.valueOf(taxID));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(taxID));
	} 
	
	funcname = "SHoundGetTaxParent";
	System.out.println (funcname);
	taxID = 56636;
	int retParent = sh.SHoundGetTaxParent(taxID);
	try{
	    assertTrue(retParent >0);
	    retParent = sh.SHoundGetTaxParent(neg1);
	    assertEquals (-1, retParent);
	    writeLog(funcname, "OK", String.valueOf(taxID));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(taxID));
	} 
	
	funcname = "SHoundGetAllTaxAncestors";
	System.out.println (funcname);
	taxID = 56636;
	try{
	    retList = sh.SHoundGetAllTaxAncestors(taxID);
	    assertNotNull(retList);
	    assertTrue (retList.length>0);
	    assertNull(sh.SHoundGetAllTaxAncestors(neg1));
	    writeLog(funcname, "OK", String.valueOf(taxID));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(taxID));
	} 
	
	funcname = "SHoundIsTaxMerged";
	System.out.println (funcname);
	taxID = 56636;
	boolean retVal = sh.SHoundIsTaxMerged(taxID);
	try{
	    assertEquals(boolFalse, retVal);
	    retVal = sh.SHoundIsTaxMerged(neg1);
	    assertEquals (boolFalse, retVal);
	    writeLog(funcname, "OK", String.valueOf(taxID));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(taxID));
	} 
	
	funcname = "SHoundIsTaxDeleted";
	System.out.println (funcname);
	taxID = 56636;
	retVal = sh.SHoundIsTaxDeleted(taxID);
	try{
	    assertEquals(boolFalse, retVal);
	    retVal = sh.SHoundIsTaxDeleted(neg1);
	    assertEquals (boolFalse, retVal);
	    writeLog(funcname, "OK", String.valueOf(taxID));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(taxID));
	} 
	
	funcname = "SHoundGetTaxNameFromTaxIDByClass";
	System.out.println (funcname);
	taxID = 56636;
	String nameClass = "SLRI_taxon_name_name_class_scientific_name";
	String retStr = sh.SHoundGetTaxNameFromTaxIDByClass(taxID, nameClass);
	try{
	    assertNotNull(retStr);
	    assertNull (sh.SHoundGetTaxNameFromTaxIDByClass(neg1, nameClass));
	    assertNull (sh.SHoundGetTaxNameFromTaxIDByClass(taxID, nullStr));
	    assertNull (sh.SHoundGetTaxNameFromTaxIDByClass(taxID, emptyStr));
	    writeLog(funcname, "OK", String.valueOf(taxID)+ " & "+nameClass);
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(taxID)+ " & "+nameClass);
	} 
	
	funcname = "SHoundGetTaxNameFromTaxID";	
	System.out.println (funcname);
	taxID = 56636;
	try{
	    retStr = sh.SHoundGetTaxNameFromTaxID(taxID);
	    assertNotNull(retStr);
	    assertNull (sh.SHoundGetTaxNameFromTaxID(neg1));
	    writeLog(funcname, "OK", String.valueOf(taxID));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(taxID));
	} 

	funcname = "SHoundGetNewTaxIDFromMergedTaxID";	
	System.out.println (funcname);
	taxID = 395;
	try{
	    int newTaxID = sh.SHoundGetNewTaxIDFromMergedTaxID(taxID);
	    assertTrue (newTaxID > -1);
	    assertEquals (-1, sh.SHoundGetNewTaxIDFromMergedTaxID(neg1));
	    writeLog(funcname, "OK", String.valueOf(taxID));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(taxID));
	} 
	 
	
	funcname = "SHoundGetTaxLineageFromTaxID";
	System.out.println (funcname);
	taxID = 56636;
	String type = "SHoundGetTaxLineage_FULL";
	try{	
	    retStr = sh.SHoundGetTaxLineageFromTaxID(taxID, type);
	    assertNotNull(retStr);
 	    assertNull (sh.SHoundGetTaxLineageFromTaxID(neg1, type));
	    assertNull (sh.SHoundGetTaxLineageFromTaxID(taxID, nullStr));
	    assertNull (sh.SHoundGetTaxLineageFromTaxID(taxID, emptyStr));
	    writeLog(funcname, "OK", String.valueOf(taxID));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(taxID));
	} 
    }
		
	
    public static void testRedundantGiFunctions() throws Exception
    {
	String funcname = "SHoundRedundantGroup";	
	System.out.println(funcname);
	int gi = 32 ;
	int[] retList = sh.SHoundRedundantGroup(gi);
	try{
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull (sh.SHoundRedundantGroup(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	} 
	
	funcname = "SHoundRedundantGroupList";
	System.out.println(funcname);
	int[] giList = {32, 4586891};
	try{
	    retList = sh.SHoundRedundantGroupList(giList);
	    assertTrue(retList.length>0);
	    assertNull(sh.SHoundRedundantGroupList (emIntList));
	    assertNull(sh.SHoundRedundantGroupList (nullIntList));
	    retList = sh.SHoundRedundantGroupList(negList);
	    assertTrue (retList.length == 0);
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList));
	} 
	
	funcname = "SHoundRedundantGroupIDFromGi";
	System.out.println(funcname);
	gi = 32 ;
	int retGi = sh.SHoundRedundantGroupIDFromGi(gi);
	try{
	    assertTrue (retGi > -1);
	    assertEquals (-1, sh.SHoundRedundantGroupIDFromGi(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	} 
	
	funcname = "SHoundRedundantGroupIDFromGiList";	
	System.out.println(funcname);
	try{
	    retList = sh.SHoundRedundantGroupIDFromGiList(giList);
	    assertNotNull (retList);
	    assertTrue (retList.length>0);
	    assertNull (sh.SHoundRedundantGroupIDFromGiList(emIntList));
	    assertNull (sh.SHoundRedundantGroupIDFromGiList(nullIntList));
	    retList = sh.SHoundRedundantGroupIDFromGiList(negList);
	    assertTrue(retList.length == 0);
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList));
	} 
	
	funcname = "SHoundRedundantGroupFromID";	
	System.out.println(funcname);
	int gid = 123 ;
	try{
	    retList = sh.SHoundRedundantGroupFromID(gid);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull (sh.SHoundRedundantGroupFromID(neg1));
	    writeLog(funcname, "OK", String.valueOf(gid));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gid));
	} 
	
	funcname = "SHoundRedundantGroupFromIDList";
	System.out.println(funcname);
	int[] gidList = {32, 123, 362477};
	try{
	    retList = sh.SHoundRedundantGroupFromIDList(gidList);
	    assertNotNull (retList);
	    assertTrue(retList.length >0);
	    assertNull (sh.SHoundRedundantGroupFromIDList(emIntList));
	    assertNull (sh.SHoundRedundantGroupFromIDList(nullIntList));
	    retList= sh.SHoundRedundantGroupFromIDList(negList);
	    assertTrue (retList.length == 0);
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(gidList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(gidList));
	} 
	
	funcname = "SHoundFirstOfRedundantGroupFromID";
	System.out.println(funcname);
	gid = 230779;
	try{
	    retGi = sh.SHoundFirstOfRedundantGroupFromID(gid);
	    assertTrue (retGi >0);
	    assertEquals (-1, sh.SHoundFirstOfRedundantGroupFromID(neg1));
	    writeLog(funcname, "OK", String.valueOf(gid));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gid));
	} 
	
	funcname = "SHoundFirstOfRedundantGroupFromIDList";
	System.out.println(funcname);
	try{
	    retList = sh.SHoundFirstOfRedundantGroupFromIDList(gidList);
	    assertNotNull(retList);
	    assertTrue(retList.length >0);
	    assertNull (sh.SHoundFirstOfRedundantGroupFromIDList(emIntList));
	    assertNull (sh.SHoundFirstOfRedundantGroupFromIDList(nullIntList));
	    retList = sh.SHoundFirstOfRedundantGroupFromIDList(negList);
	    assertTrue (retList.length == 0);
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(gidList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(gidList));
	} 
	
	funcname = "SHoundGOIDFromRedundantGi";	
	System.out.println(funcname);
	gi = 4557225;
	try{
	    retList = sh.SHoundGOIDFromRedundantGi(gi);
	    assertNotNull(retList);
	    assertTrue (retList.length >0);	
	    assertNull (sh.SHoundGOIDFromRedundantGi(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	} 
	
	funcname = "SHoundGOIDFromRedundantGiList";
	System.out.println(funcname);
	int[] giList2 = {4557225, 122};
	try{
	    retList = sh.SHoundGOIDFromRedundantGiList(giList2);
	    assertNotNull(retList);
	    assertTrue (retList.length>0);
	    assertNull (sh.SHoundGOIDFromRedundantGiList(emIntList));
	    assertNull (sh.SHoundGOIDFromRedundantGiList(nullIntList));
	    retList= sh.SHoundGOIDFromRedundantGiList(negList);
	    assertTrue(retList.length == 0);
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList2));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList2));
	} 
    }
    
    
    public static void testConservedDomainFunctions() throws Exception
    {
    	String funcname = "SHoundGiFromCDDID";
	System.out.println(funcname);
    	String cddID = "pfam00664";
    	int[] retList = sh.SHoundGiFromCDDID(cddID);
	try{
    	    assertNotNull(retList);
    	    assertTrue (retList.length>0);
	    assertNull(sh.SHoundGiFromCDDID(nullStr));
	    assertNull(sh.SHoundGiFromCDDID(emptyStr));
	    writeLog(funcname, "OK", cddID);
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", cddID);
	} 
	
	funcname = "SHoundGiFromCDDIDList";
	System.out.println(funcname);
	String[] cddIDList = {"pfam00202", "pfam01835"};
	try{
	    retList = sh.SHoundGiFromCDDIDList(cddIDList);
	    assertNotNull(retList);
	    assertTrue (retList.length>0);
	    assertNull(sh.SHoundGiFromCDDIDList(nullList));
	    retList = sh.SHoundGiFromCDDIDList(emList);
	    assertTrue(retList.length == 0);
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(cddIDList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(cddIDList));
	} 
	
	funcname = "SHoundCDDIDFromGi";
	System.out.println(funcname);
	int gi = 4557225;
	String[] retStrList = sh.SHoundCDDIDFromGi(gi);
	try{
	    assertNotNull(retStrList);
	    assertTrue(retStrList.length>0);
	    assertNull (sh.SHoundCDDIDFromGi(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	} 
	
	funcname = "SHoundCDDIDFromGiList";	
	System.out.println(funcname);
	int[] giList = {4501841, 4557225};
	try{
	    retStrList = sh.SHoundCDDIDFromGiList(giList);
	    assertNotNull(retStrList);
	    assertTrue(retStrList.length>0);
	    assertNull (sh.SHoundCDDIDFromGiList(nullIntList));
	    assertNull(sh.SHoundCDDIDFromGiList(emIntList));
	    retStrList = sh.SHoundCDDIDFromGiList(negList);
	    assertTrue (retStrList.length == 0);
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList));
	} 
	
	funcname = "SHoundCDDScoreFromGi";	
	System.out.println(funcname);
	gi = 34870013;
	cddID = "COG0606";
	try{
	    float evalue = sh.SHoundCDDScoreFromGi(gi, cddID);
	    assertTrue(evalue>0);
	    assertTrue(sh.SHoundCDDScoreFromGi(neg1, cddID) <0);
	    assertTrue (sh.SHoundCDDScoreFromGi(gi, nullStr)<0);
	    assertTrue(sh.SHoundCDDScoreFromGi(gi, emptyStr)<0);
	    writeLog(funcname, "OK", String.valueOf(gi)+" & "+cddID);
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi)+" & "+cddID);
	} 
		
	funcname = "SHoundGetGisByNumberOfDomains";
	System.out.println("SHoundGetGisByNumberOfDomains ... takes a while");
	int numOfDomains = 40;
	try{
	    retList = sh.SHoundGetGisByNumberOfDomains(numOfDomains);
	    assertNotNull(retList);
            assertTrue(retList.length>0);
	    assertNull (sh.SHoundGetGisByNumberOfDomains(neg1));
	    writeLog(funcname, "OK", String.valueOf(numOfDomains));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(numOfDomains));
	} 
	
	funcname = "SHoundGetDomainIdFromLabel";	
	System.out.println(funcname);
	String label = "COLFI";
	try{     
	    retStrList = sh.SHoundGetDomainIdFromLabel(label);
	    assertNotNull(retStrList);
	    assertTrue(retStrList.length>0);
	    assertNull(sh.SHoundGetDomainIdFromLabel(nullStr));
	    assertNull(sh.SHoundGetDomainIdFromLabel(emptyStr));
	    writeLog(funcname, "OK", label);
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", label);
	} 
	
	funcname = "SHoundGetGisByDomainId";
	System.out.println(funcname);
	String domainID = "smart00038";
	try{ 
	    retList = sh.SHoundGetGisByDomainId(domainID);
	    assertNotNull(retList);
	    assertTrue (retList.length>0);
	    assertNull(sh.SHoundGetGisByDomainId(nullStr));
	    assertNull(sh.SHoundGetGisByDomainId(emptyStr));
	    writeLog(funcname, "OK", domainID);
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", domainID);
	} 	
		
		
	funcname = "SHoundGetDomainLabelFromDomainId";
	System.out.println(funcname);
	domainID = "smart00038";
	try{
	    retStrList = sh.SHoundGetDomainLabelFromDomainId(domainID);
	    assertNotNull(retStrList);
	    assertNull(sh.SHoundGetDomainLabelFromDomainId(nullStr));
	    assertNull(sh.SHoundGetDomainLabelFromDomainId(emptyStr));
	    writeLog(funcname, "OK", domainID);
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", domainID);
	} 	
	
	funcname = "SHoundGetGisByDomainIdAndEvalue";	
	System.out.println(funcname);
	domainID = "smart00038";
	double evalue = 1.0;
	try{
	    retList = sh.SHoundGetGisByDomainIdAndEvalue(domainID, evalue);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull(sh.SHoundGetGisByDomainIdAndEvalue(nullStr,evalue));
	    assertNull (sh.SHoundGetGisByDomainIdAndEvalue(domainID, (double)neg1));
	    assertNull (sh.SHoundGetGisByDomainIdAndEvalue(emptyStr,evalue));
	    writeLog(funcname, "OK", domainID+" & "+String.valueOf(evalue));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", domainID+" & "+String.valueOf(evalue));
	} 	
	
	funcname = "SHoundGetDomainsFromGi";
	System.out.println(funcname);
	gi = 6322015;
	String retStr = sh.SHoundGetDomainsFromGi(gi);
	try{
	    assertNotNull(retStr);
	    assertNull(sh.SHoundGetDomainsFromGi(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi));
	} 	
	
	funcname = "SHoundGetDomainsFromGiWithEvalue";	
	System.out.println(funcname);
	gi = 6322015;
	evalue = 0.5;
	try{	
	    retStr = sh.SHoundGetDomainsFromGiWithEvalue(gi, evalue);
	    assertNotNull(retStr);
	    assertNull( sh.SHoundGetDomainsFromGiWithEvalue(neg1, evalue));
	    assertNull(sh.SHoundGetDomainsFromGiWithEvalue(gi, (double)neg1));
	    writeLog(funcname, "OK", String.valueOf(gi)+" & "+String.valueOf(evalue));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(gi)+" & "+String.valueOf(evalue));
	} 	
    }
    
    
    public static void testGeneOntologyFunctions() throws Exception
    {
	String funcname = "SHoundGODBGetRecordByReference";
	System.out.println (funcname);
	String refType = "EC";
	int[] retList = sh.SHoundGODBGetRecordByReference(refType);
	try{
	    assertNotNull (retList);
	    assertTrue(retList.length>0);
	    assertNull(sh.SHoundGODBGetRecordByReference(nullStr));
	    assertNull(sh.SHoundGODBGetRecordByReference(emptyStr));
	    writeLog(funcname, "OK", refType);
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", refType);
	} 
	
	funcname = "SHoundGODBGetChildrenOf";
	System.out.println (funcname);
	int goID = 7596;
	try{
	    retList = sh.SHoundGODBGetChildrenOf(goID);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull(sh.SHoundGODBGetChildrenOf(neg1));
	    writeLog(funcname, "OK", String.valueOf(goID));
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", String.valueOf(goID));
	} 
	   
	funcname =  "SHoundGODBGetparentOf";
	System.out.println (funcname);
	goID = 7596;
	try{
	    retList = sh.SHoundGODBGetParentOf(goID);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull( sh.SHoundGODBGetParentOf(neg1));
	    writeLog(funcname, "OK", String.valueOf(goID));
	}catch(Error e)
	{
	      writeLog(funcname, "FAIL", String.valueOf(goID));
	} 
	
	funcname = "SHoundGODBGetAllAncestors";
	System.out.println (funcname);
	goID = 7596;
	try{
	    retList = sh.SHoundGODBGetAllAncestors(goID);
	    assertNotNull(retList);
	    assertTrue(retList.length >0);
	    assertNull(sh.SHoundGODBGetAllAncestors(neg1));
	    writeLog(funcname, "OK", String.valueOf(goID));
	}catch(Error e)
	{
	      writeLog(funcname, "FAIL", String.valueOf(goID));
	} 
	
	funcname = "SHoundGODBGetAllChildren";
	System.out.println (funcname);
	goID = 7596;
	try{
	    retList = sh.SHoundGODBGetAllChildren(goID);
	    assertNotNull(retList);
            assertTrue (retList.length>0);
	    assertNull(sh.SHoundGODBGetAllChildren(neg1));
	    writeLog(funcname, "OK", String.valueOf(goID));
	}catch(Error e)
	{
	      writeLog(funcname, "FAIL", String.valueOf(goID));
	} 
	
	funcname = "SHoundGODBGetNameByID";
	System.out.println (funcname);
	goID = 7596;
	String retStr = sh.SHoundGODBGetNameByID(goID);
	try{
	    assertNotNull(retStr);
	    assertNull (sh.SHoundGODBGetNameByID(neg1)); 
	    writeLog(funcname, "OK", String.valueOf(goID));
	}catch(Error e)
	{
	      writeLog(funcname, "FAIL", String.valueOf(goID));
	} 
	
	funcname = "SHoundGODBGetClassification";	
	System.out.println (funcname);
	goID = 7596;
	try{
	    retStr = sh.SHoundGODBGetClassification(goID);
	    assertNotNull (retStr);
	    assertNull (sh.SHoundGODBGetClassification(neg1)); 
	    writeLog(funcname, "OK", String.valueOf(goID));
	}catch(Error e)
	{
	      writeLog(funcname, "FAIL", String.valueOf(goID));
	} 
    }
  
  
    public static void testOMIMFunctions() throws Exception
    {
	String funcname = "SHoundGiFromOMIM";
	System.out.println(funcname);
	int omimId = 103950;
	int[] retList = sh.SHoundGiFromOMIM(omimId);
	try{
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull(sh.SHoundGiFromOMIM(neg1));
	    writeLog(funcname, "OK", String.valueOf(omimId));
	}catch(Error e)
	{
	      writeLog(funcname, "FAIL", String.valueOf(omimId));
	} 
	 
	funcname = "SHoundGiFromOMIMList";
	System.out.println(funcname);
	int[] omimList = {137150,103950,108345};
	try{
	    retList = sh.SHoundGiFromOMIMList(omimList);
	    assertNotNull(retList);
	    assertTrue(retList.length==4);
	    assertNull(sh.SHoundGiFromOMIMList(nullIntList));
	    assertNull( sh.SHoundGiFromOMIMList(emIntList));
	    retList = sh.SHoundGiFromOMIMList(negList);
	    assertTrue(retList.length ==0);
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(omimList));
	}catch(Error e)
	{
	      writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(omimList));
	} 
		
	funcname = "SHoundOMIMFromGi";
	System.out.println(funcname);
	int gi = 4505355;
	try{
	    retList = sh.SHoundOMIMFromGi(gi);
	    assertNotNull(retList);
	    assertTrue (retList.length>0);
	    assertNull(sh.SHoundOMIMFromGi(neg1));
	    writeLog(funcname, "OK", String.valueOf(gi));
	}catch(Error e)
	{
	      writeLog(funcname, "FAIL", String.valueOf(gi));
	} 
	
	funcname = "SHoundOMIMFromGiList";	
	System.out.println(funcname);
	int[] giList3 = {4505355, 4557225, 4501841};
	try{
	    retList = sh.SHoundOMIMFromGiList(giList3);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull(sh.SHoundOMIMFromGiList(nullIntList));
	    assertNull(sh.SHoundOMIMFromGiList(emIntList));
	    retList = sh.SHoundOMIMFromGiList(negList);
	    assertTrue(retList.length ==0);
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList3));
	}catch(Error e)
	{
	      writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList3));
	} 
    }
	
	
    public static void testNeighboursFunctions() throws Exception
    {
		
	String funcname = "SHoundNeighboursFromGi";
	System.out.println(funcname);
	int gi = 230779;
	double evalue = 1.0;
	boolean limit = false;
	int[] retList = sh.SHoundNeighboursFromGi(gi, evalue, limit);
	try{
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull (sh.SHoundNeighboursFromGi(neg1, evalue, limit));
	    assertNull (sh.SHoundNeighboursFromGi(gi, (double)neg1, limit));
	    writeLog(funcname, "OK", String.valueOf(gi)+" & "+String.valueOf(evalue)+" & "+"false");
	}catch(Error e)
	{
	      writeLog(funcname, "FAIL", String.valueOf(gi)+" & "+String.valueOf(evalue)+" & "+"false");
	} 
	
	funcname = "SHoundNeighboursFromGiList";
	System.out.println(funcname);
	int[] giList = {230779, 4557225, 4501843};
	try{
	    retList = sh.SHoundNeighboursFromGi(gi, evalue, limit);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull (sh.SHoundNeighboursFromGiList(nullIntList, evalue, limit));
	    assertNull (sh.SHoundNeighboursFromGiList(emIntList, evalue, limit));
	    assertNull (sh.SHoundNeighboursFromGiList(giList,(double)neg1, limit));
	    retList = sh.SHoundNeighboursFromGiList(negList, evalue, limit);
	    assertTrue(retList.length == 0);
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList));
	}catch(Error e)
	{
	      writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList));
	} 
		
	funcname = "SHoundNeighboursFromTaxID";	
	System.out.println(funcname);
	int taxID = 107806;
	try{
	    retList = sh.SHoundNeighboursFromTaxID(taxID, evalue);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull (sh.SHoundNeighboursFromTaxID(neg1, evalue));
	    assertNull (sh.SHoundNeighboursFromTaxID(taxID, (double)neg1));
	    writeLog(funcname, "OK", String.valueOf(taxID)+" & "+String.valueOf(evalue));
	}catch(Error e)
	{
	      writeLog(funcname, "FAIL", String.valueOf(taxID)+" & "+String.valueOf(evalue));
	} 
	
	funcname = "SHound3DNeighboursFromGi";	
	System.out.println(funcname);
	gi = 230779;
	try{
	    retList = sh.SHound3DNeighboursFromGi(gi, evalue);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull(sh.SHound3DNeighboursFromGi(neg1, evalue));
	    assertNull(sh.SHound3DNeighboursFromGi(gi, (double)neg1));
	    writeLog(funcname, "OK", String.valueOf(gi)+" & "+String.valueOf(evalue));
	}catch(Error e)
	{
	      writeLog(funcname, "FAIL", String.valueOf(gi)+" & "+String.valueOf(evalue));
	} 
	
	funcname = "SHound3DNeighboursFromTaxID";	
	System.out.println("SHound3DNeighboursFromTaxID ... takes a while");	
	taxID = 107806;
	try{
	    retList = sh.SHound3DNeighboursFromTaxID(taxID, evalue);
	    assertNotNull(retList);
	    assertTrue(retList.length>0);
	    assertNull(sh.SHound3DNeighboursFromTaxID(neg1, evalue));
	    assertNull(sh.SHound3DNeighboursFromGi(taxID, (double)neg1));
	    writeLog(funcname, "OK", String.valueOf(taxID)+" & "+String.valueOf(evalue));
	}catch(Error e)
	{
	      writeLog(funcname, "FAIL", String.valueOf(taxID)+" & "+String.valueOf(evalue));
	} 
    }


    public static void testKeyedListFunctions() throws Exception
    {
	String funcname = "SHoundGetFastaKeyedList";
	System.out.println(funcname);
	int[] giList = {33,44,55};
	Hashtable h = sh.SHoundGetFastaKeyedList(giList);
	try {
	     Object result = h.get("44");
	     assertNotNull (result);
	     assertNull(sh.SHoundGetFastaKeyedList(nullIntList));
	     assertNull(sh.SHoundGetFastaKeyedList(emIntList));
	     h = sh.SHoundGetFastaKeyedList(negList);
	     assertEquals("NULL", h.get("-1"));
	     writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList));
	}catch(Error e)
	{
	      writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList));
	} 
	
	funcname = "SHoundRedundantGroupKeyedList";
	System.out.println(funcname);
	int[] giList2 = {6382060,666,4557225};
	try{
	    h = sh.SHoundRedundantGroupKeyedList(giList2);
	    Object result2 = h.get("4557225");
	    assertNotNull (result2);
	    assertNull(sh.SHoundRedundantGroupKeyedList(nullIntList));
	    assertNull(sh.SHoundRedundantGroupKeyedList(emIntList));
	    h = sh.SHoundRedundantGroupKeyedList(negList);
	    assertEquals("NULL", h.get("-1"));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(giList2));
	}catch(Error e)
	{
	      writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(giList2));
	} 
		
	funcname = "SHoundGetFastaFromRedundantGroupIDKeyedList";
	System.out.println(funcname);
	int[] gidList = {362477,123,111};
	try{
	    h = sh.SHoundGetFastaFromRedundantGroupIDKeyedList(gidList);
	    Object result3 = h.get("123");
	    assertNotNull (result3);
	    assertNull(sh.SHoundGetFastaFromRedundantGroupIDKeyedList(nullIntList));
	    assertNull(sh.SHoundGetFastaFromRedundantGroupIDKeyedList(emIntList));
	    h = sh.SHoundGetFastaFromRedundantGroupIDKeyedList(negList);
	    assertEquals("NULL", h.get("-1"));
	    writeLog(funcname, "OK", SLRIMisc.SaveArrAsString(gidList));
	}catch(Error e)
	{
	      writeLog(funcname, "FAIL", SLRIMisc.SaveArrAsString(gidList));
	} 
    }
	
    /*We do not release jseqrem on release 3.0, exclude this test*/
    /*public static void testJseqremFunctions() throws Exception
    {
	String funcname = "SHoundGOAnnotFromRedundantGi";
	System.out.println(funcname);
	int gi = 4557225;
	ArrayList alist = sh.SHoundGOAnnotFromRedundantGi(gi);
	try{
	    assertTrue(alist.size()>0);
	    assertNull(sh.SHoundGOAnnotFromRedundantGi(neg1));
	    writeLog(funcname, "OK");
	}catch(Error e)
	{
	      writeLog(funcname, "FAIL");
	} 
   }*/
   
    public static void testFiniFunctions() throws Exception
    {
	String funcname = "SHoundFini";
	System.out.println(funcname);
	try{
	    assertEquals(boolTrue, sh.SHoundFini()); 
	    writeLog(funcname, "OK", "");
	}catch(Error e)
	{
	    writeLog(funcname, "FAIL", "");
	}
    }
   
   public static void main (String[] args) throws Exception
   {
       junit.textui.TestRunner.run(SeqHoundTest.class);
	   System.out.println("Please see java_test.log for details.");
    
   }
} 

