/*
 * EventIndicator.h
 *
 *  Created on: Apr 19, 2016
 *      Author: z.j
 */

#ifndef EVENT_INDICATOR_H
#define EVENT_INDICATOR_H

#include <pthread.h>

namespace dpe {

    // Thread A calls wait() and blocks if m_eventIsSet is never set "true" before
    // Thread B calls set() to set m_eventIsSet and send notification to Thread A
    class EventIndicator {
    public:
        EventIndicator(bool eventIsSet = false);
        virtual ~EventIndicator();

        void wait();

        void set();

        // @description - reset the event 
        void reset();

    private: 
        pthread_cond_t m_condition;
        pthread_mutex_t m_mutex;
        bool m_eventIsSet;
    };
}

#endif
