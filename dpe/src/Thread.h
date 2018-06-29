/*
 * Thread.h
 *
 *  Created on: Feb 16, 2016
 *      Author: z.j
 */

#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <time.h>
#include <string>
#include <limits.h>

namespace dpe {

    // TODO support restart the thread if it is already exited?
    //      call terminate() then call start() ?
    class Thread {
    public:

        //
        // Start the thread, default is joinable
        // normally if you don't care when the thread exits, better to create 
        // a detached thread, so that the thread resources will be free after exit.
        // but anyway, if you create a joinable thread and forget to join it,
        // the resources will be also recycled after the thread object is deleted,
        // because pthread_detach is called in the destructor function
        //
        bool start(bool isJoinable = true);

        //
        // Wait for termination of the thread. The function returns when the
        // thread has terminated.
        //
        bool wait();

        // return a joinable thread exit status code, only valid after returning true from wait() 
        long getExitStatus() const;

        // terminate (kill) the thread 
        void terminate();

        // return true if thread is running
        bool isRunning() const;

        // get thread name
        std::string getName() const;

        //
        // Return this POSIX thread id. pthread_self() returns the id of the
        // thread calling it, which may not be the same as getId()
        //
        unsigned long getId() const;

        // check if the thread is joinable or not
        bool isJoinable() const;

        // call pthread_detach to change a running thread to PTHREAD_CREATE_DETACHED state
        void detach();

        //
        // start/stop watchdog timer for this thread
        // if this timer expires then the thread supervision function
        // assumes the thread is hanging and terminates the process
        // @param delaTime the number of seconds after which the watchdog 
        //                 timer should be expired
        //
        void startWatchdogTimer(unsigned int deltaTime);
        void stopWatchdogTimer();

        // check if the watchdog timer is expired
        // @return true, if expired
        //         false, otherwise
        bool isWatchdogTimerExpired() const; 

        // sleep milliseconds
        static void sleep(int milli);

        // get the thread schedule policy, return -1 if error
        int getSchedulePolicy();

        // get the thread priority if schedule policy is SCHED_FIFO/SCHED_RR
        // return -1 if error
        int getPriority();

        void setStackSize(unsigned int stackSize);
        unsigned int getStackSize();

    protected:
        Thread(std::string theThreadName);

        virtual 
        ~Thread();

        virtual unsigned long run() = 0;

    private:
        // The start_routine of the thread
        static void* entry(void* theParameter);

        // Thread name
        std::string m_threadName;

        // Indicates if thread is running
        bool m_isRunning;
        // Thread properties
        pthread_attr_t  m_threadAttributes;
        // Thread ID
        pthread_t       m_threadHandle;

        bool m_isJoinable;
        long m_exitStatus;

        volatile time_t m_watchdogTime;

        unsigned int m_stackSize;
    };

    // --------------------------
    inline long Thread::getExitStatus() const {
        return m_exitStatus;
    }

    // --------------------------
    inline std::string Thread::getName() const {
        return m_threadName;
    }

    // --------------------------
    inline unsigned long Thread::getId() const {
        if (m_isRunning) {
            return m_threadHandle;
        }

        return 0L;
    }

    // --------------------------
    inline bool Thread::isRunning() const {
        return m_isRunning;
    }

    // --------------------------
    inline bool Thread::isJoinable() const{
        return m_isJoinable;
    }

    // --------------------------
    inline void Thread::detach() {
        if (m_isRunning && m_isJoinable) {
            pthread_detach(m_threadHandle);
            m_isJoinable = false;
        }
    }

    // ---------------------------
    inline void Thread::startWatchdogTimer(unsigned int deltaTime) {
        // TODO
    }

    // ---------------------------
    inline void Thread::stopWatchdogTimer() {
        m_watchdogTime = 0;
    }

    // ---------------------------
    inline bool Thread::isWatchdogTimerExpired() const {
        // TODO
        return false;
    }

    // ---------------------------
    inline int Thread::getSchedulePolicy() {
        int policy;
        if (pthread_attr_getschedpolicy(&m_threadAttributes, &policy) == 0) {
            return policy;
        }
        return -1;
    }

    // ---------------------------
    inline int Thread::getPriority() {
        struct sched_param param;
        if (pthread_attr_getschedparam(&m_threadAttributes, &param) == 0) {
            return param.__sched_priority;
        }
        return -1;
    }

}


#endif
