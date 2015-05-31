import java.util.ArrayList;
import java.util.Iterator;

/*
 * Created on 26-May-2004
 *
 * TODO To change the template for this generated file go to
 * Window - Preferences - Java - Code Generation - Code and Comments
 */
/**
 * @author mmatan
 * 
 * Objects of this class will store the contents of a 
 * seqhound bind_module, or bmd, record.  It will act as 
 * a storage object until the contents can be inserted 
 * into the relational database.
 * 
 * TODO To change the template for this generated type comment go to
 * Window - Preferences - Java - Code Generation - Code and Comments
 */



public class bmdRecord {
	String bmd_source__uid = null;
	//String BIND__Interaction_a = null;
	bmd_source_rec bmd_source = null;
	bmd_ints_rec bmd_ints = null;
	bmd_objects_rec bmd_objects__A = null;
	bmd_objects_rec bmd_objects__B = null;
	bmd_refs_rec bmd_ref = null;  //stores an individual reference while it is being 
	ArrayList bmd_refs = null;  //stores all the references for a record.
	bmd_complex2ints_rec bmd_complex2ints = null;
	bmd_complex2subunits_rec bmd_complex2subunits = null;
	bmd_complexes_rec bmd_complexes = null;
	
	
	public bmdRecord() {
		bmd_source = new bmd_source_rec();
		bmd_ints = new bmd_ints_rec();
		bmd_objects__A = new bmd_objects_rec();
		bmd_objects__B = new bmd_objects_rec();
		bmd_ref = new bmd_refs_rec();
		bmd_complex2ints = new bmd_complex2ints_rec();
		bmd_complex2subunits = new bmd_complex2subunits_rec();
		bmd_refs = new ArrayList();
		bmd_complexes = new bmd_complexes_rec();
	}
	/* This method will determine whether 
	 * this record has all it's requisite
	 * fields filled, and may also do some
	 * further validation.  Returns true if 
	 * the record is valid and ready to be 
	 * transcribed into the database, false
	 * otherwise.
	 */
	public boolean validateRecord() {
		boolean isValid = true;
		if (bmd_source.id == null) {
			System.err.println("validateRecord:record has no interaction/complex/pathway id");
			isValid = false;
		}
		
	
		if (bmd_source.type ==null) {
			System.err.println("validateRecord:record does not a type (interaction,complex or pathway)");
			isValid = false;
			return isValid;  //maybe I should just set it to some unknown value?
			
		}
		
		if (bmd_source.type.compareTo("interaction")==0) {
			
			if (bmd_refs.size() >0) {
				Iterator bmd_refsI = bmd_refs.iterator();
				boolean hasExperimentalSystem = false;
				while (bmd_refsI.hasNext()) {
					bmd_refs_rec ref = (bmd_refs_rec) bmd_refsI.next();
					if (ref.method!=null) {
						hasExperimentalSystem = true;
						break;
					}
				}
				if (hasExperimentalSystem = false) {
					System.err.println("validateRecord:record does not an experimental method");
					isValid = false;
				}
			}
			else {
				System.err.println("validateRecord:record does not an experimental method");
				isValid = false;
			}
		
			if (bmd_source.descr == null) {
				System.err.println("validateRecord: Warning: record does not have a description");
				//isValid = false;
			}
		
			if (bmd_objects__A.label == null) {
				System.err.println("validateRecord:object A does not have a label");
				isValid = false;
			}
		
			if (bmd_objects__B.label == null) {
				System.err.println("validateRecord:object B does not have a label");
				isValid = false;
			}
			
			if (bmd_objects__A.type == null) {
				System.err.println("validateRecord:object A does not have a type");
				isValid = false;
				return false;
			}
		
			if (bmd_objects__B.type == null) {
				System.err.println("validateRecord:object B does not have a type");
				isValid = false;
				return false;
			}	
			
			if (bmd_objects__A.type.compareTo("not specified")!=0 ) {
				if (bmd_objects__A.id == null) {
					System.err.println("validateRecord:object A does not have an id");
					isValid = false;
				}
				
				if (bmd_objects__A.db == null) {
					System.err.println("validateRecord:object A does not have a db");
					isValid = false;
				}
			}
			
			if (bmd_objects__B.type.compareTo("not specified")!=0 ) {
				if (bmd_objects__B.id == null) {
					System.err.println("validateRecord:object B does not have an id");
					isValid = false;
				}	
		
				if (bmd_objects__B.db == null) {
					System.err.println("validateRecord:object B does not have a db");
					isValid = false;
				}
			}
			if (bmd_objects__A.type.compareTo("small_molecule")!=0 & bmd_objects__A.type.compareTo("not specified")!=0) {
				if (bmd_objects__A.tax == null) {
					System.err.println("validateRecord: Warning object A does not have a tax id");
					//isValid = false;
				}
			}
		
			if (bmd_objects__B.type.compareTo("small_molecule")!=0 & bmd_objects__B.type.compareTo("not specified")!=0) {
				if (bmd_objects__B.tax == null) {
					System.err.println("validateRecord: Warning object B does not have a tax id");
					//isValid = false;
				}
			}
		
		}
		else if (bmd_source.type.compareTo("complex")==0 | bmd_source.type.compareTo("pathway")==0) {
			if (bmd_complex2ints.int_ids == null) {
				System.err.println("validateRecord: complex/pathway has no interaction ids");
				isValid = false;
			}
			
			if (bmd_source.descr == null) {
				System.err.println("validateRecord: complex/pathway has no description");
				isValid = false;
			}
		}
			
		if (isValid == false) {
			//System.exit(0);
			return false;
		}
		else {
			return true;
		}
		

		
	}
	
	/*public boolean validateReference(bmd_refs_rec bmd_refs) {
		return true;
	}*/
		
	
	
}
