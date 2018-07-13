/*
 * DpEngineConfig.h
 *
 *  Created on: June 28, 2018
 *      Author: j.zhou
 */

#ifndef DP_ENGINE_CONFIG_H
#define DP_ENGINE_CONFIG_H

#include <string>
#include "loggerConfig.h"

namespace dpe {

    class DpEngineConfig {
    public:
        DpEngineConfig();
        ~DpEngineConfig();

        void parseJsonConfig(std::string configFileName);

        std::string m_mobileIdDbName;
        std::string m_userDbname;  

        std::string m_imsiServerIp;
        unsigned short m_imsiServerPort;    

        std::string m_engineServerIp;
        unsigned short m_engineServerPort;

        std::string m_oamServerIp;
        unsigned short m_oamServerPort;
    };

}

#endif 
