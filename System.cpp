#include "System.h"

using namespace PythonCppUtility;
using namespace std;

ScriptSystem::ScriptSystem(size_t worker_thread_count) : scheduler_(worker_thread_count, false), sources_(), modules_(), running_(){}

bool ScriptSystem::start(){
    if(running_){
        return false;
    }else{
        running_ = true;
        modules_.import_modules();

        Py_Initialize();

        //Acquiring interpreter lock and starting threading
        PyEval_InitThreads();

        //Release interpreter lock
        main_thread_state_ = PyEval_SaveThread();

        scheduler_.start();
        return true;
    }
}

bool ScriptSystem::stop(){
    if(running_){
        scheduler_.stop();

        // Swap main thread state back in: otherwise the thread state of the last executed thread is used and Py_Finalize segfaults
        PyEval_RestoreThread(main_thread_state_); 
        Py_Finalize();
        running_ = false;
        return true;
    }else{
        return false;
    }
}

bool ScriptSystem::is_running() const{
    return running_;
}

future<bool> ScriptSystem::execute(SourceRef source, Run::BeforeCallback before, Run::AfterCallback after){
    Run *run = new Run{source, before, after};
    scheduler_.submit(run);
    return run->create_future();
}

bool ScriptSystem::execute_and_wait(SourceRef source, Run::BeforeCallback before, Run::AfterCallback after) {
    return execute(source, before, after).get();
}


future<bool> ScriptSystem::execute(const Source::Id &id, Run::BeforeCallback before, Run::AfterCallback after){
    return execute(sources_.get_source(id), before, after);
}

bool ScriptSystem::execute_and_wait(const Source::Id &id, Run::BeforeCallback before, Run::AfterCallback after) {
    return execute_and_wait(sources_.get_source(id), before, after);
}



SourceManager &ScriptSystem::sources(){
    return sources_;
}

const SourceManager &ScriptSystem::sources() const{
    return sources_;
}

ModuleManager &ScriptSystem::modules(){
    return modules_;
}

const ModuleManager &ScriptSystem::modules() const{
    return modules_;
}

ScriptSystem::~ScriptSystem(){
    stop();
}