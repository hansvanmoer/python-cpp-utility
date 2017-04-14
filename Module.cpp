#include "Module.h"

#include <utility>

using namespace PythonCppUtility;
using namespace std;

ModuleDefinition::ModuleDefinition(const ModuleDefinition::Id &id_, ModuleDefinition::Initializer initializer_) : id(id_), initializer(initializer_){}

ModuleError::ModuleError(const ModuleDefinition::Id &id, const std::string &message) : ScriptError(message), id_(id){}

const ModuleDefinition::Id &ModuleError::id() const{
    return id_;
}

DuplicateModuleError::DuplicateModuleError(const ModuleDefinition::Id &id) : ModuleError(id, string{"duplicate module: "}+id){}

NoSuchModuleError::NoSuchModuleError(const ModuleDefinition::Id &id) : ModuleError(id, string{"no such module: "}+id){}

ModuleManager::ModuleManager() : definitions_(){}

bool ModuleManager::has_module(const ModuleDefinition::Id &id) const{
    return definitions_.find(id) != definitions_.end();
}

void ModuleManager::add_module(const ModuleDefinition& definition){
    assert(!definition.id.empty());
    assert(definition.initializer != nullptr);
    if(has_module(definition.id)){
        throw DuplicateModuleError{definition.id};
    }
    definitions_.insert(make_pair(definition.id, new ModuleDefinition{definition}));
}

void ModuleManager::add_module(const ModuleDefinition::Id &id, ModuleDefinition::Initializer initializer){
    assert(!id.empty());
    assert(initializer != nullptr);
    if(has_module(id)){
        throw DuplicateModuleError{id};
    }
    definitions_.insert(make_pair(id, new ModuleDefinition{id, initializer}));
}

void ModuleManager::remove_module(const ModuleDefinition::Id &id){
    auto found = definitions_.find(id);
    if(found == definitions_.end()){
        throw NoSuchModuleError{id};
    }
    definitions_.erase(found);
}

void ModuleManager::import_modules() const{
    for(auto i : definitions_){
        PyImport_AppendInittab(i.first.c_str(), i.second->initializer);
    }
}

ModuleManager &ModuleManager::operator=(ModuleManager &&manager){
    if(&manager != this){
        swap(definitions_, manager.definitions_);
    }
    return *this;
}

ModuleManager::~ModuleManager(){
    for(auto i : definitions_){
        delete i.second;
    }
};


