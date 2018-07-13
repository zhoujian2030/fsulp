/*
 * dpeMain.cpp
 *
 *  Created on: June 29, 2018
 *      Author: j.zh
 */

#include "logger.h"
#include "DpEngineConfig.h"
#include "UeLoginInfoReceiver.h"
#include <string>

using namespace std;
using namespace dpe;

#define DPE_VERSION "1.0.0.080629" 

// -----------------------------------
void ShowUsage()
{
    printf("usage: \n");
    printf("./dpe -c [path/to/dpe.conf]\n");
    printf("./dpe -h or --help\n");
    printf("\nexample: \n");
    printf("./dpe -c /etc/dpe.conf\n");
}

int main(int argc, char* argv[]) {
    string configFileName;
    if (argc > 1) {
        int i;
        string option;
        for (i=1; i<argc;) {
            option = string(argv[i++]);
            if (option.compare("-c") == 0) {
                if (i < argc) {
                    configFileName = string(argv[i++]);
                } else {
                    ShowUsage();
                    return 0;
                }
            } else if (option.compare("-v") == 0) {
                printf("Version: %s\n",DPE_VERSION);
                return 0;
            } else {
                ShowUsage();
                return 0;
            }
        }
    }

    DpEngineConfig* pDpeConfig = new DpEngineConfig();
    pDpeConfig->parseJsonConfig(configFileName);

    UeLoginInfoReceiver* pUeInfoReceiver = new UeLoginInfoReceiver(pDpeConfig);
    pUeInfoReceiver->start();

    // pUeInfoReceiver->wait();
    unsigned int usleepTime = 100 * 1000;
    while (true) {
        usleep(usleepTime);
        pUeInfoReceiver->notify();
    }

    return 1; 
}

