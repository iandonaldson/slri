/* $Id: Seqhound.cpp,v 1.15 2004/11/25 16:06:58 hfeldman Exp $ */
/***************************************************************************
  Copyright (C) 2001 Mount Sinai Hospital (MSH)

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License 
  as published by the Free Software Foundation; either version 2.1 of
  the License, or any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the
         Free Software Foundation, Inc.,
         59 Temple Place, Suite 330, Boston, MA
         02111-1307  USA
  or visit http://www.gnu.org/licenses/lgpl.txt

  SPECIAL EXCEPTIONS

  As a special exception, Mount Sinai Hospital gives permission to
  link this program with the following non-GPL programs or libraries,
  and distribute the resulting executable, without including the source
  code for these in the source distribution:

  a) CodeBase 6.5 or greater from Sequiter Software Inc.

  b) The C or C++ interface to Oracle 8.x or greater, from
     Oracle Corporation or IBM DB2 UDB.


  PROGRAM: SLRI C++ Seqhound remote functions.


  AUTHORS: Hao Lieu


  REVISION/CONTRIBUTION NOTES:

  September 1, 2002  Original MSH Revision.
                Hogue Lab - University of Toronto Biochemistry
                Department and the Samuel Lunenfeld Research
                Institute, Mount Sinai Hospital
                http://bioinfo.mshri.on.ca  hogue@mshri.on.ca
                ftp://ftp.mshri.on.ca/


  PUBLICATION TO CITE:

***************************************************************************
*/
#include <Seqhound.hpp>
#include <slri_misc.hpp>
#include <map>

BEGIN_SCOPE(seqhound)

USING_NCBI_SCOPE;
using namespace objects;
using namespace slri;

/*RY: type definition for API list functions*/
typedef list<int> TIList;
typedef list<string> TSList;
typedef map<int, list<int> > TIMap;


/****************************************************
                   CONSTRUCTOR
*****************************************************/
SeqHound::SeqHound(){
  isInited = false;
}

/*****************************************************
             TIME OUT FUNCTIONS
*****************************************************/

int SeqHound::SHoundGetTimeOut(){
  return gsec;
}

void SeqHound::SHoundUserSetTimeOut(int sec){
  gUsersec = sec;
}

void SeqHound::SHoundSetTimeOut(int sec){
  gsec = sec;
}
  
int SeqHound::SHoundUserGetTimeout(void){
  return gUsersec;
}

/***********************************************************

                          THE ERROR FUNCTIONS

************************************************************/
void SeqHound::SetError(int ERR){

  m_error = ERR;
}

bool SeqHound::IsErrorSet(void){
  return m_error < 0 ? true : false;
}

void SeqHound::ErrorReset(void){
  m_error = 0;
}

int SeqHound::GetError(void){
  return m_error;
}

int SeqHound::ParseSeqHoundError(const string& pline){

  if(pline.empty()){
    ERR_POST(Error << "[ParseSeqHoundError] Passed an empty string.");
    SetError(SEQHOUND_BUFFER_ERROR);
    return SEQHOUND_BUFFER_ERROR;
  }

  /* search for the first instance of " \n\t" in pline  */
  size_t pos = pline.find_first_of(" \0\n\t");

  /* this substring will then contain the SeqHound return value */
  string error = pline.substr(0, pos);

  if(error == "SEQHOUND_ERROR"){
    SetError(SEQHOUND_ERROR);
    ERR_POST(Error << "[ParseSeqHoundError] Received " << pline);
    return SEQHOUND_ERROR;
  }else if(error == "SEQHOUND_NULL"){
    SetError(SEQHOUND_NULL);
    ERR_POST(Error << "[ParseSeqHoundError] Received " << pline);
    return SEQHOUND_NULL;
  }else if(error == "SEQHOUND_OK"){
    SetError(SEQHOUND_OK);
    return SEQHOUND_OK;
  }else{
    SetError(SEQHOUND_ABNORMAL);
    ERR_POST(Error << "[ParseSeqHoundError] Abnormal SEQHOUND code: " << pline);
    return SEQHOUND_ABNORMAL;
  }
}

/*******************************************************************/
/*                         SEQHOUND_API                            */
/*******************************************************************/


/* if successful, returns SEQHOUND_OK and the value from the 
   server gets stored into SHoundReply else returns an error value */
int SeqHound::SHoundWWWGetFile(string host, string url, string& SHoundReply){
  STimeout timeout;
  streamsize k = 4096;

  if(host.empty() || url.empty()){
    ERR_POST(Error << "[SHoundWWGetFile] Invalid empty arguments.");
    SetError(WWWGETFILE_ERROR);
    return WWWGETFILE_ERROR;
  }

  size_t args = url.find("?");
  string arg, path;
  if(args == string::npos){
    ERR_POST(Error << "[SHoundWWWGetFile] No ? found in url.");
    return WWWGETFILE_ERROR;
  }else{
    /* extract the arguments */
    arg = url.substr(args+1);
    /* the left over string is the path */
    path = url.erase(args);
  }

  SHoundUserSetTimeOut(USERTO);
  SHoundSetTimeOut(DEFAULTTO);
  timeout.sec = SHoundGetTimeOut();
  timeout.usec = 0;

  /* Create a high level stream to the http connector */
  /* kEmptyStr is a NCBI_C++ built in object to represent the empty string */ 
  CConn_HttpStream ios(host, path, kEmptyStr, kEmptyStr, 80, 0, &timeout, k);

  /* Pass it the args. It's suppose to pass in the args in the
     construction above, but for some reason it doesn't work.
     It might be the same bug as with the C library, see seqhoundapi.c 
     to see that. Instead, just write the args to the stream
     and then read back the seqhound response. */

  ios << arg << flush;     /* do NOT use endl to flush the iostream. seqrem is
			    not robust enough to detect the '\n' that endl adds. 
			    instead, just use flush. */

  /* reads until it hits a '\n' */
  getline(ios, SHoundReply, '\n');
  int i;
  if( (i = ParseSeqHoundError(SHoundReply)) == SEQHOUND_OK){
    /* when successful, the actually response is retrieved into SHoundReply */
    string replyTmp;
    SHoundReply = "";
    while(!ios.eof()){ 
      getline(ios, replyTmp, '\n');
      SHoundReply+=replyTmp+'\n';
    }

    /* - 1 removes the final '\n' we appended in the loop above */
    SHoundReply = SHoundReply.substr(0, SHoundReply.length() - 1);
    return SEQHOUND_OK;
  } else if( i == SEQHOUND_NULL ){
    ERR_POST(Error << "[WWWGetFile] SEQHOUND_NULL value detected.");
    return SEQHOUND_NULL;
  } else if (i == SEQHOUND_ERROR){
    ERR_POST(Error << "[WWWGetFile] SEQHOUND_ERROR detected.");
    return SEQHOUND_ERROR;
  } else if (i == SEQHOUND_BUFFER_ERROR){
    ERR_POST(Error << "[WWWGetFile] Empty reply buffer.");
    return SEQHOUND_BUFFER_ERROR;
  }else{
    ERR_POST(Error << "[WWWGetFile] Abnormal SeqHound error.");
    return SEQHOUND_ABNORMAL;
  }
}

/* if successful, returns SEQHOUND_OK and the value from the 
   server gets stored into SHoundReply else returns an error value */
bool SeqHound::SHoundIsServerAlive(string configFileKey, string fPath){

  if(configFileKey.empty()){
    ERR_POST(Error << "[SHoundIsServerAlive] Passed configFileKey was Empty.");
    return false;
  }
  if(fPath.empty()){
    ERR_POST(Error << "[SHoundIsServerAlive] Passed fPath was empty.");
    return false;
  }
  
  CNcbiIstream* is = new CNcbiIfstream("shoundrem.ini");
  CNcbiRegistry reg(*is, CNcbiRegistry::ePersistent); 

  if(reg.Empty()){
    ERR_POST(Error << "[SHoundIsServerAlive] .shoundremrc is empty");
    delete is;
    return false;
  }
  
  slri_sername = reg.Get("remote", configFileKey);
  if(slri_sername.empty()){
    ERR_POST(Error << "[SHoundIsServerAlive] Cannot get " << configFileKey << " from config file.");
    delete is;
    return false;
  }

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fPath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "[SHoundIsServerAlive] " << slri_sername << " is down.");
  }else if( i == SEQHOUND_OK ){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    if(SLRIMisc::IsBoolean(SHoundReply)){
      delete is;
      return NStr::StringToBool(SHoundReply);
    }else{
      ERR_POST(Error << "[SHoundIsServerAlive] Did not return a boolean.");
    }
  }else{
    ERR_POST(Error << "[SHoundIsServerAlive] " << slri_sername << " is dead.");
  }
  delete is;
  return false;
}

bool SeqHound::SHoundInit(bool NetEntrezOnToo, string appname){

  CNcbiIfstream is("shoundrem.ini");
  if(!is)
  {
	  ERR_POST(Fatal<<"[SHoundInit]: cannot open, or find shoundrem.ini");
	  return false;
  }
  CNcbiRegistry reg(is, CNcbiRegistry::ePersistent);

  if(reg.Empty()){
    ERR_POST(Error << "[SHoundInit] shoundrem.ini is empty");
    is.close();
    return false;
  }
  
  slri_cgipath = reg.Get("remote", "CGI");
  if(slri_cgipath.empty()){
    ERR_POST(Error << "[SHoundInit] Failed to get the CGI path");
	is.close();
    return false;
  }

  string fpath;
  string bool2str = NStr::BoolToString(NetEntrezOnToo);

  fpath = slri_cgipath+"?fnct=SeqHoundInit&NetEntrezOnToo="+bool2str+"&appname="+appname;
  ERR_POST(Info << "[SHoundInit] request " << fpath);
  if(SHoundIsServerAlive("server1", fpath)){
    is.close();
	isInited = true;
    return true;
  }
  ERR_POST(Warning << "[SHoundInit] Trying server2.");
  if(SHoundIsServerAlive("server2", fpath)){
	is.close();
    isInited = true;
    return true;
  }
  is.close();
  return false;
}

bool SeqHound::SHoundIsNetEntrezOn(void){
  
  string fpath = slri_cgipath+"?fnct=SeqHoundIsNetEntrezOn";
  ERR_POST(Info << "[SeqHoundIsNetEntrezOn] request " << fpath);
  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundIsNetEntrezOn");
  }else if( i == SEQHOUND_OK ) {
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    if(SLRIMisc::IsBoolean(SHoundReply)){
      return NStr::StringToBool(SHoundReply);
    }else{
      ERR_POST(Error << "[SHoundIsNetEntrezOn] Did not return a boolean.");
    }
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "[SHoundIsNetEntrezOn] NULL return value.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "[SHoundIsNetEntrezOn] Sent invalid parameters.");
  }else{
    ERR_POST(Error << "[SHoundIsNetEntrezOn] returned an error");
  }
  return false;
}

bool SeqHound::SHoundNetEntrezInit(const string appname){
  
  string fpath = slri_cgipath+"?fnct=SeqHoundNetEntrezInit&appname="+appname;
  ERR_POST(Info << "[SeqHoundNetEntrezInit] request " << fpath);
  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundNetEntrezInit");
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    if(SLRIMisc::IsBoolean(SHoundReply)){
      return NStr::StringToBool(SHoundReply);
    }else{
      ERR_POST(Error << "[SHoundNetEntrezInit] Did not return a boolean.");
    }
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "[SHoundNetEntrezInit] Returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "[SHoundNetEntrezInit] sent invalid parameters.");
  }else{
    ERR_POST(Error << "[SHoundNetEntrezInit] returned an error");
  }
  return false;
}

void SeqHound::SHoundFini(void){

  string fpath = slri_cgipath+"?fnct=SeqHoundFini";
  ERR_POST(Info << "[SHoundFini] request " << fpath);
  if(SHoundWWWGetFile(slri_sername, fpath, SHoundReply) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundFini");
  }
  return;
}

bool SeqHound::SHoundIsInited(void){

  return isInited;
}

string SeqHound::SHoundGetCurrentServer(void){

  if(slri_sername.empty()){
    return "SeqHound is not Inited. Call SHoundInit first.";
  }
 
  return slri_sername;
}

/******************************************************/
/* SeqId to/from Gi, Accession to/from Gi Conversions */
/******************************************************/
int SeqHound::SHoundFindAcc(string account){

  if(account.empty()){
    ERR_POST(Error << "SHoundFindAcc: Invalid parameter.");
    return SEQHOUND_ERROR;
  }
  
  string fpath = slri_cgipath+"?fnct=SeqHoundFindAcc&acc="+account;
  ERR_POST(Info << "SeqHoundFindAcc request " << fpath);
  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundFindAcc");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return atoi(SHoundReply.c_str());
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundFindAcc returned NULL.");
    return SEQHOUND_NULL;
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundFindAcc sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundFindAcc received an abnormal error");
    return SEQHOUND_ABNORMAL;
  }
}

list<int> *SeqHound::SHoundFindAccList( list<string>& stlAcc){
 
  list <int> *pvngifin = new list<int>;
  if(stlAcc.empty()){
    ERR_POST(Error << "SHoundFindAccList: Empty list given as parameter.");
    return pvngifin;
  }

  list<string>::iterator iter = stlAcc.begin();
  string fpath;
  while(iter != stlAcc.end()){
    list <string> Temp;
    MaxSizeList(stlAcc, Temp, iter);
    string s = SLRIMisc::SaveListStrAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundFindAccList&pacc="+s;
    ERR_POST(Info << "SHoundFindAccList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundFindAccList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(pvngifin, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundFindAccList returned NULL.");
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundFindAccList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundFindAccList received an abnormal error.");
    }
  }
  pvngifin->sort();
  pvngifin->unique();
  return pvngifin;
}

int SeqHound::SHoundFindName(string name){

  if(name.empty()){
    ERR_POST(Error << "SHoundFindName: Invalid parameter");
    return SEQHOUND_ERROR;
  }
  string fpath = slri_cgipath+"?fnct=SeqHoundFindName&name="+name;
  ERR_POST(Info << "SHoundFindName request: " << fpath);
  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SeqHoundWWWGetFile failed in SHoundFindName");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return atoi(SHoundReply.c_str());
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundFindName returned NULL.");
    return SEQHOUND_NULL;
  }else if( i == WWWGETFILE_ERROR ){
    ERR_POST(Error << "SHoundFindName sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "ShoundFindName received an abnormal error");
    return SEQHOUND_ABNORMAL;
  }
}


list<int> *SeqHound::SHoundFindNameList( list<string>& stlAcc){

  list <int> *pvngifin = new list<int>;
  if(stlAcc.empty()){
    ERR_POST(Error << "SHoundFindNameList: Empty list given as parameter.");
    return pvngifin;
  }

  list<string>::iterator iter = stlAcc.begin();
  string fpath;
  while(iter != stlAcc.end()){
    list <string> Temp;
    MaxSizeList(stlAcc, Temp, iter);
    string s = SLRIMisc::SaveListStrAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundFindNameList&pname="+s;
	ERR_POST(Info << "SeqHoundFindNameList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundFindAccList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(pvngifin, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundFindNameList returned NULL.");
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundFindNameList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundFindNameList received an abnormal error.");
    }
  }
  pvngifin->sort();
  pvngifin->unique();
  return pvngifin;
}


string SeqHound::SHoundAccFromGi(int Gi){

  if(Gi <= 0){
    ERR_POST(Error << "SHoundAccFromGi: Invalid Gi sent.");
    return kEmptyStr;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundAccFromGi&gi="+str;
  ERR_POST(Info << "SeqHoundAccFromGi request: " << fpath << "END");

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundAccFromGi");
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return SHoundReply;
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundAccFromGi returned NULL.");
  }else if( i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundAccFromGi sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundAccFromGi received an abnormal error");
  }
  return kEmptyStr;
}


list<string> *SeqHound::SHoundAccFromGiList( list<int>& stlGi ){

  list <string> *stlAcc = new list<string>;
  if(stlGi.empty()){
    ERR_POST(Error << "SHoundAccFromGiList: Empty list given as parameter.");
    return stlAcc;
  }

  list<int>::iterator iter = stlGi.begin();
  string fpath;
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundAccFromGiList&pgi="+s;
    ERR_POST(Info << "SeqHoundFindAccFromGiList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundAccFromGiList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrStr2ListStr(stlAcc, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundFindAccFromGiList returned NULL.");
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundFindAccFromGiList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundFindAccFromGiList received an abnormal error.");
    }
  }
  stlAcc->sort();
  stlAcc->unique();
  return stlAcc;
}

/* the calling function is responsible for freeing the object by delete. */
CSLRISeq_id *SeqHound::SHoundSeqIdFromGi( int Gi){

    if(Gi <= 0){
    ERR_POST(Error << "SHoundSeqIdFromGi: Invalid parameter.");
    return NULL;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundSeqIdFromGi&gi="+str;
  ERR_POST(Info << "SeqHoundFindSeqIdFromGi request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundSeqIdFromGi");
  }else if(i == SEQHOUND_OK){
    CSLRISeq_id *bs = new CSLRISeq_id();
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      asn_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *asn_in >> *bs;
    return bs;
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundSeqIdFromGi returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundSeqIdFromGi sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundSeqIdFromGi received an abnormal error.");
  }
  return NULL;
}

/* Caller must delete the CSLRIValNode object with delete. */
CSLRIValNode *SeqHound::SHoundSeqIdFromGiList(list<int>& stlGi){

  if(stlGi.empty()){
    ERR_POST(Error << "SHoundSeqIdFromGiList: Invalid parameter: Empty list.");
    return NULL;
  }
  CSLRIValNode *slriList = new CSLRIValNode();
  list<int>::iterator iter = stlGi.begin();
  string fpath;
  int flag = 0;
  while(iter != stlGi.end()){
    list<int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundSeqIdFromGiList&pgi="+s;
    ERR_POST(Info << "SeqHoundSeqIdFromGiList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundSeqIdFromGiList");
    }else if(i == SEQHOUND_OK){
      istringstream outstr(SHoundReply);
      auto_ptr<CObjectIStream>
	asn_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
      *asn_in >> *slriList;
      flag = 1;
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundSeqIdFromGiList returned NULL.");
    }else if (i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundSeqIdFromGiList sent invalid parameters.");
    } else{
      ERR_POST(Error << "SHoundSeqIdFromGiList received an abnormal error.");
    }
  }
  if(flag > 0){
    return slriList;
  }else{
    delete slriList;
    return NULL;
  }
}

