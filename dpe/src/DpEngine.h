/*
 * DpEngine.h
 *
 *  Created on: June 30, 2018
 *      Author: j.zhou
 */

#ifndef DP_ENGINE_H
#define DP_ENGINE_H

#include "Thread.h"

namespace dpe {

    class DpEngineConfig;

    class DpEngine : public Thread {
    public:
        DpEngine(DpEngineConfig* pConfig);
        virtual ~DpEngine();

    protected:
        virtual unsigned long run();
        
    private:
        DpEngineConfig* m_pConfig;
    };

}

#endif 
