///
/// Contains types to make management of embedded modules easier
///

#ifndef SCRIPT_MODULE_H
#define	SCRIPT_MODULE_H

#include "ScriptError.h"

#include <unordered_map>

#include <boost/python.hpp>

namespace Game{

	namespace Script{
		
		///
		/// A simple type containing all module metadata for embedded modules
		///
		struct ModuleDefinition{
			
			///
			/// The type of the id refering to a module
			///
			using Id = std::string;

			///
			/// The type of the initializer function as defined by CPython
			///
			using Initializer = PyObject * (*)();

			///
			/// The module's unique id
			///
			Id id;

			///
			/// The module's initializer function
			///
			Initializer initializer;

			///
			/// Creates a new module definition
			/// \param id the module's unique id
			/// \param initializer the module's initializer function
			///
			ModuleDefinition(const Id &id, Initializer initializer);
		};

		///
		/// A base type for all errors related to module management
		///
		class ModuleError : public ScriptError{
		public:
			///
			/// Creates a new module error
			/// \param id the module's id
			/// \param message the message for this error
			///
			ModuleError(const ModuleDefinition::Id &id, const std::string &message);
		
			///
			/// \return the id of the module that caused this error
			///
			const ModuleDefinition::Id &id() const;

		private:
			ModuleDefinition::Id id_;
		};

		///
		/// An error that is thrown when attempting to add define the same module twice or define two distinct modules with the same id
		///
		class DuplicateModuleError : public ModuleError{
		public:
			///
			/// Creates a new error
			/// \param id the module's id
			///
			DuplicateModuleError(const ModuleDefinition::Id &id);
		};

		///
		/// An error that is thrown when attempting to load a module that was not defined
		///
		class NoSuchModuleError : public ModuleError{
		public:
			///
			/// Creates a new error
			/// \param id the undefined module's id
			///
			NoSuchModuleError(const ModuleDefinition::Id &id);
		};

		///
		/// This type keeps track of registered modules
		///
		class ModuleManager{
		public:

			///
			/// Creates a new module manager
			///
			ModuleManager();

			///
			/// Creates a new module manager from the original via a move constructor
			/// \param manager the original module manager
			///
			ModuleManager(ModuleManager &&manager);

			///
			/// Moves all registered modules from the original to this module manager, discarding the current modules
			/// \param manager the original module manager
			/// \return a reference to this module manager
			///
			ModuleManager &operator=(ModuleManager &&manager);

			///
			/// Checks whether a module with the given id is defined
			/// \param the module's id
			/// \return true if the module was registered, false otherwise
			///
			bool has_module(const ModuleDefinition::Id &id) const;
			
			///
			/// Adds a module definition
			/// \param definition the module's definition
			/// \throw DuplicateModuleError if a module with the definition's id was already defined
			///
			void add_module(const ModuleDefinition &definition);

			///
			/// Adds a module definition
			/// \param id the module's id
			/// \param initializer the module's initializer
			/// \throw DuplicateModuleError if a module with the definition's id was already defined
			///
			void add_module(const ModuleDefinition::Id &id, ModuleDefinition::Initializer initializer);

			///
			/// Removes a module
			/// \param id the module's id
			/// \throw NoSuchModul if no module with the specified id was found
			///
			void remove_module(const ModuleDefinition::Id &id);
		
			///
			/// Adds all defined modules to the current embedded python interpreter
			/// Should be called by the script system singleton before it is started
			///
			void import_modules() const;

			///
			/// Destroys this module manager
			/// Does tot unregister the modules from the python interpreter, nor does it delete or modify the module's initializer function
			///				
			~ModuleManager();

		private:
			std::unordered_map<ModuleDefinition::Id, ModuleDefinition *> definitions_;

			ModuleManager(const ModuleManager &) = delete;
			ModuleManager &operator=(const ModuleManager &) = delete;
		};

	}

}

#endif	/* MODULE_H */