/******************************************************************************
 Definition Line
******************************************************************************/

string SeqHound::SHoundGetDefline(int Gi){

  if(Gi <= 0){
    ERR_POST(Error << "SHoundGetDefline: Invalid parameters.");
    return kEmptyStr;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetDefline&gi="+str;
  ERR_POST(Info << "SeqHoundGetDefline request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetDefline");
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return SHoundReply;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetDefline returned NULL.");
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetDefline sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetDefline received an abnormal error");
  }
  return kEmptyStr;
}


list<string> *SeqHound::SHoundGetDeflineList( list<int>& stlGi ){

  list <string> *stlDef = new list<string>;
  if(stlGi.empty()){
    ERR_POST(Error << "SHoundGetDeflineList: Empty list given as parameter.");
    return stlDef;
  }

  list<int>::iterator iter = stlGi.begin();
  string fpath;
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundGetDeflineList&pgi="+s;
    ERR_POST(Info << "SeqHoundGetDeflineList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetDeflineList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrStr2ListStr(stlDef, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundGetDeflineList returned NULL.");
    }else if (i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundGetDeflineList sent invalid parameters.");
    } else{
      ERR_POST(Error << "SHoundGetDeflineList received an abnormal error.");
    }
  }
  stlDef->sort();
  stlDef->unique();
  return stlDef;
}

/**************************************************/
/* Sequence and Structure checks                  */
/**************************************************/
/* SHoundMoleculeType returns the type of the molecule given its gi and goid
 * returns: protein, dna, rna or null, this function deprecates SHoundIsProtein
 */
string SeqHound::SHoundMoleculeType(int Gi){
  
  if(Gi <=0){
	ERR_POST(Error << "SHoundMoleculeType: Invalid paramter.");
	return kEmptyStr;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundMoleculeType&gi="+str;
  ERR_POST(Info << "SeqHoundMoleculeType request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundMoleculeType.");
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return SHoundReply; 
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundMoleculeType: NULL return value.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundMoleculeType: sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundMoleculeType: received an abnormal error.");
  }
  return kEmptyStr;
}


bool SeqHound::SHoundIsProtein(int Gi){

  if(Gi <= 0){
    ERR_POST(Error << "SHoundIsProtein: Invalid paramter.");
    return false;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundIsProtein&gi="+str;
  ERR_POST(Info << "SeqHoundIsProtein request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundIsProtein");
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    if(SLRIMisc::IsBoolean(SHoundReply)){
      return NStr::StringToBool(SHoundReply);
    }else{
      ERR_POST(Error << "SHoundIsProtein: Did not return a boolean.");
    }
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundIsProtein: NULL return value.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundIsProtein sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundIsProtein: received an abnormal error.");
  }
  return false;
}

bool SeqHound::SHoundIsNRFirst(int Gi){

  if(Gi <= 0){
    ERR_POST(Error << "SHoundIsNRFirst: Invalid parameter.");
    return false;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundIsNRFirst&gi="+str;
  ERR_POST(Info << "SeqHoundIsNRFirst request: " << fpath);
  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundIsNRFirst");
  }else if(i == SEQHOUND_OK ){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    if(SLRIMisc::IsBoolean(SHoundReply)){
      return NStr::StringToBool(SHoundReply);
    }else{
      ERR_POST(Error << "SHoundIsNRFirst: Did not return a boolean.");
    }
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundIsNRFirst: NULL return value.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundIsNRFirst: sent invalid parameters to WWWGetFile.");
  }else{
    ERR_POST(Error << "SHoundIsNRFirst: received an abnormal error.");
  }
  return false;
}

bool SeqHound::SHoundExists(int Gi){

  if(Gi <= 0){
    ERR_POST(Error << "SHoundExists: Invalid Gi.");
    return false;
  }
  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundExists&gi="+str;
  ERR_POST(Info << "SeqHoundExists request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundExists");
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    if(SLRIMisc::IsBoolean(SHoundReply)){
      return NStr::StringToBool(SHoundReply);
    }else{
      ERR_POST(Error << "SHoundExists: did not return a boolean.");
    }
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundExists: NULL return value.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundExists: sent invalid parameters to WWWGetFile.");
  }else{
    ERR_POST(Error << "SHoundExists: received an abnormal error");
  }
  return false;
}

int SeqHound::SHound3DbWhat(int mmdbid){

  if(mmdbid <= 0){
    ERR_POST(Error << "SeqHound3DbWhat: Invalid parameter.");
    return SEQHOUND_ERROR;
  }
  string str = NStr::IntToString(mmdbid, false);

  string fpath = slri_cgipath+"?fnct=SeqHound3DbWhat&mmdbid="+str;
  ERR_POST(Info << "SeqHound3DbWhat request " << fpath);
  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHound3DbWhat");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return atoi(SHoundReply.c_str());
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHound3DbWhat returned NULL.");
    return SEQHOUND_NULL;
  }else if ( i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHound3DbWhat sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHound3DbWhat received an abnormal error");
    return SEQHOUND_ABNORMAL;
  }
}



int SeqHound::SHoundGetLargestMMDBID(void){

  string fpath = slri_cgipath+"?fnct=SeqHoundGetLargestMMDBID";
  ERR_POST(Info << "[SHoundGetLargestMMDBID] request " << fpath);
  
  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetLargestMMDBID");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    cout << atoi(SHoundReply.c_str()) <<endl;
    return atoi(SHoundReply.c_str());
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetLargestMMDBID returned NULL.");
    return SEQHOUND_NULL;
  }else if ( i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetLargestMMDBID sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundGetLargestMMDBID received an abnormal error");
    return SEQHOUND_ABNORMAL;
  }
}

bool SeqHound::SHound3DExists(int mmdbid){

  if(mmdbid <= 0){
    ERR_POST(Error << "SHound3DExists: Invalid parameters.");
    return false;
  }

  string str = NStr::IntToString(mmdbid, false);
  string fpath = slri_cgipath+"?fnct=SeqHound3DExists&mmdbid="+str;
  ERR_POST(Info << "SeqHound3DExists request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundExists");
    return false;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    if(SLRIMisc::IsBoolean(SHoundReply)){
      return NStr::StringToBool(SHoundReply);
    }else{
      ERR_POST(Error << "SHound3DExists: did not return a boolean.");
    }
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHound3DExists: NULL return value.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHound3DExists: sent invalid parameters to WWWGetFile.");
  }else{
    ERR_POST(Error << "SHound3DExists: received an abnormal error");
  }
  return false;
}

int SeqHound::SHoundSequenceLength(int Gi){

  if(Gi <= 0){
    ERR_POST(Error << "SHoundSequenceLength: Invalid parameter. Negative/Zero Gi");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundSequenceLength&gi="+str;
  ERR_POST(Info << "SeqHoundSequenceLength request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundSequenceLength");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return atoi(SHoundReply.c_str());
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundSequenceLength returned NULL.");
    return SEQHOUND_NULL;
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundSequenceLength sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundSequenceLength received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}
/*************************************************************
 Sequence fetch - Bioseq
**************************************************************/
/* Caller must free the memory by using delete */
CBioseq *SeqHound::SHoundGetBioseq(int Gi){

  if(Gi <= 0){
    ERR_POST(Error << "SHoundGetBioseq: Invalid GI.");
    return NULL;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetBioseq&gi="+str;
  ERR_POST(Info << "SeqHoundGetBioseq request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetBioseq");
  }else if(i == SEQHOUND_OK){
    CBioseq *bs = new CBioseq();
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      asn_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *asn_in >> *bs;
    return bs;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetBioseq: return value is NULL");
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetBioseq sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetBioseq: returned an abnormal value.");
  }
  return NULL;
}

/* Caller must delete the CSLRIValNode object with delete. */
CSLRIValNode *SeqHound::SHoundGetBioseqList(list<int>& stlGi){

  if(stlGi.empty()){
    ERR_POST(Error << "SHoundGetBioseqList: Invalid parameter: Empty list.");
    return NULL;
  }
  CSLRIValNode *slriList = new CSLRIValNode();
  list<int>::iterator iter =  stlGi.begin();
  string fpath;
  int flag = 0;
  while(iter != stlGi.end()){
    list<int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundGetBioseqList&pgi="+s;
    ERR_POST(Info << "SeqHoungGetBioseqList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetBioseqList");
    }else if(i == SEQHOUND_OK){
      istringstream outstr(SHoundReply);
      auto_ptr<CObjectIStream>
	asn_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
      *asn_in >> *slriList;
      flag = 1;
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundGetBioseqList returned NULL.");
    }else if (i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundGetBioseqList sent invalid parameters.");
    } else{
      ERR_POST(Error << "SHoundGetBioseqList received an abnormal error.");
    }
  }
  if(flag > 0){
    return slriList;
  }else{
    delete slriList;
    return NULL;
  }
}

/*************************************************************************
 Seqence fetch - BioseqSet
**************************************************************************/
/* Caller must free memory using delete */
CBioseq_set *SeqHound::SHoundGetBioseqSet(int Gi){


  if(Gi <= 0){
    ERR_POST(Error << "[GetBioseqSet]: Invalid GI.");
    return NULL;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetBioseqSet&gi="+str;
  ERR_POST(Info << "SeqHoundGetBioseqSet request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetBioseqSet");
  }else if(i == SEQHOUND_OK){
    CBioseq_set *bs = new CBioseq_set();
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      asn_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *asn_in >> *bs;
    return bs;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetBioseqSet: return value is NULL");
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetBioseqSet sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetBioseqSet: returned an abnormal value.");
  }
  return NULL;
}

/* Caller must delete the CSLRIValNode object with delete. */
CSLRIValNode *SeqHound::SHoundGetBioseqSetList(list<int>& stlGi){

  if(stlGi.empty()){
    ERR_POST(Error << "SHoundGetBioseqSetList: Invalid parameter: Empty list.");
    return NULL;
  }
  CSLRIValNode *slriList = new CSLRIValNode();
  list<int>::iterator iter = stlGi.begin();
  string fpath;
  int flag = 0;
  while(iter != stlGi.end()){
    list<int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundGetBioseqSetList&pgi="+s;
    ERR_POST(Info << "SeqHoundGetBioseqSetList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetBioseqSetList");
    }else if(i == SEQHOUND_OK){
      istringstream outstr(SHoundReply);
      auto_ptr<CObjectIStream>
	asn_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
      *asn_in >> *slriList;
      flag = 1;
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundGetBioseqSetList returned NULL.");
    }else if (i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundGetBioseqSetList sent invalid parameters.");
    } else{
      ERR_POST(Error << "SHoundGetBioseqSetList received an abnormal error.");
    }
  }
  if(flag > 0){
    return slriList;
  }else{
    delete slriList;
    return NULL;
  }
}

/*********************************************************************
 Sequence fetch - SeqEntry
**********************************************************************/
/* Caller must free object using delete */
CSeq_entry *SeqHound::SHoundGetSeqEntry(int Gi){

  if(Gi <= 0){
    ERR_POST(Error << "SHoundGetSeqEntry: Invalid GI.");
    return NULL;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetSeqEntry&gi="+str;
  ERR_POST(Info << "SeqHoundGetSeqEntry request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetSeqEntry");
  }else if(i == SEQHOUND_OK){
    CSeq_entry *bs = new CSeq_entry();
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      asn_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *asn_in >> *bs;
    return bs;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetSeqEntry: return value is NULL");
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetSeqEntry sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetSeqEntry: returned an abnormal value.");
  }
  return NULL;
}

/* Caller must delete the CSLRIValNode object with delete. */
CSLRIValNode *SeqHound::SHoundGetSeqEntryList(list<int>& stlGi){

  if(stlGi.empty()){
    ERR_POST(Error << "SHoundGetSeqEntryList: Invalid parameter: Empty list.");
    return NULL;
  }
  CSLRIValNode *slriList = new CSLRIValNode();
  string fpath;
  int flag = 0;
  list<int>::iterator iter = stlGi.begin();
  while(iter != stlGi.end()){
    list<int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundGetSeqEntryList&pgi="+s;
    ERR_POST(Info << "SeqHoundGetSeqEntryList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetSeqEntryList");
    }else if(i == SEQHOUND_OK){
      istringstream outstr(SHoundReply);
      auto_ptr<CObjectIStream>
	asn_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
      *asn_in >> *slriList;
      flag = 1;
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundGetSeqEntryList returned NULL.");
    }else if (i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundGetSeqEntryList sent invalid parameters.");
    } else{
      ERR_POST(Error << "SHoundGetSeqEntryList received an abnormal error.");
    }
  }
  if(flag > 0){
    return slriList;
  }else{
    delete slriList;
    return NULL;
  }
}
/*****************************************************************************
 Sequence fetch - Fasta
******************************************************************************/
/* Caller must free memory using delete */
CSLRIFasta *SeqHound::SHoundGetFasta(int Gi){

  if(Gi <= 0){
    ERR_POST(Error << "SHoundGetFasta: Invalid GI.");
    return NULL;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetFasta&gi="+str;
  ERR_POST(Info << "SeqHoundGetFasta request: " << fpath);
  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetFasta");
  }else if(i == SEQHOUND_OK){
    CSLRIFasta *bs = new CSLRIFasta();
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      asn_in(CObjectIStream::Open(eSerial_AsnText, outstr));
    *asn_in >> *bs;
    return bs;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetFasta: return value is NULL");
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetFasta sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetFasta: returned an abnormal value.");
  }
  return NULL;
}

/* retrieves a Fasta structure from a redundant group id.
   Caller must free memory using delete */
CSLRIFasta *SeqHound::SHoundGetFastaFromRedundantGroupID (int groupid){

  if(groupid <= 0){
    ERR_POST(Error << "SHoundGetFastaFromRedundantGroupID: Invalid Groupid");
    return NULL;
  }

  string str = NStr::IntToString(groupid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetFastaFromRedundantGroupID&gid="+str;
  ERR_POST(Info << "SeqHoundGetFastaFromRedundantGroupID request: " << fpath);
  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetFastaFromRedundantGroupID");
  }else if(i == SEQHOUND_OK){
    CSLRIFasta *bs = new CSLRIFasta();
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
    asn_in(CObjectIStream::Open(eSerial_AsnText, outstr));
    *asn_in >> *bs;
    return bs;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetFastaFromRedundantGroupID: return value is NULL");
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetFastaFromRedundantGroupID sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetFastaFromRedundantGroupID: returned an abnormal value.");
  }
  return NULL;
}


/* Caller must delete the CSLRIValNode object with delete. */
CSLRIValNode *SeqHound::SHoundGetFastaList(list<int>& stlGi){

  if(stlGi.empty()){
    ERR_POST(Error << "SHoundGetFastaList: Invalid parameter: Empty list.");
    return NULL;
  }
  CSLRIValNode *slriList = new CSLRIValNode();
  string fpath;
  int flag = 0;
  list<int>::iterator iter = stlGi.begin();
  while( iter != stlGi.end()){
    list<int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundGetFastaList&pgi="+s;
    ERR_POST(Info << "SeqHoundGetFastaList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetFastaList");
    }else if(i == SEQHOUND_OK){
      istringstream outstr(SHoundReply);
      auto_ptr<CObjectIStream>
	asn_in(CObjectIStream::Open(eSerial_AsnText, outstr));
      *asn_in >> *slriList;
      flag = 1;
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundGetFastaList returned NULL.");
    }else if (i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundGetFastaList sent invalid parameters.");
    } else{
      ERR_POST(Error << "SHoundGetFastaList received an abnormal error.");
    }
  }
  if(flag > 0){
    return slriList;
  }else{
    delete slriList;
    return NULL;
  }
}

/*****************************************************************************
 Sequence fetch - GenBank flat file
 writes into a file
******************************************************************************/

bool SeqHound::SHoundGetGenBankff(int Gi, ofstream& pfile){

  if(Gi <= 0){
    ERR_POST(Error << "[GetGenBankff] Invalid GI.");
    return false;
  }
  if(!pfile){
    ERR_POST(Error << "[GetGenBankff] File pointer is null.");
    return false;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetGenBankff&gi="+str;
  ERR_POST(Info << "SeqHoundGetGenBankff request: " << fpath);
  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetGenBankff");
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    pfile << SHoundReply << flush;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetGenBankff: return value is NULL");
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetGenBankff sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetGenBankff: returned an abnormal value.");
  }
  return true;
}

bool SeqHound::SHoundGetGenBankffList(list<int>& stlGi, ofstream& pfile){

  if(stlGi.empty()){
    ERR_POST(Error << "[GetGenBankffList] Empty list given as parameter.");
    return false;
  }
  if(!pfile){
    ERR_POST(Error << "[GetGenBankffList] File pointer is null.");
    return false;
  }

  string fpath;
  list<int>::iterator iter = stlGi.begin();
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundGetGenBankffList&pgi="+s;
    ERR_POST(Info << "SeqHoundGetGenBankffList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Info << "SeqHoundWWWGetFile failed in SHoundGetGenBankffList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      pfile << SHoundReply << endl;
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundGetGenBankffList returned NULL.");
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundGetGenBankffList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundGetGenBankffList received an abnormal error.");
    }
  }
  return true;
}

/*******************************************************************
 Sequence fetch - GenBank flat file
********************************************************************/

bool SeqHound::SHoundGetXMLBioseq(int Gi, string file_name){

  if(Gi <= 0){
    ERR_POST(Error << "[GetXMLBioseq] Invalid parameter: Negative/Zero gi.");
    return false;
  }
  if(file_name.empty()){
    ERR_POST(Error << "[GetXMLBioseq] Invalid parameter: Empty string passed in.");
    return false;
  }

  CBioseq *bs = SHoundGetBioseq(Gi);
  auto_ptr<CObjectOStream>
    xml_out(CObjectOStream::Open(file_name, eSerial_Xml));
  *xml_out << *bs;
  delete bs;
  return true;
}

