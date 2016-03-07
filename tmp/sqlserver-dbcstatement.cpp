#include "sqlserver-dbcstatement.hpp"
#include <atldbcli.h>

using namespace ATL;

dbcStatementSqlServer::dbcStatementSqlServer(dbcSessionSqlServer *session,void * command):session_(session),command_(command)
{
}

dbcStatementSqlServer::~dbcStatementSqlServer()
{
	if(command_) delete (CCommand<CManualAccessor>*)command_;
}

int dbcStatementSqlServer::execute(wstring_t sqlstr)
{
	CDBPropSet propset(DBPROPSET_ROWSET);
	propset.AddProperty(DBPROP_IRowsetChange, true);
	propset.AddProperty(DBPROP_UPDATABILITY,
			DBPROPVAL_UP_INSERT | DBPROPVAL_UP_CHANGE | DBPROPVAL_UP_DELETE);

	CCommand<CManualAccessor>& rs = *((CCommand<CManualAccessor>*)command_);
	if (rs.Open(*session_->getSession(), sqlstr, &propset,	NULL, 
				DBGUID_DEFAULT, FALSE) != S_OK)
	{
		MessageBox(NULL,L"Couldn't open rowset",L"caption",0);
		return -1;
	}

	if (rs.m_spRowset == NULL) return 0;

	// Get the column information
	DBORDINAL ulColumns         = 0;
	DBCOLUMNINFO* pColumnInfo   = NULL;
	LPOLESTR pStrings           = NULL;
	if (rs.GetColumnInfo(&ulColumns, &pColumnInfo, &pStrings) != S_OK)
	{
		MessageBox(NULL,L"Couldn't retrieve column info",L"caption",0);
		return -1;
	}

	struct MYBIND
	{
		MYBIND()
		{
			memset(this, 0, sizeof(*this));
		}

		wchar_t   szValue[40];
		DWORD	  dwStatus;
	};

	struct MYBIND* pBind = new MYBIND[ulColumns];
	rs.CreateAccessor((int) ulColumns, &pBind[0], sizeof(MYBIND)*ulColumns);
	for (ULONG l=0; l<ulColumns; l++)
		rs.AddBindEntry(l+1, DBTYPE_WSTR, sizeof(WCHAR)*40, &pBind[l].szValue,
			NULL, &pBind[l].dwStatus);
	rs.Bind();

	// Display the data (to the maximum # of records allowed)
	int nMaxRecords = 100;
	int nItem=0;
	ULONG ulFields = (ULONG) rs.GetColumnCount();
	while(rs.MoveNext() == S_OK)
	{
		if (nItem < nMaxRecords)
		{
			if(nItem > 0) wprintf(L"\n");

			wprintf(L"row %d\n",nItem + 1);
			wprintf(L"=========================================\n");
			for (ULONG j=1; j<=ulFields; j++)
			{
				LPWSTR lpszColName = pColumnInfo[j-1].pwszName;
				wprintf(L"%s:\t",lpszColName);
				//#pragma warning(suppress: 6011) // unexpected null dereference prefast warning. 
				if (pBind[j-1].dwStatus == DBSTATUS_S_ISNULL)
				{
					wprintf(L"null\n");
				}
				else
				{
					wprintf(L"%s\n",pBind[j - 1].szValue);
				}
			}
			nItem++;
		}else
			break;
	}
	CoTaskMemFree(pColumnInfo);
	CoTaskMemFree(pStrings);
	delete [] pBind;
	pBind = NULL;

	rs.Close();

	return 0;
}

void dbcStatementSqlServer::abort()
{
}

int  dbcStatementSqlServer::getUpdateCount()
{
	return 0;
}
