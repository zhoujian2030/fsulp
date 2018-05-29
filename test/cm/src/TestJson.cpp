/*
 * TestJson.cpp
 *
 *  Created on: May 28, 2018
 *      Author: j.zh
 */

#include "TestJson.h"
#include "cJSON.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include "file.h"
#include <string>

using namespace std;

// -------------------------------
TEST_F(TestJson, Basic_Test)
{
    char s[] = "{\"name\":\"Tom\",\"age\":30}";
    cJSON *root = cJSON_Parse(s);

    ASSERT_TRUE(root != 0);
    cJSON *jsonObj = cJSON_GetObjectItem(root, "name");
    ASSERT_TRUE(jsonObj != 0);
    ASSERT_EQ(jsonObj->type, cJSON_String);
    ASSERT_EQ(memcmp(jsonObj->valuestring, "Tom", 3), 0);
    jsonObj = cJSON_GetObjectItem(root, "age");
    ASSERT_TRUE(jsonObj != 0);
    ASSERT_EQ(jsonObj->type, cJSON_Number);
    ASSERT_EQ(jsonObj->valueint, 30);

    cJSON_Delete(root);
}

// -------------------------------
TEST_F(TestJson, Parse_Json_File)
{
    int numBytesWritten = 0;
    int fd = FileOpen("/tmp/Parse_Json_File.json", FILE_CREATE, FILE_WRITE_ONLY);

    string jsonLine = "{\n";
    FileWrite(fd, jsonLine.c_str(), jsonLine.length(), &numBytesWritten);
    jsonLine = "\t\"PollingInterval\" : 5,\n";
    FileWrite(fd, jsonLine.c_str(), jsonLine.length(), &numBytesWritten);
    jsonLine = "\t\"Loglevel\" : \"DEBUG\"\n";
    FileWrite(fd, jsonLine.c_str(), jsonLine.length(), &numBytesWritten);
    
    jsonLine = "}\n";
    FileWrite(fd, jsonLine.c_str(), jsonLine.length(), &numBytesWritten);

    FileClose(fd);

    fd = FileOpen("/tmp/Parse_Json_File.json", FILE_OPEN, FILE_READ_ONLY);
    ASSERT_TRUE(fd != -1);

    int numBytesRead = 0;
    char jsonBuffer[8192];
    FileRead(fd, jsonBuffer, 8192, &numBytesRead);
    ASSERT_TRUE(numBytesRead > 0);
    cout << "numBytesRead = " << numBytesRead << ", jsonBuffer = " << jsonBuffer << endl;

    cJSON* jsonRoot = cJSON_Parse(jsonBuffer);
    ASSERT_TRUE(jsonRoot != 0);
    cJSON *jsonObj = cJSON_GetObjectItemCaseSensitive(jsonRoot, "PollingInterval");
    ASSERT_TRUE(jsonObj != 0);
    ASSERT_EQ(jsonObj->type, cJSON_Number);
    ASSERT_EQ(jsonObj->valueint, 5);

    jsonObj = cJSON_GetObjectItemCaseSensitive(jsonRoot, "Loglevel");
    ASSERT_TRUE(jsonObj != 0);
    ASSERT_EQ(jsonObj->type, cJSON_String);
    ASSERT_EQ(memcmp(jsonObj->valuestring, "DEBUG", strlen("DEBUG")), 0);

    cJSON_Delete(jsonRoot);
}
