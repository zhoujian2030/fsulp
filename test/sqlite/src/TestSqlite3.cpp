/*
 * TestSqlite3.cpp
 *
 *  Created on: June 22, 2018
 *      Author: j.zh
 */

#include "TestSqlite3.h"
#include <iostream>
#include <sqlite3.h>

using namespace std;

// -------------------------------
TEST_F(TestSqlite3, Open_Db_And_Create_Table)
{
    sqlite3 *dbConn;
    char *errMsg = 0;
    int ret;

    system("rm -rf /tmp/test.db");

    // int ret = sqlite3_open(":memory:", &dbConn);
    ret = sqlite3_open("/tmp/test.db", &dbConn);
    ASSERT_TRUE(ret == SQLITE_OK);
    ASSERT_TRUE(dbConn != 0);

    char sql[] = "CREATE TABLE UserInfo(" \
                    "ID     INTEGER PRIMARY KEY," \
                    "IMSI   VARCHAR (20)," \
                    "M_TMSI INTEGER," \
                    "RNTI   INTEGER   NOT NULL," \
                    "TIME   TimeStamp NOT NULL DEFAULT (datetime('now','localtime')));";

    // create table
    ret = sqlite3_exec(dbConn, sql, 0, 0, &errMsg);
    ASSERT_TRUE(ret == SQLITE_OK);
    ASSERT_TRUE(errMsg == 0);

    sqlite3_close(dbConn);

    // create the same table again
    ret = sqlite3_open("/tmp/test.db", &dbConn);
    ASSERT_TRUE(ret == SQLITE_OK);
    ret = sqlite3_exec(dbConn, sql, 0, 0, &errMsg);
    ASSERT_TRUE(ret != SQLITE_OK);
    ASSERT_TRUE(errMsg != 0);
    sqlite3_free(errMsg);

    sqlite3_close(dbConn);
}

// -------------------------------
TEST_F(TestSqlite3, Insert_Record)
{
    sqlite3 *dbConn;
    char *errMsg = 0;
    int ret;

    system("rm -rf /tmp/test.db");

    ret = sqlite3_open("/tmp/test.db", &dbConn);
    ASSERT_TRUE(ret == SQLITE_OK);
    ASSERT_TRUE(dbConn != 0);

    char createTbSql[] = "CREATE TABLE UserInfo(" \
                            "ID     INTEGER PRIMARY KEY," \
                            "IMSI   VARCHAR (20)," \
                            "M_TMSI INTEGER," \
                            "RNTI   INTEGER   NOT NULL," \
                            "TIME   TimeStamp NOT NULL DEFAULT (datetime('now','localtime')));";
    
    // create table
    ret = sqlite3_exec(dbConn, createTbSql, 0, 0, &errMsg);
    ASSERT_TRUE(ret == SQLITE_OK);
    ASSERT_TRUE(errMsg == 0);

    char insertRecordSql[] = "INSERT INTO UserInfo (IMSI, RNTI) VALUES ('460078139656276', 5678);";

    ret = sqlite3_exec(dbConn, insertRecordSql, 0, 0, &errMsg);
    ASSERT_TRUE(ret == SQLITE_OK);
    ASSERT_TRUE(errMsg == 0);

    sqlite3_close(dbConn);
}

