#include "Test.h"

#include <iostream>
#include <algorithm>

using namespace Test;
using namespace std;

Entry::Entry(const string &name_, Function function_) : name(name_), function(function_), result(Result::SUCCESS), message(){}

Tests::Tests() : tests_(), tests_by_name_(), latest_result_(Result::SUCCESS), aggregate_result_(Result::SUCCESS){}

void Tests::add(const string &name, Function test_fn){
	auto found = tests_by_name_.find(name);
	if(found == tests_by_name_.end()){
		Entry *test = new Entry{name, test_fn};
		tests_by_name_.emplace(name, test);
		tests_.push_back(test);
	}else{
		throw invalid_argument{string{"duplicate test: "}+name};
	}
}

void Tests::execute(const string &name){
	auto found = tests_by_name_.find(name);
	if(found == tests_by_name_.end()){
		throw invalid_argument{string{"no test found for name: "}+name};
	}else{
		execute(*found->second);
	}
}

void Tests::execute_all(AbortPolicy abort_policy){
	cout << "running all tests" << endl;
	switch(abort_policy){
		case AbortPolicy::ABORT_ON_FAILURE:
			for(Entry *test : tests_){
				if(execute(*test) != Result::SUCCESS){
					return;
				}
			}
			break;
		case AbortPolicy::ABORT_ON_ERROR:
			for(Entry *test : tests_){
				if(execute(*test) == Result::ERROR){
					return;
				}
			}
			break;
		default:
			for(Entry *test : tests_){
				execute(*test);	
			}
			break;
	}
	cout << "test execution complete " << aggregate_result_ << endl;
}

Result Tests::execute(Entry &test){
	cout << "running test " << test.name << endl;
	try{
		test.function();
		add_result(Result::SUCCESS);
	}catch(TestError &e){
		test.result = e.result;
		test.message = e.test_message;
	}catch(exception &e){
		test.result = Result::ERROR;
		test.message = e.what();
	}catch(...){
		test.result = Result::ERROR;
		test.message = "unknown error";
	}
	cout << "test " << test.name << " " << test.result << " " << test.message << endl;
	add_result(test.result);
	return test.result;
}

void Tests::add_result(Result result){
	latest_result_ = result;
	int agg_value = static_cast<int>(aggregate_result_);
	int result_value = static_cast<int>(result);
	if(agg_value < result_value){
		aggregate_result_ = result;
	}
}

Result Tests::aggregate_result() const{
	return aggregate_result_;
}

Result Tests::latest_result() const{
	return latest_result_;
}

Tests &Tests::instance(){
	static Tests tests;
	return tests;
}

Tests::~Tests(){
	for(Entry *test : tests_){
		delete test;
	}
}

ostream &Test::operator<<(ostream &output, Result result){
	switch(result){
		case Result::ERROR:
			return cout << "ERROR";
		case Result::FAILURE:
			return cout << "FAILURE";
		default:
			return cout << "SUCCESS";
	}
}

FunctionResult::FunctionResult(Result result_, const string &message_) : result(result_), message(message_){}

TestError::TestError(Result result_, const string &test_message_) : runtime_error("test error"), result(result_), test_message(test_message_){}

void Test::done(Result result, const string &message){
	throw TestError{result, message};
}

void Test::fail(const string &message){
	done(Result::FAILURE, message);
}

void Test::error(const string &message){
	done(Result::ERROR, message);
}

void success(){
	done(Result::SUCCESS, "");
}

void Test::add_test(const string &name, Function fn){
	Tests::instance().add(name, fn);
}

void Test::add_test(const string &name, ReturningFunction fn){
	Tests::instance().add(name, [=](){
		FunctionResult result = fn();
		if(result.result != Result::SUCCESS){
			done(result.result, result.message);
		}
	});
}

void Test::execute_test(const string &name){
	Tests::instance().execute(name);
}

void Test::execute_all_tests(AbortPolicy abort_policy){
	Tests::instance().execute_all(abort_policy);
}

int Test::test_main(int arg_count, const char **args){
	if(arg_count <= 1){
		execute_all_tests();
	}else{
		for(int i = 1; i < arg_count; ++i){
			execute_test(args[i]);
		}
	}
	return static_cast<int>(Tests::instance().aggregate_result());
}
