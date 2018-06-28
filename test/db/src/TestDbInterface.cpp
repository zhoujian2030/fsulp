/*
 * TestDbInterface.cpp
 *
 *  Created on: June 28, 2018
 *      Author: j.zh
 */

#include "TestDbInterface.h"
#include <iostream>
#include "dbInterface.h"
#include <string>

using namespace std;

// -------------------------------
TEST_F(TestDbInterface, DbGetConnection)
{
    string dbName("/tmp/TestDbInterface.db");
    
    system(("rm -rf " + dbName).c_str());

    DbConnection dbConn;
    ASSERT_EQ(DB_SUCCESS, DbGetConnection(&dbConn, dbName.c_str()));
    ASSERT_TRUE(dbConn.sqlite != 0);
    DbCloseConnection(&dbConn);
}

// -------------------------------
TEST_F(TestDbInterface, DbInsertMobileIdImsi)
{
    string dbName("/tmp/TestDbInterface.db");
    
    system(("rm -rf " + dbName).c_str());

    DbConnection dbConn;
    ASSERT_EQ(DB_SUCCESS, DbGetConnection(&dbConn, dbName.c_str()));
    ASSERT_TRUE(dbConn.sqlite != 0);

    char sql[] = "create table login_info (" \
                    "id     INTEGER PRIMARY KEY," \
                    "imsi   VARCHAR (16)," \
                    "m_tmsi INTEGER," \
                    "time   TimeStamp NOT NULL DEFAULT (datetime('now','localtime')));";
    char *errMsg = 0;

    ASSERT_EQ(SQLITE_OK, sqlite3_exec(dbConn.sqlite, sql, 0, 0, &errMsg));

    ASSERT_EQ(DB_SUCCESS, DbInsertLoginImsi(&dbConn, "460078020683852"));
    ASSERT_EQ(DB_SUCCESS, DbInsertLoginImsi(&dbConn, "460029204530266"));
    ASSERT_EQ(DB_SUCCESS, DbInsertLoginImsi(&dbConn, "460002471325262"));
    ASSERT_EQ(DB_SUCCESS, DbInsertLoginImsi(&dbConn, "460025020192690"));
    ASSERT_EQ(DB_SUCCESS, DbInsertLoginImsi(&dbConn, "460029020757283"));
    ASSERT_EQ(DB_SUCCESS, DbInsertLoginImsi(&dbConn, "460020267351682"));
    ASSERT_EQ(DB_SUCCESS, DbInsertLoginImsi(&dbConn, "460000056590797"));
    ASSERT_EQ(DB_SUCCESS, DbInsertLoginImsi(&dbConn, "460001782523192"));
    ASSERT_EQ(DB_SUCCESS, DbInsertLoginImsi(&dbConn, "460000570732209"));
    ASSERT_EQ(DB_SUCCESS, DbInsertLoginImsi(&dbConn, "460003009184645"));
    ASSERT_EQ(DB_SUCCESS, DbInsertLoginImsi(&dbConn, "460002371697632"));
    ASSERT_EQ(DB_SUCCESS, DbInsertLoginImsi(&dbConn, "460005663291764"));

    DbCloseConnection(&dbConn);
}