bool SeqHound::SHoundGetXMLSeqEntry(int Gi, string file_name){

  if(Gi <= 0){
    ERR_POST(Error << "[GetXMLSeqEntry] Invalid parameter: Negative/Zero gi.");
    return false;
  }
  if(file_name.empty()){
    ERR_POST(Error << "Invalid parameter: Empty string passed in.");
    return false;
  }

  CSeq_entry *bs = SHoundGetSeqEntry(Gi);
  auto_ptr<CObjectOStream>
    xml_out(CObjectOStream::Open(file_name, eSerial_Xml));
  *xml_out << *bs;
  delete bs;
  return true;
}


/***********************************************************
 Structure fetch - Biostruc
************************************************************/
/* Caller must free memory using delete */
CBiostruc *SeqHound::SHoundGet3D(int mmdbid){

  if(mmdbid < 0){
    ERR_POST(Error << "SHoundGet3D: Invalid parameter.");
    return NULL;
  }

  string str = NStr::IntToString(mmdbid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGet3D&mmdbid="+str;
  ERR_POST(Info << "SeqHoundGet3D request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGet3D");
  }else if(i == SEQHOUND_OK){
    CBiostruc *bs = new CBiostruc();
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> *bs;
    return bs;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGet3D: return value is NULL");
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGet3D sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGet3D: returned an abnormal value.");
  }
  return NULL;
}

/***********************************************************
 Structure fetch - Biostruc
************************************************************/
/* Caller must free memory using delete */
CBiostruc *SeqHound::SHoundGetFixed3D(int mmdbid,int mdlLvl, int maxModels){

  if(mmdbid < 0){
    ERR_POST(Error << "SHoundGetFixed3D: Invalid parameter.");
    return NULL;
  }

  string str1 = NStr::IntToString(mmdbid, false);
  string str2 = NStr::IntToString(mdlLvl, false);
  string str3 = NStr::IntToString(maxModels, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetFixed3D&mmdbid="+str1+"&mdlLvl="+str2+"&maxModels="+str3;
  ERR_POST(Info << "SeqHoundGetFixed3D request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetFixed3D");
  }else if(i == SEQHOUND_OK){
    CBiostruc *bs = new CBiostruc();
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> *bs;
    return bs;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetFixed3D: return value is NULL");
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetFixed3D sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetFixed3D: returned an abnormal value.");
  }
  return NULL;
}


/* Caller must free memory using delete */
CBiostruc *SeqHound::SHoundGet3DfromPdbId(string pdbid){

  if(pdbid.empty()){
    ERR_POST(Error << "[Get3DfromPdbId] Invalid parameter. Empty string");
    return NULL;
  }

  string fpath = slri_cgipath+"?fnct=SeqHoundGet3DfromPdbId&pdbid="+pdbid;
  ERR_POST(Info << "SeqHoundGet3DfromPdbid request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGet3DfromPdbid");
  }else if(i == SEQHOUND_OK){
    CBiostruc *bs = new CBiostruc();
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> *bs;
    return bs;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGet3DfromPdbId: return value is NULL");
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGet3DfromPdbId sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGet3DfromPdbId: returned an abnormal value.");
  }
  return NULL;
}

bool SeqHound::SHoundGetXML3D(int mmdbid, string file){

  if(mmdbid <= 0){
    ERR_POST(Error << "SHoundXML3D: Invalid Gi.");
    return false;
  }
  if(file.empty()){
    ERR_POST(Error << "SHoundXML3D: Invalid file.");
    return false;
  }

  CBiostruc *bs = SHoundGet3D(mmdbid);
  auto_ptr<CObjectOStream>
    out(CObjectOStream::Open(file, eSerial_Xml));
  *out << *bs;
  delete bs;
  return true;
}

bool SeqHound::SHoundGetPDB3D(int mmdbid, string& file){

  if(mmdbid <= 0){
    ERR_POST(Error << "[GetPDB3D]: Invalid mmdbid.");
    return false;
  }
  if(file.empty()){
    ERR_POST(Error << "[GetPDB3D]: Invalid file name.");
    return false;
  }

  string str = NStr::IntToString(mmdbid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetPDB3DPL&mmdbid="+str;
  ERR_POST(Info << "SeqHoundGetPDB3D request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetPDB3D");
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    ofstream ofile(file.c_str());
    ofile << SHoundReply << endl;
    ofile.close();
    return true;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetPDB3D: return value is NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetPDB3D sent invalid paramters.");
  }else{
    ERR_POST(Error << "SHoundGetPDB3D: returned abnormal value.");
  }

  return false;
}

/*******************************************************************************
 SeqHound links
*******************************************************************************/
list<int> *SeqHound::SHoundGiFrom3D(int mmdbid, bool ProteinOnly){

  list <int> *stlGi = new list<int>;
  if(mmdbid <= 0){
    ERR_POST(Error << "SHoundGiFrom3D: Invalid parameter.");
    return stlGi;
  }
  string str = NStr::IntToString(mmdbid, false);
  string b2s = NStr::BoolToString(ProteinOnly);
  b2s = NStr::ToUpper(b2s);
  string fpath = slri_cgipath+"?fnct=SeqHoundGiFrom3D&mmdbid="+str+"&ProteinOnly="+b2s;
  ERR_POST(Info << "SeqHoundGiFrom3D request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGiFrom3D");
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGiFrom3D returned NULL.");
  }else if( i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGiFrom3D sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGiFrom3D received an abnormal error.");
  }
  return stlGi;
}

list<int> *SeqHound::SHoundGiFrom3DList(list<int>& stlMmdbid, bool ProteinOnly){

  list <int> *stlGi = new list<int>;
  if(stlMmdbid.empty()){
    ERR_POST(Error << "SHoundGiFrom3DList: Empty list of mmdbid.");
    return stlGi;
  }
  string b2s = NStr::BoolToString(ProteinOnly);
  b2s = NStr::ToUpper(b2s);
  string fpath;
  list<int>::iterator iter = stlMmdbid.begin();
  while(iter != stlMmdbid.end()){
    list <int> Temp;
    MaxSizeList(stlMmdbid, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundGiFrom3DList&pmmdbid="+s+"&ProteinOnly="+b2s;
    ERR_POST(Info << "SeqHoundGiFrom3DList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGiFrom3DList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
    }else if(i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundGiFrom3DList returned NULL.");
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundGiFrom3DList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundGiFrom3DList received an abnormal error.");
    }
  }
  stlGi->sort();
  stlGi->unique();
  return stlGi;
}

int SeqHound::SHoundGiFromPDBchain(string pdbcode, string chain){

  if(pdbcode.empty()){
    ERR_POST(Error << "[GiFromPDBchain] Invalid parameter.");
    return SEQHOUND_ERROR;
  }
  string fpath = slri_cgipath+"?fnct=SeqHoundGiFromPDBchain&pdbcode="+pdbcode+"&chain="+chain;
  ERR_POST(Info << "SeqHoundGiFromPDBchain request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGiFromPDBchain");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return atoi(SHoundReply.c_str());
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGiFromPDBchain returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGiFromPDBchain sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundGiFromPDBchain received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

int SeqHound::SHound3DFromGi(int Gi){

  if(Gi <= 0){
    ERR_POST(Error << "[3DFromGi] Invalid parameter.");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHound3DFromGi&gi="+str;
  ERR_POST(Info << "SeqHound3DFromGi request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHound3DFromGi");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return atoi(SHoundReply.c_str());
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHound3DFromGi returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHound3DFromGi sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHound3DFromGi received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

list<int> *SeqHound::SHound3DFromGiList( list<int>& stlGi){

  list <int> *stl3D = new list<int>;
  if(stlGi.empty()){
    ERR_POST(Error << "[3DFromGiList] Empty list given as parameter.");
    return stl3D;
  }

  string fpath;
  list<int>::iterator iter = stlGi.begin();
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHound3DFromGiList&pgi="+s;
    ERR_POST(Info << "SeqHound3DFromGiList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHound3DFromGiList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stl3D, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHound3DFromGiList returned NULL.");
    }else if (i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHound3DFromGiList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHound3DFromGiList received an abnormal error.");
    }
  }
  stl3D->sort();
  stl3D->unique();
  return stl3D;
}

int SeqHound::SHoundDNAFromProtein(int Gi){

  if(Gi <= 0){
    ERR_POST(Error << "[DNAFromProtein] Invalid parameter.");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundDNAFromProtein&gi="+str;
  ERR_POST(Info << "SeqHoundDNAFromProtein request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundDNAFromProtein");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return atoi(SHoundReply.c_str());
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundDNAFromProtein returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundDNAFromProtein sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundDNAFromProtein received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

list<int> *SeqHound::SHoundDNAFromProteinList( list<int>& stlGi){

  list <int> *stl3D = new list<int>;
  if(stlGi.empty()){
    ERR_POST(Error << "[DNAFromProteinList] Empty list given as parameter.");
    return stl3D;
  }

  string fpath;
  list<int>::iterator iter = stlGi.begin();
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundDNAFromProteinList&pgi="+s;
    ERR_POST(Info << "SeqHoundDNAFromProteinList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHound3DFromGiList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stl3D, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundDNAFromProteinList returned NULL.");
    }else if (i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundDNAFromProteinList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundDNAFromProteinList received an abnormal error.");
    }
  }
  stl3D->sort();
  stl3D->unique();
  return stl3D;
}


list<int> *SeqHound::SHoundProteinFromDNA(int Gi){

  list<int> *stlGi = new list<int>;
  if(Gi <= 0){
    ERR_POST(Error << "[ProteinFromDNA] Invalid parameter.");
    return stlGi;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundProteinFromDNA&gi="+str;
  ERR_POST(Info << "SeqHoundProteinFromDNA request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundProteinFromDNA");
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
    return stlGi;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundProteinFromDNA returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundProteinFromDNA sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundProteinFromDNA received an abnormal error.");
  }
  return stlGi;
}

list<int> *SeqHound::SHoundProteinFromDNAList( list<int>& stlGi){


  list <int> *stl3D = new list<int>;
  if(stlGi.empty()){
    ERR_POST(Error << "[PrtoeinFromDNAList] Empty list given as parameter.");
    return stl3D;
  }
  string fpath;
  list<int>::iterator iter = stlGi.begin();
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundProteinFromDNAList&pgi="+s;
    ERR_POST(Info << "SeqHoundProteinFromDNAList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundProteinFromDNAList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stl3D, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundProteinFromDNAList returned NULL.");
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundProteinFromDNAList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundProteinFromDNAList received an abnormal error.");
    }
  }
  stl3D->sort();
  stl3D->unique();
  return stl3D;
}


/* return pmids or muids given a Gi and the reference id.
 * if want PubMed ID set 'ReferenceType'==1, if want Medline ID,
 * set 'ReferenceType'==2.
 * This function deprecates SHoundMuidFromGi
 */
list<int> *SeqHound::SHoundGetReferenceIDFromGi(int Gi, int reftype){

  list<int> *stlRefID = new list<int>;
  if(Gi <= 0 || reftype <=0){
    ERR_POST(Error << "SHoundGetReferenceIDFromGi: Invalid parameter: Negative/Zero Gi or type");
    return stlRefID;
  }

  string str1 = NStr::IntToString(Gi, false);
  string str2 = NStr::IntToString(reftype, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetReferenceIDFromGi&gi="+str1+"&referencetype="+str2;
  ERR_POST(Info << "SeqHoundGetReferenceIDFromGi: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetReferenceIDFromGi");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlRefID, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetReferenceIDFromGi returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetReferenceIDFromGi sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetReferenceIDFromGi received an abnormal error.");
  }
  return stlRefID;
}


/* return pmids or muids given a Gi list and the reference id.
 * if want PubMed ID set 'ReferenceType'==1, if want Medline ID,
 * set 'ReferenceType'==2.
 * This function deprecates SHoundMuidFromGiList
 */
list<int> *SeqHound::SHoundGetReferenceIDFromGiList(list<int>& stlGi, int reftype){

  list<int> *stlRef = new list<int>;
  if(reftype <=0 ){
    ERR_POST(Error << "SHoundGetReferenceIDFromGiList: Invalid referencetype.");
    return stlRef;
  }
   
  if(stlGi.empty()) {
    ERR_POST(Error << "SHoundGetReferenceIDFromGiList: Empty gilistgiven as parameter.");
    return stlRef;
  }
  
  list<int>::iterator iter = stlGi.begin();
  string str2 = NStr::IntToString(reftype, false);
  string fpath;
  while(iter != stlGi.end()){
    list<int> temp;
    MaxSizeList(stlGi, temp, iter);
    string str1= SLRIMisc::SaveListIntAsString(temp);
    fpath = slri_cgipath+"?fnct=SeqHoundGetReferenceIDFromGiList&gilist="+str1+"&reftype="+str2; 
    ERR_POST(Info << "SeqHoundGetReferenceIDFromGiList request: " << fpath);
    int i;
    if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
        ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetReferenceIDFromGiList");
    }else if (i == SEQHOUND_OK){
    	size_t pos = SHoundReply.find_last_of("\n\0");
    	SHoundReply = SHoundReply.substr(0, pos);
    	SLRIMisc::StrInt2ListInt(stlRef, SHoundReply);
    }else if (i == SEQHOUND_NULL){
    	ERR_POST(Error << "SHoundGetReferenceIDFromGiList returned NULL.");
    }else if(i == WWWGETFILE_ERROR){
        ERR_POST(Error << "SHoundGetReferenceIDFromGiList sent invalid parameters.");
    }else{
    	ERR_POST(Error << "SHoundGetReferenceIDFromGiList received an abnormal error.");
    }
  }
  stlRef->sort();
  stlRef->unique();
  return stlRef;
}



list<int> *SeqHound::SHoundMuidFromGi(int Gi){

  list<int> *stlMu = new list<int>;
  if(Gi <= 0){
    ERR_POST(Error << "[MuidFromGi] Invalid paramter: Negative Gi.");
    return stlMu;
  }

  string gi = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundMuidFromGi&gi="+gi;
  ERR_POST(Info << "SeqHoundMuidFromGi request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundMuidFromGi");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlMu, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundMuidFromGi returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundMuidFromGi sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundMuidFromGi received an abnormal error.");
  }
  return stlMu;
}

list<int> *SeqHound::SHoundMuidFromGiList( list<int>& stlGi){

  list <int> *stlMu = new list<int>;
  if(stlGi.empty()){
    ERR_POST(Error << "[MuidFromGiList] Empty list given as parameter.");
    return stlMu;
  }

  string fpath;
  list<int>::iterator iter = stlGi.begin();
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundMuidFromGiList&pgi="+s;
    ERR_POST(Info << "SeqHoundMuidFromGiList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundMuidFromGiList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlMu, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundMuidFromGiList returned NULL.");
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundMuidFromGiList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundMuidFromGiList received an abnormal error.");
    }
  }
  stlMu->sort();
  stlMu->unique();
  return stlMu;
}

/* Return the gis associated with a publication id 'ReferenceID'.
 * If 'ReferenceID' is a pubmed publication, set 'ReferenceType' == 1,
 * if 'ReferenceID' is a medline publication, set 'ReferenceType' == 2, 
 *
 * A publication may contain many gis, eg a high throughput publication.
 *
 * The return list may contain duplicates.
 */
list<int> *SeqHound::SHoundGiFromReferenceID(int refid, int reftype){

  list<int> *stlgi = new list<int>;
  if(refid <= 0 || reftype <=0){
    ERR_POST(Error << "SHoundGiFromReferenceID: Invalid parameter: Negative/Zero referenceid or type");
    return stlgi;
  }

  string str1 = NStr::IntToString(refid, false);
  string str2 = NStr::IntToString(reftype, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGiFromReferenceID&referenceid="+str1+"&referencetype="+str2;
  ERR_POST(Info << "SeqHoundGiFromReferenceID: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGiFromReferenceID");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlgi, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGiFromReferenceID returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGiFromReferenceID sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGiFromReferenceID received an abnormal error.");
  }
  return stlgi;
}



/* Return the gis associated with a publication list 'ReferenceList'.
 * If 'ReferenceList' contains pubmed publications, set 'ReferenceType' == 1,
 * if 'ReferenceList' contains medline publications, set 'ReferenceType' == 2, 
 *
 * The return list is unique and sorted.
 */
list<int> *SeqHound::SHoundGiFromReferenceList(list<int>& stlRef, int reftype){

  list<int> *stlGi = new list<int>;
  if(reftype <=0 ){
    ERR_POST(Error << "SHoundGiFromReferenceList: Invalid referencetype.");
    return stlGi;
  }
   
  if(stlRef.empty()) {
    ERR_POST(Error << "SHoundGiFromReferenceList: Empty referencelist given as parameter.");
    return stlGi;
  }
  
  list<int>::iterator iter = stlRef.begin();
  string str2 = NStr::IntToString(reftype, false);
  string fpath;
  while(iter != stlRef.end()){
  	list<int> temp;
	MaxSizeList(stlRef, temp, iter);
	string str1= SLRIMisc::SaveListIntAsString(temp);
	fpath = slri_cgipath+"?fnct=SeqHoundGiFromReferenceList&reflist="+str1+"&reftype="+str2; 
	ERR_POST(Info << "SeqHoundGiFromReferenceList request: " << fpath);
    int i;
    if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGiFromReferenceList");
  	}else if (i == SEQHOUND_OK){
    	size_t pos = SHoundReply.find_last_of("\n\0");
    	SHoundReply = SHoundReply.substr(0, pos);
    	SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
  	}else if (i == SEQHOUND_NULL){
    	ERR_POST(Error << "SHoundGiFromReferenceList returned NULL.");
  	}else if(i == WWWGETFILE_ERROR){
        ERR_POST(Error << "SHoundGiFromReferenceList sent invalid parameters.");
  	}else{
    	ERR_POST(Error << "SHoundGiFromReferenceList received an abnormal error.");
  	}
  }
  stlGi->sort();
  stlGi->unique();
  return stlGi;
}


