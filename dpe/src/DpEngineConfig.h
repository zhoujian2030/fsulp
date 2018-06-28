/*
 * DpEngineConfig.h
 *
 *  Created on: June 28, 2018
 *      Author: j.zhou
 */

#ifndef DP_ENGINE_CONFIG_H
#define DP_ENGINE_CONFIG_H

#include <string>

namespace dpe {

    class DpEngineConfig {
    public:
        DpEngineConfig();
        ~DpEngineConfig();

        std::string m_mobileIdDbName;
        std::string m_userInfoDbname;        
    };

}

#endif 