static int TestSqlite3_Query_Record_Callback(void *data, int argc, char **argv, char **azColName){
   cout << (char*)data << endl;

   for(int i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

// -------------------------------
TEST_F(TestSqlite3, Query_Record)
{
    sqlite3 *dbConn;
    char *errMsg = 0;
    int ret;

    system("rm -rf /tmp/test.db");

    ret = sqlite3_open("/tmp/test.db", &dbConn);
    ASSERT_TRUE(ret == SQLITE_OK);
    ASSERT_TRUE(dbConn != 0);

    char createTbSql[] = "CREATE TABLE UserInfo(" \
                            "ID     INTEGER PRIMARY KEY," \
                            "IMSI   VARCHAR (20)," \
                            "M_TMSI INTEGER," \
                            "RNTI   INTEGER   NOT NULL," \
                            "TIME   TimeStamp NOT NULL DEFAULT (datetime('now','localtime')));";
    
    // create table
    ret = sqlite3_exec(dbConn, createTbSql, 0, 0, &errMsg);
    ASSERT_TRUE(ret == SQLITE_OK);
    ASSERT_TRUE(errMsg == 0);

    char insertRecord1Sql[] = "INSERT INTO UserInfo (IMSI, RNTI) VALUES ('460078139656276', 5678);";
    char insertRecord2Sql[] = "INSERT INTO UserInfo (IMSI, RNTI) VALUES ('460078139656276', 1234);";

    ret = sqlite3_exec(dbConn, insertRecord1Sql, 0, 0, &errMsg);
    ASSERT_TRUE(ret == SQLITE_OK);
    ASSERT_TRUE(errMsg == 0);
    ret = sqlite3_exec(dbConn, insertRecord2Sql, 0, 0, &errMsg);
    ASSERT_TRUE(ret == SQLITE_OK);
    ASSERT_TRUE(errMsg == 0);

    char querySql[] = "SELECT * FROM UserInfo WHERE IMSI='460078139656276';";
    const char* data = "Query_Record callback function called";

    ret = sqlite3_exec(dbConn, querySql, TestSqlite3_Query_Record_Callback, (void*)data, &errMsg);
    ASSERT_TRUE(ret == SQLITE_OK);
    ASSERT_TRUE(errMsg == 0);

    sqlite3_close(dbConn);
}

// -------------------------------
TEST_F(TestSqlite3, API_Insert_sqlite3_prepare_v2)
{
    sqlite3 *dbConn;
    char *errMsg = 0;
    int ret;

    system("rm -rf /tmp/test.db");

    ret = sqlite3_open("/tmp/test.db", &dbConn);
    ASSERT_TRUE(ret == SQLITE_OK);
    ASSERT_TRUE(dbConn != 0);

    char createTbSql[] = "CREATE TABLE UserInfo(" \
                            "ID     INTEGER PRIMARY KEY," \
                            "IMSI   VARCHAR (20)," \
                            "M_TMSI INTEGER," \
                            "RNTI   INTEGER   NOT NULL," \
                            "TIME   TimeStamp NOT NULL DEFAULT (datetime('now','localtime')));";
    
    // create table
    ret = sqlite3_exec(dbConn, createTbSql, 0, 0, &errMsg);
    ASSERT_TRUE(ret == SQLITE_OK);
    ASSERT_TRUE(errMsg == 0);

    char insertSql[] = "INSERT INTO UserInfo (IMSI, RNTI) VALUES (?, ?)";
    sqlite3_stmt *pSqlStmt = 0;

    // precompile insert sql statement
    ret = sqlite3_prepare_v2(dbConn, insertSql, strlen(insertSql), &pSqlStmt, 0);
    ASSERT_TRUE(ret == SQLITE_OK);
    ASSERT_TRUE(pSqlStmt != 0);

    // add row 1 info in sql statement
    ret = sqlite3_bind_text(pSqlStmt, 1, "460078139656276", -1, SQLITE_STATIC);
    ASSERT_EQ(SQLITE_OK, ret);
    ret = sqlite3_bind_int(pSqlStmt, 2, 1234);
    ASSERT_EQ(SQLITE_OK, ret);

    // insert row 1
    ret = sqlite3_step(pSqlStmt);
    ASSERT_EQ(SQLITE_DONE, ret);

    // add row 2 info in sql statement
    ret = sqlite3_bind_int(pSqlStmt, 2, 5678);
    ASSERT_EQ(SQLITE_MISUSE, ret);
    sqlite3_reset(pSqlStmt);

    ret = sqlite3_bind_int(pSqlStmt, 2, 5678);
    ASSERT_EQ(SQLITE_OK, ret);
    ret = sqlite3_bind_text(pSqlStmt, 1, "460078139656276", -1, SQLITE_STATIC);
    ASSERT_EQ(SQLITE_OK, ret);

    // insert row 2
    ret = sqlite3_step(pSqlStmt);
    ASSERT_EQ(SQLITE_DONE, ret);
    
    sqlite3_finalize(pSqlStmt);
    sqlite3_close(dbConn);
}

// -------------------------------
TEST_F(TestSqlite3, API_Query_sqlite3_prepare_v2)
{
    sqlite3 *dbConn;
    char *errMsg = 0;
    int ret;

    system("rm -rf /tmp/test.db");

    ret = sqlite3_open("/tmp/test.db", &dbConn);
    ASSERT_TRUE(ret == SQLITE_OK);
    ASSERT_TRUE(dbConn != 0);

    char createTbSql[] = "CREATE TABLE UserInfo(" \
                            "ID     INTEGER PRIMARY KEY," \
                            "IMSI   VARCHAR (20)," \
                            "M_TMSI INTEGER," \
                            "RNTI   INTEGER   NOT NULL," \
                            "TIME   TimeStamp NOT NULL DEFAULT (datetime('now','localtime')));";
    
    // create table
    ret = sqlite3_exec(dbConn, createTbSql, 0, 0, &errMsg);
    ASSERT_TRUE(ret == SQLITE_OK);
    ASSERT_TRUE(errMsg == 0);

    char insertRecord1Sql[] = "INSERT INTO UserInfo (IMSI, RNTI) VALUES ('460078139656276', 5678);";
    char insertRecord2Sql[] = "INSERT INTO UserInfo (IMSI, RNTI) VALUES ('460078139656276', 1234);";

    ret = sqlite3_exec(dbConn, insertRecord1Sql, 0, 0, &errMsg);
    ASSERT_TRUE(ret == SQLITE_OK);
    ASSERT_TRUE(errMsg == 0);
    ret = sqlite3_exec(dbConn, insertRecord2Sql, 0, 0, &errMsg);
    ASSERT_TRUE(ret == SQLITE_OK);
    ASSERT_TRUE(errMsg == 0);

    char querySql[] = "SELECT * FROM UserInfo WHERE IMSI = ?";
    sqlite3_stmt *pSqlStmt = 0;

    // precompile query sql statement
    ret = sqlite3_prepare_v2(dbConn, querySql, strlen(querySql), &pSqlStmt, 0);
    ASSERT_TRUE(ret == SQLITE_OK);
    ASSERT_TRUE(pSqlStmt != 0);

    // query by IMSI
    ret = sqlite3_bind_text(pSqlStmt, 1, "460078139656276", -1, SQLITE_STATIC);
    ASSERT_EQ(SQLITE_OK, ret);

    // Get 1st query result
    ret = sqlite3_step(pSqlStmt);
    ASSERT_EQ(SQLITE_ROW, ret);

    // check query result
    ASSERT_EQ(5, sqlite3_column_count(pSqlStmt));
    // ID
    ASSERT_EQ(SQLITE_INTEGER, sqlite3_column_type(pSqlStmt, 0));
    ASSERT_EQ(1, sqlite3_column_int(pSqlStmt, 0));

    // IMSI
    ASSERT_EQ(SQLITE_TEXT, sqlite3_column_type(pSqlStmt, 1));
    ASSERT_EQ(0, memcmp(sqlite3_column_text(pSqlStmt, 1), "460078139656276", strlen("460078139656276")));

    // M-TMSI
    ASSERT_EQ(SQLITE_NULL, sqlite3_column_type(pSqlStmt, 2));
    ASSERT_EQ(0, sqlite3_column_int(pSqlStmt, 2));

    // RNTI
    ASSERT_EQ(SQLITE_INTEGER, sqlite3_column_type(pSqlStmt, 3));
    ASSERT_EQ(5678, sqlite3_column_int(pSqlStmt, 3));

    // TIME
    ASSERT_EQ(SQLITE_TEXT, sqlite3_column_type(pSqlStmt, 4));
    char* timeString = (char*)sqlite3_column_text(pSqlStmt, 4);
    cout << "TIME: " << timeString << endl;
    ASSERT_EQ(strlen("2018-06-27 15:10:34"), strlen(timeString));

    // Get 2nd query result
    ret = sqlite3_step(pSqlStmt);
    ASSERT_EQ(SQLITE_ROW, ret);

    // check query result
    ASSERT_EQ(5, sqlite3_column_count(pSqlStmt));
    // ID
    ASSERT_EQ(SQLITE_INTEGER, sqlite3_column_type(pSqlStmt, 0));
    ASSERT_EQ(2, sqlite3_column_int(pSqlStmt, 0));

    // IMSI
    ASSERT_EQ(SQLITE_TEXT, sqlite3_column_type(pSqlStmt, 1));
    ASSERT_EQ(0, memcmp(sqlite3_column_text(pSqlStmt, 1), "460078139656276", strlen("460078139656276")));

    // M-TMSI
    ASSERT_EQ(SQLITE_NULL, sqlite3_column_type(pSqlStmt, 2));
    ASSERT_EQ(0, sqlite3_column_int(pSqlStmt, 2));

    // RNTI
    ASSERT_EQ(SQLITE_INTEGER, sqlite3_column_type(pSqlStmt, 3));
    ASSERT_EQ(1234, sqlite3_column_int(pSqlStmt, 3));

    // TIME
    ASSERT_EQ(SQLITE_TEXT, sqlite3_column_type(pSqlStmt, 4));
    timeString = (char*)sqlite3_column_text(pSqlStmt, 4);
    cout << "TIME: " << timeString << endl;
    ASSERT_EQ(strlen("2018-06-27 15:10:34"), strlen(timeString));

    // Get 3rd query result
    ret = sqlite3_step(pSqlStmt);
    ASSERT_EQ(SQLITE_DONE, ret);

    // sqlite3_reset(pSqlStmt);
    sqlite3_finalize(pSqlStmt);
    sqlite3_close(dbConn);
}