list<int> *SeqHound::SHoundMuidFrom3D(int mmdbid){

  list<int> *stlMu = new list<int>;
  if(mmdbid <= 0){
    ERR_POST(Error << "[MuidFrom3D] Invalid paramter: Negative mmdbid.");
    return stlMu;
  }

  string str = NStr::IntToString(mmdbid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundMuidFrom3D&mmdbid="+str;
  ERR_POST(Info << "SeqHoundMuidFrom3D request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundMuidFrom3D");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlMu, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundMuidFrom3D returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundMuidFrom3D sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundMuidFrom3D received an abnormal error.");
  }
  return stlMu;
}

list<int> *SeqHound::SHoundMuidFrom3DList( list<int>& stlmmdbid){

  list <int> *stlMu = new list<int>;
  if(stlmmdbid.empty()){
    ERR_POST(Error << "[MuidFrom3DList] Empty list given as parameter.");
    return stlMu;
  }

  string fpath;
  list<int>::iterator iter = stlmmdbid.begin();
  while(iter != stlmmdbid.end()){
    list <int> Temp;
    MaxSizeList(stlmmdbid, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundMuidFrom3DList&pmmdbid="+s;
    ERR_POST(Info << "SeqHoundMuidFrom3DList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundMuidFrom3DList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlMu, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundMuidFrom3DList returned NULL.");
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundMuidFrom3DList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundMuidFrom3DList received an abnormal error.");
    }
  }
  stlMu->sort();
  stlMu->unique();
  return stlMu;
}


int SeqHound::SHoundTaxIDFromGi(int Gi){

  if(Gi <= 0){
    ERR_POST(Error << "[TaxIDFromGi] Invalid parameter.");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundTaxIDFromGi&gi="+str;
  ERR_POST(Info << "SeqHoundTaxIDFromGi request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundTaxIDFromGi");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return atoi(SHoundReply.c_str());
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundTaxIDFromGi returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundTaxIDFromGi sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundTaxIDFromGi received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

list<int> *SeqHound::SHoundTaxIDFromGiList( list<int>& stlGi){

  list <int> *stlTax = new list<int>;
  if(stlGi.empty()){
    ERR_POST(Error << "[TaxIDFromGiList] Empty list given as parameter.");
    return stlTax;
  }

  string fpath;
  list<int>::iterator iter = stlGi.begin();
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundTaxIDFromGiList&pgi="+s;
    ERR_POST(Info << "SeqHoundTaxIDFromGiList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundTaxIDFromGiList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlTax, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundTaxIDFromGiList returned NULL.");
    }else if (i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundTaxIDFromGiList sent invalide parameters.");
    }else{
      ERR_POST(Error << "SHoundTaxIDFromGiList received an abnormal error.");
    }
  }
  stlTax->sort();
  stlTax->unique();
  return stlTax;
}

list<int> *SeqHound::SHoundTaxIDFrom3D(int mmdbid){

  list<int> *stlTax = new list<int>;
  if(mmdbid <= 0){
    ERR_POST(Error << "[TaxIDFrom3D] Invalid paramter: Negative mmdbid.");
    return stlTax;
  }

  string str = NStr::IntToString(mmdbid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundTaxIDFrom3D&mmdbid="+str;
  ERR_POST(Info << "SeqHoundTaxIDFrom3D request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundTaxIDFrom3D");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlTax, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundTaxIDFrom3D returned NULL.");
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundTaxIDFrom3D sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundTaxIDFrom3D received an abnormal error.");
  }
  return stlTax;
}

list<int> *SeqHound::SHoundTaxIDFrom3DList( list<int>& stlmmdb){

  list <int> *stlTax = new list<int>;
  if(stlmmdb.empty()){
    ERR_POST(Error << "[TaxIDFrom3DList] Empty list given as parameter.");
    return stlTax;
  }

  string fpath;
  list<int>::iterator iter = stlmmdb.begin();
  while(iter != stlmmdb.end()){
    list <int> Temp;
    MaxSizeList(stlmmdb, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundTaxIDFrom3DList&pmmdbid="+s;
    ERR_POST(Info << "SeqHoundTaxIDFrom3DList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundTaxIDFrom3DList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlTax, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundTaxIDFrom3DList returned NULL.");
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundTaxIDFrom3DList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundTaxIDFrom3DList received an abnormal error.");
    }
  }
  stlTax->sort();
  stlTax->unique();
  return stlTax;
}

list<int> *SeqHound::SHoundProteinsFromTaxID(int taxid){

  list<int> *stlProt = new list<int>;
  if(taxid <= 0){
    ERR_POST(Error << "[ProteinsFromTaxID] Invalid paramter: Negative taxid.");
    return stlProt;
  }

  string str = NStr::IntToString(taxid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundProteinsFromTaxID&taxid="+str;
  ERR_POST(Info << "SeqHoundProteinsFromTaxID request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundProteinsFromTaxID");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlProt, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundProteinsFromTaxID returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundProteinsFromTaxID sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundProteinsFromTaxID received an abnormal error.");
  }
  return stlProt;
}

list<int> *SeqHound::SHoundProteinsFromTaxIDIII(int taxid){

  list<int> *stlProt = new list<int>;
  if(taxid <= 0){
    ERR_POST(Error << "[ProteinsFromTaxIDIII] Invalid paramter: Negative taxid.");
    return stlProt;
  }

  string str = NStr::IntToString(taxid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundProteinsFromTaxIDIII&taxid="+str;
  ERR_POST(Info << "SeqHoundProteinsFromTaxIDIII request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundProteinsFromTaxIDIII");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlProt, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundProteinsFromTaxIDIII returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundProteinsFromTaxIDIII sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundProteinsFromTaxIDIII received an abnormal error.");
  }
  return stlProt;
}

list<int> *SeqHound::SHoundProteinsFromTaxIDList( list<int>& stltaxid){

  list <int> *stlProt = new list<int>;
  if(stltaxid.empty()){
    ERR_POST(Error << "[ProteinsFromTaxIDList] Empty list given as parameter.");
    return stlProt;
  }

  string fpath;
  list<int>::iterator iter = stltaxid.begin();
  while(iter != stltaxid.end()){
    list <int> Temp;
    MaxSizeList(stltaxid, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundProteinsFromTaxIDList&ptaxid="+s;
    ERR_POST(Info << "SeqHoundProteinsFromTaxIDList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundProteinsFromTaxIDList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlProt, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundProteinsFromTaxIDList returned NULL.");
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundProteinsFromTaxIDList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundProteinsFromTaxIDList received an abnormal error.");
    }
  }
  stlProt->sort();
  stlProt->unique();
  return stlProt;
}

list<int> *SeqHound::SHoundDNAFromTaxID(int taxid){

  list<int> *stlDNA = new list<int>;
  if(taxid <= 0){
    ERR_POST(Error << "[DNAFromTaxID] Invalid paramter: Negative taxid.");
    return stlDNA;
  }

  string str = NStr::IntToString(taxid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundDNAFromTaxID&taxid="+str;
  ERR_POST(Info << "SeqHoundDNAFromTaxID request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundDNAFromTaxID");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlDNA, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundDNAFromTaxID returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundDNAFromTaxID sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundDNAFromTaxID received an abnormal error.");
  }
  return stlDNA;
}

list<int> *SeqHound::SHoundDNAFromTaxIDIII(int taxid){

  list<int> *stlDNA = new list<int>;
  if(taxid <= 0){
    ERR_POST(Error << "[DNAFromTaxIDIII] Invalid paramter: Negative taxid.");
    return stlDNA;
  }

  string str = NStr::IntToString(taxid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundDNAFromTaxIDIII&taxid="+str;
  ERR_POST(Info << "SeqHoundDNAFromTaxIDIII request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundDNAFromTaxIDIII");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlDNA, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundDNAFromTaxIDIII returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundDNAFromTaxIDIII sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundDNAFromTaxIDIII received an abnormal error.");
  }
  return stlDNA;
}

list<int> *SeqHound::SHoundDNAFromTaxIDList(list<int>& stltaxid){

  list <int> *stlDNA = new list<int>;
  if(stltaxid.empty()){
    ERR_POST(Error << "[DNAFromTaxIDList] Empty list given as parameter.");
    return stlDNA;
  }

  string fpath;
  list<int>::iterator iter = stltaxid.begin();
  while(iter != stltaxid.end()){
    list <int> Temp;
    MaxSizeList(stltaxid, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundDNAFromTaxIDList&ptaxid="+s;
    ERR_POST(Info << "SeqHoundDNAFromTaxIDList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundDNAFromTaxIDList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlDNA, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundDNAFromTaxIDList returned NULL.");
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundDNAFromTaxIDList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundDNAFromTaxIDList received an abnormal error.");
    }
  }
  stlDNA->sort();
  stlDNA->unique();
  return stlDNA;
}

list<int> *SeqHound::SHoundProteinsFromOrganism(int taxid, unsigned char chromflags){

  list<int> *stli = new list<int>;
  if(taxid <= 0){
    ERR_POST(Error << "[ProteinsFromOrganism] Invalid paramter: Negative taxid");
    return stli;
  }
  if(chromflags <= 0){
    ERR_POST(Error << "[ProteinsFromOrganism] Invalid parameter: Negative chromflags.");
    return stli;
  }

  string chr = NStr::IntToString(chromflags, false);
  string str = NStr::IntToString(taxid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundProteinsFromOrganism&taxid="+str+"&chromflags="+chr;
  ERR_POST(Info << "SeqHoundProteinsFromOrganism request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundProteinsFromOrganism");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stli, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundProteinsFromOrganism returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundProteinsFromOrganism sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundProteinsFromOrganism received an abnormal error.");
  }
  return stli;
}

list<int> *SeqHound::SHoundProteinsFromOrganismList(list<int>& stltaxid, unsigned char chromflags){

  list <int> *stlDNA = new list<int>;
  if(stltaxid.empty()){
    ERR_POST(Error << "[ProteinsFromOrganismList] Empty list given as parameter.");
    return stlDNA;
  }
  if(chromflags <= 0){
    ERR_POST(Error << "[ProteinsFromOrganismList] Invalid parameter: Negative chromflags.");
    return stlDNA;
  }
  string str = NStr::IntToString(chromflags, false);
  list<int>::iterator iter =  stltaxid.begin();
  string fpath;
  while(iter != stltaxid.end()){
    list <int> Temp;
    MaxSizeList(stltaxid, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundProteinsFromOrganismList&ptaxid="+s+"&chromflags="+str;
    ERR_POST(Info << "SeqHoundProteinsFromOrganismList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundProteinsFromOrganismList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlDNA, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundProteinsFromOrganismList returned NULL.");
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundProteinsFromOrganismList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundProteinsFromOrganismList received an abnormal error.");
    }
  }
  stlDNA->sort();
  stlDNA->unique();
  return stlDNA;
}

list<int> *SeqHound::SHoundProteinsFromChromosome(int chromid){

  list<int> *stlProt = new list<int>;
  if(chromid <= 0){
    ERR_POST(Error << "[ProteinsFromChromosome] Invalid paramter: Negative chromid.");
    return stlProt;
  }

  string str = NStr::IntToString(chromid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundProteinsFromChromosome&chromid="+str;
  ERR_POST(Info << "SeqHoundProteinsFromChromosome request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundProteinsFromChromosome");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlProt, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundProteinsFromChromosome returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundProteinsFromChromosme sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundProteinsFromChromosome received an abnormal error.");
  }
  return stlProt;
}

list<int> *SeqHound::SHoundProteinsFromChromosomeList(list<int>& stlchrom){

  list <int> *stlProt = new list<int>;
  if(stlchrom.empty()){
    ERR_POST(Error << "[ProteinsFromChromosomeList] Empty list given as parameter.");
    return stlProt;
  }

  list<int>::iterator iter = stlchrom.begin();
  string fpath;
  while(iter != stlchrom.end()){
    list <int> Temp;
    MaxSizeList(stlchrom, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundProteinsFromChromosomeList&pchromid="+s;
    ERR_POST(Info << "SeqHoundProteinsFromChromosomeList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundProteinsFromChromosomeList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlProt, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundProteinsFromChromosomeList returned NULL.");
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundProteinsFromChromosomeList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundProteinsFromChromosomeList received an abnormal error.");
    }
  }
  stlProt->sort();
  stlProt->unique();
  return stlProt;
}

list<int> *SeqHound::SHoundAllGenomes(void){

  list<int> *stlGen = new list<int>;
  string fpath = slri_cgipath+"?fnct=SeqHoundAllGenomes";
  ERR_POST(Info << "SeqHoundAllGenomes request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundAllGenomes");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlGen, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundAllGenomes returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundAllGenomes sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundAllGenomes received an abnormal error.");
  }
  return stlGen;
}

list<int> *SeqHound::SHoundChromosomeFromGenome(int taxid, unsigned char chromflags){

  list<int> *stli = new list<int>;
  if(taxid <= 0){
    ERR_POST(Error << "[ChromosomeFromGenome] Invalid paramter: Negative taxid");
    return stli;
  }
  if(chromflags <= 0){
    ERR_POST(Error << "[ChromosomeFromGenome] Invalid parameter: Negative chromflags.");
    return stli;
  }

  string chr = NStr::IntToString(chromflags,false);
  string str = NStr::IntToString(taxid,false);
  string fpath = slri_cgipath+"?fnct=SeqHoundChromosomeFromGenome&taxid="+str+"&chromflags="+chr;
  ERR_POST(Info << "SeqHoundChromosomeFromGenome request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundChromosomeFromGenome");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stli, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundChromosomeFromGenome returned NULL.");
  }else{
    ERR_POST(Error << "SHoundChromosomeFromGenome received an abnormal error.");
  }
  return stli;
}

list<int> *SeqHound::SHoundChromosomeFromGenomeList(list<int>& stlGenom, unsigned char chromflags){

  list <int> *stlDNA = new list<int>;
  if(stlGenom.empty()){
    ERR_POST(Error << "[ChromosomeFromGenomeList] Empty list given as parameter.");
    return stlDNA;
  }
  if(chromflags <= 0){
    ERR_POST(Error << "[ChromosomeFromGenomeList] Invalid parameter: Negative chromflags.");
    return stlDNA;
  }
  string str = NStr::IntToString(chromflags, false);
  list<int>::iterator iter = stlGenom.begin(); 
  string fpath;
  while(iter != stlGenom.end()){
    list <int> Temp;
    MaxSizeList(stlGenom, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundChromosomeFromGenomeList&ptaxid="+s+"&chromflags="+str;
    ERR_POST(Info << "SeqHoundChromosomeFromGenomeList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundChromosomeFromGenomeList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlDNA, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundChromosomeFromGenomeList returned NULL.");
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundChromosomeFromGenomeList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundChromosomeFromGenomeList received an abnormal error.");
    }
  }
  stlDNA->sort();
  stlDNA->unique();
  return stlDNA;
}

list<int> *SeqHound::SHoundDNAFromOrganism(int taxid, unsigned char chromflags){

  list<int> *stli = new list<int>;
  if(taxid <= 0){
    ERR_POST(Error << "[DNAFromOrganism] Invalid parameter: Negative taxid");
    return stli;
  }
  if(chromflags <= 0){
    ERR_POST(Error << "[DNAFromOrganism] Invalid parameter: Negative chromflags.");
    return stli;
  }

  string chr = NStr::IntToString(chromflags, false);
  string str = NStr::IntToString(taxid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundDNAFromOrganism&taxid="+str+"&chromflags="+chr;
  ERR_POST(Info << "SeqHoundDNAFromOrganism request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundDNAFromOrganism");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stli, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundDNAFromOrganism returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundDNAFromOrganism sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundDNAFromOrganism received an abnormal error.");
  }
  return stli;
}

list<int> *SeqHound::SHoundDNAFromOrganismList(list<int>& stlTax, unsigned char chromflags){

  list <int> *stlDNA = new list<int>;
  if(stlTax.empty()){
    ERR_POST(Error << "[DNAFromOrganismList] Empty list given as parameter.");
    return stlDNA;
  }
  if(chromflags <= 0){
    ERR_POST(Error << "[DNAFromOrganismList] Invalid parameter: Negative chromflags.");
    return stlDNA;
  }
  string str = NStr::IntToString(chromflags, false);
  list<int>::iterator iter = stlTax.begin();
  string fpath;
  while(iter != stlTax.end()){
    list <int> Temp;
    MaxSizeList(stlTax, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundDNAFromOrganismList&ptaxid="+s+"&chromflags="+str;
    ERR_POST(Info << "SeqHoundDNAFromOrganismList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundDNAFromOrganismList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlDNA, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundDNAFromOrganismList returned NULL.");
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundDNAFromOrganismList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundDNAFromOrganismList received an abnormal error.");
    }
  }
  stlDNA->sort();
  stlDNA->unique();
  return stlDNA;
}

list<int> *SeqHound::SHoundDNAFromChromosome(int chromid){

  list<int> *stlDNA = new list<int>;
  if(chromid <= 0){
    ERR_POST(Error << "[DNAFromChromosome] Invalid paramter: Negative chromid.");
    return stlDNA;
  }

  string str = NStr::IntToString(chromid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundDNAFromChromosome&chromid="+str;
  ERR_POST(Info << "SeqHoundDNAFromChromosome request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundDNAFromChromosome");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlDNA, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundDNAFromChromosome returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundDNAFromChromsome sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundDNAFromChromosome received an abnormal error.");
  }
  return stlDNA;
}

list<int> *SeqHound::SHoundDNAFromChromosomeList(list<int>& stlchrom){

  list <int> *stlDNA = new list<int>;
  if(stlchrom.empty()){
    ERR_POST(Error << "[DNAFromChromosome] Empty list given as parameter.");
    return stlDNA;
  }

  list<int>::iterator iter = stlchrom.begin();
  string fpath;
  while(iter != stlchrom.end()){
    list <int> Temp;
    MaxSizeList(stlchrom, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundDNAFromChromosomeList&pchromid="+s;
    ERR_POST(Info << "SeqHoundDNAFromChromosomeList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundDNAFromChromosomeList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlDNA, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundDNAFromChromosomeList returned NULL.");
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundDNAFromChromosomeList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundDNAFromChromosomeList received an abnormal error.");
    }
  }
  stlDNA->sort();
  stlDNA->unique();
  return stlDNA;
}

