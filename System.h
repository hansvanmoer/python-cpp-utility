/* 
 * File:   ScriptSystem.h
 * Author: hans
 *
 * Created on March 22, 2017, 9:08 PM
 */

#ifndef SCRIPT_SYSTEM_H
#define	SCRIPT_SYSTEM_H

#include "Source.h"
#include "Run.h"
#include "Scheduler.h"
#include "Module.h"

#include <memory>
#include <functional>
#include <future>

#include <boost/python.hpp>

namespace Game{
    
    namespace Script{
        
        
        
        class ScriptSystem{
        public:
            
            ScriptSystem(std::size_t worker_thread_count = 1);
            
            bool start();
            
            bool stop();
            
            bool is_running() const;
            
            std::future<bool> execute(SourceRef source, Run::BeforeCallback before = Run::BeforeCallback{[](boost::python::object){}}, Run::AfterCallback after = Run::AfterCallback{[](boost::python::object){}});
            
            bool execute_and_wait(SourceRef source, Run::BeforeCallback before = Run::BeforeCallback{[](boost::python::object){}}, Run::AfterCallback after = Run::AfterCallback{[](boost::python::object){}});
            
            SourceFactory &sources();
            
            const SourceFactory &sources() const;

            ModuleManager &modules();

            const ModuleManager &modules() const;
            
            ~ScriptSystem();
            
        private:
            PyThreadState *main_thread_state_;
            Scheduler scheduler_;
            SourceFactory sources_;
            ModuleManager modules_;
            bool running_;
                        
            ScriptSystem(const ScriptSystem &) = delete;
            ScriptSystem &operator=(const ScriptSystem &) = delete;
        };
        
    }
}

#endif	/* SCRIPTSYSTEM_H */

