/**
TODO:
1) Testing, testing and one more time testing.
**/

/*ini-file parsing*/
#include <corelib/ncbireg.hpp>

/*ASN.1 I/O*/
#include <corelib/ncbistd.hpp> 
#include <serial/classinfo.hpp> 
#include <serial/member.hpp> 
#include <serial/objectio.hpp> 
#include <serial/objecttype.hpp> 
#include <serial/objistr.hpp>
#include <serial/objostr.hpp>
#include <serial/objostrasnb.hpp>
#include <serial/serial.hpp>

/*BIND spec*/
#include <BIND_Submit.hpp>
#include <BIND_Interaction_set.hpp>
#include <BIND_Interaction.hpp>
#include <BIND_place_set.hpp>
#include <BIND_place.hpp>
#include <BIND_descr.hpp>
#include <BIND_condition_set.hpp>
#include <BIND_condition.hpp>
#include <BIND_loc.hpp>
#include <BIND_Complex_set.hpp>
#include <BIND_Molecular_Complex.hpp>
#include <Molecular_Complex_id.hpp>
#include <BIND_id.hpp>
#include <BIND_small_molecule_id.hpp>
#include <Internal_small_molecule_id.hpp>
#include <BIND_other_db.hpp>
#include <Geninfo_id.hpp>
#include <Interaction_id.hpp>
#include <BIND_object.hpp>
#include <BIND_object_type_id.hpp>
#include <BIND_pub_set.hpp>
#include <BIND_pub_object.hpp>
#include <objects/pub/Pub.hpp>
#include <objects/biblio/PubMedId.hpp>
#include <objects/medline/Medline_entry.hpp>
#include <BIND_object_origin.hpp>
#include <BIND_mol_object.hpp>
#include <BIND_mol_sub_num.hpp>
#include <objects/seqfeat/BioSource.hpp>
#include <objects/seqfeat/Org_ref.hpp>

/*MySQL driver - requiers MySQL header files as well*/
#include <dbapi/driver/exception.hpp>
#include <dbapi/driver/mysql/interfaces.hpp>

/*redundancy check*/
#include <hash_map>

#include "Seqhound.hpp"

USING_NCBI_SCOPE;
using namespace objects;
using namespace _bind;


/* class declared globally, first initialized in main() */
/* used in PBIND_object and in CalculateRog method*/
using namespace seqhound;
SeqHound g_sh;

/******
 * class is based on MySQL NCBI C++ TK driver,
 * for the specification refer to the Visio diagrams
 ******
 */
class Bindmod_db
{
private:
	CDB_Connection	*con;
	CMySQLContext	*context;

	/******
	 * contains current maximum uid identifier, see InsertInto_bmd_source description
	 * updated in InsertInto_bmd_source ONLY, 
	 * s_uid is used in InsertInto_bmd_refs/InsertInto_bmd_ints/InsertInto_bmd_complexes
	 * uid is used in InsertInto_bmd_objects/InsertInto_bmd_ints/InsertInto_bmd_complexes
	 ******
	 */
	Uint4 uid;
	string s_uid;
		
	/******
	 * contains uid/objid pairs - 
	 * used for syncronization between bmd_objects and bmd_ints/bmd_complex2subunits
	 * filled in InsertInto_bmd_objects, erased in InsertInto_bmd_ints/InsertInto_bmd_complexes
	 ******
	 */
	hash_map < Uint4, vector <Uint4> > h_obj;
	hash_map < Uint4, vector <Uint4> >::const_iterator ch_obj;



public:

	/*** CONSTRUCTORS AND DESTRUCTORS ***/
	
	/* the class has no default constructor, the explicit constructor is Init method */
	bool Init(const string& host, const string& database, const string& user, const string& password);
	~Bindmod_db();


	/*** EXCEPTIONS ***/

	/******
	 * the general strategy is simple: 
	 * any method of the class will return true if everything is fine,
	 * if it meets some error such as an invalid passed parameter or access violation 
	 * it returns false - the error is recoverable and we can continue
	 * but if it catches a database API error - it throws the exception - database
	 * consistency must be maintained at all costs
	 *
	 * this structure was intentionally made public to be accessible from outside world
	 ******
	 */
	
	typedef struct _Bindmod_excpt
	{
		string f_name;			//Bindmod_db method name
	} Bindmod_excpt;

	Bindmod_excpt bm_e;


	/*** INSERTION FUNCTIONS ***/
	
