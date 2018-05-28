#include <unistd.h>
#include "lteLogger.h"
#include "lteUlpMgr.h"
#include "lteConfig.h"
#include <stdio.h>


// -----------------------------------
void ShowUsage()
{
    printf("usage: \n");
    printf("./ulp -c [path/to/ulp.conf]\n");
    printf("./ulp -h or --help\n");
    printf("\nexample: \n");
    printf("./ulp -c /etc/ulp.conf\n");
}

// -----------------------------------
#define DELAY_INTERVAL_MS   1000*5000
int main(int argc, char* argv[]) {
    // parse optional argv
    char* configFileName = 0;
    if (argc > 1) {
        int i;
        char* option;
        for (i=1; i<argc;) {
            option = argv[i++];
            if (IsStringEqual(option, "-c")) {
                if (i < argc) {
                    configFileName = argv[i++];
                } else {
                    ShowUsage();
                    return 0;
                }
            } else {
                ShowUsage();
                return 0;
            }
        }
    }

    ConfigUpdate(configFileName);
    ConfigShow();

    InitUlpLayer(1, 1);

    LoggerSetLogLevel(gLteConfig.loglevel);

    sleep(1);

    while (1) {

        UlpOneMilliSecondIsr();

        usleep(gLteConfig.pollingInterval);
    }

    return 0; 
}
