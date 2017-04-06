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

namespace Game{

	namespace Script{

		///
		/// This type specifies the basic interface to allow the script system to access python source code
		///  
		class Source{
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
		class BufferedSource : public Source{
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
		class SourceError : public ScriptError{
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
		class FileLoadError : public SourceError{
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
		class AlreadyLoadedError : public SourceError{
		public:
		
			AlreadyLoadedError(const Source::Id &id);
		};
		
		///
		/// An implementation of Source that loads it's python code from a file
		///
		class FileSource : public BufferedSource{
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
		class DuplicateSourceError : public SourceError{
		public:
			///
			/// Creates a new error
			/// \param the dupicate ID
			///
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