	/******
	 * uid is automatically incremented by MySQL - this is the ONLY database where it is happening
	 * all other databases are getting uid via (Uint4 uid;) variable which is updated when new record
	 * is inserted into bmd_source
	 * databases affected: bmd_source, bmd_record_types, bmd_dbases
	 * NOTE: bmd_record_types and bmd_dbases are to be fullfilled manually, using bind_module.sql file
	 ******
	 */
	bool InsertInto_bmd_source(const string& db, const string& acc, Uint4 id, const string& type, ostrstream& blob);
	/******
	 * objid identifier is incremented automatically by MySQL when new record is inserted into bmd_objects
	 * all other databases are getting this variable via SQL statement SELECT MAX
	 * rog is always set to 0 when new record is inserted, 
	 * rog is calculated during maintenance mode by bool CalculateRog(void)
	 * to update this field
	 * databases affected: bmd_objects, bmd_mol_type, bmd_obj_dbases 
	 * NOTE: bmd_mol_type and bmd_obj_dbases are to be fullfilled manually, using bind_module.sql file
	 ******
	 */
	bool InsertInto_bmd_objects(const string& type, const string& db, const string& acc, Uint4 id, Uint4 tax, const string& label);
	/******
	 * databases affected: bmd_refs, bmd_exp_methods, bmd_refs_db
	 * NOTE: bmd_exp_methods and bmd_refs_db are to be fullfilled manually, using bind_module.sql file
	 ******
	 */
	bool InsertInto_bmd_refs(const string& db, const string& acc, Uint4 id, Uint4 method = 0);
	/****** 
	 * objAid/objAiB are taken from bmd_objects therefore bmd_objects MUST BE fullfilled earlier
	 * rig is always set to 0 when new record is inserted 
	 * rig is calculated during maintenance mode by bool CalculateRig(void)
	 * databases affected: bmd_ints 
	 ******
	 */
	bool InsertInto_bmd_ints(const string& descr);
	/******
	 * const list<Uint4>& ll contains a list of interaction id's
	 * complex2subunits.objid is taken from bmd_objects therefore bmd_objects MUST BE fullfilled earlier
	 * databases affected: bmd_complexes, bmd_complex2subunits, bmd_complex2ints 
	 ******
	 */
	bool InsertInto_bmd_complexes(Uint4 numofsubunits, const string& descr, const list<Uint4>& ll);

	/*** MAINTENANCE FUNCTIONS ***/
	
	/*******
	 * updates bmd_objects.rog field, requiers bmd_objects database to contain the data
	 * uses STL hash_map with the vector inside, does two iterations - during the first one
	 * the function fills the hash_map table, during the second pass it updates the database
	 * databases affected: bmd_objects 
	 *******
	 */
	bool CalculateRog(void);
	/*******
	 * updates bmd_ints.rog field, requiers bmd_ints database to contain the data
	 * databases affected: bmd_ints 
	 *******
	 */
	bool CalculateRig(void);

	/*******
	 * A bitwise hash function written by Justin Sobel
	 * supplementary hash function for hash_map table
	 *******
	 */
	Uint4 JSHash(string& str)
	{
		Uint4 hash = 1315423911;

		for(Uint4 i = 0; i < str.length(); i++)
			hash ^= ((hash << 5) + str[i] + (hash >> 2));

		return (hash & 0x7FFFFFFF);
	};

	/******
	 * currently the optimization touches bmd_source since there are too many
	 * BLOB's inserted and bmd_ints with bmd_objects since there are many UPDATES on them
	 * see MySQL manual "7.2.12 Speed of UPDATE Queries" and "13.5.2.5 OPTIMIZE TABLE Syntax"
	 ******
	 */
	bool OptimizeDatabase(void);

	/* function to convert the given UNSIGNED integer to an ASCII string */
	/* STL is too slow and CRT is too slow as well */
	string uitoa(Uint4 n)
	{
		char p[90];
		char *s = p, *end;
		Uint2 j = 0;
		do
		{
			p[j++] = n%10 + '0';
		} while ((n /= 10) > 0);
		p[j] = '\0';
		j--;
		end = s + j;

		for ( ; s < end; s++, end-- )
		{
			*s ^= *end;
			*end ^= *s;
			*s ^= *end;
		}

		string str = p;
		return str;
	}
};

/* class declared globally, first initialized in main() */
Bindmod_db g_bdb;

bool Bindmod_db::Init(const string& host, const string& database, const string& user, const string& password)
{
	try
	{	
		context = new CMySQLContext ();
		con = context->Connect(host, user, password, 0);
		auto_ptr<CDB_LangCmd>lcmd(con->LangCmd("use " + database));
		lcmd->Send();
	}
	catch(CDB_Exception& e)
	{
		bm_e.f_name = "Bindmod_db::Init";
		throw(bm_e);
	}
	catch(...)
	{
		return false;
	}
	return true;
}

Bindmod_db::~Bindmod_db()
{
	delete con;
	delete context;
}

