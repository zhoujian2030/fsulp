#include <unistd.h>
#include "logger.h"
#include <stdio.h>
#include "hbt.h"
#include "testTrigger.h"
#include <string.h>
#include <stdlib.h>

// -----------------------------------
void ShowUsage()
{
    printf("usage: \n");
    printf("./hbtester -n [number heart]\n");
    printf("./hbtester -h or --help\n");
    printf("\nexample: \n");
    printf("./hbtester -n 144000\n");
}

// --------------------------------
int IsStringEqual(char* src, char* dst)
{
    if (src == 0 || dst == 0) {
        return 0;
    }

    int length = strlen(src);
    if (length != strlen(dst)) {
        return 0;
    }

    int i = 0;
    while (i < length) {
        if (src[i] != dst[i]) {
            return 0;
        }
        i++;
    }

    return 1;
}

extern unsigned int gMaxHeartbeatReqSend;

// -----------------------------------
int main(int argc, char* argv[]) {
    // parse optional argv
    if (argc > 1) {
        int i;
        char* option;
        for (i=1; i<argc;) {
            option = argv[i++];
            if (IsStringEqual(option, "-n")) {
                if (i < argc) {
                    gMaxHeartbeatReqSend = atoi(argv[i++]);
                    // printf("gMaxHeartbeatReqSend = %d\n", gMaxHeartbeatReqSend);
                } else {
                    ShowUsage();
                    return 0;
                }
            } else {
                ShowUsage();
                return 0;
            }
        }
    } else {
        ShowUsage();
        return 0;
    }

    char logFileName[] = "tester.log";

    LoggerConfig loggerConfig;
    memset((void*)&loggerConfig, 0, sizeof(LoggerConfig));
    loggerConfig.logType = 0;
    loggerConfig.logLevel = INFO;
    loggerConfig.logToFileFlag = 1;
    strcpy(loggerConfig.logFilePath, logFileName);
    loggerConfig.maxLogFileSize = 1024 * 1024 * 10;
    
    LoggerInit(&loggerConfig);

    LOG_MSG(HB_LOGGER_NAME, INFO, "gMaxHeartbeatReqSend = %d\n", gMaxHeartbeatReqSend);

    StartHeartbeatTester();
   
    sleep(1);

    StartTestTrigger();

    return 0; 
}
