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

// ----------------------------------
int DbSelectLoginCountByImsi(DbConnection* pDbConn, const char* imsi)
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

    int result, row = 0;
    sqlite3_stmt* pSqlStmt = 0;
    char sql[] = "SELECT count( * ) FROM login_info WHERE imsi= ? ";

    result = sqlite3_prepare_v2(pDbConn->sqlite, sql, strlen(sql), &pSqlStmt, 0);
    if (result != SQLITE_OK) {        
        LOG_MSG(LOGGER_MODULE_DB, ERROR, "sqlite3_prepare_v2 erroCode = %d, sql = %s\n", result, sql);
        if (pSqlStmt) {
            sqlite3_finalize(pSqlStmt);
        }
        return 0;
    }

    result = sqlite3_bind_text(pSqlStmt, 1, imsi, -1, SQLITE_STATIC);
    if (result != SQLITE_OK) {
        LOG_MSG(LOGGER_MODULE_DB, ERROR, "sqlite3_bind_text erroCode = %d, imsi = %s\n", result, imsi);
        sqlite3_finalize(pSqlStmt);
        return 0;
    }

    result = sqlite3_step(pSqlStmt);
    if (result == SQLITE_ROW && sqlite3_column_count(pSqlStmt)) {
        row = sqlite3_column_int(pSqlStmt, 0);
    } else {
        LOG_MSG(LOGGER_MODULE_DB, ERROR, "sqlite3_step errorCode = %d, imsi = %s\n", result, imsi);
    }

    LOG_MSG(LOGGER_MODULE_DB, DEBUG, "row = %d, imsi = %s\n", row, imsi);
    sqlite3_finalize(pSqlStmt);

    return row;
}

// ----------------------------------
int DbQueryLoginInfoByImsiAndDate(
    DbConnection* pDbConn, 
    void* param,
    const char* imsi, 
    const char* startDate, 
    const char* endDate, 
    QueryLoginInfoCallback callbackFunc,
    int* pResultSetCount)
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

    if ((startDate == 0) || (endDate == 0) || (strlen(startDate) > MAX_DB_TIMESTAMP_LENGTH) ||
        (strlen(endDate) > MAX_DB_TIMESTAMP_LENGTH))
    {
        LOG_MSG(LOGGER_MODULE_DB, ERROR, "invalid date format, imsi = %s, startDate  = %s, endDate = %s\n", imsi, startDate, endDate)
        return DB_ERROR;
    }

    if (callbackFunc == 0) {
        LOG_MSG(LOGGER_MODULE_DB, ERROR, "callbackFunc is NULL, imsi = %s\n", imsi)
        return DB_ERROR;
    }

    int count = 0, result, ret = DB_ERROR;
    sqlite3_stmt* pSqlStmt = 0;
    char sql[] = "SELECT * FROM login_info WHERE imsi = ? AND time >= ? AND time <= ? ";

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

    result = sqlite3_bind_text(pSqlStmt, 2, startDate, -1, SQLITE_STATIC);
    if (result != SQLITE_OK) {
        LOG_MSG(LOGGER_MODULE_DB, ERROR, "sqlite3_bind_text erroCode = %d, startDate = %s\n", result, startDate);
        sqlite3_finalize(pSqlStmt);
        return ret;
    }

    result = sqlite3_bind_text(pSqlStmt, 3, endDate, -1, SQLITE_STATIC);
    if (result != SQLITE_OK) {
        LOG_MSG(LOGGER_MODULE_DB, ERROR, "sqlite3_bind_text erroCode = %d, endDate = %s\n", result, endDate);
        sqlite3_finalize(pSqlStmt);
        return ret;
    }

    result = sqlite3_column_count(pSqlStmt);
    if (4 != result) {
        LOG_MSG(LOGGER_MODULE_DB, ERROR, "sqlite3_column_count = %d\n", result);
        sqlite3_finalize(pSqlStmt);
        return ret;
    }

    while (sqlite3_step(pSqlStmt) == SQLITE_ROW) {
        count++;
        (*callbackFunc)(
            param,
            sqlite3_column_int(pSqlStmt, 0),
            sqlite3_column_text(pSqlStmt, 1),
            sqlite3_column_int(pSqlStmt, 2),
            sqlite3_column_text(pSqlStmt, 3)
        );
    }

    ret = DB_SUCCESS;
    sqlite3_finalize(pSqlStmt);

    if (pResultSetCount) {
        *pResultSetCount = count;
    }

    LOG_MSG(LOGGER_MODULE_DB, DEBUG, "count = %d, imsi = %s, startDate = %s, endDate = %s\n", count, imsi, startDate, endDate);

    return ret;
}