bool Bindmod_db::InsertInto_bmd_source(const string& db, const string& acc, Uint4 id, const string& type, ostrstream& blob)
//example: INSERT INTO bmd_source (db, acc, id, data_blob, type) SELECT db, '', 50000, '\0x30\0x31', type FROM bmd_dbases, bmd_record_types WHERE bmd_dbases.db_name = 'BIND' AND bmd_record_types.type_name = 'interaction';
//example: SELECT MAX(uid) FROM bmd_source;
{
	try
	{
		{
			string s = "INSERT INTO bmd_source (db, acc, id, data_blob, type) SELECT db, ";
			s += '\'' + acc + "', " + uitoa(id) + ", '";
			char *tmp = new char[blob.pcount()*2 + 1];
			mysql_escape_string(tmp, blob.str(), blob.pcount());
			s += tmp;
			delete [] tmp;
			s += "', type FROM bmd_dbases, bmd_record_types WHERE bmd_dbases.db_name = '" 
				+ db + "' AND bmd_record_types.type_name = '" + type + '\'';
			auto_ptr<CDB_LangCmd>lcmd(con->LangCmd(s));
			lcmd->Send();
		}

		{
			auto_ptr<CDB_LangCmd>lcmd(con->LangCmd("SELECT MAX(uid) FROM bmd_source"));
			lcmd->Send();
			auto_ptr<CDB_Result> r(lcmd->Result());
			r->Fetch();
			CDB_Int src_uid;
			r->GetItem(&src_uid);
			uid = src_uid.Value();
			s_uid = uitoa(uid);
		}
	}
	catch(CDB_Exception& e)
	{
		bm_e.f_name = "Bindmod_db::InsertInto_bmd_source";
		throw(bm_e);
	}
	catch(...)
	{
		return false;
	}

	return true;
}

bool Bindmod_db::InsertInto_bmd_objects(const string& type, const string& db, const string& acc, Uint4 id, Uint4 tax, const string& label)
//example: INSERT INTO bmd_objects (type, db, acc, id, tax, label, rog) SELECT type, db, 'NP_610369', 24586550, 7227, 'EG:BACR37P7.5', 0 FROM bmd_mol_type a, bmd_obj_dbases b WHERE a.type_name = 'protein' AND b.db_name = 'GenBank';  
//example: SELECT MAX(objid) from bmd_objects;
{
	if(!uid) return false;
	
	try
	{
		{
			string s = "INSERT INTO bmd_objects (type, db, acc, id, tax, label, rog) SELECT type, db, '";
			s += acc + "', " + uitoa(id) + ", " + uitoa(tax) + ", '" + label 
				+ "', 0 FROM bmd_mol_type a, bmd_obj_dbases b WHERE a.type_name = '" 
				+ type + "' AND b.db_name = '" + db + '\'';

			auto_ptr<CDB_LangCmd>lcmd(con->LangCmd(s));
			lcmd->Send();
		}

		{
			auto_ptr<CDB_LangCmd>lcmd(con->LangCmd("SELECT MAX(objid) from bmd_objects"));
			lcmd->Send();
			auto_ptr<CDB_Result> r(lcmd->Result());
			r->Fetch();
			CDB_Int o_objid;
			r->GetItem(&o_objid);
			h_obj[uid].push_back(o_objid.Value());
		}
	}
	catch(CDB_Exception& e)
	{
		bm_e.f_name = "Bindmod_db::InsertInto_bmd_objects";
		throw(bm_e);
	}

	return true;
}

/*const string& acc is not used now at all*/
bool Bindmod_db::InsertInto_bmd_refs(const string& db, const string& acc, Uint4 id, Uint4 method)
//example: INSERT INTO bmd_refs (uid, db, acc, id, method) SELECT 13, db, '', 11805837, method FROM bmd_refs_db r, bmd_exp_methods m WHERE r.db_name = 'PubMed' AND m.method_descr = 'autoradiography';
{
	if(!s_uid.size()) return false;
	
	try
	{
		string s = "INSERT INTO bmd_refs (uid, db, acc, id, method) SELECT ";
		s += s_uid + ", db, '', " + uitoa(id) + 
			", method FROM bmd_refs_db r, bmd_exp_methods m WHERE r.db_name = '" 
			+ db + "' AND m.method_descr = " + uitoa(method);
		
		auto_ptr<CDB_LangCmd>lcmd(con->LangCmd(s));
		lcmd->Send();
	}
	catch(CDB_Exception& e)
	{
		bm_e.f_name = "Bindmod_db::InsertInto_bmd_refs";
		throw(bm_e);
	}

	return true;
}

bool Bindmod_db::InsertInto_bmd_ints(const string& descr)
//example: INSERT INTO bmd_ints (uid, objAid, objBid, rig, descr) VALUES(123, 5, 7, 0, 'this is an interaction between a and b!');
{
	if(!s_uid.size() || !uid) return false;

	string obja, objb;

	ch_obj = h_obj.find(uid);
	if( ch_obj != h_obj.end())
	{
		obja = uitoa(ch_obj -> second[0]); 
		objb = uitoa(ch_obj -> second[1]);
	}
	else
		return false;
	
	try
	{
		auto_ptr<CDB_LangCmd>lcmd(con->LangCmd(
			"INSERT INTO bmd_ints (uid, objAid, objBid, rig, descr) VALUES(" 
			+ s_uid + ", " + obja + ", " + objb + ", 0, '" + descr + "')"));
		lcmd->Send();
	}
	catch(CDB_Exception& e)
	{
		bm_e.f_name = "Bindmod_db::InsertInto_bmd_ints";
		throw(bm_e);
	}
	return true;
}

