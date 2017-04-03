/* 
 * File:   Scheduler.h
 * Author: hans
 *
 * Created on March 22, 2017, 9:52 PM
 */

#ifndef SCRIPT_SCHEDULER_H
#define	SCRIPT_SCHEDULER_H

#include "Run.h"

#include <thread>
#include <condition_variable>
#include <mutex>
#include <list>
#include <vector>


namespace Game{
    
    namespace Script{
    
        class Scheduler{
        public:
            enum class State{
                STARTED, STOPPING, STOPPED
            };
            
            Scheduler(std::size_t max_thread_count = 1, bool start = false); 
            
            Scheduler(Scheduler &&scheduler);
            
            Scheduler &operator=(Scheduler &&scheduler);
            
            bool start();
            
            bool submit(Run *task);
            
            bool stop();
            
            State state() const;
            
            ~Scheduler();
            
        private:
            std::size_t max_thread_count_;
            std::vector<std::thread> threads_;
            std::list<Run *> tasks_;
            State state_;
            mutable std::mutex mutex_;
            std::condition_variable condition_variable_;
            
            void execute_tasks();
            
            Run *wait_for_next_task();
            
            Scheduler(const Scheduler &) = delete;
            Scheduler &operator=(const Scheduler &) = delete;
        };
    
    }
}


#endif	/* SCHEDULER_H */

