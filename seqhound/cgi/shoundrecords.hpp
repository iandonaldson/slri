#include <corelib/ncbistd.hpp>
#include <shoundtypes.hpp>
#include <ncbi.h>
#include <seqhound.h>
#ifndef SH_RECORDS
#define SH_RECORDS

using namespace ncbi;
using namespace shoundquery;

namespace shoundrecord
{
/* forward declaration for compiler because I
 * use the class before I define it. Definition below */
class CSHoundRecord;   


enum eContentType
{
   eHTML,
   eTEXT
};
	
/* Registry class */
class CSHoundRecordRegistry
{
private:
   list<CSHoundRecord*> *registry;
   list<CSHoundRecord*>::iterator RecIT;
public:
   CSHoundRecordRegistry();
   ~CSHoundRecordRegistry();

   void LoadRegistry(void);
   CSHoundRecord* GetDataRecord(eFormatType type, Int4 key);
   CSHoundRecord* GetRecord(eFormatType type); 
private:
   CSHoundRecord* GetDefaultRecord(void);

};  /* end CSHoundRecordRegistry */


/* All record types inherit from this base class */
class CSHoundRecord
{
private:
protected:
   string sRec;
   CharPtr pRec;
   Int4 iKey;
public:
   /* The public functions */
	
   CSHoundRecord();
   
   /* a class that has virtual functions must have a
    * virtual destructor. allows polymorphic destruction */
   virtual ~CSHoundRecord(void);

   virtual CSHoundRecord* Clone(void);
   /* returns the type of record */
   virtual eFormatType RecordType(void);
   virtual string DisplayRecord(eContentType eFormat);
   virtual string SetRecord(Pointer rec);
   virtual void SetKey(Int4 key);
   virtual Int4 GetKey(void);
private:
   /* The private functions */

   /* returns the record as a string */
   virtual string StringRecord(void);
   virtual string HTMLRecord(void);

}; /* end class CSHoundRecord */


class CSHoundDefline : public CSHoundRecord
{
private:   /* private class variables */
public:    /* public functions */

   CSHoundDefline(void);
   virtual ~CSHoundDefline(void);

   virtual CSHoundRecord* Clone(void);
   virtual eFormatType RecordType(void);
   virtual string DisplayRecord(eContentType eFormat);
   virtual string SetRecord(Pointer record);
   virtual void SetKey(Int4 record);
private:   /* private class functions */
   virtual string StringRecord(void);
   virtual string HTMLRecord(void);

}; /* end CSHoundDefline class */


class CSHoundFasta : public CSHoundRecord
{
private:
public:

   CSHoundFasta(void);
   virtual ~CSHoundFasta(void);

   virtual string SetRecord(Pointer rec);
   virtual CSHoundRecord* Clone(void);
   virtual eFormatType RecordType(void);
   virtual string DisplayRecord(eContentType eFormat);
   virtual void SetKey(Int4 key);	
private:
   virtual string HTMLRecord(void);
   virtual string StringRecord(void);
   
};  /* end CSHoundFasta class */


class CSHoundASN : public CSHoundRecord
{
private:
public:

   CSHoundASN(string n);
   CSHoundASN(void);
   virtual ~CSHoundASN(void);

   virtual CSHoundRecord* Clone(void);
   virtual eFormatType RecordType(void);
   virtual string DisplayRecord(eContentType eFormat);
   virtual string SetRecord(Pointer record);	
   virtual void SetKey(Int4 key);
private:
   virtual string HTMLRecord(void);
   virtual string StringRecord(void);
   
};  /* end CSHoundASN class */


class CSHoundXML : public CSHoundRecord
{
private:
public:

   CSHoundXML(void);
   virtual ~CSHoundXML(void);

   virtual CSHoundRecord* Clone(void);
   virtual eFormatType RecordType(void);
   virtual string DisplayRecord(eContentType eFormat);
   virtual string SetRecord(Pointer rec);	
   virtual void SetKey(Int4 key);
private:
   virtual string HTMLRecord(void); 
   virtual string StringRecord(void);
   
};  /* end CSHoundXML class */


class CSHoundGenBank : public CSHoundRecord
{
private:
public:

   CSHoundGenBank(string n);
   CSHoundGenBank(void);
   virtual ~CSHoundGenBank(void);

   virtual CSHoundRecord* Clone(void);
   virtual eFormatType RecordType(void);
   virtual string DisplayRecord(eContentType eFormat);
   virtual string SetRecord(Pointer rec);	
   virtual void SetKey(Int4 key);
private:
   virtual string HTMLRecord(void);
   virtual string StringRecord(void);
   
};  /* end CSHoundGenBank class */


class CSHoundProteinReport : public CSHoundRecord
{
private:
public:
   CSHoundProteinReport(string n);
   CSHoundProteinReport(void);
   virtual ~CSHoundProteinReport(void);

   virtual CSHoundRecord* Clone(void);
   eFormatType RecordType(void);
   string DisplayRecord(eContentType eFormat);
   string SetRecord(Pointer rec);
   void SetKey(Int4 key);
private:
   virtual string HTMLRecord(void);
   virtual string StringRecord(void);
};

/* Predicate class */
template<class T>
class PRecord : public unary_function<T, bool>
{
   const eFormatType test;
public:
   explicit PRecord(eFormatType f) : test(f) {}
   bool operator()(T e)
   { return e->RecordType() == test; }	   
};  /* end predicate class */





}  /* end namespace shoundrecords */
 

#endif