bool Bindmod_db::InsertInto_bmd_complexes(Uint4 numofsubunits, const string& descr, const list<Uint4>& ll)
//example: INSERT INTO bmd_complex2subunits (uid, objid) VALUES (3, 4), (28, 39);
//example: INSERT INTO bmd_complexes (uid, numsubunits, descr) VALUES (5, 12, "wow, it is a complex!");
//example: INSERT INTO bmd_complex2ints (uid_complex, uid_ints) VALUES (5,6), (7,8), (9,10);
{
	if(!s_uid.size() || !uid) return false;

	try
	{
		ch_obj = h_obj.find(uid);
		if( ch_obj != h_obj.end())
		{
			string s = "INSERT INTO bmd_complex2subunits (uid, objid) VALUES ";
			vector <Uint4>::const_iterator v_i = ch_obj -> second.begin();
			while(v_i != ch_obj -> second.end())
			{
				s += '(' + s_uid + ", " + uitoa(*v_i) + ')';
				v_i++;
			}
				
			auto_ptr<CDB_LangCmd>lcmd(con->LangCmd(s));
			lcmd->Send();
		}
		else
			return false;

		{
			auto_ptr<CDB_LangCmd>lcmd(con->LangCmd("INSERT INTO bmd_complexes (uid, numsubunits, descr) VALUES "));
			lcmd->More('(' + s_uid + ", " + uitoa(numofsubunits) + ", " + descr + ')');
			lcmd->Send();
		}

		{
			string s = "INSERT INTO bmd_complex2ints (uid_complex, uid_ints) VALUES ";
			list <Uint4>::const_iterator ll_i = ll.begin();
			while( ll_i != ll.end() )
			{
				s += '(' + s_uid + ", " + uitoa(*ll_i) + ')';
				ll_i++;
			}
				
			auto_ptr<CDB_LangCmd>lcmd(con->LangCmd(s));
			lcmd->Send();
		}
	}
	catch(CDB_Exception& e)
	{
		bm_e.f_name = "Bindmod_db::InsertInto_bmd_complexes";
		throw(bm_e);
	}

	return true;

}

bool Bindmod_db::CalculateRog(void)
//example: SELECT objid, type_name, db, id, acc FROM bmd_objects, bmd_mol_type WHERE bmd_objects.type = bmd_mol_type.type;
//example: UPDATE bmd_objects SET rog = 3 WHERE objid = 4 AND objid = 26 AND objid = 47;
{
	try
	{
		hash_map < Uint4, vector <Uint4> >					hm;
		//Uint4 is a hash, vector contains objid's
		hash_map < Uint4, vector <Uint4> >::const_iterator	hm_i;
		vector <Uint4>::const_iterator						v_i;

		{
			auto_ptr<CDB_LangCmd> lcmd(con->LangCmd("SELECT objid, type_name, db, id, acc FROM bmd_objects, bmd_mol_type WHERE bmd_objects.type = bmd_mol_type.type;"));
			lcmd->Send();
			while (lcmd->HasMoreResults())
			{
				auto_ptr<CDB_Result> r(lcmd->Result());
				while (r->Fetch())
				{
					CDB_Int		objid;
					CDB_VarChar	type;
					CDB_Int		db;
					CDB_Int		id;
					CDB_VarChar	acc;

					r->GetItem(&objid);
					r->GetItem(&type);
					r->GetItem(&db);
					r->GetItem(&id);
					r->GetItem(&acc);

					string s;	
					(type.Value() == "protein") 
						? s = uitoa(g_sh.SHoundRedundantGroupIDFromGI(id.Value())) 
						: s = '|';

					Uint4 hash = JSHash(uitoa(db.Value()) + '|' + uitoa(id.Value()) 
						+ '|' + acc.Value() + '|' + s);

					/*
					if hash does not exist - create it,
					if it does - then insert objid ONLY
					*/
					hm[hash].push_back(objid.Value());
				}
			} //while (lcmd->HasMoreResults())
		}

		hm_i = hm.begin();
		Uint4 rog = 1;
		while( hm_i != hm.end() )
		{
			auto_ptr<CDB_LangCmd> lcmd(con->LangCmd("UPDATE bmd_objects SET rog = " 
				+ uitoa(rog) + " WHERE objid = " + uitoa(hm_i -> second[0])));
			
			v_i = hm_i -> second.begin();
			v_i++; //skip hm_i -> second[0]
			while(v_i != hm_i -> second.end())
			{
				cout << *v_i;
				lcmd->More(" AND objid = " + uitoa(*v_i));
				v_i++;
			}

			lcmd->Send();
			hm_i++;
			rog++;
		}

	}
	catch(CDB_Exception& e)
	{
		bm_e.f_name = "Bindmod_db::CalculateRog";
		throw(bm_e);
	}


	return true;
}

