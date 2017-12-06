#include "CDbFactory.h"
#include "IHsvSortedTableConfigurationEx.h"
#include "hsvsrtdb.h"

CDbFactory::~CDbFactory()
{
	for(std::map<int, SortedTable *>::iterator it = m_dbs.begin();it!=m_dbs.end(); ++it)
		delete it->second;
}


void CDbFactory::Init(int tableid)
{
	IHsvSortedTableConfigurationEx *conf = reinterpret_cast<IHsvSortedTableConfigurationEx*>(tableid);
	std::map<int, SortedTable *>::iterator it = m_dbs.find(tableid);

	if(it != m_dbs.end()) {
		/* SortedTable was already created. Clear the table entries */
		it->second->ClearTable();
	}
	else {
		/* Create a new SortedTable instance */
		if(conf)
		{
			SortedTable *tb = new SortedTable(conf);
			if(tb)
				m_dbs[tableid] = tb;
		}
	}
}

#define GET_TABLE(typ,def) \
	typ ret = def;\
	std::map<int, SortedTable *>::iterator it = m_dbs.find(tableid);\
	if(it != m_dbs.end())

int CDbFactory::GetNoOfRecords(int tableid)
{
	GET_TABLE(int,0)
	{
		ret = it->second->GetNoOfRecords();
	}
	return ret;
}

Bool CDbFactory::Insert(int tableid,Nat32 PrimaryKey,Nat32 SecondaryKey,Nat32 TeritiaryKey,Nat32 QuaternaryKey,Address data)
{
	GET_TABLE(Bool,0)
	{
		ret = it->second->InsertRecord(PrimaryKey,SecondaryKey,TeritiaryKey,QuaternaryKey,data);
	}
	return ret;
}
Bool CDbFactory::Update(int tableid,Nat32 PrimaryKey,Nat32 SecondaryKey,Nat32 TeritiaryKey,Nat32 QuaternaryKey,Address data)
{
	GET_TABLE(Bool,0)
	{
		ret = it->second->UpdateRecord(PrimaryKey,SecondaryKey,TeritiaryKey,QuaternaryKey,data);
	}
	return ret;
}
Bool CDbFactory::Delete(int tableid,Nat32 PrimaryKey,Nat32 SecondaryKey,Nat32 TeritiaryKey,Nat32 QuaternaryKey)
{
	GET_TABLE(Bool,0)
	{
		ret = it->second->DeleteRecord(PrimaryKey,SecondaryKey,TeritiaryKey,QuaternaryKey);
	}
	return ret;
}
Bool CDbFactory::SearchOnPrimaryKey(int tableid,Nat32 PrimaryKey,int * from,int * to)
{
	GET_TABLE(Bool,0)
	{
		ret = it->second->SearchOnPrimaryKey(PrimaryKey,from,to);
	}
	return ret;
}
Bool CDbFactory::SearchOnPrimaryAndSecondaryKey(int tableid,Nat32 PrimaryKey,Nat32 SecondaryKey,int * from,int * to)
{
	GET_TABLE(Bool,0)
	{
		ret = it->second->SearchOnPrimaryAndSecondaryKey(PrimaryKey,SecondaryKey,from,to);
	}
	return ret;
}
Bool CDbFactory::SearchOnPrimarySecondaryAndTeritiaryKey(int tableid,Nat32 PrimaryKey,Nat32 SecondaryKey,Nat32 TeritiaryKey,int * from,int * to)
{
	GET_TABLE(Bool,0)
	{
		ret = it->second->SearchOnPrimarySecondaryAndTeritiaryKey(PrimaryKey,SecondaryKey,TeritiaryKey,from,to);
	}
	return ret;
}
Bool CDbFactory::QueryOnKey(int tableid,Nat32 PrimaryKey,Nat32 SecondaryKey,Nat32 TeritiaryKey,Nat32 QuaternaryKey,Address data)
{
	GET_TABLE(Bool,0)
	{
		ret = it->second->QueryOnKey(PrimaryKey,SecondaryKey,TeritiaryKey,QuaternaryKey,data);
	}
	return ret;
}
Bool CDbFactory::QueryOnIndex(int tableid,int index,Address data)
{
	GET_TABLE(Bool,0)
	{
		ret = it->second->QueryOnIndex(index,data);
	}
	return ret;
}
Bool CDbFactory::SearchOnKey(int tableid,Nat32 PrimaryKey,Nat32 SecondaryKey,Nat32 TeritiaryKey,Nat32 QuaternaryKey,int * index)
{
	GET_TABLE(Bool,0)
	{
		ret = it->second->SearchOnKey(PrimaryKey,SecondaryKey,TeritiaryKey,QuaternaryKey,index);
	}
	return ret;
}
Bool CDbFactory::SearchOnKey2(int tableid,Nat32 PrimaryKey,Nat32 SecondaryKey,Nat32 TeritiaryKey,Nat32 QuaternaryKey,int op,int * index)
{
	GET_TABLE(Bool,0)
	{
		ret = it->second->SearchOnKey2(PrimaryKey,SecondaryKey,TeritiaryKey,QuaternaryKey,op,index);
	}
	return ret;
}
