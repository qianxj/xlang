#include "sqlserver-dbcsession.hpp"
#include <atldbcli.h>
#include "windows.h"
#include "sqlserver-dbcstatement.hpp"

using namespace ATL;

dbcSessionSqlServer::dbcSessionSqlServer(ATL::CSession * session):session_(session)
{
}

dbcSessionSqlServer::~dbcSessionSqlServer()
{
	if(session_)delete session_;
}

void dbcSessionSqlServer::start()
{
	session_->StartTransaction();
}

void dbcSessionSqlServer::abort()
{
	session_->Abort();
}

void dbcSessionSqlServer::commit()
{
	session_->Commit();
}

dbcStatement * dbcSessionSqlServer::createStatement()
{
	CCommand<CManualAccessor> * command_ = new CCommand<CManualAccessor>;
	return new dbcStatementSqlServer(this,command_);
}

ATL::CSession * dbcSessionSqlServer::getSession()
{
	return session_;
}