bool Bindmod_db::CalculateRig(void)
//example: SELECT a.rog, b.rog FROM bmd_objects a, bmd_objects b, bmd_ints i WHERE i.objAid = a.objid AND i.objBid = b.objid;
//example: UPDATE bmd_ints SET rig = 3 WHERE uid = 45 AND uid = 196;
{
	try
	{
		hash_map < Uint4, vector <Uint4> >					hm;
		hash_map < Uint4, vector <Uint4> >::const_iterator	hm_i;
		vector <Uint4>::const_iterator						v_i;
		
		{
			auto_ptr<CDB_LangCmd> lcmd(con->LangCmd("SELECT uid, a.rog, b.rog FROM bmd_objects a, bmd_objects b, bmd_ints i WHERE i.objAid = a.objid AND i.objBid = b.objid"));
			lcmd->Send();
			while (lcmd->HasMoreResults())
			{
				auto_ptr<CDB_Result> r(lcmd->Result());
				while (r->Fetch())
				{
					CDB_Int ints_uid;
					CDB_Int objAid;
					CDB_Int objBid;

					r->GetItem(&ints_uid);
					r->GetItem(&objAid);
					r->GetItem(&objBid);

					Uint4 hash = JSHash(uitoa(objAid.Value()) + '|' + uitoa(objBid.Value()));

					hm[hash].push_back(ints_uid.Value());
				}
			}
		}
		
		hm_i = hm.begin();
		Uint4 rig = 1;
		
		while( hm_i != hm.end() )
		{
			auto_ptr<CDB_LangCmd> lcmd(con->LangCmd("UPDATE bmd_ints SET rig = " + uitoa(rig) + " WHERE uid = " + uitoa(hm_i -> second[0])));

			v_i = hm_i -> second.begin();
			v_i++;	//skip hm_i -> second[0]
			while(v_i != hm_i -> second.end())
			{
				lcmd->More(" AND objid = " + uitoa(*v_i));
				v_i++;
			}

			lcmd->Send();
			hm_i++;
			rig++;
		}
	}
	catch(CDB_Exception& e)
	{
		bm_e.f_name = "Bindmod_db::CalculateRig";
		throw(bm_e);
	}

	return true;
}

bool Bindmod_db::OptimizeDatabase(void)
{
	try
	{	
		{
			auto_ptr<CDB_LangCmd>lcmd(con->LangCmd("OPTIMIZE TABLE bmd_source"));
			lcmd->Send();
		}
		{
			auto_ptr<CDB_LangCmd>lcmd(con->LangCmd("OPTIMIZE TABLE bmd_ints"));
			lcmd->Send();
		}
		{
			auto_ptr<CDB_LangCmd>lcmd(con->LangCmd("OPTIMIZE TABLE bmd_objects"));
			lcmd->Send();
		}
	}
	catch(CDB_Exception& e)
	{
		bm_e.f_name = "Bindmod_db::OptimizeDatabase";
		throw(bm_e);
	}
	return true;
}

/****** 
 * BIND-Interaction -> a BIND-object
 * BIND-Interaction -> b BIND-object
 * BIND-Molecular-Complex -> BIND-mol-object -> sub-unit BIND-object
 *
 * function is used in CInteractionHook.ReadObject and CComplexHook.ReadObject
 ******
 */
inline void PBIND_object(const CBIND_object& bop, string& type, string& db, string& acc, string& label, Uint4& id, Uint4& tax)
{
	const CBIND_object::TId& type_id = bop.GetId();
	CBIND_object_type_id::E_Choice id_type = type_id.Which();
	CBIND_object_origin::E_Choice org_type = bop.GetOrigin().Which();

	switch(id_type)
	{
		case CBIND_object_type_id_Base::e_Protein:
			type = "protein";
			db = "GenBank";
			id = type_id.GetProtein().GetGi();
			if(org_type != CBIND_object_origin_Base::e_Not_specified
				&& org_type != CBIND_object_origin_Base::e_not_set)
				tax = bop.GetOrigin().GetOrg().GetOrg().GetTaxId();
			label = bop.GetShort_label();
			acc = g_sh.SHoundAccFromGi(id);
			if(acc.empty())
				acc = '-';
			break;
		case CBIND_object_type_id_Base::e_Dna:
			type = "DNA";
			db = "GenBank";
			id = type_id.GetDna().GetGi();
			if(org_type != CBIND_object_origin_Base::e_Not_specified
				&& org_type != CBIND_object_origin_Base::e_not_set)
				tax = bop.GetOrigin().GetOrg().GetOrg().GetTaxId();
			label = bop.GetShort_label();
			acc = g_sh.SHoundAccFromGi(id);
			if(acc.empty())
				acc = '-';
			break;
		case CBIND_object_type_id_Base::e_Rna:
			type = "RNA";
			db = "GenBank";
			id = type_id.GetRna().GetGi();
			if(org_type != CBIND_object_origin_Base::e_Not_specified
				&& org_type != CBIND_object_origin_Base::e_not_set)
				tax = bop.GetOrigin().GetOrg().GetOrg().GetTaxId();
			label = bop.GetShort_label();
			acc = g_sh.SHoundAccFromGi(id);
			if(acc.empty())
				acc = '-';
			break;
		case CBIND_object_type_id_Base::e_Small_molecule:
			label = bop.GetShort_label();
			type = "small-molecule";
			if(type_id.GetSmall_molecule().IsInternal())
			{
				db = "BIND";
				id = type_id.GetSmall_molecule().GetInternal().Get();
				tax = bop.GetOrigin().GetOrg().GetOrg().GetTaxId();
			}
			else if(type_id.GetSmall_molecule().IsOther_db())
			{
				db = type_id.GetSmall_molecule().GetOther_db().GetDbname();
				acc = type_id.GetSmall_molecule().GetOther_db().GetStrp();
				id = type_id.GetSmall_molecule().GetOther_db().GetIntp();
			}
			break;
		case CBIND_object_type_id_Base::e_Complex:
			type = "complex";
			db = "BIND";
			acc = "-";			
			if(org_type != CBIND_object_origin_Base::e_Not_specified
				&& org_type != CBIND_object_origin_Base::e_not_set)
				tax = bop.GetOrigin().GetOrg().GetOrg().GetTaxId();
			label = bop.GetShort_label();
			id = type_id.GetComplex().Get();
			break;
		case CBIND_object_type_id_Base::e_Gene:
			type = "gene";
			db = "GenBank";
			if(org_type != CBIND_object_origin_Base::e_Not_specified
				&& org_type != CBIND_object_origin_Base::e_not_set)
				tax = bop.GetOrigin().GetOrg().GetOrg().GetTaxId();
			label = bop.GetShort_label();
			id = type_id.GetGene().GetGi();
			acc = g_sh.SHoundAccFromGi(id);
			if(acc.empty())
				acc = '-';
			break;
		case CBIND_object_type_id_Base::e_Photon:
			type = "photon";
			db = "BIND";
			acc = "-";
			break;
		case CBIND_object_type_id_Base::e_Not_specified:
			type = "unknown";
			db = "BIND";
			acc = "-";
			id = 0;
			tax = 0;
			break;
		case CBIND_object_type_id_Base::e_not_set:
			break;
		default:
			break;
	}
	
}

