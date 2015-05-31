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
public class bmd_source_rec {
	private int uid;
	String db = null;
	StringBuffer id = null;
	String acc = null;
	String type = null;
	private StringBuffer data_clob = null;
	StringBuffer descr = null;
	
	public bmd_source_rec() {
		
	}

	void setUid(int uid) {
		this.uid = uid;
	}

	int getUid() {
		return uid;
	}

	void setAcc(String acc) {
		this.acc = acc;
	}

	String getAcc() {
		return acc;
	}

	void setData_clob(StringBuffer data_clob) {
		this.data_clob = data_clob;
	}

	StringBuffer getData_clob() {
		return data_clob;
	}

}