list<int> *SeqHound::SHoundRedundantGroup(int Gi){

  list<int> *stlRed = new list<int>;
  if(Gi <= 0){
    ERR_POST(Error << "[RedundantGroup] Invalid paramter: Negative Gi.");
    return stlRed;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundRedundantGroup&gi="+str;
  ERR_POST(Info << "SeqHoundRedundantGroup request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundRedundantGroup");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlRed, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundRedundantGroup returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundRedundantGroup sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundRedundantGroup received an abnormal error.");
  }
  return stlRed;
}

list<int> *SeqHound::SHoundRedundantGroupList(list<int>& stlGi){

  list <int> *stlRed = new list<int>;
  if(stlGi.empty()){
    ERR_POST(Error << "[RedundantGroupList] Empty list given as parameter.");
    return stlRed;
  }

  list<int>::iterator iter = stlGi.begin();
  string fpath;
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundRedundantGroupList&pgi="+s;
    ERR_POST(Info << "SeqHoundRedundantGroupList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundRedundantGroupList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlRed, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundRedundantGroupList returned NULL.");
    }else if( i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundRedundantGroupList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundRedundantGroupList received an abnormal error.");
    }
  }
  stlRed->sort();
  stlRed->unique();
  return stlRed;
}

int SeqHound::SHoundRedundantGroupIDFromGI(int Gi){

  if(Gi <= 0){
    ERR_POST(Error << "[RedundantGroupIDFromGI] Invalid parameter.");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundRedundantGroupIDFromGI&gi="+str;
  ERR_POST(Info << "SeqHoundRedundantGroupIDFromGI request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundRedundantGroupIDFromGI");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return atoi(SHoundReply.c_str());
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundRedundantGroupIDFromGI returned NULL.");
    return SEQHOUND_NULL;
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundRedundantGroupIDFromGI sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundRedundantGroupIDFromGI received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

list<int> *SeqHound::SHoundRedundantGroupIDFromGIList(list<int>& stlGi){

  list <int> *stlRed = new list<int>;
  if(stlGi.empty()){
    ERR_POST(Error << "[RedundantGroupIDFromGIList] Empty list given as parameter.");
    return NULL;
  }

  list<int>::iterator iter = stlGi.begin();
  string fpath;
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundRedundantGroupIDFromGIList&pgi="+s;
    ERR_POST(Info << "SeqHoundRedundantGroupIDFromGIList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundRedundantGroupIDFromGIList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlRed, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundRedundantGroupIDFromGIList returned NULL.");
    }else if( i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundRedundantGroupIDFromGIList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundRedundantGroupIDFromGIList received an abnormal error.");
    }
  }
  stlRed->sort();
  stlRed->unique();
  return stlRed;
}

list<int> *SeqHound::SHoundRedundantGroupFromID(int group){

  list<int> *stlRed = new list<int>;
  if(group <= 0){
    ERR_POST(Error << "[RedundantGroupFromID] Invalid parameter: Negative/Zero group.");
    return stlRed;
  }

  string str = NStr::IntToString(group, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundRedundantGroupFromID&group="+str;
  ERR_POST(Info << "SeqHoundRedundantGroupFromID request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundRedundantGroupFromID");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlRed, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundRedundantGroupFromID returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundRedundantGroupFromID sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundRedundantGroupFromID received an abnormal error.");
  }
  return stlRed;
}

list<int> *SeqHound::SHoundRedundantGroupFromIDList(list<int>& stlgroup){

  list <int> *stlRed = new list<int>;
  if(stlgroup.empty()){
    ERR_POST(Error << "[RedundantGroupFromIDList] Empty list given as parameter.");
    return stlRed;
  }

  list<int>::iterator iter = stlgroup.begin();
  string fpath;
  while(iter != stlgroup.end()){
    list <int> Temp;
    MaxSizeList(stlgroup, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundRedundantGroupFromIDList&pgroup="+s;
    ERR_POST(Info << "SeqHoundRedundantGroupFromIDList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundRedundantGroupFromIDList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlRed, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundRedundantGroupFromIDList returned NULL.");
    }else if( i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundRedundantGroupFromIDList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundRedundantGroupFromIDList received an abnormal error.");
    }
  }
  stlRed->sort();
  stlRed->unique();
  return stlRed;
}

