/*
 * DIPlib 3.0
 * This file contains definitions for exceptions and support functions.
 *
 * (c)2014-2017, Cris Luengo.
 * Based on original DIPlib code: (c)1995-2014, Delft University of Technology.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


//
// NOTE!
// This file is included through diplib.h -- no need to include directly
//


#ifndef DIP_ERROR_H
#define DIP_ERROR_H

#include <exception>
#include <vector>
#include <string>

/// \file
/// \brief Defines error macros and default error strings. This file is always included through `diplib.h`.
/// \see infrastructure


namespace dip {


/// \addtogroup infrastructure
/// \{


/// \brief Base exception class. All exceptions thrown in *DIPlib* are derived of this class.
///
/// You can catch this exception at the top level, where you can communicate the problem to the user,
/// and only if you want to prevent your program from exiting abnormally.
/// This class is derived from `std::exception`, so you can choose to catch that instead.
class Error : public std::exception {

   public:

      Error() = default;
      explicit Error( char const* message ) : message_( message ) {}
      explicit Error( const std::string& message ) : message_( message ) {};

      /// \brief Return a message indicating what caused the exception to be thrown, as well as the location
      /// where the error occurred.
      ///
      /// Sometimes multiple locations are given, this is an (incomplete) stack trace that might help figure
      /// out the error. Such a stack trace is generally created when it is a helper function that threw the
      /// exception. Some calling functions will catch such an exception, add its name to the stack trace,
      /// and re-throw the exception.
      ///
      /// \see DIP_ADD_STACK_TRACE, DIP_TRY, DIP_CATCH
      virtual char const* what() const noexcept override { // std::exception::what() is declared noexcept, but this one is not.
         return message_.c_str();
      }

      /// \brief Add an entry to the stack trace. Typically called through the `#DIP_ADD_STACK_TRACE` macro.
      Error& AddStackTrace(
            std::string const& functionName,
            std::string const& fileName,
            unsigned int lineNumber
      ) {
         message_ += "\nin function: " + functionName +
                   " (" + fileName + " at line number " + std::to_string( lineNumber ) + ")";
         return *this;
      }

   private:

      std::string message_;
};

/// \brief Exception class indicating that an internal inconsistency was found (the library code is wrong).
///
/// You shouldn't need to catch exceptions of this type.
///
/// To throw an exception of this type, use the `#DIP_THROW_ASSERTION` and `#DIP_ASSERT` macros.
class AssertionError : public Error {
      using Error::Error;
};

/// \brief Exception class indicating that a function received an inconsistent or out of range parameter
/// (the calling code is wrong).
///
/// Catch exceptions of this type only if you don't control the input arguments (i.e. in a use interface).
///
/// To throw an exception of this type, use the `#DIP_THROW` and `#DIP_THROW_IF` macros.
class ParameterError : public Error {
      using Error::Error;
};

/// \brief Exception class indicating that something happened that we couldn't predict (e.g. file error).
///
/// Catch exceptions of this type if you want to account for run time errors. Note that memory allocation
/// errors are typically `std::bad_alloc`, unless a different allocator is chosen. None of the
/// library functions catch and translate this exception.
///
/// To throw an exception of this type, use the `#DIP_THROW_RUNTIME` macro.
class RunTimeError : public Error {
      using Error::Error;
};


namespace E {
// These are some of the standard what() strings thrown.
// These should all happen in multiple places. You don't need to add
// a string here that is used in only one function or one file.

// image creation errors
constexpr char const* IMAGE_NOT_RAW = "Image is not raw";
constexpr char const* IMAGE_NOT_FORGED = "Image is not forged";

// image data type errors
constexpr char const* DATA_TYPE_NOT_SUPPORTED = "Data type not supported";
constexpr char const* WRONG_DATA_TYPE = "Data type does not match";
constexpr char const* IMAGE_NOT_BINARY = "Image is not binary";

// image dimensionality and dimensions error
constexpr char const* SIZE_EXCEEDS_LIMIT = "Size exceeds address limit";
constexpr char const* ILLEGAL_DIMENSIONALITY = "Illegal dimensionality";
constexpr char const* DIMENSIONALITY_NOT_SUPPORTED = "Dimensionality not supported";
constexpr char const* DIMENSIONALITIES_DONT_MATCH = "Dimensionalities don't match";
constexpr char const* ILLEGAL_DIMENSION = "Illegal dimension";
constexpr char const* SIZES_DONT_MATCH = "Sizes don't match";
constexpr char const* IMAGE_NOT_SCALAR = "Image is not scalar";
constexpr char const* IMAGE_NOT_TENSOR = "Image is not tensor";
constexpr char const* NTENSORELEM_DONT_MATCH = "Number of tensor elements doesn't match";

// image properties errors
constexpr char const* NO_NORMAL_STRIDE = "Image has a non-normal stride";
constexpr char const* IMAGE_NOT_COLOR = "Image is not color";
constexpr char const* INCONSISTENT_COLORSPACE = "Image's number of tensor elements and color space are inconsistent";

// mask image properties errors
constexpr char const* MASK_NOT_BINARY = "Mask image not binary";
constexpr char const* MASK_NOT_SCALAR = "Mask image not scalar";
constexpr char const* MASK_DIMENSIONS_NOT_COMPATIBLE = "Mask image dimensions not compatible";

// indexing errors
constexpr char const* INDEX_OUT_OF_RANGE = "Index out of range";
constexpr char const* COORDINATES_OUT_OF_RANGE = "Coordinates out of range";
constexpr char const* ITERATOR_NOT_VALID = "Iterator is not valid";

// miscellaneous errors
constexpr char const* NOT_IMPLEMENTED = "Functionality has not (yet) been implemented";

// array errors
constexpr char const* ARRAY_ILLEGAL_SIZE = "Array has an illegal size";
constexpr char const* ARRAY_SIZES_DONT_MATCH = "Array sizes don't match";
constexpr char const* ARRAY_OVERFLOW = "Array overflow";
constexpr char const* INITIALIZERLIST_ILLEGAL_SIZE = "Initializer list has an illegal size";

// function parameter errors
constexpr char const* INVALID_PARAMETER = "Parameter has invalid value";
constexpr char const* INVALID_FLAG = "Invalid flag";
constexpr char const* PARAMETER_OUT_OF_RANGE = "Parameter value out of range";
constexpr char const* ARRAY_PARAMETER_WRONG_LENGTH = "Array parameter has the wrong number of elements";
constexpr char const* FILTER_SHAPE_NOT_SUPPORTED = "Filter shape is not supported";
constexpr char const* CONNECTIVITY_NOT_SUPPORTED = "Connectivity is not supported";

}

//
// Test and throw exception
//

#if DIP__HAS_PRETTY_FUNCTION
// This is a better thing to use than __func__, if available
#define DIP__FUNC__ __PRETTY_FUNCTION__
#else
// This is in the C++11 standard, so should always be available
#define DIP__FUNC__ __func__
#endif

/// \brief Adds information from current function (including source file and location within file) to the `dip::Error`.
///
/// This macro is useful for building a stack trace. If you want a stack trace, each function must catch `dip::Error`,
/// add its name to the stack trace, and re-throw the exception.
///
/// You can use this macro as follows:
///
/// ```cpp
///     try {
///        // some DIPlib functions that might throw here
///     }
///     catch( dip::Error& e ) {
///        DIP_ADD_STACK_TRACE( e );
///        throw;
///     }
/// ```
///
/// The `#DIP_START_STACK_TRACE` and `#DIP_END_STACK_TRACE` macros help build this code.
#define DIP_ADD_STACK_TRACE( error ) error.AddStackTrace( DIP__FUNC__, __FILE__, __LINE__ )

/// \brief Throw a `dip::ParameterError`.
#define DIP_THROW( str ) do { auto e = dip::ParameterError( str ); DIP_ADD_STACK_TRACE( e ); throw e; } while( false )

/// \brief Test a condition, throw a `dip::ParameterError` if the condition is met.
#define DIP_THROW_IF( test, str ) do { if( test ) DIP_THROW( str ); } while( false )

/// \brief Throw a `dip::RunTimeError`.
#define DIP_THROW_RUNTIME( str ) do { auto e = dip::RunTimeError( str ); DIP_ADD_STACK_TRACE( e ); throw e; } while( false )


/// \def DIP_THROW_ASSERTION(str)
/// \brief Throw a `dip::AssertionError`.
///
/// If `ENABLE_ASSERT` is set to `OFF` during compilation, this macro is does nothing:
///
/// ```
///     cmake -DENABLE_ASSERT=OFF ...
/// ```

/// \def DIP_ASSERT(test)
/// \brief Test a condition, throw a `dip::AssertionError` if the condition is not met.
///
/// If `ENABLE_ASSERT` is set to `OFF` during compilation, this macro is does nothing:
///
/// ```
///     cmake -DENABLE_ASSERT=OFF ...
/// ```
///
/// You would typically disable assertions for production code, as assertions are only used to test internal
/// consistency or detect bugs in the code.

#ifdef DIP__ENABLE_ASSERT

#define DIP_THROW_ASSERTION( str ) do { auto e = dip::AssertionError( str ); DIP_ADD_STACK_TRACE( e ); throw e; } while( false )

#define DIP_ASSERT( test ) do { if( !( test ) ) DIP_THROW_ASSERTION( "Failed assertion: " #test ); } while( false )

#else

#define DIP_THROW_ASSERTION( str )

#define DIP_ASSERT( test )

#endif


/// \def DIP_START_STACK_TRACE
/// \brief Starts a try/catch block that builds a stack trace when an exception is thrown.
///
/// To build a stack trace, some library functions catch *DIPlib* exceptions, add their name and other info to it,
/// then re-throw. To simplify this mechanism and make it easier to future changes, this macro and its partner
/// `#DIP_END_STACK_TRACE` are used by these library functions. Use then as follows:
///
/// ```cpp
///     DIP_START_STACK_TRACE
///     // some DIPlib functions that might throw here
///     DIP_END_STACK_TRACE
/// ```
///
/// This expands to the exact same code as shown under `#DIP_ADD_STACK_TRACE`.
///
/// NOTE! `DIP_START_STACK_TRACE` starts a try/catch block, which must be closed with `DIP_END_STACK_TRACE` to
/// prevent malformed syntax. Thus you should never use one of these two macros without the other one.
///
/// When compiling with the `EXCEPTIONS_RECORD_STACK_TRACE` set to `OFF`, these macros don't do anything. Turn the
/// option off if your application would make no use of the stack trace, as building the stack trace does incur some
/// runtime cost.

/// \def DIP_END_STACK_TRACE
/// \brief Ends a try/catch block that builds a stack trace when an exception is thrown. See `#DIP_START_STACK_TRACE`.

#ifdef DIP__EXCEPTIONS_RECORD_STACK_TRACE

// NOTE! Yes, we've got an opening brace here and no closing brace. This macro always needs to be paired with DIP_END_STACK_TRACE.
#define DIP_START_STACK_TRACE try {

// NOTE! Yes, we start with a closing brace here. This macro always needs to be paired with DIP_START_STACK_TRACE.
#define DIP_END_STACK_TRACE } catch( dip::Error& e ) { DIP_ADD_STACK_TRACE( e ); throw; }

#else

#define DIP_START_STACK_TRACE
#define DIP_END_STACK_TRACE

#endif

/// \}

} // namespace dip

#endif // DIP_ERROR_H
