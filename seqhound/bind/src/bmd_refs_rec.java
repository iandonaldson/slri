/*
 * Created on 7-Jun-2004
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
public class bmd_refs_rec {
	String method = null;
	String db = null;
	Integer id = null;
	String acc = null;
	
	public void setMethod(int val) {
		if (method != null) {
			System.err.println("setMethod:warning, method already set, overwriting.");
		}
		method = resolveExperimentalSystem(val);
		
	}
	
	public boolean isMethodSet() {
		if (method != null) {
			return true;
		}
		else {
			return false;
		}
	}
	
	public boolean isIdSet() {
		if (id != null) {
			return true;
		}
		else {
			return false;
		}
	}
	
	public boolean isAccSet() {
		if (acc != null) {
			return true;
		}
		else {
			return false;
		}
	}	
	
	//This is used to determine if the record 
	//has been populated or not
	public boolean isPopulated() {
		if (method != null) {
			return true;
		}
		else if (db != null) {
			return true;
		}
		else if (id != null) {
			return true;
		}
		else if (acc != null) {
			return true;
		}
		return false;
	}
	
	//This maps the experimental system enumeration to
	//experimental system name, with the exception of 
	//"other", which corresponds to 255
	static final String[] valToExp = {"not specified",
			"alanine-scannin",
			"affinity-chromatography",
			"atomic-force-microscopy",
			"autoradiography",
			"competition-binding",
			"cross-linking",
			"deuterium-hydrogen-exchange",
			"electron-microscopy",
			"electron-spin-resonance",
			"elisa",
			"equilibrium-dialysis",
			"fluorescence-anisotropy",
			"footprinting",
			"gel-retardation-assays",
			"gel-filtration-chromatography",
			"hybridization",
			"immunoblotting",
			"immunoprecipitation",
			"immunostaining",
			"interaction-adhesion-assay",
			"-scattering",
			"mass-spectrometry",
			"membrane-filtration",
			"monoclonal-antibody-blockade",
			"nuclear-translocation-assay",
			"phage-display",
			"reconstitution",
			"resonance-energy-transfer",
			"site-directed-mutagenesis",
			"sucrose-gradient-sedimentation",
			"surface-plasmon-resonance-chip",
			"transient-coexpression",
			"three-dimensional-structure",
			"two-hybrid-test",
			"allele-specific-complementation",
			"far-western",
			"colocalization",
			};
	
	public String resolveExperimentalSystem(int val) {
		if (val == 255) {
			return new String("other");
		}
		else if (val > -1 & val < 38){
			 return valToExp[val];	
		}
		else {
			System.err.println("resolveExperimentalSystem: invalid integer value for experimental system:" + val);
			return null;
		}
	}
}