/******
 * PMID's are extracted from:
 * BIND-Interaction -> BIND-descr -> BIND-condition-set -> BIND-condition -> BIND-pub-set
 * BIND-Interaction -> BIND-descr -> BIND-place-set -> BIND-place -> BIND-pub-set
 * BIND-Interaction -> BIND-descr -> BIND-loc -> BIND-pub-set
 * BIND-Interaction -> BIND-object -> BIND-place-set -> BIND-place -> BIND-pub-set
 * BIND-Molecular-Complex -> BIND-pub-set
 *
 * function is used in CInteractionHook.ReadObject and CComplexHook.ReadObject
 * function will extract SUPPORTED pmid's ONLY!
 ******
 */
inline Uint4 PBIND_pub_object(const CBIND_pub_object& pub)
{
	Uint4 pmid = 0;

	const CBIND_pub_object::TPub& publication = pub.GetPub();
	if(pub.GetOpinion() == CBIND_pub_object_Base::eOpinion_support)
	{
		if(publication.IsPmid())
			pmid = (publication.GetPmid()).Get();
		else if(publication.IsMuid())
			pmid = publication.GetMuid();
	}
	return  pmid;
};

class CInteractionHook : public CReadObjectHook 
{ 
public: 
    void ReadObject(CObjectIStream& in, const CObjectInfo& object) 
    { 
		DefaultRead(in,object);
		CBIND_Interaction *bi = (CBIND_Interaction *)object.GetObjectPtr();
		
		/*BLOB to save it inside the database*/
		ostrstream ostr; 
        CObjectOStreamAsnBinary out(ostr, false); 
		out << *bi;

		g_bdb.InsertInto_bmd_source("BIND", "", bi->GetIid(), "interaction", ostr);

		Uint4 id;
		Uint4 tax;
		string db;
		string acc;
		string label;
		string type;

		const CBIND_object& oA = bi->GetA();
		const CBIND_object& oB = bi->GetB();

		PBIND_object(oA, type, db, acc, label, id, tax);
		g_bdb.InsertInto_bmd_objects(type, db, acc, id, tax, label);
		PBIND_object(oB, type, db, acc, label, id, tax);
		g_bdb.InsertInto_bmd_objects(type, db, acc, id, tax, label);

		
		//contains pmid as the first Uint4, second hash_map contains 
		//BIND-experimental-system as the first Uint4 and NULL as the second one
		hash_map <Uint4, hash_map <Uint4, Uint4> > exp_m;
		hash_map <Uint4, hash_map <Uint4, Uint4> >::const_iterator exp_m_i;
		hash_map <Uint4, Uint4>::const_iterator e_i;

		Uint4 pmid = 0;
		Uint4 experimentid = 0;

		const CBIND_descr& b_desc = bi->GetDescr();

		//BIND-Interaction -> BIND-descr -> BIND-condition-set -> BIND-condition -> BIND-pub-set
		//BIND-Interaction -> BIND-descr -> BIND-condition-set -> BIND-condition -> system BIND-experimental-system
		if(b_desc.IsSetCond())
		{
			const CBIND_condition_set& cond_set = b_desc.GetCond();
			CBIND_condition_set::TConditions cond = cond_set.GetConditions();
			CBIND_condition_set::TConditions::const_iterator c_i = cond.begin();
			
			while(c_i != cond.end())
			{
				experimentid = (*c_i)->GetSystem();

				CBIND_pub_set::TPubs publications = (*c_i)->GetSource().GetPubs();
				CBIND_pub_set::TPubs::const_iterator p_i = publications.begin();
				while( p_i!= publications.end() )
				{
					pmid = PBIND_pub_object(**p_i);
					if(pmid) 
						exp_m[pmid][experimentid] = 0;
					p_i++;
				}
				c_i++;
			}
		}

		//BIND-Interaction -> BIND-descr -> BIND-place-set -> BIND-place -> BIND-pub-set
		if(b_desc.IsSetPlace())
		{
			const CBIND_place_set& place_set = b_desc.GetPlace();
			CBIND_place_set::TPlaces place = place_set.GetPlaces();
			CBIND_place_set::TPlaces::const_iterator c_i = place.begin();
			experimentid = 0;
			while(c_i != place.end())
			{
				CBIND_pub_set::TPubs publications = (*c_i)->GetSource().GetPubs();
				CBIND_pub_set::TPubs::const_iterator p_i = publications.begin();
				while( p_i!= publications.end() )
				{
					pmid = PBIND_pub_object(**p_i);
					if(pmid) exp_m[pmid][experimentid] = 0;
					p_i++;
				}

				c_i++;
			}
		}

		//BIND-Interaction -> BIND-descr -> BIND-loc -> BIND-pub-set
		if(b_desc.IsSetBinding_sites())
		{
			if(b_desc.GetBinding_sites().IsSetSource())
			{
				CBIND_pub_set::TPubs publications = b_desc.GetBinding_sites().GetSource().GetPubs();
				CBIND_pub_set::TPubs::const_iterator p_i = publications.begin();
				experimentid = 0;
				while( p_i!= publications.end() )
				{
					pmid = PBIND_pub_object(**p_i);
					if(pmid) exp_m[pmid][experimentid] = 0;
					p_i++;
				}
			}
		}

		//BIND-Interaction -> BIND-object A -> BIND-place-set -> BIND-place -> BIND-pub-set
		if(oA.IsSetPlace())
		{
			const CBIND_place_set& place_set = oA.GetPlace();
			CBIND_place_set::TPlaces place = place_set.GetPlaces();
			CBIND_place_set::TPlaces::const_iterator c_i = place.begin();
			experimentid = 0;
			while(c_i != place.end())
			{
				CBIND_pub_set::TPubs publications = (*c_i)->GetSource().GetPubs();
				CBIND_pub_set::TPubs::const_iterator p_i = publications.begin();
				while( p_i!= publications.end() )
				{
					pmid = PBIND_pub_object(**p_i);
					if(pmid) exp_m[pmid][experimentid] = 0;
					p_i++;
				}
				c_i++;
			}
		}

		//BIND-Interaction -> BIND-object B -> BIND-place-set -> BIND-place -> BIND-pub-set
		if(oB.IsSetPlace())
		{
			const CBIND_place_set& place_set = oB.GetPlace();
			CBIND_place_set::TPlaces place = place_set.GetPlaces();
			CBIND_place_set::TPlaces::const_iterator c_i = place.begin();
			experimentid = 0;
			while(c_i != place.end())
			{
				CBIND_pub_set::TPubs publications = (*c_i)->GetSource().GetPubs();
				CBIND_pub_set::TPubs::const_iterator p_i = publications.begin();
				while( p_i!= publications.end() )
				{
					pmid = PBIND_pub_object(**p_i);
					if(pmid) exp_m[pmid][experimentid] = 0;
					p_i++;
				}
				c_i++;
			}
		}

		exp_m_i = exp_m.begin();
		while(exp_m_i != exp_m.end())
		{
			e_i = exp_m_i -> second.begin();
			while(e_i != exp_m_i -> second.end())
			{
				g_bdb.InsertInto_bmd_refs("PubMed", "", exp_m_i -> first, e_i -> first);
				e_i++;
			}
			exp_m_i++;
		}
		
		g_bdb.InsertInto_bmd_ints(b_desc.GetSimple_descr());
		
        in.SetDiscardCurrObject(); 
    } 
}; 

