///
/// Contains a scheduler with a thread pool to manage the script runs
///

#ifndef SCRIPT_SCHEDULER_H
#define	SCRIPT_SCHEDULER_H

#include "Run.h"

#include <thread>
#include <condition_variable>
#include <mutex>
#include <list>
#include <vector>


namespace Game{

	namespace Script{
	
	///
	/// A type representing a scheduler
	///
	class Scheduler{
		public:
			///
			/// An enumeration type representing the scheduler's state
			///
			enum class State{
				///
				/// The scheduler is started, accepting and running new tasks
				///
				STARTED, 
				///
				/// The scheduler is stopping, finishing currently running tasks and pushing new tasks on a queue for future execution
				///
				STOPPING, 
				///
				/// The scheduler has stopped, all new tasks are pushed on a queue for future execution
				///
				STOPPED
			};
			
			///
			/// Creates a new scheduler
			/// \param max_thread_count the maximum number of threads used to execute scripts
			/// \param start if set to true, the scheduler will start before the constructor completes, otherwise it remains in the stopped state
			///
			Scheduler(std::size_t max_thread_count = 1, bool start = false); 

			///
			/// Move constructor
			//
			Scheduler(Scheduler &&scheduler);
			
			///
			/// Move assignment operator
			///
			Scheduler &operator=(Scheduler &&scheduler);

			///
			/// Starts the scheduler if it is not already started or stopping
			/// This method is thread safe  and may block
			/// \return true if the scheduler was started, false if it was either already started or stopping
			///
			bool start();

			///
			/// Adds a new run to the scheduler's queue
			/// The scheduler takes ownership of the task once it is added to the queue and will delete it when appropriate
			/// This method is thread safe and may block
			/// \param task the script run to execute
			/// \return true if the scheduler is running, false otherwise
			bool submit(Run *task);

			///
			/// Stops the scheduler if it is started
			/// This method is thread safe and may block until all currently executing tasks are completed
			/// Any tasks not yet started will be added to the queue and will execute when the scheduler is again started 
			/// \return true if the scheduler was stopped, false if it was not running or already stopping when this method was called
			///
			bool stop();
			
			///
			/// Returns the current state of the scheduler
			/// This method is thread safe and may block
			/// \return the scheduler's state
			///
			State state() const;
			
			///
			/// Stops the scheduler if it is running and destroys it
			/// This will destroy all tasks that are not yet started
			///
			~Scheduler();

		private:
			std::size_t max_thread_count_;
			std::vector<std::thread> threads_;
			std::list<Run *> tasks_;
			State state_;
			mutable std::mutex mutex_;
			std::condition_variable condition_variable_;

			void execute_tasks();

			Run *wait_for_next_task();

			Scheduler(const Scheduler &) = delete;
			Scheduler &operator=(const Scheduler &) = delete;
		};
	}	
}


#endif	/* SCHEDULER_H */

