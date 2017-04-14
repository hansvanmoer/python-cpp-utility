/* 
 * File:   main.cpp
 * Author: hans
 *
 * Created on March 2, 2017, 1:34 PM
 */

#include <iostream>
#include <chrono>
#include <thread>
#include <chrono>

#include <boost/python.hpp>

#include "Script.h"

#include "Test.h"

using namespace PythonCppUtility;

using namespace std;

struct TestType{
    
    int increment(){
        return 2;
    };
    
};

BOOST_PYTHON_MODULE(TestModule){
    using namespace boost::python;
    
    class_<TestType>("TestType")
        .def("increment", &TestType::increment);
}

void basic_test(){
    ScriptSystem system;
    
    string code{
        "from TestModule import TestType\n"
        "print(\"parameter: \", number)\n"
        "test_object = TestType()\n"
        "number=number+test_object.increment()\n"
    };

    system.sources().create_source("test", code);
    
    system.modules().add_module("TestModule", PyInit_TestModule);
    
    system.start();
    
    SourceRef source = system.sources().get_source("test");
    int i = 10;
    system.execute_and_wait(source, [=](boost::python::object locals){
        locals["number"] = i;
    }, [](boost::python::object locals){
        cout << "result:" << boost::python::extract<int>(locals["number"]) << endl;
    });
}


/*
 * 
 */
int main(int argc, const char** argv) {
	Test::add_test("basic", basic_test);
	return Test::test_main(argc, argv);
}

