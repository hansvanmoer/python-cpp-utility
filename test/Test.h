#ifndef TEST_TEST_H
#define TEST_TEST_H

#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

namespace Test{

	enum class Result{
		SUCCESS = 0, FAILURE = 1, ERROR = 99
	};
	
	std::ostream &operator<<(std::ostream &output, Result result);

	using Function = std::function<void ()>;

	struct FunctionResult{
		Result result;
		std::string message;

		
		FunctionResult(Result result = Result::SUCCESS, const std::string &message = std::string{});
		FunctionResult(const std::string &message);
	};

	using ReturningFunction = std::function<FunctionResult ()>;

	struct Entry{
		std::string name;
		Function function;	
		Result result;
		std::string message;

		Entry(const std::string &name, Function function);
	};

	enum class AbortPolicy{
		ABORT_ON_FAILURE, ABORT_ON_ERROR, RUN_ALL_TESTS
	};

	class TestError : public std::runtime_error{
	public:
		TestError(Result result, const std::string &message);
		
		const Result result;
		const std::string test_message;
	};

	class Tests{
	public:		

		static Tests &instance();
	
		void add(const std::string &name, Function test);

		void execute(const std::string &name);
		
		void execute_all(AbortPolicy abort_policy);

		Result latest_result() const;

		Result aggregate_result() const;
	
	private:
		std::vector<Entry *> tests_;
		std::unordered_map<std::string, Entry *> tests_by_name_;
		Result latest_result_;
		Result aggregate_result_;		

		void add_result(Result new_result);

		Result execute(Entry &test);

		Tests();

		~Tests();		

		Tests(const Tests &) = delete;
		Tests &operator=(const Tests &) = delete;
	};

	void done(Result result, const std::string &message);

	void success();

	void fail(const std::string &message);

	void error(const std::string &message);

	void add_test(const std::string &name, Function test);

	void add_test(const std::string &name, ReturningFunction test);

	void execute_test(const std::string &name);

	void execute_all_tests(AbortPolicy abort_policy = AbortPolicy::RUN_ALL_TESTS);
	
	int test_main(int arg_count, const char **args);
}

#endif
