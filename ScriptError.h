/* 
 * File:   ScriptError.h
 * Author: hans
 *
 * Created on March 22, 2017, 8:14 PM
 */

#ifndef SCRIPT_SCRIPT_ERROR_H
#define	SCRIPT_SCRIPT_ERROR_H

#include <stdexcept>

namespace Game{
    
    namespace Script{
        
        class ScriptError : public std::runtime_error{
            using std::runtime_error::runtime_error;
        };
        
    }
    
}

#endif	/* SCRIPTERROR_H */

