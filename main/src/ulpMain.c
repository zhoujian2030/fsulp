#include <unistd.h>
#include "lteLogger.h"
#include "lteUlpMgr.h"

// -----------------------------------
int main(int argc, char* argv[]) {

    InitUlpLayer(1, 1);

    LoggerSetLogLevel(LOG_LEVEL_INFO);

    sleep(1);

    while (1) {

        UlpOneMilliSecondIsr();

        usleep(5000);  // 5ms
    }

    return 0; 
}