class CComplexHook : public CReadObjectHook 
{ 
public: 
    void ReadObject(CObjectIStream& in,const CObjectInfo& object) 
    { 
        DefaultRead(in,object);
		CBIND_Molecular_Complex *bc = (CBIND_Molecular_Complex *)object.GetObjectPtr();

		ostrstream ostr; 
        CObjectOStreamAsnBinary out(ostr, false); 
		out << *bc;

		g_bdb.InsertInto_bmd_source("BIND", "", bc->GetMcid(), "complex", ostr);

		Uint4 id;
		Uint4 tax;
		string db;
		string acc;
		string label;
		string type;

		CBIND_Molecular_Complex::TSub_units subunitslist = bc->GetSub_units();
		CBIND_Molecular_Complex::TSub_units::const_iterator i = subunitslist.begin();
		while( i != subunitslist.end() )
		{
			PBIND_object((**i).GetSub_unit(), type, db, acc, label, id, tax);
			//g_bdb.InsertInto_bmd_objects(type, db, acc, id, tax, label);
			i++;
		}

		list <Uint4> pmid_l;
		list <Uint4>::const_iterator pm_i;
		Uint4 pmid = 0;

		//BIND-Molecular-Complex -> BIND-pub-set
		CBIND_pub_set::TPubs publications = bc->GetSource().GetPubs();
		CBIND_pub_set::TPubs::const_iterator p_i = publications.begin();
		while( p_i != publications.end() )
		{
			pmid = PBIND_pub_object(**p_i);
			if(pmid) pmid_l.push_back(pmid);
			
			p_i++;
		}
		
		pm_i = pmid_l.begin();
		while(pm_i != pmid_l.end())
		{
			//g_bdb.InsertInto_bmd_refs("PubMed", "", *pm_i);
			pm_i++;
		}

		list <Uint4> ll;

		CBIND_Molecular_Complex::TInteraction_list interaction_list = bc->GetInteraction_list();
		CBIND_Molecular_Complex::TInteraction_list::const_iterator i_l = interaction_list.begin();
		while( i_l != interaction_list.end() )
		{
			ll.push_back(*i_l);
			i_l++;
		}

		g_bdb.InsertInto_bmd_complexes(bc->GetSub_num().GetNum(), bc->GetDescr(), ll);

        in.SetDiscardCurrObject(); 
    } 
};

