/*
 * dbInterface.h
 *
 *  Created on: June 27, 2018
 *      Author: j.zhou
 */

#ifndef DB_INTERFACE_H
#define DB_INTERFACE_H

#include "sqlite3.h"

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

void DbInit(DbConfig* pDbConfig);

int  DbGetConnection(DbConnection* pDbConn, const char* dbName);
void DbCloseConnection(DbConnection* pDbConn);
int  DbInsertLoginImsi(DbConnection* pDbConn, const char* imsi);
int  DbInsertMTmsi(DbConnection* pDbConn, unsigned int mTmsi);
int  DbInsertImsiAndMTmsi(DbConnection* pDbConn, const char* imsi, unsigned int mTmsi);

#ifdef __cplusplus
}
#endif

#endif
