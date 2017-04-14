///
/// This header defines the script system
///
#ifndef PYTHON_CPP_UTILITY_SYSTEM_H
#define	PYTHON_CPP_UTILITY_SYSTEM_H

#include "Source.h"
#include "Run.h"
#include "Scheduler.h"
#include "Module.h"

#include <memory>
#include <functional>
#include <future>

#include <boost/python.hpp>

namespace PythonCppUtility {

    ///
    /// This type should be used as a singleton to control the python interpreter's lifecycle
    /// No scripts should be executed either before start() was called or after stop() is called
    /// Note that support for multiple interpreters is not guaranteed for most implementations of CPython
    /// Support for restarting the python interpreter after it is shutdown is also limited due to CPython quirks
    /// The recommended use is this:
    ///
    /// 1) create an instance of ScriptSystem as a the singleton
    /// 2) Add modules using the module manager
    /// 3) call ScriptSystem::start()
    /// 4) Run your scripts using the execute_??? methods
    /// 5) call ScriptSystem::stop() (which blocks until all currently executing scripts are finished, pushing the others on the xaiting queue)
    /// 6) Destroy the script system instance
    ///
    /// Note that you can add modules and sources whenever you want but that modules will only be (re)loaded when start() is called
    ///
    class ScriptSystem {
    public:

        ///
        /// Creates an instance of a script system
        /// This does not start the python interpreter
        /// \param worker_thread_count the amount of worker threads. Due to CPythons interpreter lock, using many threads will not lead to significant improvement
        ///
        ScriptSystem(std::size_t worker_thread_count = 1);

        ///
        /// Starts the python interpreter after initializing the modules added to the system's module manager
        ///
        bool start();

        ///
        /// Waits for all currently executing scripts to finish, pushes the remaining scripts onto the scheduler's waiting queue and stops the interpreter
        ///
        bool stop();

        ///
        /// \return true if the script system is started, false otherwise
        ///
        bool is_running() const;

        ///
        /// Schedules a script to run from the specified source. Does not block until the script finishes
        /// The caller is responsible for concurrency and consistency of the data used in the callback functions at the moment of execution
        /// \param source a reference to the script's source
        /// \param before a callback to be executed while the GIL is acquired but before the script runs. Should be used to put objects into python's local dictionary for the script to use as arguments
        /// \param after a callback to be executed after the script runs but before the GIL is released. Should be used to put objects to get the results of thes script out of python's local dictionary.
        /// \throw ScriptError if the script could not be executed for whatever reason
        /// \return a future that returns "true" when the script and the after callback has finished executing, will propagate a ScriptError if the script raises a python error or either callback function throws an exception
        ///
        std::future<bool> execute(SourceRef source, Run::BeforeCallback before = Run::BeforeCallback{[](boost::python::object) {}}, Run::AfterCallback after = Run::AfterCallback{[](boost::python::object) {}});

        ///
        /// Schedules a script to run from the specified source. Does not block until the script finishes
        /// The caller is responsible for concurrency and consistency of the data used in the callback functions at the moment of execution
        /// \param source_id the ID of this script's source
        /// \param before a callback to be executed while the GIL is acquired but before the script runs. Should be used to put objects into python's local dictionary for the script to use as arguments
        /// \param after a callback to be executed after the script runs but before the GIL is released. Should be used to put objects to get the results of thes script out of python's local dictionary.
        /// \throw NoSuchSourceError if no source with the specified ID was registered
        /// \throw ScriptError if the script could not be executed for whatever reason
        /// \return a future that returns "true" when the script and the after callback has finished executing, will propagate a ScriptError if the script raises a python error or either callback function throws an exception
        ///
        std::future<bool> execute(const Source::Id &source_id, Run::BeforeCallback before = Run::BeforeCallback{[](boost::python::object) {}}, Run::AfterCallback after = Run::AfterCallback{[](boost::python::object) {}});


        ///
        /// Schedules a script to run from the specified source. Blocks until the script finishes
        /// The caller is responsible for concurrency and consistency of the data used in the callback functions at the moment of execution
        /// \param source a reference to the script's source
        /// \param before a callback to be executed while the GIL is acquired but before the script runs. Should be used to put objects into python's local dictionary for the script to use as arguments
        /// \param after a callback to be executed after the script runs bt before the GIL is released. Should be used to put objects to get the results of thes script out of python's local dictionary.
        /// \throw ScriptError if the script could not be executed for whatever reason, the script raises a python error or one of the callback functions throws an exception
        /// \return "true" when the script and the after callback has finished executing
        ///
        bool execute_and_wait(SourceRef source, Run::BeforeCallback before = Run::BeforeCallback{[](boost::python::object) {}}, Run::AfterCallback after = Run::AfterCallback{[](boost::python::object) {}});

        ///
        /// Schedules a script to run from the specified source. Blocks until the script finishes
        /// The caller is responsible for concurrency and consistency of the data used in the callback functions at the moment of execution
        /// \param source_id a reference to the script's source
        /// \param before a callback to be executed while the GIL is acquired but before the script runs. Should be used to put objects into python's local dictionary for the script to use as arguments
        /// \param after a callback to be executed after the script runs bt before the GIL is released. Should be used to put objects to get the results of thes script out of python's local dictionary.
        /// \throw NoSuchSourceError if no source with the specified ID was registered
        /// \throw ScriptError if the script could not be executed for whatever reason, the script raises a python error or one of the callback functions throws an exception
        /// \return "true" when the script and the after callback has finished executing
        ///
        bool execute_and_wait(const Source::Id &source_id, Run::BeforeCallback before = Run::BeforeCallback{[](boost::python::object) {}}, Run::AfterCallback after = Run::AfterCallback{[](boost::python::object) {}});


        ///
        /// \return a reference to the script system's source manager
        ///
        SourceManager &sources();

        ///
        /// \return a reference to the script system's source manager
        ///
        const SourceManager &sources() const;

        ///
        /// \return a reference to the script system's module manager
        ///
        ModuleManager &modules();

        ///
        /// \return a reference to the script system's module manager
        ///
        const ModuleManager &modules() const;

        ///
        /// Stops the interpreter if it is running and finalizes it
        ///
        ~ScriptSystem();

    private:
        PyThreadState *main_thread_state_;
        Scheduler scheduler_;
        SourceManager sources_;
        ModuleManager modules_;
        bool running_;

        ScriptSystem(const ScriptSystem &) = delete;
        ScriptSystem &operator=(const ScriptSystem &) = delete;
    };

}

#endif	/* PYTHON_CPP_UTILITY_SYSTEM_H */

