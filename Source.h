/* 
 * File:   Source.h
 * Author: hans
 *
 * Created on March 22, 2017, 8:00 PM
 */

#ifndef SCRIPT_SOURCE_H
#define	SCRIPT_SOURCE_H

#include "ScriptError.h"

#include <string>
#include <memory>
#include <unordered_map>

#include <boost/python.hpp>

namespace Game{
    
    namespace Script{
    
        class Source{
        public:
            using Id = std::string;

            virtual ~Source();
            
            virtual boost::python::str code() = 0;
            
            const Id &id() const;
            
        protected:
            Source(const Id &id);

        private:
            
            Id id_;
            
            Source(const Source &) = delete;
            Source &operator=(const Source &) = delete;
        };
                
        class BufferedSource : public Source{
        public:
            BufferedSource(const Id &id, const std::string &code);
            
            BufferedSource(const Id &id, std::string &&code);
            
            boost::python::str code();
        
            virtual ~BufferedSource();
            
        protected:
            
            BufferedSource(const Id &id);
        
            void buffer(const std::string &code);
            
            void buffer(std::string &&code);
            
            const std::string &buffer() const;
            
        private:
            std::string buffer_;
        };        
        
        class SourceError : public ScriptError{
        public:
            SourceError(const Source::Id &id, const std::string &message);
            
            const Source::Id &id() const;
            
        private:
            Source::Id id_;
        };
        
        class FileLoadError : public SourceError{
        public:            
            FileLoadError(const Source::Id &id, const std::string &path);
            
            const std::string &path() const;
            
        private:
            std::string path_;
        };
        
        class AlreadyLoadedError : public SourceError{
        public:
            AlreadyLoadedError(const Source::Id &id);
        };
        
        class FileSource : public BufferedSource{
        public:
            FileSource(const Id &id, const std::string &path, bool defer_load = false);
            
            FileSource(const std::string &path, bool defer_load = false);
            
            ~FileSource();
            
            boost::python::str code();
            
            const std::string path() const;
            
            bool loaded() const;
            
            void load();
            
        private:
            std::string path_;
            bool loaded_;
        };
        
        class DuplicateSourceError : public SourceError{
        public:
            DuplicateSourceError(const Source::Id &id);
        };
        
        class NoSuchSourceError : public SourceError{
        public:
            NoSuchSourceError(const Source::Id &id);
        };
        
        using SourceRef = std::shared_ptr<Source>;
        
        class SourceFactory{
        public:
            
            SourceFactory();
            
            SourceRef create_source(const Source::Id &id, std::string &&buffer);
            
            SourceRef create_source(const Source::Id &id, const std::string &buffer);
            
            SourceRef create_source_from_file(const Source::Id &id, const std::string &path, bool defer_load = false);
            
            SourceRef create_source_from_file(const std::string &path, bool defer_load = false);
            
            SourceRef get_source(const Source::Id &id) const;
            
            bool has_source(const Source::Id &id) const;
            
        private:
            
            SourceRef add_source(Source *source);
            
            std::unordered_map<Source::Id, SourceRef> sources_;
        };
        
    }
    
}

#endif	/* SOURCE_H */

