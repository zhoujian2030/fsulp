/*
 * dbImpl.c
 *
 *  Created on: June 27, 2018
 *      Author: j.zhou
 */

#include "dbCommon.h"
#include "dbInterface.h"
#include "logger.h"

static DbConfig gDbConfig_s = {
    "/tmp/eq5_mobile_id.db",
    "/tmp/eq5_user_info.db"
};

// ----------------------------------
void DbInit(DbConfig* pDbConfig)
{
    if (pDbConfig) {
        memcpy((void*)&gDbConfig_s, (void*)pDbConfig, sizeof(DbConfig));
        LOG_MSG(LOGGER_MODULE_DB, DEBUG, "dbMobileId = %s\n", gDbConfig_s.dbMobileId)
    }
}

// ----------------------------------
int DbGetConnection(DbConnection* pDbConn, const char* dbName)
{
    if (pDbConn == 0 || dbName == 0) {
        LOG_MSG(LOGGER_MODULE_DB, ERROR, "pDbConn = %p, dbName = %p\n", pDbConn, dbName)
        return DB_ERROR;
    }

    int result;

    result = sqlite3_open(dbName, &pDbConn->sqlite);
    if (result ==  SQLITE_OK) {
        LOG_MSG(LOGGER_MODULE_DB, DEBUG, "get DB connection success from DB[%s]\n", dbName)
        return DB_SUCCESS;
    } else {
        LOG_MSG(LOGGER_MODULE_DB, ERROR, "sqlite3_open error, dbName = %s\n", dbName)
        return DB_ERROR;
    }     
}

// ----------------------------------
void DbCloseConnection(DbConnection* pDbConn)
{
    if (pDbConn) {
        LOG_MSG(LOGGER_MODULE_DB, DEBUG, "pDbConn->sqlite = %p\n", pDbConn->sqlite)
        sqlite3_close(pDbConn->sqlite);
    }
}

// ----------------------------------
int DbInsertLoginImsi(DbConnection* pDbConn, const char* imsi)
{
    if ((pDbConn == 0) || (pDbConn->sqlite == 0)) {
        LOG_MSG(LOGGER_MODULE_DB, ERROR, "pDbConn or pDbConn->sqlite is NULL, pDbConn = %p\n", pDbConn)
        return DB_ERROR;
    }

    if ((imsi == 0) || (strlen(imsi) == 0) || (strlen (imsi) > MAX_IMSI_BYTE_LENGTH)) 
    {
        LOG_MSG(LOGGER_MODULE_DB, ERROR, "invalid imsi = %s\n", imsi)
        return DB_ERROR;
    }

    int result, ret = DB_ERROR;
    sqlite3_stmt* pSqlStmt = 0;
    char sql[] = "insert into login_info (imsi) values (?)";

    result = sqlite3_prepare_v2(pDbConn->sqlite, sql, strlen(sql), &pSqlStmt, 0);
    if (result != SQLITE_OK) {        
        LOG_MSG(LOGGER_MODULE_DB, ERROR, "sqlite3_prepare_v2 erroCode = %d, sql = %s\n", result, sql);
        if (pSqlStmt) {
            sqlite3_finalize(pSqlStmt);
        }
        return ret;
    }

    result = sqlite3_bind_text(pSqlStmt, 1, imsi, -1, SQLITE_STATIC);
    if (result != SQLITE_OK) {
        LOG_MSG(LOGGER_MODULE_DB, ERROR, "sqlite3_bind_text erroCode = %d, imsi = %s\n", result, imsi);
        sqlite3_finalize(pSqlStmt);
        return ret;
    }

    result = sqlite3_step(pSqlStmt);
    if (result == SQLITE_DONE) {
        LOG_MSG(LOGGER_MODULE_DB, DEBUG, "insert imsi = %s\n", imsi);
        ret = DB_SUCCESS;
    } else {
        LOG_MSG(LOGGER_MODULE_DB, ERROR, "sqlite3_step errorCode = %d, imsi = %s\n", result, imsi);
    }

    sqlite3_finalize(pSqlStmt);
    return ret;
}

// ----------------------------------
int DbInsertMTmsi(DbConnection* pDbConn, unsigned int mTmsi)
{
    // TODO
    return DB_ERROR;
}

// ----------------------------------
int DbInsertImsiAndMTmsi(DbConnection* pDbConn, const char* imsi, unsigned int mTmsi)
{
    // TODO
    return DB_ERROR;
}
