///
/// Contains types and functions to load and manage script sources
///

#ifndef SCRIPT_SOURCE_H
#define	SCRIPT_SOURCE_H

#include "ScriptError.h"

#include <string>
#include <memory>
#include <unordered_map>

#include <boost/python.hpp>

namespace Game {

    namespace Script {

        ///
        /// This type specifies the basic interface to allow the script system to access python source code
        ///  

        class Source {
        public:

            ///
            /// A ID type used to uniquely identify a source 
            ///
            using Id = std::string;

            ///
            /// Destroys this source, will not be called by the script system before all executions of a script are finished
            ///
            virtual ~Source();

            ///
            /// Allows access to the python source code
            /// \return a reference to the python source code
            ///
            virtual boost::python::str code() = 0;

            ///
            /// \return the ID of this script, should be unique within the application
            ///
            const Id &id() const;

        protected:

            ///
            /// Creates a new script
            /// \param id the unique ID of this script
            ///
            Source(const Id &id);

        private:

            Id id_;

            Source(const Source &) = delete;
            Source &operator=(const Source &) = delete;
        };

        ///
        /// An implementation of the Source interface that buffers python code in memory to allow fast access
        ///

        class BufferedSource : public Source {
        public:

            ///
            /// Creates a newscript source
            /// \param id the unique ID of the script source
            /// \param code the python source code as a string
            ///
            BufferedSource(const Id &id, const std::string &code);

            ///
            /// Creates a newscript source
            /// \param id the unique ID of the script source
            /// \param code the python source code as a string
            ///
            BufferedSource(const Id &id, std::string &&code);

            ///
            /// see Source::code
            ///
            boost::python::str code();

            ///
            /// Destroys this source and deallocates the python code buffer
            ///
            virtual ~BufferedSource();

        protected:

            ///
            /// Creates a new buffer without setting the code buffer
            /// \param id the unique ID of the script source
            ///
            BufferedSource(const Id &id);

            ///
            /// Sets the script's code buffer to the specified python code
            /// \param code the code
            ///
            void buffer(const std::string &code);

            /// Sets the script's code buffer to the specified python code
            /// \param code the code
            ///
            void buffer(std::string &&code);

            ///
            /// \return a reference to the internal code buffer
            ///
            const std::string &buffer() const;

        private:
            std::string buffer_;
        };

        ///
        /// A base error type for all errors related to source management
        ///

        class SourceError : public ScriptError {
        public:

            ///
            /// Creates a new source error
            /// \param id the ID of the source that caused the error
            /// \param message the message of this error
            ///
            SourceError(const Source::Id &id, const std::string &message);

            ///
            /// \return the ID of the source that caused this error
            ///
            const Source::Id &id() const;

        private:
            Source::Id id_;
        };

        ///
        /// An error indicating that a file containing source code could not be read
        ///

        class FileLoadError : public SourceError {
        public:

            ///
            /// Creates a new file load error
            /// \param id the ID of the source that failed to load
            /// \param path the path of the file that failed to load
            ///
            FileLoadError(const Source::Id &id, const std::string &path);

            ///
            /// \return the path of the file that failed to load
            ///
            const std::string &path() const;

        private:
            std::string path_;
        };

        ///
        /// An error indicating that the source was already loaded
        ///

        class AlreadyLoadedError : public SourceError {
        public:

            ///
            /// Creates a new error 
            /// \param id the unique ID of the script that caused the error
            ///
            AlreadyLoadedError(const Source::Id &id);
        };

        ///
        /// An implementation of Source that loads it's python code from a file
        ///

        class FileSource : public BufferedSource {
        public:
            ///
            /// Creates a new source from the specified file
            /// \param id the unique id of the source
            /// \param path the system dependent path to the source file
            /// \param defer_load if set to true, the source code will be loaded in memory before the first useage, if set to false it will be loaded immediately
            ///
            FileSource(const Id &id, const std::string &path, bool defer_load = false);

            ///
            /// Creates a new source from the specified file and an ID based on the path
            /// \param path the system dependent path to the source file
            /// \param defer_load if set to true, the source code will be loaded in memory before the first useage, if set to false it will be loaded immediately
            ///
            FileSource(const std::string &path, bool defer_load = false);

            ///
            /// Destroys this source
            ///
            ~FileSource();

            ///
            /// see Source::code()
            ///
            boost::python::str code();

            ///
            /// \return the system dependent path of the file from where the source code was/will be loaded
            ///
            const std::string path() const;

