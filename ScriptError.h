///
/// Contains the base error type for all script errors
///

#ifndef PYTHON_CPP_UTILITY_SCRIPT_ERROR_H
#define	PYTHON_CPP_UTILITY_SCRIPT_ERROR_H

#include <stdexcept>

namespace PythonCppUtility {

    ///
    /// A base type for all script errors
    ///
    class ScriptError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

}

#endif	/* PYTHON_CPP_UTILITY_SCRIPT_ERROR_H */

