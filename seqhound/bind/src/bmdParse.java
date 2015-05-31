/*
 * Created on 26-May-2004
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

import org.apache.xerces.parsers.SAXParser;
import org.xml.sax.Attributes;
import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.SAXException;


import java.io.*;
import java.io.IOException;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Stack;

public class bmdParse extends DefaultHandler {
	
	Stack context = new Stack();
	int InteractionCount = 0;
	
	bmdRecord bmd = new bmdRecord();  //object to store the contents of an interaction/complex/pathway record before processing it into the db or index
	bmdRecordProcessor bmdp = new bmdRecordProcessor();  //object to process the contents of a bmdRecord, either into a db (for bind_module) or into flat files (for bind_index)
	
	public bmdParse (String XMLFileListFile) {
		
		SAXParser parser = new SAXParser();
		
		parser.setContentHandler (this);
		
		File inputList = new File(XMLFileListFile);
		BufferedReader in;
		try {
			in = new BufferedReader(new FileReader(inputList));

			String currentXMLFile = null;
        
        	while ((currentXMLFile = in.readLine())!=null) {
        		context = new Stack();
        		System.out.println("Now Processing BIND XML file " + currentXMLFile);
	        //  Parse the Document
				parser.parse(currentXMLFile);
        	}
        
		} catch (FileNotFoundException e1) {
			System.err.println("Fatal Error: Input File not found.");
			e1.printStackTrace();
			System.exit(1);
		} catch (SAXException e) {
				System.err.println("Failed to parse XML file");
	            System.err.println (e);
	    } catch (IOException e) {
	        	System.err.println("Failed on io with XML file");
	            System.err.println (e);
	        
		}
		

    }
	

	
    //  Start Element Event Handler
    public void startElement (String uri, String local,
        String qName, Attributes atts)  {
        //System.out.println (local);
    	context.push(local);
    	//System.out.println((String) context.peek());
    	if (local.compareTo("BIND-Interaction") == 0) {
    		String contextSummary = concatStringStack();
    		if (contextSummary.compareTo("<BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
    			bmd.bmd_source.type = new String("interaction");
    			bmd.bmd_source.db = new String("BIND");
    		}
    		else {
    			System.err.println("Unknown context for element BIND-Interaction: " + contextSummary);
    		}
    		
    		InteractionCount++;
    	}
    	else if (local.compareTo("BIND-Molecular-Complex") == 0) {
    		String contextSummary = concatStringStack();
    		if (contextSummary.compareTo("<BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>")==0) {
    			bmd.bmd_source.type = new String("complex");
    			bmd.bmd_source.db = new String("BIND");
    		}
    		else {
    			System.err.println("Unknown context for element BIND-Molecular-Complex: " + contextSummary);
    		}
    	}
    	else if (local.compareTo("BIND-Pathway") == 0) {
    		String contextSummary = concatStringStack();
    		if (contextSummary.compareTo("<BIND-Pathway><BIND-Pathway-set_pathways><BIND-Pathway-set><BIND-Submit_pathways><BIND-Submit>")==0) {
    			bmd.bmd_source.type = new String("pathway");
    			bmd.bmd_source.db = new String("BIND");
    		}
    		else {
    			System.err.println("Unknown context for element BIND-Pathway: " + contextSummary);
    		}
    	}
    	else if (local.compareTo("BIND-object-type-id_not-specified")==0) {
       		String contextSummary = concatStringStack();
    		if (contextSummary.compareTo("<BIND-object-type-id_not-specified><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
    			bmd.bmd_objects__B.type = new String("not specified");
    			System.out.println("type for object B: " + bmd.bmd_objects__B.type);
    		}
    		else if (contextSummary.compareTo("<BIND-object-type-id_not-specified><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
    			bmd.bmd_objects__A.type = new String("not specified");
    			System.out.println("type for object A: " + bmd.bmd_objects__A.type);
    		}
    		else {
    			System.err.println("Unhandled context for element BIND-object-type-id_not-specified: " + contextSummary);
    		}
    	}
    	else if (local.compareTo("BIND-small-molecule-id_other-db")==0) {
    		String contextSummary = concatStringStack();
    		if (contextSummary.compareTo("<BIND-small-molecule-id_other-db><BIND-small-molecule-id><BIND-object-type-id_small-molecule><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
    			bmd.bmd_objects__B.type = "small_molecule";
    			System.out.println("Type for object B: " + bmd.bmd_objects__B.type);
    		}
    		else if (contextSummary.compareTo("<BIND-small-molecule-id_other-db><BIND-small-molecule-id><BIND-object-type-id_small-molecule><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
    			bmd.bmd_objects__A.type = "small_molecule";
    			System.out.println("Type for object A: " + bmd.bmd_objects__A.type);
    		}
    		else { 
    			System.err.println("Unhandled Context for element BIND-small-molecule-id_other-db: " + contextSummary );
       		}
    	}
    	else if (local.compareTo("BIND-condition")==0) {
    		if (bmd.bmd_ref.isPopulated()) {
    			bmd.bmd_refs.add(bmd.bmd_ref);
    			bmd.bmd_ref = new bmd_refs_rec();
    		}
    	}
    	else if (local.compareTo("BIND-mol-object_sub-unit")==0) {
    		String contextSummary = concatStringStack();
    		if (contextSummary.compareTo("<BIND-mol-object_sub-unit><BIND-mol-object><BIND-Molecular-Complex_sub-units><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>")==0) {
    			if (bmd.bmd_complex2subunits.bmd_object_temp == null) {
    				bmd.bmd_complex2subunits.bmd_object_temp = new bmd_objects_rec();
    				bmd.bmd_complex2subunits.bmd_object_temp.label = new StringBuffer("");
    			}
    			else {
    				System.err.println("Error:new BIND-mol-object_sub-unit element reached without temporary holder being empty.  Erasing temporary holder.");
    			}
    		}
    		else {
    			System.err.println("Unhandled Context for element BIND-mol-olbject_sub-unit: " + concatStringStack());
    		}
    	}
    	
    }
    
    public void characters(char[] ch, int start, int length) {

    	String currentElement = (String) context.peek();
    	//System.out.println(currentElement);
    	if (currentElement.compareTo("Geninfo-id")==0) {
    		String contextSummary = concatStringStack();
    		if (contextSummary.compareTo(new String("<Geninfo-id><BIND-id_gi><BIND-id><BIND-object-type-id_protein><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")) == 0) {
    			if (bmd.bmd_objects__A.id == null) {
    			    bmd.bmd_objects__A.id = new StringBuffer();
    			}
    			bmd.bmd_objects__A.id.append(new String(ch,start, length ));
    			bmd.bmd_objects__A.type = new String("protein");
    			bmd.bmd_objects__A.db = new StringBuffer("GenBank");
    			System.out.println("type for object A: " + bmd.bmd_objects__A.type);
    			System.out.println("DB for object A: " + bmd.bmd_objects__A.db);
    			System.out.println("ID for object A: " + bmd.bmd_objects__A.id);
    		}
    		else  if (contextSummary.compareTo("<Geninfo-id><BIND-id_gi><BIND-id><BIND-object-type-id_protein><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>") == 0) {
    		    if (bmd.bmd_objects__B.id == null) {
    			    bmd.bmd_objects__B.id = new StringBuffer();
    			}
    			bmd.bmd_objects__B.id.append(new String(ch,start, length ));
    			bmd.bmd_objects__B.type = new String("protein");
    			bmd.bmd_objects__B.db = new StringBuffer("GenBank");
    			System.out.println("type for object B: " + bmd.bmd_objects__B.type);
    			System.out.println("DB for object B: " + bmd.bmd_objects__B.db);
    			System.out.println("ID for object B: " + bmd.bmd_objects__B.id);
    			
    		}
    		else  if (contextSummary.compareTo("<Geninfo-id><BIND-id_gi><BIND-id><BIND-object-type-id_dna><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>") == 0) {
    		    if (bmd.bmd_objects__A.id == null) {
    			    bmd.bmd_objects__A.id = new StringBuffer();
    			}
    			bmd.bmd_objects__A.id.append(new String(ch,start, length ));
    			bmd.bmd_objects__A.type = new String("DNA");
    			bmd.bmd_objects__A.db = new StringBuffer("GenBank");
    			System.out.println("type for object A: " + bmd.bmd_objects__A.type);
    			System.out.println("DB for object A: " + bmd.bmd_objects__A.db);
    			System.out.println("ID for object A: " + bmd.bmd_objects__A.id);
    		}
    		else  if (contextSummary.compareTo("<Geninfo-id><BIND-id_gi><BIND-id><BIND-object-type-id_dna><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>") == 0) {
    		    if (bmd.bmd_objects__B.id == null) {
    			    bmd.bmd_objects__B.id = new StringBuffer();
    			}
    			bmd.bmd_objects__B.id.append(new String(ch,start, length ));
    			bmd.bmd_objects__B.type = new String("DNA");
    			bmd.bmd_objects__B.db = new StringBuffer("GenBank");
    			System.out.println("type for object B: " + bmd.bmd_objects__B.type);
    			System.out.println("DB for object B: " + bmd.bmd_objects__B.db);
    			System.out.println("ID for object B: " + bmd.bmd_objects__B.id);
    		}  
    		else  if (contextSummary.compareTo("<Geninfo-id><BIND-id_gi><BIND-id><BIND-object-type-id_rna><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>") == 0) {
    		    if (bmd.bmd_objects__A.id == null) {
    			    bmd.bmd_objects__A.id = new StringBuffer();
    			}
    			bmd.bmd_objects__A.id.append(new String(ch,start, length ));
    			bmd.bmd_objects__A.type = new String("RNA");
    			bmd.bmd_objects__A.db = new StringBuffer("GenBank");
    			System.out.println("type for object A: " + bmd.bmd_objects__A.type);
    			System.out.println("DB for object A: " + bmd.bmd_objects__A.db);
    			System.out.println("ID for object A: " + bmd.bmd_objects__A.id);
    		}
    		else  if (contextSummary.compareTo("<Geninfo-id><BIND-id_gi><BIND-id><BIND-object-type-id_rna><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>") == 0) {
    		    if (bmd.bmd_objects__B.id == null) {
    			    bmd.bmd_objects__B.id = new StringBuffer();
    			}
    			bmd.bmd_objects__B.id.append(new String(ch,start, length ));
    			bmd.bmd_objects__B.type = new String("RNA");
    			bmd.bmd_objects__B.db = new StringBuffer("GenBank");
    			System.out.println("type for object B: " + bmd.bmd_objects__B.type);
    			System.out.println("DB for object B: " + bmd.bmd_objects__B.db);
    			System.out.println("ID for object B: " + bmd.bmd_objects__B.id);
    		}
    		else  if (contextSummary.compareTo("<Geninfo-id><BIND-id_gi><BIND-id><BIND-object-type-id_gene><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>") == 0) {
    		    if (bmd.bmd_objects__A.id == null) {
    			    bmd.bmd_objects__A.id = new StringBuffer();
    			}
    			bmd.bmd_objects__A.id.append(new String(ch,start, length ));
    			bmd.bmd_objects__A.type = new String("gene");
    			bmd.bmd_objects__A.db = new StringBuffer("GenBank");
    			System.out.println("type for object A: " + bmd.bmd_objects__A.type);
    			System.out.println("DB for object A: " + bmd.bmd_objects__A.db);
    			System.out.println("ID for object A: " + bmd.bmd_objects__A.id);
    		}
    		else  if (contextSummary.compareTo("<Geninfo-id><BIND-id_gi><BIND-id><BIND-object-type-id_gene><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>") == 0) {
    		    if (bmd.bmd_objects__B.id == null) {
    			    bmd.bmd_objects__B.id = new StringBuffer();
    			}
    			bmd.bmd_objects__B.id.append(new String(ch,start, length ));
    			bmd.bmd_objects__B.type = new String("gene");
    			bmd.bmd_objects__B.db = new StringBuffer("GenBank");
    			System.out.println("type for object B: " + bmd.bmd_objects__B.type);
    			System.out.println("DB for object B: " + bmd.bmd_objects__B.db);
    			System.out.println("ID for object B: " + bmd.bmd_objects__B.id);
    		}
    		else  if (contextSummary.compareTo(new String("<Geninfo-id><BIND-id_gi><BIND-id><BIND-object-type-id_protein><BIND-object-type-id><BIND-object_id><BIND-object><BIND-mol-object_sub-unit><BIND-mol-object><BIND-Molecular-Complex_sub-units><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>")) == 0) {
    		    if (bmd.bmd_complex2subunits.bmd_object_temp.id == null) {
    			    bmd.bmd_complex2subunits.bmd_object_temp.id = new StringBuffer();
    			}
    			bmd.bmd_complex2subunits.bmd_object_temp.id.append(new String(ch,start, length ));
    			bmd.bmd_complex2subunits.bmd_object_temp.type = new String("protein");
    			bmd.bmd_complex2subunits.bmd_object_temp.db = new StringBuffer("GenBank");
    			System.out.println("type for subunit of complex: " + bmd.bmd_complex2subunits.bmd_object_temp.type);
    			System.out.println("DB for subunit of complex: " + bmd.bmd_complex2subunits.bmd_object_temp.db);
    			System.out.println("ID for subunit of complex: " + bmd.bmd_complex2subunits.bmd_object_temp.id);
    		}
    		else  if (contextSummary.compareTo(new String("<Geninfo-id><BIND-id_gi><BIND-id><BIND-object-type-id_dna><BIND-object-type-id><BIND-object_id><BIND-object><BIND-mol-object_sub-unit><BIND-mol-object><BIND-Molecular-Complex_sub-units><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>")) == 0) {
    		    if (bmd.bmd_complex2subunits.bmd_object_temp.id == null) {
    			    bmd.bmd_complex2subunits.bmd_object_temp.id = new StringBuffer();
    			}
    			bmd.bmd_complex2subunits.bmd_object_temp.id.append(new String(ch,start, length ));
    			bmd.bmd_complex2subunits.bmd_object_temp.type = new String("dna");
    			bmd.bmd_complex2subunits.bmd_object_temp.db = new StringBuffer("GenBank");
    			System.out.println("type for subunit of complex: " + bmd.bmd_complex2subunits.bmd_object_temp.type);
    			System.out.println("DB for subunit of complex: " + bmd.bmd_complex2subunits.bmd_object_temp.db);
    			System.out.println("ID for subunit of complex: " + bmd.bmd_complex2subunits.bmd_object_temp.id);
    		}
    		else  if (contextSummary.compareTo(new String("<Geninfo-id><BIND-id_gi><BIND-id><BIND-object-type-id_rna><BIND-object-type-id><BIND-object_id><BIND-object><BIND-mol-object_sub-unit><BIND-mol-object><BIND-Molecular-Complex_sub-units><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>")) == 0) {
    		    if (bmd.bmd_complex2subunits.bmd_object_temp.id == null) {
    			    bmd.bmd_complex2subunits.bmd_object_temp.id = new StringBuffer();
    			}
    			bmd.bmd_complex2subunits.bmd_object_temp.id.append(new String(ch,start, length ));
    			bmd.bmd_complex2subunits.bmd_object_temp.type = new String("rna");
    			bmd.bmd_complex2subunits.bmd_object_temp.db = new StringBuffer("GenBank");
    			System.out.println("type for subunit of complex: " + bmd.bmd_complex2subunits.bmd_object_temp.type);
    			System.out.println("DB for subunit of complex: " + bmd.bmd_complex2subunits.bmd_object_temp.db);
    			System.out.println("ID for subunit of complex: " + bmd.bmd_complex2subunits.bmd_object_temp.id);
    		}
    		else  if (contextSummary.compareTo(new String("<Geninfo-id><BIND-id_gi><BIND-id><BIND-object-type-id_gene><BIND-object-type-id><BIND-object_id><BIND-object><BIND-mol-object_sub-unit><BIND-mol-object><BIND-Molecular-Complex_sub-units><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>")) == 0) {
    		    if (bmd.bmd_complex2subunits.bmd_object_temp.id == null) {
    			    bmd.bmd_complex2subunits.bmd_object_temp.id = new StringBuffer();
    			}
    			bmd.bmd_complex2subunits.bmd_object_temp.id.append(new String(ch,start, length ));
    			bmd.bmd_complex2subunits.bmd_object_temp.type = new String("gene");
    			bmd.bmd_complex2subunits.bmd_object_temp.db = new StringBuffer("GenBank");
    			System.out.println("type for subunit of complex: " + bmd.bmd_complex2subunits.bmd_object_temp.type);
    			System.out.println("DB for subunit of complex: " + bmd.bmd_complex2subunits.bmd_object_temp.db);
    			System.out.println("ID for subunit of complex: " + bmd.bmd_complex2subunits.bmd_object_temp.id);
    		}
    		else { 
    			System.err.println("Unhandled Context for element Geninfo-id: " + concatStringStack());
    			
    		}	
    	}
    	// Handle <Interaction-id> element, which could be the id of a BIND-Interaction, or a member of a complex or pathway
    	else if (currentElement.compareTo("Interaction-id")==0) {
    		String contextSummary = concatStringStack();
    		if (contextSummary.compareTo("<Interaction-id><BIND-Interaction_iid><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>") == 0) {
    			if (bmd.bmd_source.id == null) {
    			    bmd.bmd_source.id = new StringBuffer();
    			}
    			bmd.bmd_source.id.append(new String(ch,start,length));
    			System.out.println("Id of interaction: " + bmd.bmd_source.id);
    		}
    		else if (contextSummary.compareTo("<Interaction-id><BIND-Molecular-Complex_interaction-list><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>") == 0) {
    			Integer id = new Integer(new String(ch,start,length));
    			if (bmd.bmd_complex2ints.int_ids == null) {
    				bmd.bmd_complex2ints.int_ids = new ArrayList();
    			}
    			bmd.bmd_complex2ints.int_ids.add(bmd.bmd_complex2ints.int_ids.size(),id);
    			System.out.println("Id of interaction in complex: " + bmd.bmd_complex2ints.int_ids.get(bmd.bmd_complex2ints.int_ids.size()-1));
    		}
    		else if (contextSummary.compareTo("<Interaction-id><BIND-Pathway_pathway><BIND-Pathway><BIND-Pathway-set_pathways><BIND-Pathway-set><BIND-Submit_pathways><BIND-Submit>") == 0) {
    			Integer id = new Integer(new String(ch,start,length));
    			if (bmd.bmd_complex2ints.int_ids == null) {
    				bmd.bmd_complex2ints.int_ids = new ArrayList();
    				bmd.bmd_complex2ints.int_ids.add(0,id);
    			}
    			else {
    				bmd.bmd_complex2ints.int_ids.add(bmd.bmd_complex2ints.int_ids.size(),id);
    			}
    			System.out.println("Id of interaction in pathway: " + bmd.bmd_complex2ints.int_ids.get(bmd.bmd_complex2ints.int_ids.size()-1));
    		}
    		else {
    			System.err.println("Unhandled Context for element Interaction-id: " + concatStringStack() + ":" + new String(ch,start,length));
       		}
    	}
    	// Handle <Object-id_id> element, which could be taxon id origin form a BIND-object
    	else if (currentElement.compareTo(tag_Object__id_id) == 0) {
    		String contextSummary = concatStringStack();
    		if (contextSummary.compareTo("<Object-id_id><Object-id><Dbtag_tag><Dbtag><Org-ref_db><Org-ref><BioSource_org><BioSource><BIND-object-origin_org><BIND-object-origin><BIND-object_origin><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>") == 0) {
    			Integer tax = new Integer(new String(ch,start,length));
    			bmd.bmd_objects__A.tax = tax;
    			System.out.println("Taxid of object A: " + bmd.bmd_objects__A.tax);
    		}
    		else if (contextSummary.compareTo("<Object-id_id><Object-id><Dbtag_tag><Dbtag><Org-ref_db><Org-ref><BioSource_org><BioSource><BIND-object-origin_org><BIND-object-origin><BIND-object_origin><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>") == 0) {
    			Integer tax = new Integer(new String(ch,start,length));
    			bmd.bmd_objects__B.tax = tax;
    			System.out.println("Taxid of object B: " + bmd.bmd_objects__B.tax);
    		}
    		else if (contextSummary.compareTo("<Object-id_id><Object-id><Dbtag_tag><Dbtag><Org-ref_db><Org-ref><BioSource_org><BioSource><BIND-object-origin_org><BIND-object-origin><BIND-object_origin><BIND-object><BIND-mol-object_sub-unit><BIND-mol-object><BIND-Molecular-Complex_sub-units><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>") == 0) {
    			Integer tax = new Integer(new String(ch,start,length));
    			bmd.bmd_complex2subunits.bmd_object_temp.tax = tax;
    			System.out.println("Taxid of molecular complex subunit: " + bmd.bmd_complex2subunits.bmd_object_temp.tax);
    		}
    		else {
    			System.err.println("Unhandled Context for element Object-id_id: " + concatStringStack());
    		}
    		
    	}
    	//Handle BIND-descr_simple-descr element, which could represent the descr field for an interaction
    	else if (currentElement.compareTo("BIND-descr_simple-descr")==0){ //tag_BIND__descr_simple__descr) == 0) {
    		String contextSummary = concatStringStack();
    		//System.out.println(contextSummary);
    		if (contextSummary.compareTo("<BIND-descr_simple-descr><BIND-descr><BIND-Interaction_descr><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
    			if (bmd.bmd_source.descr == null) {
    				bmd.bmd_source.descr = new StringBuffer("");
    			}
    			bmd.bmd_source.descr.append(new String(ch,start,length));
    			System.out.println("interaction simple description:" + bmd.bmd_source.descr.toString());
    		}
    		else { 
    			System.err.println("Unhandled Context for BIND-descr_simple-descr: " + contextSummary);
    		}
    	}
    	else if (currentElement.compareTo("BIND-Molecular-Complex_descr")==0) {
    		String contextSummary = concatStringStack();
    		if (contextSummary.compareTo("<BIND-Molecular-Complex_descr><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>")==0) {
    			if (bmd.bmd_source.descr == null) {
    				bmd.bmd_source.descr = new StringBuffer("");
    			}
    			bmd.bmd_source.descr.append(new String(ch,start,length));
    			System.out.println("Molecular Complex simple description: " + bmd.bmd_source.descr.toString());
    		}
    		else { 
    			System.err.println("Unhandled Context for BIND-Molecular-Complex_descr: " + contextSummary + ":" + new String(ch,start,length));
    		}
    	}
    	else if (currentElement.compareTo("BIND-path-descr_descr")==0) {
    		String contextSummary = concatStringStack();
    		if (contextSummary.compareTo("<BIND-path-descr_descr><BIND-path-descr><BIND-Pathway_descr><BIND-Pathway><BIND-Pathway-set_pathways><BIND-Pathway-set><BIND-Submit_pathways><BIND-Submit>")==0) {
    			if (bmd.bmd_source.descr == null) {
    				bmd.bmd_source.descr = new StringBuffer("");
    			}
    			bmd.bmd_source.descr.append(new String(ch,start,length));
    			System.out.println("Molecular Pathway simple description: " + bmd.bmd_source.descr.toString());
    		}
    		else { 
    			System.err.println("Unhandled Context for BIND-path-descr_descr: " + contextSummary + ":" + new String(ch,start,length));
    		}
    	}
    	else if (currentElement.compareTo("BIND-object_short-label")==0) {
    		String contextSummary = concatStringStack();
    		//System.out.println(contextSummary);
    		if (contextSummary.compareTo("<BIND-object_short-label><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0 ) {
    			if (bmd.bmd_objects__A.label == null) {
    				bmd.bmd_objects__A.label = new StringBuffer("");
    			}
    			bmd.bmd_objects__A.label.append(new String(ch,start,length));
    			System.out.println("short label for object A: " + bmd.bmd_objects__A.label.toString());
    		}
    		else if (contextSummary.compareTo("<BIND-object_short-label><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0 ) {
    			if (bmd.bmd_objects__B.label == null) {
    				bmd.bmd_objects__B.label = new StringBuffer("");
    			}
    			bmd.bmd_objects__B.label.append(new String(ch,start,length));
    			System.out.println("short label for object B: " + bmd.bmd_objects__B.label.toString());
    		}
    		else if (contextSummary.compareTo("<BIND-object_short-label><BIND-object><BIND-mol-object_sub-unit><BIND-mol-object><BIND-Molecular-Complex_sub-units><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>")==0 ) {
    			if (bmd.bmd_complex2subunits.bmd_object_temp == null) {
    				bmd.bmd_complex2subunits.bmd_object_temp = new bmd_objects_rec();
    				bmd.bmd_complex2subunits.bmd_object_temp.label = new StringBuffer("");
    			}
    			System.out.println(new String(ch,start,length));
    			bmd.bmd_complex2subunits.bmd_object_temp.label.append(new String(ch,start,length));
    			System.out.println("short label for molecular complex subunit: " + bmd.bmd_complex2subunits.bmd_object_temp.label.toString());
    		}
    		else {
    			System.err.println("Unhandled context for element BIND-object_short-label: " + contextSummary + ":" + new String(ch,start,length));
    		}
    	}
    	else if (currentElement.compareTo("Molecular-Complex-id")==0) {
    		String contextSummary = concatStringStack();
    		//System.out.println(contextSummary);
    		if (contextSummary.compareTo("<Molecular-Complex-id><BIND-Molecular-Complex_mcid><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>")==0 ) {
    		    if (bmd.bmd_source.id == null) {
    			    bmd.bmd_source.id = new StringBuffer();
    			}
    			bmd.bmd_source.id.append(new String(ch,start,length));
    			System.out.println("Id of complex: " + bmd.bmd_source.id);
    		}
    		else if (contextSummary.compareTo("<Molecular-Complex-id><BIND-object-type-id_complex><BIND-object-type-id><BIND-object_id><BIND-object><BIND-mol-object_sub-unit><BIND-mol-object><BIND-Molecular-Complex_sub-units><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>")==0 ) {
    			//handle complex subunit of complex
    		}
    		else if (contextSummary.compareTo("<Molecular-Complex-id><BIND-object-type-id_complex><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0 ) {
    		    if (bmd.bmd_objects__A.id == null) {
    			    bmd.bmd_objects__A.id = new StringBuffer();
    			}
    			bmd.bmd_objects__A.id.append(new String(ch,start, length ));
    			bmd.bmd_objects__A.type = new String("complex");
    			bmd.bmd_objects__A.db = new StringBuffer("BIND");
    			System.out.println("type for object A: " + bmd.bmd_objects__A.type);
    			System.out.println("DB for object A: " + bmd.bmd_objects__A.db);
    			System.out.println("ID for object A: " + bmd.bmd_objects__A.id);
    		}
    		else if (contextSummary.compareTo("<Molecular-Complex-id><BIND-object-type-id_complex><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0 ) {
    		    if (bmd.bmd_objects__B.id == null) {
    			    bmd.bmd_objects__B.id = new StringBuffer();
    			}
    			bmd.bmd_objects__B.id.append(new String(ch,start, length ));
    			bmd.bmd_objects__B.type = new String("complex");
    			bmd.bmd_objects__B.db = new StringBuffer("BIND");
    			System.out.println("type for object B: " + bmd.bmd_objects__B.type);
    			System.out.println("DB for object B: " + bmd.bmd_objects__B.db);
    			System.out.println("ID for object B: " + bmd.bmd_objects__B.id);
    		}
    		else {
    			System.err.println("Unhandled Context for element Molecular-Complex-id: " + concatStringStack());
       		}
    	}
    	else if (currentElement.compareTo("Pathway-id")==0) {
    		String contextSummary = concatStringStack();
    		//System.out.println(contextSummary);
    		if (contextSummary.compareTo("<Pathway-id><BIND-Pathway_pid><BIND-Pathway><BIND-Pathway-set_pathways><BIND-Pathway-set><BIND-Submit_pathways><BIND-Submit>")==0) {
    		    if (bmd.bmd_source.id == null) {
    			    bmd.bmd_source.id = new StringBuffer();
    			}
    			bmd.bmd_source.id.append(new String(ch,start,length));
    			System.out.println("Id of Pathway: " + bmd.bmd_source.id);
    		}
    		else { 
    			System.err.println("Unhandled Context for element Pathway-id: " + concatStringStack() + ": " + new String(ch,start,length));
       		}
    	}
    	else if (currentElement.compareTo("Internal-small-molecule-id")==0) {
    		String contextSummary = concatStringStack();
    		System.out.println(contextSummary);
    		if (contextSummary.compareTo("<Internal-small-molecule-id><BIND-small-molecule-id_internal><BIND-small-molecule-id><BIND-object-type-id_small-molecule><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
    		    if (bmd.bmd_objects__A.id == null) {
    			    bmd.bmd_objects__A.id = new StringBuffer();
    			}
    			bmd.bmd_objects__A.id.append(new String(ch,start, length ));
    			bmd.bmd_objects__A.type = new String("small_molecule");
    			bmd.bmd_objects__A.db = new StringBuffer("BIND");  //Should this be blueprint?
    			System.out.println("type for object A: " + bmd.bmd_objects__A.type);
    			System.out.println("DB for object A: " + bmd.bmd_objects__A.db);
    			System.out.println("ID for object A: " + bmd.bmd_objects__A.id);
    		}
    		else if (contextSummary.compareTo("<Internal-small-molecule-id><BIND-small-molecule-id_internal><BIND-small-molecule-id><BIND-object-type-id_small-molecule><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
    		    if (bmd.bmd_objects__B.id == null) {
    			    bmd.bmd_objects__B.id = new StringBuffer();
    			}
    			bmd.bmd_objects__B.id.append(new String(ch,start, length ));
    			bmd.bmd_objects__B.type = new String("small_molecule");
    			bmd.bmd_objects__B.db = new StringBuffer("BIND");  //Should this be blueprint?
    			System.out.println("type for object B: " + bmd.bmd_objects__B.type);
    			System.out.println("DB for object B: " + bmd.bmd_objects__B.db);
    			System.out.println("ID for object B: " + bmd.bmd_objects__B.id);
    		}
    		else { 
    			System.err.println("Unhandled Context for element Internal-small-molecule-id: " + concatStringStack() + ": " + new String(ch,start,length));
       		}
    	}
    	else if (currentElement.compareTo("BIND-other-db_dbname")==0) {
    		String contextSummary = concatStringStack();
    		if (contextSummary.compareTo("<BIND-other-db_dbname><BIND-other-db><BIND-small-molecule-id_other-db><BIND-small-molecule-id><BIND-object-type-id_small-molecule><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
    			if (bmd.bmd_objects__B.db == null) {
    				bmd.bmd_objects__B.db = new StringBuffer();
    			}
    			bmd.bmd_objects__B.db.append(ch,start,length);
    			System.out.println("DB for object B: " + bmd.bmd_objects__B.db);
    		}
    		else if (contextSummary.compareTo("<BIND-other-db_dbname><BIND-other-db><BIND-small-molecule-id_other-db><BIND-small-molecule-id><BIND-object-type-id_small-molecule><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
    			if (bmd.bmd_objects__A.db == null) {
    				bmd.bmd_objects__A.db = new StringBuffer();
    			}
    			bmd.bmd_objects__A.db.append(ch,start,length);
    			System.out.println("DB for object A: " + bmd.bmd_objects__A.db);
    		}
    		else if (contextSummary.compareTo("<BIND-other-db_dbname><BIND-other-db><BIND-small-molecule-id_other-db><BIND-small-molecule-id><BIND-object-type-id_small-molecule><BIND-object-type-id><BIND-object_id><BIND-object><BIND-mol-object_sub-unit><BIND-mol-object><BIND-Molecular-Complex_sub-units><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>")==0) {
    			if (bmd.bmd_complex2subunits.bmd_object_temp.db == null) {
    				bmd.bmd_complex2subunits.bmd_object_temp.db = new StringBuffer();
    			}
    			bmd.bmd_complex2subunits.bmd_object_temp.db.append(ch,start,length);
    			System.out.println("DB for molecular complex subunit small molecule: " + bmd.bmd_complex2subunits.bmd_object_temp.db);
    		}
    		else { 
    			System.err.println("Unhandled Context for element BIND-other-db_dbname: " + concatStringStack() + ": " + new String(ch,start,length));
       		}
    	}
    	else if (currentElement.compareTo("BIND-other-db_intp")==0) {
    		String contextSummary = concatStringStack();
    		if (contextSummary.compareTo("<BIND-other-db_intp><BIND-other-db><BIND-small-molecule-id_other-db><BIND-small-molecule-id><BIND-object-type-id_small-molecule><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
    		    if (bmd.bmd_objects__B.id == null) {
    			    bmd.bmd_objects__B.id = new StringBuffer();
    			}
    			bmd.bmd_objects__B.id.append(new String(ch,start, length ));
    			System.out.println("ID for object B: " + bmd.bmd_objects__B.id);
    		}
    		else if (contextSummary.compareTo("<BIND-other-db_intp><BIND-other-db><BIND-small-molecule-id_other-db><BIND-small-molecule-id><BIND-object-type-id_small-molecule><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
    		    if (bmd.bmd_objects__A.id == null) {
    			    bmd.bmd_objects__A.id = new StringBuffer();
    			}
    			bmd.bmd_objects__A.id.append(new String(ch,start, length ));
    			System.out.println("ID for object A: " + bmd.bmd_objects__A.id);
    		}
//    		else if (contextSummary.compareTo("<BIND-other-db_intp><BIND-other-db><BIND-object_extref><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
//    			bmd.bmd_objects__B.id = new Integer(new String(ch,start,length));
//    			System.out.println("ID for object B: " + bmd.bmd_objects__B.id);
//    		}
    		//TODO these next two may be erroneous as well, not a reference to the id, I'll have to check with the bind boys about this BIND-object_extref element, and it's semantics.
//    		else if (contextSummary.compareTo("<BIND-other-db_intp><BIND-other-db><BIND-object_extref><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
//    			bmd.bmd_objects__A.id = new Integer(new String(ch,start,length));
//    			System.out.println("ID for object A: " + bmd.bmd_objects__A.id);
//    		}
//    		else if (contextSummary.compareTo("<BIND-other-db_intp><BIND-other-db><BIND-object_extref><BIND-object><BIND-mol-object_sub-unit><BIND-mol-object><BIND-Molecular-Complex_sub-units><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>")==0) {
//    			bmd.bmd_complex2subunits.bmd_object_temp.id = new Integer(new String(ch,start,length));
//    			System.out.println("ID for molecular complex subunit: " + bmd.bmd_complex2subunits.bmd_object_temp.id);
//    		}  
    		else if (contextSummary.compareTo("<BIND-other-db_intp><BIND-other-db><BIND-small-molecule-id_other-db><BIND-small-molecule-id><BIND-object-type-id_small-molecule><BIND-object-type-id><BIND-object_id><BIND-object><BIND-mol-object_sub-unit><BIND-mol-object><BIND-Molecular-Complex_sub-units><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>")==0) {
    		    if (bmd.bmd_complex2subunits.bmd_object_temp.id == null) {
    			    bmd.bmd_complex2subunits.bmd_object_temp.id = new StringBuffer();
    			}
    			bmd.bmd_complex2subunits.bmd_object_temp.id.append(new String(ch,start, length ));
    			System.out.println("ID for molecular complex subunit small molecule: " + bmd.bmd_complex2subunits.bmd_object_temp.id);
    		}
    		else { 
    			System.err.println("Unhandled Context for element BIND-other-db_intp: " + concatStringStack() + ": " + new String(ch,start,length));
       		}
    	}
    	else if (currentElement.compareTo("BIND-other-db_strp")==0) {
    		String contextSummary = concatStringStack();
    		if (contextSummary.compareTo("<BIND-other-db_strp><BIND-other-db><BIND-small-molecule-id_other-db><BIND-small-molecule-id><BIND-object-type-id_small-molecule><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
    			bmd.bmd_objects__B.acc = new String(ch,start,length);
    			System.out.println("Accession for object B: " + bmd.bmd_objects__B.acc);
    		}
    		else if (contextSummary.compareTo("<BIND-other-db_strp><BIND-other-db><BIND-small-molecule-id_other-db><BIND-small-molecule-id><BIND-object-type-id_small-molecule><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
    			bmd.bmd_objects__A.acc = new String(ch,start,length);
    			System.out.println("Accession for object A: " + bmd.bmd_objects__A.acc);
    		}
//    		else if (contextSummary.compareTo("<BIND-other-db_strp><BIND-other-db><BIND-object_extref><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
//    			bmd.bmd_objects__A.acc = new String(ch,start,length);
//    			System.out.println("Accession for object A: " + bmd.bmd_objects__A.acc);
//    		}
    		//TODO these next ones may be errorneous; the bind-object_extref does not really refer to the object primary ID.
//    		else if (contextSummary.compareTo("<BIND-other-db_strp><BIND-other-db><BIND-object_extref><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
//    			bmd.bmd_objects__B.acc = new String(ch,start,length);
//    			System.out.println("Accession for object B: " + bmd.bmd_objects__B.acc);
//    		}
//    		else if (contextSummary.compareTo("<BIND-other-db_strp><BIND-other-db><BIND-object_extref><BIND-object><BIND-mol-object_sub-unit><BIND-mol-object><BIND-Molecular-Complex_sub-units><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>")==0) {
//    			bmd.bmd_complex2subunits.bmd_object_temp.acc = new String(ch,start,length);
//    			System.out.println("Accession for molecular complex subunit: " + bmd.bmd_complex2subunits.bmd_object_temp.acc);
//    		}  
    		else if (contextSummary.compareTo("<BIND-other-db_strp><BIND-other-db><BIND-small-molecule-id_other-db><BIND-small-molecule-id><BIND-object-type-id_small-molecule><BIND-object-type-id><BIND-object_id><BIND-object><BIND-mol-object_sub-unit><BIND-mol-object><BIND-Molecular-Complex_sub-units><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>")==0) {
    			bmd.bmd_complex2subunits.bmd_object_temp.acc = new String(ch,start,length);
    			System.out.println("Accession for molecular complex subunit small molecule: " + bmd.bmd_complex2subunits.bmd_object_temp.acc);
    		}
    		else { 
    			System.err.println("Unhandled Context for element BIND-other-db_strp: " + concatStringStack() + ": " + new String(ch,start,length));
       		}
    	}
    	else if (currentElement.compareTo("BIND-experimental-system")==0) {
    		String contextSummary = concatStringStack();
    		if (contextSummary.compareTo("<BIND-experimental-system><BIND-condition_system><BIND-condition><BIND-condition-set_conditions><BIND-condition-set><BIND-descr_cond><BIND-descr><BIND-Interaction_descr><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0 ) {
    			int expVal = (new Integer(new String(ch,start,length))).intValue();
    			if (bmd.bmd_ref.isMethodSet()) {
    				bmd.bmd_refs.add(bmd.bmd_ref);
    				bmd.bmd_ref = new bmd_refs_rec();
    			}
    			bmd.bmd_ref.setMethod(expVal);
    			System.out.println("Experimental system name value: " + bmd.bmd_ref.method);
    		}
    		else {
    			System.err.println("Unhandled context for element BIND-experimental-system: " + contextSummary + ":" + new String(ch,start,length));
    		}
    	}
    	else if (currentElement.compareTo("Pub_muid")==0) {
    		String contextSummary = concatStringStack();
    		//System.out.println(contextSummary);
    		if (contextSummary.compareTo("<Pub_muid><Pub><BIND-pub-object_pub><BIND-pub-object><BIND-pub-set_pubs><BIND-pub-set><BIND-condition_source><BIND-condition><BIND-condition-set_conditions><BIND-condition-set><BIND-descr_cond><BIND-descr><BIND-Interaction_descr><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
//    			save old record, and copy experimental system method over to new record
    			if (bmd.bmd_ref.isIdSet() | bmd.bmd_ref.isAccSet()) {  
    				String expsys_temp = null;
    				if (bmd.bmd_ref.isMethodSet()) {
    					expsys_temp = bmd.bmd_ref.method;
    				}
    				bmd.bmd_refs.add(bmd.bmd_ref);
    				bmd.bmd_ref = new bmd_refs_rec();
    				if (expsys_temp != null) {
    					bmd.bmd_ref.method = expsys_temp;
    				}
    			}
    			bmd.bmd_ref.id = new Integer(new String(ch,start,length));
    			bmd.bmd_ref.db = "Medline";
    			System.out.println("Interaction condition Medline ID: " + new String(ch,start,length));
        	}
    		else if (contextSummary.compareTo("<Pub_muid><Pub><BIND-pub-object_pub><BIND-pub-object><BIND-pub-set_pubs><BIND-pub-set><BIND-action_source><BIND-action><BIND-action-set_actions><BIND-action-set><BIND-descr_action><BIND-descr><BIND-Interaction_descr><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
    			//save old record, and copy experimental system method over to new record
    			if (bmd.bmd_ref.isPopulated()) {  
    				bmd.bmd_refs.add(bmd.bmd_ref);
    				bmd.bmd_ref = new bmd_refs_rec();
    			}
    			bmd.bmd_ref.id = new Integer(new String(ch,start,length));
    			bmd.bmd_ref.db = "Medline";
    			System.out.println("Pathway source Medline ID: " + new String(ch,start,length));
    		}
    		else if (contextSummary.compareTo("<Pub_muid><Pub><BIND-pub-object_pub><BIND-pub-object><BIND-pub-set_pubs><BIND-pub-set><BIND-Molecular-Complex_source><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>")==0) {
    			//save old record, and copy experimental system method over to new record
    			if (bmd.bmd_ref.isPopulated()) {  
    				bmd.bmd_refs.add(bmd.bmd_ref);
    				bmd.bmd_ref = new bmd_refs_rec();
    			}
    			bmd.bmd_ref.id = new Integer(new String(ch,start,length));
    			bmd.bmd_ref.db = "Medline";
    			System.out.println("Pathway source Medline ID: " + new String(ch,start,length));
    		}
    		else if (contextSummary.compareTo("<Pub_muid><Pub><BIND-pub-object_pub><BIND-pub-object><BIND-pub-set_pubs><BIND-pub-set><BIND-Pathway_source><BIND-Pathway><BIND-Pathway-set_pathways><BIND-Pathway-set><BIND-Submit_pathways><BIND-Submit>")==0) {
//    			//save old record, and copy experimental system method over to new record
    			if (bmd.bmd_ref.isPopulated()) {  
    				bmd.bmd_refs.add(bmd.bmd_ref);
    				bmd.bmd_ref = new bmd_refs_rec();
    			}
    			bmd.bmd_ref.id = new Integer(new String(ch,start,length));
    			bmd.bmd_ref.db = "Medline";
    			System.out.println("Pathway source Medline ID: " + new String(ch,start,length));
        	}
    		else {
    			System.err.println("Unhandled context for element Pub_muid: " + contextSummary + ":" + new String(ch,start,length));
    		}
    		
    	}
    	else if (currentElement.compareTo("PubMedId")==0) {
    		String contextSummary = concatStringStack();
    		//System.out.println(contextSummary + ":" + new String(ch, start,length));
    		if (contextSummary.compareTo("<PubMedId><Pub_pmid><Pub><BIND-pub-object_pub><BIND-pub-object><BIND-pub-set_pubs><BIND-pub-set><BIND-condition_source><BIND-condition><BIND-condition-set_conditions><BIND-condition-set><BIND-descr_cond><BIND-descr><BIND-Interaction_descr><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
//    			save old record, and copy experimental system method over to new record
    			if (bmd.bmd_ref.isIdSet() | bmd.bmd_ref.isAccSet()) {  
    				String expsys_temp = null;
    				if (bmd.bmd_ref.isMethodSet()) {
    					expsys_temp = bmd.bmd_ref.method;
    				}
    				bmd.bmd_refs.add(bmd.bmd_ref);
    				bmd.bmd_ref = new bmd_refs_rec();
    				if (expsys_temp != null) {
    					bmd.bmd_ref.method = expsys_temp;
    				}
    			}
    			bmd.bmd_ref.id = new Integer(new String(ch,start,length));
    			bmd.bmd_ref.db = "PubMed";
    			System.out.println("Interaction condition PubMed ID: " + new String(ch,start,length));
        	}
    		else if (contextSummary.compareTo("<PubMedId><Pub_pmid><Pub><BIND-pub-object_pub><BIND-pub-object><BIND-pub-set_pubs><BIND-pub-set><BIND-Interaction_source><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")==0) {
    			//save old record, and copy experimental system method over to new record
    			if (bmd.bmd_ref.isPopulated()) {  
    				bmd.bmd_refs.add(bmd.bmd_ref);
    				bmd.bmd_ref = new bmd_refs_rec();
    			}
    			bmd.bmd_ref.id = new Integer(new String(ch,start,length));
    			bmd.bmd_ref.db = "PubMed";
    			System.out.println("Pathway source PubMed ID: " + new String(ch,start,length));
    		}
    		else if (contextSummary.compareTo("<PubMedId><Pub_pmid><Pub><BIND-pub-object_pub><BIND-pub-object><BIND-pub-set_pubs><BIND-pub-set><BIND-Molecular-Complex_source><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>")==0) {
    			//save old record, and copy experimental system method over to new record
    			if (bmd.bmd_ref.isPopulated()) {  
    				bmd.bmd_refs.add(bmd.bmd_ref);
    				bmd.bmd_ref = new bmd_refs_rec();
    			}
    			bmd.bmd_ref.id = new Integer(new String(ch,start,length));
    			bmd.bmd_ref.db = "PubMed";
    			System.out.println("Pathway source PubMed ID: " + new String(ch,start,length));
    		}
    		else if (contextSummary.compareTo("<PubMedId><Pub_pmid><Pub><BIND-pub-object_pub><BIND-pub-object><BIND-pub-set_pubs><BIND-pub-set><BIND-Pathway_source><BIND-Pathway><BIND-Pathway-set_pathways><BIND-Pathway-set><BIND-Submit_pathways><BIND-Submit>")==0) {
//    			//save old record, and copy experimental system method over to new record
    			if (bmd.bmd_ref.isPopulated()) {  
    				bmd.bmd_refs.add(bmd.bmd_ref);
    				bmd.bmd_ref = new bmd_refs_rec();
    			}
    			bmd.bmd_ref.id = new Integer(new String(ch,start,length));
    			bmd.bmd_ref.db = "PubMed";
    			System.out.println("Pathway source PubMed ID: " + new String(ch,start,length));
        	}
    		else {
    			System.err.println("Unhandled context for element PubMedId: " + contextSummary + ":" + new String(ch,start,length));
    		}
    	}
    }
  
    public void endElement(String uri, String localName, 
    	String qName) {
    	if (  localName.compareTo("Interaction-id")==0 
    		| localName.compareTo("Geninfo-id")==0) {
    		System.out.println();
    	}
    	else if (localName.compareTo("BIND-Interaction")==0) {
    		if (!bmd.validateRecord()) {
    			System.err.println("Invalid Interaction Record");
    		}
    		else {
    			if (!processBMDRecord(bmd)) {
    				System.err.println("Failed to add record to DB");
    			}
    		}
    		bmd = new bmdRecord();
    	}
    	else if (localName.compareTo("BIND-Molecular-Complex") == 0) {
    		if (!bmd.validateRecord()) {
    			System.err.println("Invalid Complex Record");
    		}
    		else {
    			if (!processBMDRecord(bmd)) {
    				System.err.println("Failed to add record to DB");
    			}
    		}
    		bmd = new bmdRecord();
    	}
    	else if (localName.compareTo("BIND-Pathway") == 0) {
    		if (!bmd.validateRecord()) {
    			System.err.println("Invalid Pathway Record");
    		}
    		else {
    			if (!processBMDRecord(bmd)) {
    				System.err.println("Failed to add record to DB");
    			}
    		}
    		bmd = new bmdRecord();
    	}
    	else if (localName.compareTo("BIND-condition")==0) {
    		if (bmd.bmd_ref.isPopulated()) {
    			bmd.bmd_refs.add(bmd.bmd_ref);
    			//bmd.bmd_ref = new bmd_refs_rec();  Don't add one, cuz My db layer checks if this is null or not and adds it to bmd_refs Array list if it isn't 
    		}
    	}
    	else if (localName.compareTo("BIND-mol-object_sub-unit")==0) {
    		String contextSummary = concatStringStack();
    		if (contextSummary.compareTo("<BIND-mol-object_sub-unit><BIND-mol-object><BIND-Molecular-Complex_sub-units><BIND-Molecular-Complex><BIND-Complex-set_complexes><BIND-Complex-set><BIND-Submit_complexes><BIND-Submit>")==0) {
    			//Add the subunit to the subunit list and erase the temp subunit object
    			if (bmd.bmd_complex2subunits.bmd_object_temp != null) {
    				bmd.bmd_complex2subunits.subunitObjects.add(bmd.bmd_complex2subunits.bmd_object_temp);
    				bmd.bmd_complex2subunits.bmd_object_temp = null;
    				bmd.bmd_complexes.numsubunits++;  //Increment subunit count for this complex
    			}
    		}
    	}
    	context.pop();
    }
    
    public void endDocument() {
    	//postProcessBMDRecords();
    	System.out.println(context.size());
    	System.out.println(InteractionCount);

    	
    }
    
    
    /*This method will store the given record in 
     the database
      */
    public boolean processBMDRecord(bmdRecord bmd) {
    	if (bmdp.processRecord(bmd)) {
    		return true;
    	}
    	else {
    		System.err.println("processBMDRecord:Error: Failed to process bmd Record");
    		return false;
    	}
    }
    
    public boolean postProcessBMDRecords() {
    	if (bmdp.postProcess()) {
    		return true;
    	}
    	else {
    		System.err.println("postProcessBMDRecords:  an error occured during post processing of the bmd records by the processor class");
    		return false;
    	}
    }
    
    // Main Method
    public static void main (String[] args) {
    	
        bmdParse bmdP = new bmdParse(args[0]);
        if (!bmdP.postProcessBMDRecords()) {
    		System.err.println("Error during postprocessing of bind records.");
    	}
        
        try {
			bmdP.bmdp.conn.close();
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        bmdComputeRedundancies bmdcr = new bmdComputeRedundancies();
        bmdcr.computeRedundancies();
    }
    

    
    private String getContextOfGeninfo__id() {
    	String element = null;
    	Stack contextClone = (Stack) context.clone();
    	element = (String) contextClone.pop();
    	String contextString = concatStringStack();
    	if (contextString.compareTo(new String("<Geninfo-id><BIND-id_gi><BIND-id><BIND-object-type-id_protein><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")) == 0 ) {
    		return new String("protein_ObjectID_ObjA");
    	}
    	else if (contextString.compareTo(new String("<Geninfo-id><BIND-id_gi><BIND-id><BIND-object-type-id_protein><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")) == 0 ) {
    		return new String("protein_ObjectID_ObjB");
    	} 
    	else if (contextString.compareTo(new String("<Geninfo-id><BIND-id_gi><BIND-id><BIND-object-type-id_dna><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")) == 0 ) {
    		return new String("dna_ObjectID_ObjA");
    	}
    	else if (contextString.compareTo(new String("<Geninfo-id><BIND-id_gi><BIND-id><BIND-object-type-id_dna><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")) == 0 ) {
    		return new String("dna_ObjectID_ObjB");
    	}
    	else if (contextString.compareTo(new String("<Geninfo-id><BIND-id_gi><BIND-id><BIND-object-type-id_rna><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")) == 0 ) {
    		return new String("rna_ObjectID_ObjA");
    	}
    	else if (contextString.compareTo(new String("<Geninfo-id><BIND-id_gi><BIND-id><BIND-object-type-id_rna><BIND-object-type-id><BIND-object_id><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")) == 0 ) {
    		return new String("rna_ObjectID_ObjB");
    	}
    	else {
    		return new String("Unknown");
    	}
    }

    public String getContextOfInteraction__id() {
    	String contextString = concatStringStack();
    	//System.out.println(contextString);
    	if (contextString.compareTo(new String("<Interaction-id><BIND-Interaction_iid><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")) == 0 ) {
    		return  new String("id_interaction");
    	}
    	else {
    		return new String("Unknown");
    	}

    }
    
    String tag_Object__id_id = new String("Object-id_id");
    
    public String getContextOfObject__id_id() {
    	String contextString = concatStringStack();
    	if (contextString.compareTo(new String("<Object-id_id><Object-id><Dbtag_tag><Dbtag><Org-ref_db><Org-ref><BioSource_org><BioSource><BIND-object-origin_org><BIND-object-origin><BIND-object_origin><BIND-object><BIND-Interaction_a><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")) == 0) {
    		return new String("id_taxon_origin_bind_object_ObjA");
    	}
    	else if (contextString.compareTo(new String("<Object-id_id><Object-id><Dbtag_tag><Dbtag><Org-ref_db><Org-ref><BioSource_org><BioSource><BIND-object-origin_org><BIND-object-origin><BIND-object_origin><BIND-object><BIND-Interaction_b><BIND-Interaction><BIND-Interaction-set_interactions><BIND-Interaction-set><BIND-Submit_interactions><BIND-Submit>")) == 0) {
    		return new String("id_taxon_origin_bind_object_ObjB");
    	}
    	else {
    		return new String("Unknown");
    	}
    }
    
    private void printXMLContextStack() {
    	String element = null;
    	Stack contextClone = (Stack) context.clone();
    	
    	while(!contextClone.empty()) {
    		element = (String) contextClone.pop();
    		System.err.println(element);
    	}
    	
    }
    
    private String concatStringStack(){
    	String element = null;
    	StringBuffer concat = new StringBuffer("");
    	Stack contextClone = (Stack) context.clone();

    	while(!contextClone.empty()) {
    		element = (String) contextClone.pop();
    		concat.append("<" + element + ">");
    	}
    	return concat.toString();
    	
    }	
}