            ///
            /// \return true if the source code was already loaded and is currently buffered by this object, false otherwise
            ///
            bool loaded() const;

            ///
            /// Loads the souce code from the file
            /// \throw AlreadyLoadedError if the source file was already loaded
            ///
            void load();

        private:
            std::string path_;
            bool loaded_;
        };

        ///
        /// An error indicating that a source for this ID has already been added
        ///
        class DuplicateSourceError : public SourceError {
        public:
            ///
            /// Creates a new error
            /// \param id the dupicate ID
            ///
            DuplicateSourceError(const Source::Id &id);
        };

        ///
        /// An error indicating that the source for this ID could not be found
        ///
        class NoSuchSourceError : public SourceError {
        public:
            ///
            /// Creates a new error
            /// \param id the id of the source that could not be found
            ///
            NoSuchSourceError(const Source::Id &id);
        };

        ///
        /// A reference counted pointer to a source
        /// This makes sure that no managed source will be deleted before all scripts (running in their own threads) have finished executing
        ///
        using SourceRef = std::shared_ptr<Source>;

        ///
        /// This type can be used to create and keep track of sources
        /// The script's "user" can then keep a reference to the source to execute it later or use this type to do the lookups
        /// Sources are not destroyed until all scripts that run them are finished and all references pointing to them are destroyed
        ///
        class SourceManager {
        public:

            ///
            /// Creates a new source factory
            ///
            SourceManager();

            ///
            /// Creates a new source from the supplied buffer and adds it to the managed sources
            /// \param id the source's unique ID
            /// \param buffer the python source code buffer
            /// \throw DuplicateSourceError if a source with this ID was already added to this manager
            ///
            SourceRef create_source(const Source::Id &id, std::string &&buffer);

            ///
            /// Creates a new source from the supplied buffer and adds it to the managed sources
            /// \param id the source's unique ID
            /// \param buffer the python source code buffer
            /// \throw DuplicateSourceError if a source with this ID was already added to this manager
            ///
            SourceRef create_source(const Source::Id &id, const std::string &buffer);

            ///
            /// Creates a new source from the supplied buffer and adds it to the managed sources
            /// \param id the source's unique ID
            /// \param path a system dependent path to the python source
            /// \param defer_load if set to true, the source code will not be loaded until the source is actually used
            /// \throw DuplicateSourceError if a source with this ID was already added to this manager
            ///
            SourceRef create_source_from_file(const Source::Id &id, const std::string &path, bool defer_load = false);

            ///
            /// Creates a new source from the supplied buffer and adds it to the managed sources
            /// \param path a system dependent path to the python source
            /// \param defer_load if set to true, the source code will not be loaded until the source is actually used
            /// \throw DuplicateSourceError if a source with this ID was already added to this manager
            ///
            SourceRef create_source_from_file(const std::string &path, bool defer_load = false);

            ///
            /// Adds a source to the manager
            /// Sources can be added to multiple managers and each manager will keep track of it.
            /// \param source a reference to the source
            /// \throw DuplicateSourceError if a source with this ID was already added to this manager            
            /// \return a reference to the source
            ///
            SourceRef add_source(SourceRef source);
            
            ///
            /// Removes a source from the manager
            /// If a source is removed it will only be deleted once the last reference to it is destroyed.
            /// Script runs(including those waiting for the scheduler) will keep a reference to their source until they finish executing
            /// \throw NoSuchSourceError if the source was not managed by this manager
            /// \param source a reference to the source
            ///
            void remove_source(SourceRef source);
            
            ///
            /// Removes a source from the manager
            /// If a source is removed it will only be deleted once the last reference to it is destroyed.
            /// Script runs(including those waiting for the scheduler) will keep a reference to their source until they finish executing
            /// \throw NoSuchSourceError if no source for this ID could be found
            /// \param id the source's unique ID
            ///
            void remove_source(const Source::Id &id);
            
            ///
            /// \throw NoSuchSourceError if no source for this ID could be found
            /// \return a reference to the source
            ///
            SourceRef get_source(const Source::Id &id) const;

            ///
            /// Checks if a source was managed by this manager
            /// \param id the source's unique ID
            /// \return true if the source is managed by this manager, false otherwise
            ///
            bool has_source(const Source::Id &id) const;

        private:

            SourceRef add_source(Source *source);

            std::unordered_map<Source::Id, SourceRef> sources_;
        };

    }

}

#endif	/* SOURCE_H */

