/*
 * Thread.cpp
 *
 *  Created on: Feb 16, 2016
 *      Author: z.j
 */
#include <iostream>
#include "Thread.h"

using namespace dpe;
using namespace std;

Thread::Thread(string theThreadName)
: 
m_threadName(theThreadName),
m_isRunning(false), 
m_threadHandle(0),
m_isJoinable(true),
m_exitStatus(0),
m_watchdogTime(0),
m_stackSize(10240*1024)
{
    pthread_attr_init(&m_threadAttributes);
}

Thread::~Thread()
{
    pthread_attr_destroy(&m_threadAttributes);
    terminate();
    // TODO
}

// ---------------------------------------------------
// Start the thread, return true if thread is started
// or it is already running
bool Thread::start(bool isJoinable) {
    if (!m_isRunning) {
        m_isJoinable = isJoinable;
        int result;

        // Other value is PTHREAD_SCOPE_PROCESS
        // Only need to set this attribute for real-time thread??
        pthread_attr_setscope(&m_threadAttributes, PTHREAD_SCOPE_SYSTEM);

        // If create a joinable thread, no need to this api explicitly
        if (m_isJoinable) {
            pthread_attr_setdetachstate(&m_threadAttributes, PTHREAD_CREATE_JOINABLE);
        } else {
            pthread_attr_setdetachstate(&m_threadAttributes, PTHREAD_CREATE_DETACHED);
        }

        result = pthread_create(&m_threadHandle, &m_threadAttributes, Thread::entry, this);
        if (result != 0) {
            std::cout << "pthread_create error: " << result << std::endl;
            return false;
        }

        m_isRunning = true;
    }

    return true;
}

// -----------------------------------------------
void Thread::setStackSize(unsigned int stackSize) {
    if (!m_isRunning) {
        if (stackSize < PTHREAD_STACK_MIN) {
            m_stackSize = PTHREAD_STACK_MIN;
        } else {
            m_stackSize = stackSize;
        }

        size_t curStackSize;
        pthread_attr_getstacksize (&m_threadAttributes, &curStackSize);
        if (curStackSize != m_stackSize) {
            pthread_attr_setstacksize (&m_threadAttributes, m_stackSize);
            std::cout << "set stack size: " << m_stackSize << std::endl;
        }
    }
}

// -----------------------------------------------
unsigned int Thread::getStackSize() {
    size_t curStackSize;
    pthread_attr_getstacksize (&m_threadAttributes, &curStackSize);
    std::cout << "current stack size: " << curStackSize << std::endl;
    return curStackSize;
}

// -----------------------------------------------
void Thread::terminate() {
    if (m_isRunning) {
#ifdef DEBUG_CM
        std::cout << "Teminating the thread : " << m_threadHandle << std::endl;
#endif        
        int result = 0;
        if ((result = pthread_cancel(m_threadHandle)) != 0) {
#ifdef DEBUG_CM
            std::cout << "Error. pthread_cancel result code is " << result << std::endl;
#endif
        }
        if ((result = pthread_detach(m_threadHandle)) != 0) {
#ifdef DEBUG_CM
            std::cout << "Error. pthread_detach result code is " << result << std::endl;
#endif
        }
        
        m_isRunning = false;
    }
}

// -----------------------------------------------
bool Thread::wait() {
    if (!m_isRunning) {
        return false;
    }

    void *status;
    int result = pthread_join(m_threadHandle, &status);
    if (0 != result) {
        std::cout << "Error. return code from pthread_join is " << result << std::endl;
        return false;
    } else {
        m_exitStatus = (long)status;
        return true;
    }
}

// ------------------------------------------------
void Thread::sleep(int milli) {
    int seconds = milli / 1000;
    if (seconds > 0) {
        ::sleep(seconds);
    }

    milli = milli % 1000;
    if (milli > 0) {
        ::usleep(milli * 1000);
    }    
}

// ----------------------------------------------------
// The start routine of the thread

void* Thread::entry(void* theParameter) {
    Thread* theThread =  (Thread*)theParameter;
    unsigned long result = theThread->run();
#ifdef DEBUG_CM
    std::cout << "Thread::entry return " << result << std::endl;
#endif
    return (void*)result;
}