int SeqHound::SHoundFirstOfRedundantGroupFromID(int group){

  if(group <= 0){
    ERR_POST(Error << "[FirstOfRedundantGroupFromID] Invalid parameter.");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(group, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundFirstOfRedundantGroupFromID&group="+str;
  ERR_POST(Info << "SeqHoundFirstOfRedundantGroupFromID request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundFirstOfRedundantGroupFromID");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return atoi(SHoundReply.c_str());
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundFirstOfRedundantGroupFromID returned NULL.");
    return SEQHOUND_NULL;
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundFirstOfRedundantGroupFromID sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundFirstOfRedundantGroupFromID received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

list<int> *SeqHound::SHoundFirstOfRedundantGroupFromIDList(list<int>& stlgroup){

  list <int> *stlRed = new list<int>;
  if(stlgroup.empty()){
    ERR_POST(Error << "[RedundantGroupFromIDList] Empty list given as parameter.");
    return NULL;
  }

  list<int>::iterator iter = stlgroup.begin();
  string fpath;
  while(iter != stlgroup.end()){
    list <int> Temp;
    MaxSizeList(stlgroup, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundFirstOfRedundantGroupFromIDList&pgroup="+s;
    ERR_POST(Info << "SeqHoundFirstOfRedundantGroupFromIDList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundFirstOfRedundantGroupFromIDList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlRed, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundFirstOfRedundantGroupFromIDList returned NULL.");
    }else if( i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundFirstOfRedundantGroupFromIDList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundFirstOfRedundantGroupFromIDList received an abnormal error.");
    }
  }
  stlRed->sort();
  stlRed->unique();
  return stlRed;
}


list<int> *SeqHound::SHoundGetTaxChildNodes(int taxid){

  list<int> *stlTax = new list<int>;
  if(taxid <= 0){
    ERR_POST(Error << "[GetTaxChildNodes] Invalid parameter: Negative/Zero group.");
    return stlTax;
  }

  string str = NStr::IntToString(taxid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetTaxChildNodes&taxid="+str;
  ERR_POST(Info << "SeqHoundGetTaxChildNodes request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetTaxChildNodes");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlTax, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetTaxChildNodes returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetTaxChildNodes sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetTaxChildNodes received an abnormal error.");
  }
  return stlTax;
}

list<int> *SeqHound::SHoundGetTaxChildNodesList(list<int>& stlTaxid){

  list <int> *stlTax = new list<int>;
  if(stlTaxid.empty()){
    ERR_POST(Error << "[GetTaxChildNodesList] Empty list given as parameter.");
    return stlTax;
  }

  list<int>::iterator iter = stlTaxid.begin();
  string fpath;
  while(iter != stlTaxid.end()){
    list <int> Temp;
    MaxSizeList(stlTaxid, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundGetTaxChildNodesList&ptaxid="+s;
    ERR_POST(Info << "SeqHoundGetTaxChildNodesList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetTaxChildNodesList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlTax, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundGetTaxChildNodesList returned NULL.");
    }else if( i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundGetTaxChildNodesList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundGetTaxChildNodesList received an abnormal error.");
    }
  }
  stlTax->sort();
  stlTax->unique();
  return stlTax;
}

list<int> *SeqHound::SHoundGetAllTaxProgeny(int taxid){

  list<int> *stlTaxProg = new list<int>;
  if(taxid <= 0){
    ERR_POST(Error << "[GetAllTaxProgeny] Invalid parameter: Negative/Zero group.");
    return stlTaxProg;
  }

  string str = NStr::IntToString(taxid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetAllTaxProgeny&taxid="+str;
  ERR_POST(Info << "SeqHoundGetAllTaxProgeny request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetAllTaxProgeny");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlTaxProg, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetAllTaxProgeny returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetAllTaxProgeny sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetAllTaxProgeny received an abnormal error.");
  }
  return stlTaxProg;
}

int SeqHound::SHoundGetTaxParent(int taxid){

  if(taxid <= 0){
    ERR_POST(Error << "[GetTaxParent] Invalid parameter.");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(taxid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetTaxParent&taxid="+str;
  ERR_POST(Info << "SeqHoundGetTaxParent request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetTaxParent");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return atoi(SHoundReply.c_str());
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetTaxParent returned NULL.");
    return SEQHOUND_NULL;
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetTaxParent sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundGetTaxParent received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

list<int> *SeqHound::SHoundGetAllTaxAncestors(int taxid){

  list<int> *stlTaxAnc = new list<int>;
  if(taxid <= 0){
    ERR_POST(Error << "[GetAllTaxAncestors] Invalid parameter: Negative/Zero group.");
    return stlTaxAnc;
  }

  string str = NStr::IntToString(taxid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetAllTaxAncestors&taxid="+str;
  ERR_POST(Info << "SeqHoundGetAllTaxAncestors request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetAllTaxAncestors");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlTaxAnc, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetAllTaxAncestors returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetAllTaxAncestors sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetAllTaxAncestors received an abnormal error.");
  }
  return stlTaxAnc;
}

bool SeqHound::SHoundIsTaxMerged(int taxid){

  if(taxid <= 0){
    ERR_POST(Error << "[IsTaxMerged] SHoundIsTaxMerged: Invalid parameter.");
    return false;
  }

  string str = NStr::IntToString(taxid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundIsTaxMerged&taxid="+str;
  ERR_POST(Info << "SeqHoundIsTaxMerged request " << fpath);
  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundIsTaxMerged");
  }else if( i == SEQHOUND_OK ){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    if( SLRIMisc::IsBoolean(SHoundReply) ){
      return NStr::StringToBool(SHoundReply);
    }else {
      ERR_POST(Error << "SHoundIsTaxMerged: did not return a boolean.");
    }
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundIsTaxMerged returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundIsTaxMerged sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundIsTaxMerged received an abnormal error");
  }
  return false;
}

bool SeqHound::SHoundIsTaxDeleted(int taxid){

  if(taxid <= 0){
    ERR_POST(Error << "[IsTaxDeleted] SHoundIsTaxDeleted: Invalid parameter.");
    return false;
  }

  string str = NStr::IntToString(taxid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundIsTaxDeleted&taxid="+str;
  ERR_POST(Info << "SeqHoundIsTaxDeleted request " << fpath);
  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundIsTaxDeleted");
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    if(SLRIMisc::IsBoolean(SHoundReply) ){
      return NStr::StringToBool(SHoundReply);
    }else{
      ERR_POST(Error << "SHoundIsDeleted: did not return a boolean.");
    }
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundIsTaxDeleted returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundIsTaxDeleted sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundIsTaxDeleted received an abnormal error");
  }
  return false;
}

string SeqHound::SHoundGetTaxNameFromTaxIDByClass(int taxid, unsigned short nameClass){

  if(taxid <= 0){
    ERR_POST(Error << "SHoundGetTaxNameFromTaxIDByClass: Invalid parameter.");
    return kEmptyStr;
  }

  string str = NStr::IntToString(taxid, false);
  string cname = NStr::IntToString(nameClass, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetTaxNameFromTaxIDByClass&taxid="+str+"&type="+cname;
  ERR_POST(Info << "SeqHoundGetTaxNameFromTaxIDByClass request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetTaxNameFromTaxIDByClass");
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return SHoundReply;
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetTaxNameFromTaxIDByClass returned NULL.");
  }else if( i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetTaxNameFromTaxIDByClass sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetTaxNameFromTaxIDByClass received an abnormal error");
  }
  return kEmptyStr;
}

string SeqHound::SHoundGetTaxNameFromTaxID(int taxid){

  if(taxid <= 0){
    ERR_POST(Error << "[TaxNameFromTaxID] Invalid parameter. Negative/Zero taxid.");
    return kEmptyStr;
  }

  string str = NStr::IntToString(taxid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetTaxNameFromTaxID&taxid="+str;
  ERR_POST(Info << "SeqHoundGetTaxNameFromTaxID request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetTaxNameFromTaxID");
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return SHoundReply;
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetTaxNameFromTaxID returned NULL.");
  }else if( i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetTaxNameFromTaxID sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetTaxNameFromTaxID received an abnormal error");
  }
  return kEmptyStr;
}


string SeqHound::SHoundGetTaxLineageFromTaxID(int taxid, int type){

  if(taxid <= 0){
    ERR_POST(Error << "SHoundGetTaxLineageFromTaxID: Invalid parameters: Negative/Zero taxid");
    return kEmptyStr;
  }
  string str = NStr::IntToString(taxid, false);
  string str2 = NStr::IntToString(type, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetTaxLineageFromTaxID&taxid="+str+"&type="+str2;
  ERR_POST(Info << "SHoundGetTaxLineageFromTaxID request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetTaxLineageFromTaxID");
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return SHoundReply;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetTaxLineageFromTaxID returned NULL.");
  }else{
    ERR_POST(Error << "SHoundGetTaxLineageFromTaxID received an abnormal error.");
  }
  return kEmptyStr;
}

int SeqHound::SHoundGetBlastResult(int gi1, int gi2, CNBlast_Result_Set& pResult){

  if(gi1 <= 0 || gi2 <= 0){
    ERR_POST(Error << "SHoundGetBlastResult: Invalid GI");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(gi1, false);
  string str2 = NStr::IntToString(gi2, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetBlastResult&gi1="+str+"&gi2="+str2;
  ERR_POST(Error << "SHoundGetBlastResult request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetBlastResult");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){

    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> pResult; 
    return SEQHOUND_OK;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetBlastResult returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetBlastResult sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundGetBlastResult received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

int SeqHound::SHoundGetBlastSeqAlign(int gi1, int gi2, CSeq_align& psap){

  ERR_POST(Error << "SHoundGetBlastSeqAlign implementation not complete.");

  if(gi1 <= 0 || gi2 <= 0){
    ERR_POST(Error << "SHoundGetBlastSeqAlign: Invalid GI");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(gi1, false);
  string str2 = NStr::IntToString(gi2, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetBlastSeqAlign&gi1="+str+"&gi2="+str2;
  ERR_POST(Error << "SHoundGetBlastSeqAlign request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetBlastSeqAlign");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> psap;
    return SEQHOUND_OK;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetBlastSeqAlign returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetBlastSeqAlign sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundGetBlastSeqAlign received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

int SeqHound::SHoundNeighboursFromGiEx(int gi, CFLink_set& pNeighbours, float evalue, bool bLimit){

  if(gi <= 0){
    ERR_POST(Error << "SHoundNeighboursFromGiEx: Invalid GI");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(gi, false);
  string strf = NStr::DoubleToString(evalue);
  string bstr = NStr::BoolToString(bLimit);
  string fpath = slri_cgipath+"?fnct=SeqHoundNeighboursFromGiEx&gi="+str+"&evalue="+strf+"&bLimit="+bstr;
  ERR_POST(Info << "SHoundNeighboursFromGiEx request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundNeighboursFromGiEx");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){

    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> pNeighbours;
    return SEQHOUND_OK;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundNeighboursFromGiEx returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundNeighboursFromGiEx sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundNeighboursFromGiEx received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

int SeqHound::SHoundNeighboursFromGi(int gi, CFLink_set& pNeighbours, float evalue, bool bLimit){

  if(gi <= 0){
    ERR_POST(Error << "SHoundNeighboursFromGi: Invalid GI");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(gi, false);
  string strf = NStr::DoubleToString(evalue);
  string bstr = NStr::BoolToString(bLimit);
  string fpath = slri_cgipath+"?fnct=SeqHoundNeighboursFromGi&gi="+str+"&evalue="+strf+"&bLimit="+bstr;
  ERR_POST(Info << "SHoundNeighboursFromGi request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundNeighboursFromGi");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){

    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> pNeighbours;
    return SEQHOUND_OK;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundNeighboursFromGi returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundNeighboursFromGi sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundNeighboursFromGi received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

int SeqHound::SHoundNeighboursFromGiList(list<int>& stlGi, CFLink_set_List2& pNeighbours, float evalue){

  if(stlGi.empty()){
    ERR_POST(Error << "SHoundNeighboursFromGiList: Empty list given as parameter.");
    return SEQHOUND_ERROR;
  }
  int flag = 1;
  list<int>::iterator iter = stlGi.begin();
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string str = SLRIMisc::SaveListIntAsString(Temp);
    string strf = NStr::DoubleToString(evalue);
    string fpath = slri_cgipath+"?fnct=SeqHoundNeighboursFromGiList&gilist="+str+"&evalue="+strf;
    ERR_POST(Info << "SHoundNeighboursFromGiList request: " << fpath);
    int i;

    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundNeighboursFromGiList");
      flag = SEQHOUND_ERROR;
    }else if(i == SEQHOUND_OK){
      istringstream outstr(SHoundReply);
      auto_ptr<CObjectIStream>
	bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
      *bin_in >> pNeighbours;
      flag = SEQHOUND_OK;
    }else if(i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundNeighboursFromGiList returned NULL.");
      flag = SEQHOUND_NULL;
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundNeighboursFromGiList sent invalid parameters.");
      flag = WWWGETFILE_ERROR;
    }else{
      ERR_POST(Error << "SHoundNeighboursFromGiList received an abnormal error.");
      flag = SEQHOUND_ABNORMAL;
    }
  }
  return flag;
}
  
int SeqHound::SHoundNeighboursFromTaxID(int TaxID, CFLink_set_List2& pNeighboursList, float evalue){

  if(TaxID <= 0){
    ERR_POST(Error << "SHoundNeighboursFromTaxID: Invalid TaxID");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(TaxID, false);
  string strf = NStr::DoubleToString(evalue);
  string fpath = slri_cgipath+"?fnct=SeqHoundNeighboursFromTaxID&taxid="+str+"&evalue="+strf;
  ERR_POST(Info<< "SHoundNeighboursFromTaxID request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundNeighboursFromTaxID");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> pNeighboursList;
    return SEQHOUND_OK;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundNeighboursFromTaxID returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundNeighboursFromTaxID sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundNeighboursFromTaxID received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

int SeqHound::SHoundNeighboursOfNeighbours(int Gi, CFLink_set_List2& pNeighboursList, float evalue){

  if(Gi <= 0){
    ERR_POST(Error << "SHoundNeighboursOfNeighbours: Invalid gi");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(Gi, false);
  string strf = NStr::DoubleToString(evalue);
  string fpath = slri_cgipath+"?fnct=SeqHoundNeighboursOfNeighbours&gi="+str+"&evalue="+strf;
  ERR_POST(Info << "SHoundNeighboursOfNeighbours request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundNeighboursOfNeighbours");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> pNeighboursList;
    return SEQHOUND_OK;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundNeighboursOfNeighbours returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundNeighboursOfNeighbours sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundNeighboursOfNeighbours received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

int SeqHound::SHoundNeighboursOfNeighboursList(list<int>& stlGi, CFLink_set_List3& pNeighbours, float evalue){

  if(stlGi.empty()){
    ERR_POST(Error << "Empty list given as parameter.");
    return SEQHOUND_ERROR;
  }

  int flag = 1;
  list<int>::iterator iter = stlGi.begin();
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string str = SLRIMisc::SaveListIntAsString(Temp);
    string strf = NStr::DoubleToString(evalue);
    string fpath = slri_cgipath+"?fnct=SeqHoundNeighboursOfNeighboursList&gilist="+str+"&evalue="+strf;
    ERR_POST(Info<< "SHoundNeighboursOfNeighboursList request: " << fpath);
    int i;

    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundNeighboursOfNeighboursList");
      flag = SEQHOUND_ERROR;
    }else if(i == SEQHOUND_OK){
      istringstream outstr(SHoundReply);
      auto_ptr<CObjectIStream>
	bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
      *bin_in >> pNeighbours;
      flag = SEQHOUND_OK;
    }else if(i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundNeighboursOfNeighboursList returned NULL.");
      flag = SEQHOUND_NULL;
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundNeighboursOfNeighboursList sent invalid parameters.");
      flag = WWWGETFILE_ERROR;
    }else{
      ERR_POST(Error << "SHoundNeighboursOfNeighboursList received an abnormal error.");
      flag = SEQHOUND_ABNORMAL;
    }
  }
  return flag;
}

int SeqHound::SHoundNeighboursOfNeighboursFromTaxID(int taxID, CFLink_set_List3& pNeighbours, float evalue){

  if(taxID <= 0){
    ERR_POST(Error << "[NeighboursOfNeighboursFromTaxID]: Empty list given as parameter.");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(taxID, false);
  string strf = NStr::DoubleToString(evalue);
  string fpath = slri_cgipath+"?fnct=SeqHoundNeighboursOfNeighboursFromTaxID&taxid="+str+"&evalue="+strf;
  ERR_POST(Info<< "SHoundNeighboursOfNeighboursFromTaxID request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundNeighboursOfNeighboursList");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> pNeighbours;
    return SEQHOUND_OK;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundNeighboursOfNeighboursList returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundNeighboursOfNeighboursList sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundNeighboursOfNeighboursList received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}


int SeqHound::SHound3DNeighboursFromGiEx(int gi, CILink_set& p3DNeighbours, float evalue){

  if(gi <= 0){
    ERR_POST(Error << "SHound3DNeighboursFromGiEx: Invalid gi");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(gi, false);
  string strf = NStr::DoubleToString(evalue);
  string fpath = slri_cgipath+"?fnct=SeqHound3DNeighboursFromGiEx&gi="+str+"&evalue="+strf;
  ERR_POST(Info << "SHound3DNeighboursFromGiEx request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHound3DNeighboursFromGiEx");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> p3DNeighbours;
    return SEQHOUND_OK;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHound3DNeighboursFromGiEx returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHound3DNeighboursFromGiEx sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHound3DNeighboursFromGiEx received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

int SeqHound::SHound3DNeighboursFromGi(int gi, CILink_set& p3DNeighbours, float evalue){

  if(gi <= 0){
    ERR_POST(Error << "SHound3DNeighboursFromGi: Invalid gi");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(gi, false);
  string strf = NStr::DoubleToString(evalue);
  string fpath = slri_cgipath+"?fnct=SeqHound3DNeighboursFromGi&gi="+str+"&evalue="+strf;
  ERR_POST(Info << "SHound3DNeighboursFromGi request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHound3DNeighboursFromGi");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> p3DNeighbours;
    return SEQHOUND_OK;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHound3DNeighboursFromGi returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHound3DNeighboursFromGi sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHound3DNeighboursFromGi received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

int SeqHound::SHound3DNeighboursFromGiList(list<int> stlGi, CILink_set_List2& pNeighbours, float evalue){

  if(stlGi.empty()){
    ERR_POST(Error << "3DNeighboursFromGiList: Empty list given as parameter.");
    return SEQHOUND_ERROR;
  }

  int flag = 1;
  list<int>::iterator iter = stlGi.begin();
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string str = SLRIMisc::SaveListIntAsString(Temp);
    string strf = NStr::DoubleToString(evalue);
    string fpath = slri_cgipath+"?fnct=SeqHound3DNeighboursFromGiList&gilist="+str+"&evalue="+strf;
    ERR_POST(Info << "SHound3DNeighboursFromGiList request: " << fpath);
    int i;

    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHound3DNeighboursFromGiList");
      flag = SEQHOUND_ERROR;
    }else if(i == SEQHOUND_OK){
      istringstream outstr(SHoundReply);
      auto_ptr<CObjectIStream>
        bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
      *bin_in >> pNeighbours;
      flag = SEQHOUND_OK;
    }else if(i == SEQHOUND_NULL){
      ERR_POST(Error << "SHound3DNeighboursFromGiList returned NULL.");
      flag = SEQHOUND_NULL;
    }else if(i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHound3DNeighboursFromGiList sent invalid parameters.");
      flag = WWWGETFILE_ERROR;
    }else{
      ERR_POST(Error << "SHound3DNeighboursFromGiList received an abnormal error.");
      flag = SEQHOUND_ABNORMAL;
    }
  }
  return flag;
}

int SeqHound::SHound3DNeighboursFromTaxID(int taxid, CILink_set_List2& p3DNeighbours, float evalue){

  if(taxid <= 0){
    ERR_POST(Error << "SHound3DNeighboursFromTaxID: Invalid taxid");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(taxid, false);
  string strf = NStr::DoubleToString(evalue);
  string fpath = slri_cgipath+"?fnct=SeqHound3DNeighboursFromTaxID&taxid="+str+"&evalue="+strf;
  ERR_POST(Info << "SHound3DNeighboursFromTaxID request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHound3DNeighboursFromTaxID");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> p3DNeighbours;
    return SEQHOUND_OK;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHound3DNeighboursFromTaxID returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHound3DNeighboursFromTaxID sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHound3DNeighboursFromTaxID received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

int SeqHound::SHound3DListFromTaxID(int taxid, CILink_set& p3DNeighbours){

  if(taxid <= 0){
    ERR_POST(Error << "SHound3DListFromTaxID: Invalid taxid");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(taxid, false);
  string fpath = slri_cgipath+"?fnct=SeqHound3DListFromTaxID&taxid="+str;
  ERR_POST(Info << "SHound3DListFromTaxID request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHound3DListFromTaxID");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> p3DNeighbours;
    return SEQHOUND_OK;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHound3DListFromTaxID returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHound3DListFromTaxID sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHound3DListFromTaxID received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

int SeqHound::SHoundGiAndNumNeighboursList(CILink_set& pilsp){

  string fpath = slri_cgipath+"?fnct=SeqHoundGiAndNumNeighboursList";
  ERR_POST(Info << "SHoundGiAndNumNeighboursList request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGiAndNumNeighboursList");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> pilsp;
    return SEQHOUND_OK;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGiAndNumNeighboursList returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGiAndNumNeighboursList sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundGiAndNumNeighboursList received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

/* to get the return value, simply print pnum.
   int i = 0;
   SHoundNumNeighbours(i);
   cout << i;
*/
int SeqHound::SHoundNumNeighboursInDB(int& pnum){

  string fpath = slri_cgipath+"?fnct=SeqHoundNumNeighboursInDB";
  ERR_POST(Info << "SHoundNumNeighboursInDB request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundNumNeighboursInDB");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    pnum = atoi(SHoundReply.c_str());
    return SEQHOUND_OK;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundNumNeighboursInDB returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundNumNeighboursInDB sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundNumNeighboursInDB received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

/*retrieves a list of gene ontology ids from a locus link id*/
list<int> *SeqHound::SHoundGOIDFromLLID(int llid){

  list<int> *stlGoid = new list<int>;
  /* retrive goid from llid cross LL-LLink and LL-GO table, current implementation
	 allows llid to be negative 
  if(llid <= 0){
    ERR_POST(Error << "SHoundGOIDFromLLID: Invalid parameter: Negative/Zero llid.");
    return stlGi;
  } */

  string str = NStr::IntToString(llid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGOIDFromLLID&llid="+str;
  ERR_POST(Info << "SeqHoundGOIDFromLLID: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGOIDFromLLID");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlGoid, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGOIDFromLLID returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGOIDFromLLID sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGOIDFromLLID received an abnormal error.");
  }
  return stlGoid;
}

/*retrieves a OMIM id from a locus link id*/
int SeqHound::SHoundOMIMFromLLID(int llid){

  if(llid <= 0){
    ERR_POST(Error << "SHoundOMIMFromLLID: Invalid parameter. Negative/Zero llid.");
    return SEQHOUND_ERROR;
  } 

  string str = NStr::IntToString(llid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundOMIMFromLLID&llid="+str;
  ERR_POST(Info << "SeqHoundOMIMFromLLID: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundOMIMFromLLID");
	return SEQHOUND_ERROR;
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return  atoi (SHoundReply.c_str());
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundOMIMFromLLID returned NULL.");
	return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundOMIMFromLLID sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundOMIMFromLLID received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

/*retrieves a list of conserved domain ids from a locus link list*/
list<string> *SeqHound::SHoundCDDIDFromLLID(int llid){

  list<string> *stlcdd = new list<string>;
  if(llid <= 0){
    ERR_POST(Error << "SHoundCDDIDFromLLID: Invalid parameter: Negative/Zero llid.");
    return stlcdd;
  }

  string str = NStr::IntToString(llid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundCDDIDFromLLID&llid="+str;
  ERR_POST(Info << "SeqHoundCDDIDFromLLID request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundCDDIDFromLLID");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrStr2ListStr(stlcdd, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundCDDIDFromLLID returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundCDDIDFromLLID sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundCDDIDFromLLID received an abnormal error.");
  }
  return stlcdd;
}

/*retrieves a list of LLIDs From a goid and an evidence code */
list<int> *SeqHound::SHoundLLIDFromGOIDAndECode(int goid, string ecode ){

  list<int> *stlLLID = new list<int>;
  if(goid<= 0 ){
    ERR_POST(Error << "SHoundLLIDFromGOIDAndECode: Invalid parameter: goid negative.");
    return stlLLID;
  }
  
  if(ecode.empty()){
    ERR_POST(Error << "SHoundLLIDFromGOIDAndECode: Invalid parameter: empty ecode.");
    return stlLLID;
  }
  
  string str1 = NStr::IntToString(goid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundLLIDFromGOIDAndECode&goid="+str1+"&ecode="+ecode;
  ERR_POST(Info << "SeqHoundLLIDFromGOIDAndECode: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundLLIDFromGOIDAndECode");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlLLID, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundLLIDFromGOIDAndECode returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundLLIDFromGOIDAndECode sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundLLIDFromGOIDAndECode received an abnormal error.");
  }
  return stlLLID;
}

/*retrieves a list of LLIDs from an OMIM id*/
list<int> *SeqHound::SHoundLLIDFromOMIM (int omimid){

  list<int> *stlLLID = new list<int>;
  if(omimid <= 0){
    ERR_POST(Error << "SHoundLLIDFromOMIM: Invalid parameter: Negative/Zero omimid.");
    return stlLLID;
  }

  string str = NStr::IntToString(omimid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundLLIDFromOMIM&omimid="+str;
  ERR_POST(Info << "SeqHoundGiFromGOID: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundLLIDFromOMIM");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlLLID, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundLLIDFromOMIM returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundLLIDFromOMIM sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundLLIDFromOMIM received an abnormal error.");
  }
  return stlLLID;
}

/*retrieves a list of LLIDs From a conserved domain id*/
list<int> *SeqHound::SHoundLLIDFromCDDID(string cddid)
{
  list<int> *stlLLID = new list<int>;
  if( cddid.empty()){
    ERR_POST(Error << "SHoundLLIDFromCDDID: Invalid parameter: Empty cddid. ");
    return stlLLID;
  }

  string fpath = slri_cgipath+"?fnct=SeqHoundLLIDFromCDDID&cddid="+cddid;
  ERR_POST(Info << "SeqHoundLLIDFromCDDID: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundLLIDFromCDDID");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlLLID, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundLLIDFromCDDID returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundLLIDFromCDDID sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundLLIDFromCDDID received an abnormal error.");
  }
  return stlLLID;
}
	
list<int> *SeqHound::SHoundGiFromGOID(int GOID){

  list<int> *stlGi = new list<int>;
  if(GOID <= 0){
    ERR_POST(Error << "SHoundGiFromGOID: Invalid parameter: Negative/Zero GOID.");
    return stlGi;
  }

  string str = NStr::IntToString(GOID, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGiFromGOID&goid="+str;
  ERR_POST(Info << "SeqHoundGiFromGOID: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGiFromGOID");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGiFromGOID returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGiFromGOID sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGiFromGOID received an abnormal error.");
  }
  return stlGi;
}

list<int> *SeqHound::SHoundGiFromGOIDList(list<int>& stlGo){

  list <int> *stlGi = new list<int>;
  if(stlGo.empty()){
    ERR_POST(Error << "SHoundGiFromGOIDList: Empty list given as parameter.");
    return stlGi;
  }

  list<int>::iterator iter = stlGo.begin();
  string fpath;
  while(iter != stlGo.end()){
    list <int> Temp;
    MaxSizeList(stlGo, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundGiFromGOIDList&vnpgo="+s;
    ERR_POST(Info << "SeqHoundGiFromGOIDList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGiFromGOIDList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundGiFromGOIDList returned NULL.");
    }else if( i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundGiFromGOIDList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundGiFromGOIDList received an abnormal error.");
    }
  }
  stlGi->sort();
  stlGi->unique();
  return stlGi;
}


/*return all the gis with the given goid and evidence code*/
list<int> *SeqHound::SHoundGiFromGOIDAndECode(int goid, string ecode ){

  list<int> *stlgi = new list<int>;
  if(goid<= 0 ){
    ERR_POST(Error << "SHoundGiFromGOIDAndECode: Invalid parameter: goid negative.");
    return stlgi;
  }
 
  if(ecode.empty()){
    ERR_POST(Error << "SHoundGiFromGOIDAndECode: Invalid parameter: Empty string for parameter.");
    return stlgi;
  }

  string str1 = NStr::IntToString(goid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGiFromGOIDAndECode&goid="+str1+"&ecode="+ecode;
  ERR_POST(Info << "SeqHoundGiFromGOIDAndECode: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGiFromGOIDAndECode");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlgi, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGiFromGOIDAndECode returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGiFromGOIDAndECode sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGiFromGOIDAndECode received an abnormal error.");
  }
  return stlgi;
}


/*returns all the gis for given goid list and the evidence code, the return gilist is unique and sorted*/
list<int> *SeqHound::SHoundGiFromGOIDListAndECode(list<int>& stlGo, string ecode){

  list <int> *stlGi = new list<int>;
  if(stlGo.empty()){
    ERR_POST(Error << "SHoundGiFromGOIDListAndECode: Invalid parameter: Empty list given as parameter.");
    return stlGi;
  }
  if(ecode.empty()){
    ERR_POST(Error << "SHoundGiFromGOIDListAndECode: Invalid parameter: Empty string for parameter.");
    return stlGi;
  }
  list<int>::iterator iter = stlGo.begin();
  string fpath;
  while(iter != stlGo.end()){
    list <int> Temp;
    MaxSizeList(stlGo, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundGiFromGOIDListAndECode&golist="+s+"&ecode="+ecode;
    ERR_POST(Info << "SeqHoundGiFromGOIDListAndECode request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGiFromGOIDListAndECode");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundGiFromGOIDListAndECode returned NULL.");
    }else if( i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundGiFromGOIDListAndECode sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundGiFromGOIDListAndECode received an abnormal error.");
    }
  }
  stlGi->sort();
  stlGi->unique();
  return stlGi;
}

list<int> *SeqHound::SHoundGOIDFromGi(int Gi){

  list<int> *stlGOID = new list<int>;
  if(Gi <= 0){
    ERR_POST(Error << "SHoundGOIDFromGi: Invalid parameter: Negative/Zero Gi.");
    return stlGOID;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGOIDFromGi&gi="+str;
  ERR_POST(Info << "SeqHoundGOIDFromGi: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGOIDFromGi");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlGOID, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGOIDFromGi returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGOIDFromGi sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGOIDFromGi received an abnormal error.");
  }
  return stlGOID;
}

list<int> *SeqHound::SHoundGOIDFromGiList(list<int>& stlGi){

  list <int> *stlGo = new list<int>;
  if(stlGi.empty()){
    ERR_POST(Error << "SHoundGOIDFromGiList: Empty list given as parameter.");
    return stlGo;
  }

  list<int>::iterator iter = stlGi.begin();
  string fpath;
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundGOIDFromGiList&vnpgi="+s;
    ERR_POST(Info << "SeqHoundGOIDFromGiList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGOIDFromGiList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlGo, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundGOIDFromGiList returned NULL.");
    }else if( i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundGOIDFromGiList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundGOIDFromGiList received an abnormal error.");
    }
  }
  stlGo->sort();
  stlGo->unique();
  return stlGo;
}

list<int> *SeqHound::SHoundGOIDFromRedundantGi(int Gi){

  list<int> *stlGOID = new list<int>;
  if(Gi <= 0){
    ERR_POST(Error << "SHoundGOIDFromRedundantGi: Invalid parameter: Negative/Zero Gi.");
    return stlGOID;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGOIDFromRedundantGi&gi="+str;
  ERR_POST(Info << "SeqHoundGOIDFromRedundantGi: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGOIDFromRedundantGi");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlGOID, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGOIDFromRedundantGi returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGOIDFromRedundantGi sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGOIDFromRedundantGi received an abnormal error.");
  }
  return stlGOID;
}

list<int> *SeqHound::SHoundGOIDFromRedundantGiList(list<int>& stlGi){

  list <int> *stlGo = new list<int>;
  if(stlGi.empty()){
    ERR_POST(Error << "SHoundGOIDFromRedundantGiList: Empty list given as parameter.");
    return stlGo;
  }

  list<int>::iterator iter = stlGi.begin();
  string fpath;
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundGOIDFromRedundantGiList&vnpgi="+s;
    ERR_POST(Info << "SeqHoundGOIDFromRedundantGiList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGOIDFromRedundantGiList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlGo, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundGOIDFromRedundantGiList returned NULL.");
    }else if( i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundGOIDFromRedundantGiList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundGOIDFromRedundantGiList received an abnormal error.");
    }
  }
  stlGo->sort();
  stlGo->unique();
  return stlGo;
}


/*
 */
list<string> *SeqHound::SHoundGOECodeFromGiAndGOID(int Gi, int goid){

  list<string> *stlGOID = new list<string>;
  if(Gi <= 0 || goid <= 0){
    ERR_POST(Error << "SHoundGOECodeFromGiAndGOID: Invalid parameter: Negative/Zero Gi or goid");
    return stlGOID;
  }
  
  string str1 = NStr::IntToString(Gi,false);
  string str2 = NStr::IntToString(goid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGOECodeFromGiAndGOID&gi="+str1+"&goid="+str2;
  ERR_POST(Info << "SeqHoundGOECodeFromGiAndGOID: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGOECodeFromGiAndGOID");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrStr2ListStr(stlGOID, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGOECodeFromGiAndGOID returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGOECodeFromGiAndGOID sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGOECodeFromGiAndGOID received an abnormal error.");
  }
  return stlGOID;
}



list<int> *SeqHound::SHoundGOPMIDFromGiAndGOID(int Gi, int goid){

  list<int> *stlGOPM = new list<int>;
  if(Gi <= 0 || goid <=0){
    ERR_POST(Error << "SHoundGOPMIDFromGiAndGOID: Invalid parameter: Negative/Zero Gi or goid");
    return stlGOPM;
  }

  string str1 = NStr::IntToString(Gi, false);
  string str2 = NStr::IntToString(goid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGOPMIDFromGiAndGOID&gi="+str1+"&goid="+str2;
  ERR_POST(Info << "SeqHoundGOPMIDFromGiAndGOID: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGOPMIDFromGiAndGOID");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlGOPM, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGOPMIDFromGiAndGOID returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGOPMIDFromGiAndGOID sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGOPMIDFromGiAndGOID received an abnormal error.");
  }
  return stlGOPM;
}


list<int> *SeqHound::SHoundGiFromOMIM(int omimid){

  list<int> *stlGi = new list<int>;
  if(omimid <= 0){
    ERR_POST(Error << "SHoundGiFromOMIM: Invalid parameter: Negative/Zero omimid.");
    return stlGi;
  }

  string str = NStr::IntToString(omimid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGiFromOMIM&omimid="+str;
  ERR_POST(Info << "SeqHoundGiFromOMIM: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGiFromOMIM");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGiFromOMIM returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGiFromOMIM sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGiFromOMIM received an abnormal error.");
  }
  return stlGi;
}

list<int> *SeqHound::SHoundGiFromOMIMList(list<int>& stlomim){

  list <int> *stlGi = new list<int>;
  if(stlomim.empty()){
    ERR_POST(Error << "SHoundGiFromOMIMList: Empty list given as parameter.");
    return stlGi;
  }

  list<int>::iterator iter = stlomim.begin();
  string fpath;
  while(iter != stlomim.end()){
    list <int> Temp;
    MaxSizeList(stlomim, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundGiFromOMIMList&vnpomim="+s;
    ERR_POST(Info << "SeqHoundGiFromOMIMList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGiFromOMIMList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundGiFromOMIMList returned NULL.");
    }else if( i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundGiFromOMIMList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundGiFromOMIMList received an abnormal error.");
    }
  }
  stlGi->sort();
  stlGi->unique();
  return stlGi;
}

list<int> *SeqHound::SHoundOMIMFromGi(int Gi){

  list<int> *stlomim = new list<int>;
  if(Gi <= 0){
    ERR_POST(Error << "SHoundOMIMFromGi: Invalid parameter: Negative/Zero omimid.");
    return stlomim;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundOMIMFromGi&gi="+str;
  ERR_POST(Info << "SeqHoundOMIMFromGi: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundOMIMFromGi");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlomim, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundOMIMFromGi returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundOMIMFromGi sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundOMIMFromGi received an abnormal error.");
  }
  return stlomim;
}

list<int> *SeqHound::SHoundOMIMFromGiList(list<int>& stlGi){

  list <int> *stlomim = new list<int>;
  if(stlGi.empty()){
    ERR_POST(Error << "SHoundOMIMFromGiList: Empty list given as parameter.");
    return stlomim;
  }

  list<int>::iterator iter = stlGi.begin();
  string fpath;
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundOMIMFromGiList&vnpgi="+s;
    ERR_POST(Info << "SeqHoundOMIMFromGiList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundOMIMFromGiList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlomim, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundOMIMFromGiList returned NULL.");
    }else if( i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundOMIMFromGiList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundOMIMFromGiList received an abnormal error.");
    }
  }
  stlomim->sort();
  stlomim->unique();
  return stlomim;
}

list<int> *SeqHound::SHoundGiFromCDDID(string& cddid){

  list<int> *stlGi = new list<int>;
  if(cddid.empty()){
    ERR_POST(Error << "SHoundGiFromCDDID: Invalid parameter: Empty string for parameter.");
    return stlGi;
  }

  string fpath = slri_cgipath+"?fnct=SeqHoundGiFromCDDID&cddid="+cddid;
  ERR_POST(Info << "SeqHoundGiFromCDDID: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGiFromCDDID");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGiFromCDDID returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGiFromCDDID sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGiFromCDDID received an abnormal error.");
  }
  return stlGi;
}

list<int> *SeqHound::SHoundGiFromCDDIDList(list<string>& stlcdd){

  list <int> *stlGi = new list<int>;
  if(stlcdd.empty()){
    ERR_POST(Error << "SHoundGiFromCDDIDList: Empty list given as parameter.");
    return NULL;
  }

  list<string>::iterator iter = stlcdd.begin();
  string fpath;
  while(iter != stlcdd.end()){
    list <string> Temp;
    MaxSizeList(stlcdd, Temp, iter);
    string s = SLRIMisc::SaveListStrAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundGiFromCDDIDList&vnpcdd="+s;
    ERR_POST(Info << "SeqHoundGiFromCDDIDList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGiFromCDDIDList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundGiFromCDDIDList returned NULL.");
    }else if( i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundGiFromCDDIDList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundGiFromCDDIDList received an abnormal error.");
    }
  }
  stlGi->sort();
  stlGi->unique();
  return stlGi;
}

list<string> *SeqHound::SHoundCDDIDFromGi(int Gi){

  list<string> *stlcdd = new list<string>;
  if(Gi <= 0){
    ERR_POST(Error << "SHoundCDDIDFromGi: Invalid parameter: Negative/Zero omimid.");
    return stlcdd;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundCDDIDFromGi&gi="+str;
  ERR_POST(Info << "SeqHoundCDDIDFromGi request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundCDDIDFromGi");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrStr2ListStr(stlcdd, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundCDDIDFromGi returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundCDDIDFromGi sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundCDDIDFromGi received an abnormal error.");
  }
  return stlcdd;
}

list<string> *SeqHound::SHoundCDDIDFromGiList(list<int>& stlGi){

  list <string> *stlcdd = new list<string>;
  if(stlGi.empty()){
    ERR_POST(Error << "SHoundCDDIDFromGiList: Empty list given as parameter.");
    return stlcdd;
  }

  list<int>::iterator iter = stlGi.begin();
  string fpath;
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundCDDIDFromGiList&vnpgi="+s;
    ERR_POST(Info << "SeqHoundCDDIDFromGiList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundCDDIDFromGiList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrStr2ListStr(stlcdd, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundCDDIDFromGiList returned NULL.");
    }else if( i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundCDDIDFromGiList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundCDDIDFromGiList received an abnormal error.");
    }
  }
  stlcdd->sort();
  stlcdd->unique();
  return stlcdd;
}

float SeqHound::SHoundCDDScoreFromGi(int Gi, string cddid){

  if(Gi <= 0){
    ERR_POST(Error << "SHoundCDDScoreFromGi: Invalid parameter. Negative/Zero Gi");
    return SEQHOUND_ERROR;
  }
  if(cddid.empty()){
    ERR_POST(Error << "SHoundCDDScoreFromGi: Invalid parameter. Empty cddid string.");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundCDDScoreFromGi&gi="+str+"&cddid="+cddid;
  ERR_POST(Info << "SeqHoundCDDScoreFromGi request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundCDDScoreFromGi");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return atof(SHoundReply.c_str());
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundCDDScoreFromGi returned NULL.");
    return SEQHOUND_NULL;
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundCDDScoreFromGi sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundCDDScoreFromGi received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

list<int> *SeqHound::SHoundGiFromLLID(int llid){

  list<int> *stlGi = new list<int>; 
  if(llid <= 0){
    ERR_POST(Error << "SHoundGiFromLLID: Invalid parameter: Negative/Zero llid.");
    return stlGi;
  }

  string str = NStr::IntToString(llid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGiFromLLID&llid="+str;
  ERR_POST(Info << "SeqHoundGiFromLLID: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGiFromLLID");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGiFromLLID returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGiFromLLID sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGiFromLLID received an abnormal error.");
  }
  return stlGi;
}

list<int> *SeqHound::SHoundGiFromLLIDList(list<int>& stllid){

  list <int> *stlGi = new list<int>;
  if(stllid.empty()){
    ERR_POST(Error << "SHoundGiFromLLIDList: Empty list given as parameter.");
    return stlGi;
  }

  list<int>::iterator iter = stllid.begin();
  string fpath;
  while(iter != stllid.end()){
    list <int> Temp;
    MaxSizeList(stllid, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundGiFromLLIDList&vnpll="+s;
    ERR_POST(Info << "SeqHoundGiFromLLIDList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGiFromLLIDList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundGiFromLLIDList returned NULL.");
    }else if( i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundGiFromLLIDList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundGiFromLLIDList received an abnormal error.");
    }
  }
  stlGi->sort();
  stlGi->unique();
  return stlGi;
}

int SeqHound::SHoundLLIDFromGi(int Gi){

  if(Gi <= 0){
    ERR_POST(Error << "SHoundLLIDFromGi: Invalid parameter. Negative/Zero Gi");
    return SEQHOUND_ERROR;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundLLIDFromGi&gi="+str;
  ERR_POST(Info << "SeqHoundLLIDFromGi request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundLLIDFromGi");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return atoi(SHoundReply.c_str());
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundLLIDFromGi returned NULL.");
    return SEQHOUND_NULL;
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundLLIDFromGi sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundLLIDFromGi received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

list<int> *SeqHound::SHoundLLIDFromGiList(list<int>& stlGi){

  list <int> *stllid = new list<int>;
  if(stlGi.empty()){
    ERR_POST(Error << "SHoundLLIDFromGiList: Empty list given as parameter.");
    return stllid;
  }

  list<int>::iterator iter = stlGi.begin();
  string fpath;
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundLLIDFromGiList&vnpgi="+s;
    ERR_POST(Info << "SeqHoundLLIDFromGiList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundLLIDFromGiList");
    }else if(i == SEQHOUND_OK){
      size_t pos = SHoundReply.find_last_of("\n\0");
      SHoundReply = SHoundReply.substr(0, pos);
      SLRIMisc::StrInt2ListInt(stllid, SHoundReply);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundLLIDFromGiList returned NULL.");
    }else if( i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundLLIDFromGiList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundLLIDFromGiList received an abnormal error.");
    }
  }
  stllid->sort();
  stllid->unique();
  return stllid;
}

string SeqHound::SHoundLocusFromGi(int Gi){

  if(Gi <= 0){
    ERR_POST(Error << "SHoundLocusFromGi: Invalid parameter. Negative/Zero Gi");
    return kEmptyStr;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundLocusFromGi&gi="+str;
  ERR_POST(Info << "SeqHoundLocusFromGi request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundLocusFromGi");
    return kEmptyStr;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return SHoundReply;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundLocusFromGi returned NULL.");
    return kEmptyStr;
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundLocusFromGi sent invalid parameters.");
    return kEmptyStr;
  }else{
    ERR_POST(Error << "SHoundLocusFromGi received an abnormal error.");
    return kEmptyStr;
  }
}


list<int> *SeqHound::SHoundGODBGetRecordByReference(string refType){

  list<int> *stlGi = NULL;
  if(refType.empty()){
    ERR_POST(Error << "SHoundGODBGetRecordByReference: Invalid parameter: Empty string.");
    return stlGi;
  }

  string fpath = slri_cgipath+"?fnct=SeqHoundGODBGetRecordByReference&refType="+refType;
  ERR_POST(Info << "SeqHoundGODBGetRecordByReference request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGODBGetRecordByReference");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
	stlGi = new list<int>;
    SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGODBGetRecordByReference returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGODBGetRecordByReference sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGODBGetRecordByReference received an abnormal error.");
  }
  return stlGi;
}

list<int> *SeqHound::SHoundGODBGetChildrenOf(int goid){

  list<int> *stlGi = NULL;
  if(goid <= 0){
    ERR_POST(Error << "SHoundGODBGetChildrenOf: Invalid parameter: Negative/Zero goid.");
    return stlGi;
  }

  string str = NStr::IntToString(goid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGODBGetChildrenOf&goid="+str;
  ERR_POST(Info << "SeqHoundGODBGetChildrenOf request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGODBGetChildrenOf");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
	stlGi = new list<int>;
    SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGODBGetChildrenOf returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGODBGetChildrenOf sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGODBGetChildrenOf received an abnormal error.");
  }
  return stlGi;
}

list<int> *SeqHound::SHoundGODBGetParentOf(int goid){

  list<int> *stlGi = NULL;
  if(goid <= 0){
    ERR_POST(Error << "SHoundGODBGetParentOf: Invalid parameter: Negative/Zero goid.");
    return stlGi;
  }

  string str = NStr::IntToString(goid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGODBGetParentOf&goid="+str;
  ERR_POST(Info << "SeqHoundGODBGetParentOf request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGODBGetParentOf");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
	stlGi = new list<int>;
    SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGODBGetParentOf returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGODBGetParentOf sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGODBGetParentOf received an abnormal error.");
  }
  return stlGi;
}

list<int> *SeqHound::SHoundGODBGetAllAncestors(int goid){

  list<int> *stlGi = NULL;
  if(goid <= 0){
    ERR_POST(Error << "[GODBGetAllAncestors] Invalid parameter: Negative/Zero goid.");
    return stlGi;
  }

  string str = NStr::IntToString(goid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGODBGetAllAncestors&goid="+str;
  ERR_POST(Info << "SeqHoundGODBGetAllAncestors request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGODBGetAllAncestors");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    stlGi = new list<int>;
    SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGODBGetAllAncestors returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGODBGetAllAncestors sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGODBGetAllAncestors received an abnormal error.");
  }
  return stlGi;
}

list<int> *SeqHound::SHoundGODBGetAllChildren(int goid){

  list<int> *stlGi = NULL;
  if(goid <= 0){
    ERR_POST(Error << "[GODBGetAllChildren] Invalid parameter: Negative/Zero goid.");
    return stlGi;
  }

  string str = NStr::IntToString(goid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGODBGetAllChildren&goid="+str;
  ERR_POST(Info << "SeqHoundGODBGetAllChildren request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGODBGetAllChildren");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    stlGi = new list<int>;
    SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGODBGetAllChildren returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGODBGetAllChildren sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGODBGetAllChildren received an abnormal error.");
  }
  return stlGi;
}

string SeqHound::SHoundGODBGetNameByID(int goid){

  if(goid <= 0){
    ERR_POST(Error << "[GODBGetNameByID] Invalid parameter. Negative/Zero goid");
    return kEmptyStr;
  }

  string str = NStr::IntToString(goid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGODBGetNameByID&goid="+str;
  ERR_POST(Info << "SeqHoundGODBGetNameByID request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGODBGetNameByID");
    return kEmptyStr;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return SHoundReply;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGODBGetNameByID returned NULL.");
    return kEmptyStr;
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGODBGetNameByID sent invalid parameters.");
    return kEmptyStr;
  }else{
    ERR_POST(Error << "SHoundGODBGetNameByID received an abnormal error.");
    return kEmptyStr;
  }
}

string SeqHound::SHoundGODBGetClassification(int goid){

  if(goid <= 0){
    ERR_POST(Error << "[GODBGetClassification] Invalid parameter. Negative/Zero goid");
    return kEmptyStr;
  }

  string str = NStr::IntToString(goid, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGODBGetClassification&goid="+str;
  ERR_POST(Info << "SeqHoundGODBGetClassification request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGODBGetClassification");
    return kEmptyStr;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return SHoundReply;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGODBGetClassification returned NULL.");
    return kEmptyStr;
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGODBGetClassification sent invalid parameters.");
    return kEmptyStr;
  }else{
    ERR_POST(Error << "SHoundGODBGetClassification received an abnormal error.");
    return kEmptyStr;
  }
}

int SeqHound::SHoundAllGenomesWithScientificNames(CCLink_set& pAllGenomes){

  string fpath = slri_cgipath+"?fnct=SeqHoundAllGenomesWithScientificNames";
  ERR_POST(Info << "SHoundAllGenomesWithScientificNames request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundAllGenomesWithScienticNames");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> pAllGenomes;
    return SEQHOUND_OK;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundAllGenomesWithScientificNames returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundAllGenomesWithScientificNames sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundAllGenomesWithScientificNames received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}

int SeqHound::SHoundAllGenomesWithNumProteins(CILink_set& pAllGenomes){

  string fpath = slri_cgipath+"?fnct=SeqHoundAllGenomesWithNumProteins";
  ERR_POST(Info << "SHoundAllGenomesWithNumProteins request: " << fpath);
  int i;

  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR ){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundAllGenomesWithNumProteins");
    return SEQHOUND_ERROR;
  }else if(i == SEQHOUND_OK){
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> pAllGenomes;
    return SEQHOUND_OK;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundAllGenomesWithNumProteins returned NULL.");
    return SEQHOUND_NULL;
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundAllGenomesWithNumProteins sent invalid parameters.");
    return WWWGETFILE_ERROR;
  }else{
    ERR_POST(Error << "SHoundAllGenomesWithNumProteins received an abnormal error.");
    return SEQHOUND_ABNORMAL;
  }
}




list<int> *SeqHound::SHoundGetGisByNumberOfDomains(int numdom){

  list<int> *stlGi = new list<int>;
  if(numdom <= 0){
    ERR_POST(Error << "[GetGisByNumberOfDomains] Invalid parameter: Negative/Zero numdom.");
    return stlGi;
  }

  string str = NStr::IntToString(numdom, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetGisByNumberOfDomains&numdom="+str;
  ERR_POST(Info << "SeqHoundGetGisByNumberOfDomains request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetGisByNumberOfDomains");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetGisByNumberOfDomains returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetGisByNumberOfDomains sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetGisByNumberOfDomains received an abnormal error.");
  }
  return stlGi;
}

list<string> *SeqHound::SHoundGetDomainIdFromLabel(string label){

  list<string> *stlAcc = new list<string>;
  if(label.empty()){
    ERR_POST(Error << "[GetDomainIdFromLabel] Invalid parameter: Empty string.");
    return stlAcc;
  }

  string fpath = slri_cgipath+"?fnct=SeqHoundGetDomainIdFromLabel&label="+label;
  ERR_POST(Info << "SeqHoundGetDomainIdFromLabel request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetDomainIdFromLabel");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrStr2ListStr(stlAcc, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetDomainIdFromLabel returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetDomainIdFromLabel sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetDomainIdFromLabel received an abnormal error.");
  }
  return stlAcc;
}

list<int> *SeqHound::SHoundGetGisByDomainId(string domain){

  list<int> *stlGi = new list<int>;
  if(domain.empty()){
    ERR_POST(Error << "[GetGisByDomainId] Invalid parameter: Empty string.");
    return stlGi;
  }

  string fpath = slri_cgipath+"?fnct=SeqHoundGetGisByDomainId&domain_id="+domain;
  ERR_POST(Info << "SeqHoundGetGisByDomainId request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetGisByDomainId");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetGisByDomainId returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetGisByDomainId sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetGisByDomainId received an abnormal error.");
  }
  return stlGi;
}

string SeqHound::SHoundGetDomainLabelFromDomainId(string domain){

  if(domain.empty()){
    ERR_POST(Error << "[GetDomainLabelFromDomainId] Invalid parameter. Empty string accession");
    return kEmptyStr;
  }

  string fpath = slri_cgipath+"?fnct=SeqHoundGetDomainLabelFromDomainId&accession="+domain;
  ERR_POST(Info << "SeqHoundGetDomainLabelFromDomainId request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetDomainLabelFromDomainId");
    return kEmptyStr;
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return SHoundReply;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetDomainLabelFromDomainId returned NULL.");
    return kEmptyStr;
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetDomainLabelFromDomainId sent invalid parameters.");
    return kEmptyStr;
  }else{
    ERR_POST(Error << "SHoundGetDomainLabelFromDomainId received an abnormal error.");
    return kEmptyStr;
  }
}


list<int> *SeqHound::SHoundGetGisByDomainIdAndEvalue(string domain, float cutoff){

  list<int> *stlGi = new list<int>;
  if(domain.empty()){
    ERR_POST(Error << "[GetGisByDomainIdAndEvalue] Invalid parameter: Empty string.");
    return stlGi;
  }

  string str = NStr::DoubleToString(cutoff);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetGisByDomainIdAndEvalue&domain="+domain+"&evalue="+str;
  ERR_POST(Info << "SeqHoundGetGisByDomainIdAndEvalue request: " << fpath);
  int i;
  if( ( i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply) ) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetGisByDomainIdAndEvalue");
  }else if (i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    SLRIMisc::StrInt2ListInt(stlGi, SHoundReply);
  }else if (i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetGisByDomainIdAndEvalue returned NULL.");
  }else if(i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetGisByDomainIdAndEvalue sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetGisByDomainIdAndEvalue received an abnormal error.");
  }
  return stlGi;
}

/* Caller must free object using delete */
CStRpsdb *SeqHound::SHoundGetDomainsFromGi(int Gi){

  if(Gi <= 0){
    ERR_POST(Error << "SHoundGetDomainsFromGi: Invalid GI.");
    return NULL;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetDomainsFromGi&gi="+str;
  ERR_POST(Info << "SeqHoundGetDomainsFromGi request: " << fpath);
  int i;
 
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetDomainsFromGi");
  }else if(i == SEQHOUND_OK){
    CStRpsdb *bs = new CStRpsdb();
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> *bs;
    return bs;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetDomainsFromGi: return value is NULL");
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetDomainsFromGi sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetDomainsFromGi: returned an abnormal value.");
  }
  return NULL;
}

/* Caller must free memory using delete */
CStRpsdb *SeqHound::SHoundGetDomainsFromGiWithEvalue(int Gi, float cutoff){

  if(Gi <= 0){
    ERR_POST(Error << "SHoundGetDomainsFromGiWithEvalue: Invalid GI.");
    return NULL;
  }

  string str = NStr::IntToString(Gi, false);
  string strf = NStr::DoubleToString(cutoff);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetDomainsFromGiWithEvalue&gi="+str+"&evalue="+strf;
  ERR_POST(Info << "SeqHoundGetDomainsFromGiWithEvalue request: " << fpath);
  int i;
 
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetDomainsFromGiWithEvalue");
  }else if(i == SEQHOUND_OK){
    CStRpsdb *bs = new CStRpsdb();
    istringstream outstr(SHoundReply);
    auto_ptr<CObjectIStream>
      bin_in(CObjectIStream::Open(eSerial_AsnBinary, outstr));
    *bin_in >> *bs;
    return bs;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetDomainsFromGiWithEvalue: return value is NULL");
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetDomainsFromGiWithEvalue sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetDomainsFromGiWithEvalue: returned an abnormal value.");
  }
  return NULL;
}

/*return the accession number for the given retired gi*/
string SeqHound::SHoundGetAccFromRetiredGi(int Gi){

  if(Gi <= 0){
    ERR_POST(Error << "SHoundGetAccFromRetiredGi: Invalid parameters.");
    return kEmptyStr;
  }

  string str = NStr::IntToString(Gi, false);
  string fpath = slri_cgipath+"?fnct=SeqHoundGetAccFromRetiredGi&gi="+str;
  ERR_POST(Info << "SeqHoundGetAccFromRetiredGi request: " << fpath);

  int i;
  if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
    ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetAccFromRetiredGi");
  }else if(i == SEQHOUND_OK){
    size_t pos = SHoundReply.find_last_of("\n\0");
    SHoundReply = SHoundReply.substr(0, pos);
    return SHoundReply;
  }else if(i == SEQHOUND_NULL){
    ERR_POST(Error << "SHoundGetAccFromRetiredGi returned NULL.");
  }else if (i == WWWGETFILE_ERROR){
    ERR_POST(Error << "SHoundGetAccFromRetiredGi sent invalid parameters.");
  }else{
    ERR_POST(Error << "SHoundGetAccFromRetiredGi received an abnormal error");
  }
  return kEmptyStr;
}


/*converts a return string from the server and stores it into a map<int,
list<int>>
void  SeqHound::ProcessRetStr (string& retStr, map<int, list<int> > &mmap)
{
    TSList strList;
    SLRIMisc::DelimStr2ListStr(&strList, retStr, GROUP);
    mmap = SLRIMisc::List2Map(strList, mmap);
}
*/


/*converts a return string from the server and stores it into a map<int,
CSLRIFasta*>
void SeqHound::ProcessRetStr (string &retStr, map<int, CSLRIFasta*> &mmap)
{
    TSList strList;
    SLRIMisc::DelimStr2ListStr(&strList, retStr, GROUP);
    mmap = SLRIMisc:: List2Map(strList, mmap);
}
*/

/*caller must free this return map object with delete */
map<int, list<int>* > *SeqHound::SHoundRedundantGroupKeyedList(list<int>& stlGi)
{
  map<int, list<int>* > *mymap = new map<int, list<int>* >;
  if(stlGi.empty()){
    ERR_POST(Error << "[RedundantGroupKeyedList] Empty list given as parameter.");
    return mymap;
  }
 
 
  list<int>::iterator iter = stlGi.begin();
  string fpath;
  while(iter != stlGi.end()){
    list <int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundRedundantGroupKeyedList&pgi="+s;
    ERR_POST(Info << "SeqHoundRedundantGroupKeyedList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundRedundantGroupKeyedList");
    }else if(i == SEQHOUND_OK){
      TSList strList;
      SLRIMisc::StrStr2ListStr(&strList, SHoundReply, GROUP);
      SLRIMisc::List2Map(strList, *mymap);
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundRedundantGroupKeyedList returned NULL.");
    }else if( i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundRedundantGroupKeyedList sent invalid parameters.");
    }else{
      ERR_POST(Error << "SHoundRedundantGroupKeyedList received an abnormal error.");
    }
  }
  
  return mymap; 
}


/* retrives a list of Fasta structures from a list of gis
   Caller must free the return map object with delete. */
map<int, CSLRIFasta*> *SeqHound::SHoundGetFastaKeyedList(list<int>& stlGi)
{

  map<int, CSLRIFasta*> *mymap = new map<int, CSLRIFasta*>;
  if(stlGi.empty()){
    ERR_POST(Error << "SHoundGetFastaKeyedList: Invalid parameter: Empty list.");
    return mymap;
  }
  
  string fpath;
  int flag = 0;
  list<int>::iterator iter = stlGi.begin();
  while( iter != stlGi.end()){
    list<int> Temp;
    MaxSizeList(stlGi, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundGetFastaKeyedList&pgi="+s;
    ERR_POST(Info << "SeqHoundGetFastaKeyedList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetFastaiKeyedList");
    }else if(i == SEQHOUND_OK){
       TSList strList;
       SLRIMisc::StrStr2ListStr(&strList, SHoundReply, GROUP);
       SLRIMisc:: List2Map(strList, *mymap);      
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundGetFastaKeyedList returned NULL.");
    }else if (i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundGetFastaKeyedList sent invalid parameters.");
    } else{
      ERR_POST(Error << "SHoundGetFastaKeyedList received an abnormal error.");
    }
  }
  return mymap;
}

/* retrives a list of Fasta structures from a list of redundant group ids 
   Caller must free the return map object with delete. */
map<int, CSLRIFasta*> *SeqHound::SHoundGetFastaFromRedundantGroupIDKeyedList(list<int>& stlGroupID)
{

  map<int, CSLRIFasta*> *mymap = new map<int, CSLRIFasta*>;
  if(stlGroupID.empty()){
    ERR_POST(Error << "SHoundGetFastaFromRedundantGroupIDKeyedList: Invalid parameter: Empty list.");
    return mymap;
  }
  
  string fpath;
  int flag = 0;
  list<int>::iterator iter = stlGroupID.begin();
  while( iter != stlGroupID.end()){
    list<int> Temp;
    MaxSizeList(stlGroupID, Temp, iter);
    string s = SLRIMisc::SaveListIntAsString(Temp);
    fpath = slri_cgipath+"?fnct=SeqHoundGetFastaFromRedundantGroupIDKeyedList&pgid="+s;
    ERR_POST(Info << "SeqHoundGetFastaFromRedundantGroupIDKeyedList request: " << fpath);
    int i;
    if( (i = SHoundWWWGetFile(slri_sername, fpath, SHoundReply)) == SEQHOUND_ERROR){
      ERR_POST(Error << "SHoundWWWGetFile failed in SHoundGetFastaFromRedundantGroupIDList");
    }else if(i == SEQHOUND_OK){
       TSList strList;
       SLRIMisc::StrStr2ListStr(&strList, SHoundReply, GROUP);
       SLRIMisc:: List2Map(strList, *mymap);      
    }else if (i == SEQHOUND_NULL){
      ERR_POST(Error << "SHoundGetFastaFromRedundantGroupIDKeyedList returned NULL.");
    }else if (i == WWWGETFILE_ERROR){
      ERR_POST(Error << "SHoundGetFastaFromRedundantGroupIDKeyedList sent invalid parameters.");
    } else{
      ERR_POST(Error << "SHoundGetFastaFromRedundantGroupIDKeyedList received an abnormal error.");
    }
  }
  return mymap;
}


END_SCOPE(seqhound)

/*
$Log: Seqhound.cpp,v $
Revision 1.15  2004/11/25 16:06:58  hfeldman
fixed typo in new Fixed3D function

Revision 1.14  2004/11/22 14:56:59  mmatan

Modified shoundgetfixed3d so that is takes the same parameters as shoundget3dex.
See bug 3930 for details.

Revision 1.13  2004/11/18 19:09:46  mmatan
Added method SHoundGetFixed3D to the remote C++ api.  See bug 3930.

Revision 1.12  2004/06/17 21:01:19  ryao
fix bug in SHoundFindNameList

Revision 1.11  2004/06/15 18:25:45  ryao
Added six new locus link funcitons; Added SHoundGetFastaFromRedundantGroupID(KeyedList); Changed prototype of SHoundRedundantGroupKeyedList

Revision 1.10  2004/06/09 21:10:54  ryao
changed configuration file to compatiable with the NCBICXX toolkit; fix typoes in error messages

Revision 1.9  2004/05/03 19:49:27  rfarrall
fixed minor typo in comment for SHoundAllGenomesWithScientificNames per [bug 1339]

Revision 1.8  2004/04/05 18:29:21  ryao
Added SHoundGetFastaKeyedList SHoundRedundantGroupKeyedList

Revision 1.7  2004/02/16 17:20:37  ryao
remove SHoundGOECodeFromGi and SHoundGOPMIDFromGi, fix bug 1564.

Revision 1.6  2004/01/12 22:18:59  ryao
added SHoundGetLargestMMDBID

Revision 1.5  2003/11/28 16:30:09  ryao
Added SHoundGetReferenceIDFromGiList SHoundGiFromReferenceList SHoundGiFromGOIDListAndECode

Revision 1.3  2003/11/06 22:49:30  ryao
added SHoundGetReferenceIDFromGi SHoundGiFromReferenceID

Revision 1.2  2003/10/21 22:58:15  ryao
added SHoundMoleculeType SHoundGOPMIDFromGiAndGOID SHoundGOECodeFromGiAndGOID

Revision 1.1.1.20  2003/08/16 08:46:08  root
Automated import from Sourceforge.net

Revision 1.31  2003/08/14 19:12:21  haocl
added SHoundSequenceLength

Revision 1.30  2003/06/13 22:11:15  betel
Changes to config file opening

Revision 1.29  2003/05/23 20:38:53  haocl
changed config file to be Windows compatible

Revision 1.28  2003/02/27 22:13:42  micheld
Modified project files for combined asn files
changed ERROR to private member m_error, and functions that use it - incompatibility with windows defined ERROR

Revision 1.27  2002/11/11 21:29:36  betel
Fixed GO functions to return NULL when search fails

Revision 1.26  2002/10/18 20:30:46  haocl
changed the syntax of the namespace declaration

Revision 1.25  2002/10/17 20:03:27  haocl
removed comments with // and changed seqhound scope to work with MSCV++

Revision 1.24  2002/10/17 18:45:14  haocl
modified the seqhound scoping to work in MSCV++

Revision 1.22  2002/10/07 20:18:28  haocl
initial import of readme for c++ seqhound

Revision 1.21  2002/08/06 15:18:21  betel
Fixed sort and unique calls

Revision 1.20  2002/07/25 16:27:17  kaca
added back sorting and uniqueing for functions where the query is divided into smaller portions

Revision 1.19  2002/07/16 16:37:28  haocl
changed MaxSizeList to work with iterators, minimizes copying of list

Revision 1.18  2002/07/15 21:29:40  haocl
changed CSLRIValNode methods to return NULL

Revision 1.17  2002/07/15 19:41:00  haocl
changed methods that return objects to return pointers to objects

Revision 1.16  2002/07/11 20:40:59  haocl
implemented GetPDB3D

Revision 1.15  2002/07/11 19:22:39  haocl
fixed ProteinFromDNA to return a list

Revision 1.14  2002/07/10 19:35:21  haocl
renamed DomainLabelFromAccession and DomainAccessionFromLabel

Revision 1.13  2002/07/10 19:10:53  haocl
removed redundant sort and unique of returned lists

Revision 1.12  2002/07/10 16:17:20  haocl
added constructor and isInited functionality

Revision 1.11  2002/07/09 21:45:54  haocl
fixed GetFastaList from eSerial_AsnBinary to eSerial_AsnText

Revision 1.10  2002/07/08 17:04:15  haocl
added seqhound namespace

Revision 1.8  2002/07/05 18:14:43  haocl
minor changes

Revision 1.7  2002/07/05 14:15:24  haocl
Fixed functions that return SLRIValNodes and minor changes

Revision 1.6  2002/07/04 22:10:01  haocl
minor modifications

Revision 1.5  2002/07/04 20:54:50  haocl
moving file to slri/lib_cxx

Revision 1.4  2002/07/03 21:57:30  haocl
moved template function to .hpp file

Revision 1.3  2002/07/03 21:09:19  haocl
made changes to reflect use of slri_misc class

Revision 1.2  2002/07/02 21:39:18  haocl
initial import of nblast and slristruc libs and modified some seqhound stuff

Revision 1.9  2002/06/28 22:06:42  haocl
converted all auxiliary files into libraries, made corresponding changes in Seqhound and makefile

Revision 1.8  2002/06/28 21:05:42  haocl
all ASN functions working

Revision 1.7  2002/06/28 19:45:16  haocl
more ASN functionality added

Revision 1.6  2002/06/25 21:54:05  haocl
fixed more ASN.1 stuff

Revision 1.5  2002/06/25 16:09:00  haocl
added Id and Log tags

*/