inline char *IsKeyFound(int argc, const char **argv, const char *key)
{
		char **tmp = (char **)argv;
		char *pdest = 0;
		while(--argc > 0)
		{
			if(pdest = strstr(*++tmp, key))
				return pdest;
		}
		return 0;
}

int main(int argc, const char **argv) 
{ 
    if (argc < 2) 
	{ 
        cerr << "Usage: bind_parser <filename> <keys>\n"  
				"Keys are (all OFF by default):\n"
				"\t-no-val - forses the parser to do the maintenance only\n"
				"\t(rig/rog calculation and db optimization)\n"
				"\t-no-rog - do not calculate bmd_objects.rog\n"
				"\t-no-rig - do not calculate bmd_ints.rig\n"
				"\t-no_opt - do not optimize the database\n"
				"NOTE: \n"
				"\tbind_parser expects the <filename> to be the first one,\n"
				"\tthe order of the keys is not significant\n"
				"Examples:\n"
				"\t1. bind_parser bindfungi.1.val\n"
				"\t2. bind_parser -no-val\n"
				"\t3. bind_parser bindfungi.1.val -no-rog -no-rig\n"
				"\t4. bind_parser -no-rig -no-rog -no-val\n"
			 << endl;
        return 1; 
    }

	ifstream in_ini("bind_parser.ini");
	if(!in_ini)
	{
		cerr << "ini file should be present in the current directory to get the database configuration" << endl;
		return 1;
	}
	CNcbiRegistry ini(in_ini);
	string host = ini.Get("database", "host");
	string database = ini.Get("database", "database");
	string user = ini.Get("database", "user");
	string password = ini.Get("database", "password");
	in_ini.close();

	bool no_val	= IsKeyFound(argc, argv, "-no-val");
	bool no_rog	= IsKeyFound(argc, argv, "-no-rog");
	bool no_rig	= IsKeyFound(argc, argv, "-no-rig");
	bool no_opt	= IsKeyFound(argc, argv, "-no-opt");

	if(!g_sh.SHoundInit(false, "bind_parser"))
	{
		cerr << "Error: SeqHoundInit failed\n";
		return 1;
	}

	try
	{
		if(g_bdb.Init(host, database, user, password))
		{
			if(!no_val)
			{
				try
				{
					auto_ptr<CObjectIStream> in(CObjectIStream::Open(eSerial_AsnBinary, argv[1]));
					
					CObjectTypeInfo type_inter = CType<CBIND_Interaction>(); 
					type_inter.SetLocalReadHook(*in, new CInteractionHook);

					CObjectTypeInfo type_complex = CType<CBIND_Molecular_Complex>(); 
					type_complex.SetLocalReadHook(*in, new CComplexHook);
					
					CBIND_Submit bind_submit;
					*in >> bind_submit;
				}
				catch(CUtilException& e)
				{
					cerr << "Open failed: " << e.GetErrCodeString() << endl
						 << "Input file was: " << argv[1] << endl
						 << "Are you sure you are having a valid ASN.1 binary file of BIND-Submit type?\n";
				}
			}

			if(!no_rog)
				g_bdb.CalculateRog();
			if(!no_rig)
				g_bdb.CalculateRig();
			if(!no_opt)
				g_bdb.OptimizeDatabase();

			return 0;
		}
		else
			cerr << "Connection to db failed. ini-file used was " << argv[1] << endl;
	}
	catch(Bindmod_db::Bindmod_excpt error)
	{
		cerr << "Database error caught during processing\n"
				"Details:\n"
			 << "function name: " << error.f_name << endl;
	}
    
	return 1; 
}