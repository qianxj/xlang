#include "sqlserver-dbccollect.hpp"
#include "sqlserver-dbcsession.hpp"
#include <atldbcli.h>
#include "windows.h"

using namespace ATL;

dbcCollectSqlServer::dbcCollectSqlServer() : dataSource_(0)
{
}

dbcCollectSqlServer::~dbcCollectSqlServer()
{
	if(dataSource_)delete dataSource_;
}

dbcSession * dbcCollectSqlServer::getSession()
{
	if(!dataSource_) return 0;
	CSession * pSession = new CSession;
	HRESULT hr = pSession->Open(*dataSource_);
	if (FAILED(hr))
	{
		delete pSession;
		pSession = NULL;
	}
	return new dbcSessionSqlServer(pSession);

}

int dbcCollectSqlServer::open()
{
	if(getConnectString()==L"") return -1;

	if(dataSource_)
	{
		dataSource_->Close();
		delete dataSource_;
		dataSource_ = NULL;
	}

	dataSource_ = new ATL::CDataSource;
	HRESULT hr = dataSource_->OpenFromInitializationString(getConnectString());
	if (FAILED(hr))
	{
		delete dataSource_;
		dataSource_ = NULL; 
	}
	return 1;
}

int dbcCollectSqlServer::close() 
{
	if(dataSource_)
	{
		dataSource_->Close();
		delete dataSource_;
		dataSource_ = NULL;
	}

	return 1;
}

ATL::CDataSource * dbcCollectSqlServer::getDataSource()
{
	return dataSource_;
}

const wstring_t dbcCollectSqlServer::getConnectString()
{
	return (const wstring_t)connectString_.c_str();
}

void dbcCollectSqlServer::setConnectString(const wstring_t connectString)
{
	connectString_ = connectString;
}