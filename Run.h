///
/// This file contains some internal types used to keep track of running scripts and their thread state
///

#ifndef SCRIPT_RUN_H
#define	SCRIPT_RUN_H

#include "Source.h"

#include <functional>
#include <future>

#include <boost/python.hpp>

namespace Game{

	namespace Script{

		///
		/// A guard type to lock and unlock Python's interpreter lock using the RAII pattern
		///
		class GILGuard{
		public:

			///
			/// Creates a new guard object, blocking the thread until the GIL can be acquired
			///
			GILGuard();

			///
			/// Destroys the guard object, releases GIL
			///	
			~GILGuard();
		private:
			PyGILState_STATE state_;

			GILGuard(const GILGuard &) = delete;
			GILGuard &operator=(const GILGuard &) = delete;
		};

		///
		/// An error that is thrown to the calling thread when a run is cancelled (e.g. when the script system is shutting down before the run can be executed)
		///
		class RunCancelledError : public SourceError{
		public:
			///
			/// Creates a new error
			/// \param id the source id of the cancelled run
			///
			RunCancelledError(const Source::Id &id);
		};

		///
		/// A type modelling a single script execution.
		/// This type should not be used by the library's user and is only for internal housekeeping
		///
		class Run{
		public:
			
			///
			/// The type of the callback that is executed before a script run is executed.
			/// In this function the local dictionary of the python script should be set
			///
			using BeforeCallback = std::function<void (boost::python::object locals)>;

			///
			/// The type of the callback that is executed after a script is run
			/// In this function the result of the script can be fetched from the local dictonary
			///
			using AfterCallback = std::function<void (boost::python::object locals)>;
			
			///
			/// Creates a new Run object 
			/// The before and after callbacksare executed when the script execution thread has acquired Python's interpreter lock and should be used to respectively put arguments in and extract results from the script's local dictionary
			/// \param source a reference to the source buffer
			/// \param before a callback to be executed before the script run is executed
			/// \param after a callback to be executed after the script is executed
			///
			Run(SourceRef source, BeforeCallback before, AfterCallback after);

			///
			/// blocks until Python's interpreter lock can be acquired and executes the script
			///
			void operator() ();

			///
			/// Creates a future that can be used to wait for the result of this run.
			/// Can be called only once
			/// \return a future that will return 'true' if the script's run was successfully
			///
			std::future<bool> create_future();

			///
			/// Tells the run that an error has occurred and sets the future's exception to the current exception pointer
			/// Should only be called from within a catch block
			///
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

