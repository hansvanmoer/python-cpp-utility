/* 
 * File:   Module.h
 * Author: hans
 *
 * Created on March 23, 2017, 8:21 PM
 */

#ifndef SCRIPT_MODULE_H
#define	SCRIPT_MODULE_H

#include "ScriptError.h"

#include <unordered_map>

#include <boost/python.hpp>

namespace Game{
    
    namespace Script{
        
        struct ModuleDefinition{
            
            using Id = std::string;
            using Initializer = PyObject * (*)();
            
            Id id;
            Initializer initializer;
            
            ModuleDefinition(const Id &id, Initializer initializer);
        };
        
        class ModuleError : public ScriptError{
        public:
            ModuleError(const ModuleDefinition::Id &id, const std::string &message);
            
            const ModuleDefinition::Id &id() const;
            
        private:
            ModuleDefinition::Id id_;
        };
        
        class DuplicateModuleError : public ModuleError{
        public:
            DuplicateModuleError(const ModuleDefinition::Id &id);
        };
        
        class NoSuchModuleError : public ModuleError{
        public:
            NoSuchModuleError(const ModuleDefinition::Id &id);
        };
        
        class ModuleManager{
        public:
            ModuleManager();
            
            ModuleManager(ModuleManager &&manager);
            
            ModuleManager &operator=(ModuleManager &&manager);
            
            bool has_module(const ModuleDefinition::Id &id) const;
            
            void add_module(const ModuleDefinition &definition);
            
            void add_module(const ModuleDefinition::Id &id, ModuleDefinition::Initializer initializer);
            
            void remove_module(const ModuleDefinition::Id &id);
            
            void import_modules() const;
            
            ~ModuleManager();
            
        private:
            std::unordered_map<ModuleDefinition::Id, ModuleDefinition *> definitions_;

            ModuleManager(const ModuleManager &) = delete;
            ModuleManager &operator=(const ModuleManager &) = delete;
        };
        
    }
    
}

#endif	/* MODULE_H */

