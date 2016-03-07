#include "sqlserver-dbcdrive.hpp"
#include "sqlserver-dbccollect.hpp"
#include "dbccollect.hpp"

dbcDriveSqlServer::dbcDriveSqlServer()
{
}

dbcSession * dbcDriveSqlServer::getSession(const wstring_t connectstr)
{
	const dbcCollect * collect = getCollect(connectstr);
	if(!collect) return 0;
	return ((dbcCollect *)collect)->getSession();
}

const dbcCollect * dbcDriveSqlServer::getCollect(const wstring_t connectstr)
{
	dbcCollectSqlServer * collect = new dbcCollectSqlServer();
	collect->setConnectString(connectstr);
	if(collect->open()< 0 )
	{
		delete collect;
		return 0;
	}
	return collect;
}

