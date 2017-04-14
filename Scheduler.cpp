#include "Scheduler.h"

#include <cassert>

using namespace PythonCppUtility;
using namespace std;

Scheduler::Scheduler(std::size_t max_thread_count, bool start_after_init) : max_thread_count_(max_thread_count), threads_(), tasks_(), state_(Scheduler::State::STOPPED), mutex_(), condition_variable_(){
    assert(max_thread_count_ != 0);
    if(start_after_init){
        start();
    }
}

bool Scheduler::start(){
    unique_lock<mutex> lock{mutex_};
    if(state_ != Scheduler::State::STOPPED){
        return false;
    }
    state_ = Scheduler::State::STARTED;
    for(size_t i = 0; i < max_thread_count_; ++i){
        threads_.emplace_back([&](){
            this->execute_tasks();
        });
    }
    return true;
}

void Scheduler::execute_tasks(){
    while(true){
        Run *run = wait_for_next_task();
        if(run){
            try{
                run->operator ()();
            }catch(...){
                run->flag_error();
            }
            delete run;
        }else{
            return;
        }
    }
}

Run *Scheduler::wait_for_next_task(){
    while(true){
        unique_lock<mutex> lock{mutex_};
        if(state_ != Scheduler::State::STARTED){
            return nullptr;
        }
        if(tasks_.empty()){
            condition_variable_.wait(lock);
        }else{
            Run *run = tasks_.front();
            tasks_.pop_front();
            return run;
        }
    }
}

bool Scheduler::submit(Run* task){
    assert(task);
    unique_lock<mutex> lock{mutex_};
    tasks_.push_back(task);
    if(state_ == Scheduler::State::STARTED){
        condition_variable_.notify_one();
        return true;
    }else{
        return false;
    }
}

bool Scheduler::stop(){
    {
        unique_lock<mutex> lock{mutex_};
        if(state_ != Scheduler::State::STARTED){
            return false;
        }
        state_ = Scheduler::State::STOPPING;
        condition_variable_.notify_all();
    }
    for(vector<thread>::iterator i = threads_.begin(); i != threads_.end(); ++i){
        if(i->joinable()){
            i->join();
        }
    }
    threads_.clear();
    {
        unique_lock<mutex> lock{mutex_};
        state_ = Scheduler::State::STOPPED;
    }
}

Scheduler::State Scheduler::state() const{
    unique_lock<mutex> lock{mutex_};
    return state_;
}

Scheduler::~Scheduler(){
    stop();
    for(Run *task : tasks_){
        delete task;
    }
}