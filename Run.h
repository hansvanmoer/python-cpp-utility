/* 
 * File:   Run.h
 * Author: hans
 *
 * Created on March 22, 2017, 9:50 PM
 */

#ifndef SCRIPT_RUN_H
#define	SCRIPT_RUN_H

#include "Source.h"

#include <functional>
#include <future>

#include <boost/python.hpp>

namespace Game{
    
    namespace Script{
        
        class GILGuard{
        public:
            GILGuard();
            
            ~GILGuard();
        private:
            PyGILState_STATE state_;
        
            GILGuard(const GILGuard &) = delete;
            GILGuard &operator=(const GILGuard &) = delete;
        };
        
        class RunCancelledError : public SourceError{
        public:
            RunCancelledError(const Source::Id &id);
        };
        
        class Run{
        public:
            
            using BeforeCallback = std::function<void (boost::python::object locals)>;
            using AfterCallback = std::function<void (boost::python::object locals)>;
            
            Run(SourceRef source, BeforeCallback before, AfterCallback after);

            void operator() ();
            
            std::future<bool> create_future();
            
            void flag_error();
            
            ~Run();
            
        private:
            SourceRef source_;
            std::function<void (boost::python::object locals)> before_;
            std::function<void (boost::python::object locals)> after_;
            bool done_;
            std::promise<bool> done_promise_;
            
            
            Run(const Run &) = delete;
            Run &operator=(const Run &) = delete;
        };
    }
    
}

#endif	/* RUN_H */

