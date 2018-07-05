/*
 * dbInterface.h
 *
 *  Created on: June 27, 2018
 *      Author: j.zhou
 */

#ifndef DB_INTERFACE_H
#define DB_INTERFACE_H

#include "sqlite3.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DB_SUCCESS  = 0,
    DB_ERROR    = 1
} E_DB_ERR_CODE;

#define MAX_DB_NAME_LENGTH  128
typedef struct {
    char dbMobileId[MAX_DB_NAME_LENGTH];
    char dbUserInfo[MAX_DB_NAME_LENGTH];
} DbConfig;

typedef struct {
    sqlite3 *sqlite;
} DbConnection;

#define MAX_DB_TIMESTAMP_LENGTH    (sizeof("2018-01-01 00:00:00") + 1)
typedef struct {
    char imsi[16];
    char timestamp[MAX_DB_TIMESTAMP_LENGTH];
} UeLoginInfo;

typedef void (*QueryLoginInfoCallback)(
    void* param,
    unsigned int id,
    const char* imsi,
    unsigned int mTmsi,
    const char* timestamp
);

void DbInit(DbConfig* pDbConfig);

int  DbGetConnection(DbConnection* pDbConn, const char* dbName);
void DbCloseConnection(DbConnection* pDbConn);

// API for handling login_info table
int DbInsertLoginImsi(DbConnection* pDbConn, const char* imsi);
int DbInsertMTmsi(DbConnection* pDbConn, unsigned int mTmsi);
int DbInsertImsiAndMTmsi(DbConnection* pDbConn, const char* imsi, unsigned int mTmsi);

int DbSelectLoginCountByImsi(DbConnection* pDbConn, const char* imsi);
int DbQueryLoginInfoByImsiAndDate(
    DbConnection* pDbConn, 
    void* param,
    const char* imsi, 
    const char* beginDate, 
    const char* endDate, 
    QueryLoginInfoCallback callbackFunc,
    int* pResultSetCount);
int DbQueryLoginInfoByDate(
    DbConnection* pDbConn, 
    void* param,
    const char* beginDate, 
    const char* endDate, 
    QueryLoginInfoCallback callbackFunc,
    int* pResultSetCount);

//
int DbInsertUserLoginBehaviour(
    DbConnection* pDbConn, 
    const char* imsi, 
    const char* firstTime,
    const char* lastTime,
    unsigned int loginCount);

#ifdef __cplusplus
}
#endif

#endif
