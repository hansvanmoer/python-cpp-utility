#include "Run.h"

#include <boost/python.hpp>

using namespace Game::Script;

using namespace std;

Run::Run(SourceRef source, Run::BeforeCallback before, Run::AfterCallback after) : source_(source), before_(before), after_(after), done_(), done_promise_(){}

void Run::operator()(){
    using namespace boost::python;
    bool result = false;
    GILGuard gil_guard;
    try{
        object module = import("__main__");
        object globals = module.attr("__dict__");
        dict locals;
        before_(locals);
        exec(source_->code(), globals, locals);
        after_(locals);
        result = true;
    }catch(boost::python::error_already_set &e){
        PyErr_Print();
        done_promise_.set_exception(current_exception());
    }catch(...){
        done_promise_.set_exception(current_exception());
    }
    if(result){
        done_promise_.set_value(true);
    }
    done_ = true;
}

void Run::flag_error(){
    done_promise_.set_exception(current_exception());
}

std::future<bool> Run::create_future(){
    return done_promise_.get_future();
}

Run::~Run(){
    if(!done_){
        done_promise_.set_exception(make_exception_ptr(RunCancelledError{source_->id()}));
    }
}

GILGuard::GILGuard(){
    state_ = PyGILState_Ensure();
}

GILGuard::~GILGuard() {
    PyGILState_Release(state_);
}

RunCancelledError::RunCancelledError(const Source::Id& id) : SourceError(id, string{"script run cancelled: "}+ id){};
