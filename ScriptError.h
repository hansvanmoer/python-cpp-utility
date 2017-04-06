///
/// Contains the base error type for all script errors
///

#ifndef SCRIPT_SCRIPT_ERROR_H
#define	SCRIPT_SCRIPT_ERROR_H

#include <stdexcept>

namespace Game{

	namespace Script{
		
		///
		/// A base type for all script errors
		///
		class ScriptError : public std::runtime_error{
		public:
			using std::runtime_error::runtime_error;
		};

	}

}

#endif	/* SCRIPTERROR_H */

