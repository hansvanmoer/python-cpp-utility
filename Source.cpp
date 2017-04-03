#include "Source.h"

#include <utility>
#include <fstream>
#include <sstream>
#include <iterator>

using namespace Game::Script;

using namespace std;

Source::Source(const Source::Id &id) : id_(id){}

const Source::Id &Source::id() const{
    return id_;
}

Source::~Source(){}

BufferedSource::BufferedSource(const Source::Id &id) : Source(id), buffer_(){}

BufferedSource::BufferedSource(const Source::Id &id, const string &buffer) : Source(id), buffer_(buffer){}

BufferedSource::BufferedSource(const Source::Id &id, string &&buffer) : Source(id), buffer_(forward<string>(buffer)){};

boost::python::str BufferedSource::code(){
    return boost::python::str{buffer_};
}

void BufferedSource::buffer(const string &code){
    buffer_ = code;
}

void BufferedSource::buffer(string &&code){
    buffer_ = forward<string>(code);
}

const string &BufferedSource::buffer() const{
    return buffer_;
}

BufferedSource::~BufferedSource() {
}

SourceError::SourceError(const Source::Id &id, const string &message) : ScriptError(message), id_(id){}

const Source::Id &SourceError::id() const{
    return id_;
}

FileLoadError::FileLoadError(const Source::Id &id, const string& path) : SourceError(id, string{"unable to load script "}+id+string{" from path "}+path), path_(path){}

const string &FileLoadError::path() const{
    return path_;
}

AlreadyLoadedError::AlreadyLoadedError(const Source::Id& id) : SourceError(id, string{"script already loaded: "}+id){}

FileSource::FileSource(const Source::Id &id, const string &path, bool defer_load) : BufferedSource(id), path_(path), loaded_(){
    if(!defer_load){
        load();
    }
}

FileSource::~FileSource(){}

FileSource::FileSource(const string &path, bool defer_load) : FileSource(path,path, defer_load){}

void FileSource::load(){
    if(loaded_){
        throw AlreadyLoadedError{id()};
    }
    ifstream input{path_.c_str()};
    if(input){
        buffer(string{istreambuf_iterator<char>{input}, {}});
        loaded_ = true;
    }else{
        throw FileLoadError{id(), path_};
    }
}

bool FileSource::loaded() const{
    return loaded_;
}

boost::python::str FileSource::code(){
    if(!loaded_){
        load();
    }
    return boost::python::str{buffer()};
}

DuplicateSourceError::DuplicateSourceError(const Source::Id& id) : SourceError(id, string{"duplicate source: "}+id){}

NoSuchSourceError::NoSuchSourceError(const Source::Id& id) : SourceError(id, string{"unknown source: "}+id){}

SourceFactory::SourceFactory() : sources_(){}

SourceRef SourceFactory::create_source(const Source::Id& id, string&& buffer){
    return add_source(new BufferedSource{id, forward<string>(buffer)});
}

SourceRef SourceFactory::create_source(const Source::Id &id, const string &buffer){
    return add_source(new BufferedSource{id, buffer});
}

SourceRef SourceFactory::create_source_from_file(const Source::Id &id, const std::string &path, bool defer_load){
    return add_source(new FileSource{id, path, defer_load});
}

SourceRef SourceFactory::create_source_from_file(const std::string &path, bool defer_load){
    return add_source(new FileSource{path, defer_load});
}

bool SourceFactory::has_source(const Source::Id &id) const{
    return sources_.find(id) != sources_.end();
}

SourceRef SourceFactory::get_source(const Source::Id &id) const{
    auto found = sources_.find(id);
    if(found == sources_.end()){
        throw NoSuchSourceError{id};
    }else{
        return found->second;
    }
}

SourceRef SourceFactory::add_source(Source *source){
    Source::Id id = source->id();
    SourceRef ref{source};
    if(has_source(id)){
        throw DuplicateSourceError{id};
    }else{
        sources_.insert(make_pair(id, ref));
        return ref;
    }
}