// [TERMS&CONDITIONS]

#ifndef __INTERNALDEFINITIONS__
#define __INTERNALDEFINITIONS__

#include "ExternalDefinitions.h"

// --------------------------------------------------------------------------------------------------------
// Version: Defines the Quimera Engine's current version.
// How to use it: Version format is Major version + Minor version + Revision ("##.####.####").
// --------------------------------------------------------------------------------------------------------
const char QE_VERSION[13] = "0.0.0";
const unsigned int QE_VERSION_MAJOR = 0;
const unsigned int QE_VERSION_MINOR = 0;
const unsigned int QE_VERSION_REVISION = 0;


// --------------------------------------------------------------------------------------------------------
// Null Pointer: Defines the value that identifies a null pointer, depending on the compiler.
// --------------------------------------------------------------------------------------------------------
#ifdef QE_COMPILER_MSVC
    #if QE_COMPILER_MSVC >= 10
        #define null_q nullptr // Microsoft Visual C++ 2010 definition for null pointers
    #else
        #define null_q 0
    #endif
#elif QE_COMPILER_GCC
    #define null_q 0
#endif


// --------------------------------------------------------------------------------------------------------
// Dll Export Specifier: Defines which compiler keywords will be used to export symbols when compiling as
// a DLL. Their values are "empty" when compiling the library as static.
// --------------------------------------------------------------------------------------------------------
#ifdef QE_CONFIG_COMPILER_SHAREDLIB // QE_CONFIG_COMPILER_SHAREDLIB is specified as a preprocessor definition [TODO] Thund: Add that definition to preprocessor when configuration is ready
    #ifdef QE_OS_WINDOWS
        #ifdef QE_COMPILER_MSVC
            #define QDllExport __declspec( dllexport )
        #elif QE_COMPILER_GCC
            #define QDllExport __attribute__(( dllexport ))
        #endif
    #elif defined(QE_OS_LINUX)
        #if QE_COMPILER_GCC
            #define QDllExport __attribute__((visibility("default")))( dllexport )
        #endif
    #endif
#elif defined(QE_CONFIG_COMPILER_IMPORT) // QE_CONFIG_COMPILER_IMPORT is specified as a preprocessor definition when compiling the client system
    #ifdef QE_OS_WINDOWS
        #ifdef QE_COMPILER_MSVC
            #define QDllExport __declspec( dllimport )
        #elif QE_COMPILER_GCC
            #define QDllExport __attribute__(( dllimport ))
        #endif
    #elif defined(QE_OS_LINUX)
        #if QE_COMPILER_GCC
            #define QDllExport
        #endif
    #endif
#else // Static library
    #ifdef QE_OS_WINDOWS
        #define QDllExport
    #elif defined(QE_OS_LINUX)
        #define QDllExport
    #endif
#endif


// --------------------------------------------------------------------------------------------------------
// Casting operators: Wrappers for every casting operator to make them shorter and configurable.
// --------------------------------------------------------------------------------------------------------
#define rcast_q(type, object) reinterpret_cast<type>(object)
#define scast_q(type, object) static_cast<type>(object)
#define dcast_q(type, object) dynamic_cast<type>(object)
#define ccast_q(type, object) const_cast<type>(object)


// --------------------------------------------------------------------------------------------------------
// Assertions: Defines assertion statement behavior.
// --------------------------------------------------------------------------------------------------------
#ifndef QE_DISABLE_ASSERTS // This definition must be included as client application's preprocessor definitions to disable assert statements

    #ifdef QE_ASSERT_THROWS_EXCEPTION // This definition must be included as client application's preprocessor definitions to make assertion throw exceptions. This is used for testing purposes
        #include <exception>
        #define QE_ASSERT(expr) { if(!(expr)) throw new std::exception(); } // TODO [Thund]: Create an special exception class for this
    #else
        #ifdef QE_COMPILER_GCC
            namespace // Anonymous namespace to make it internally linked
            {
                /// <summary>
                /// Special behaviour for using GDB so it stops at the line the assertion fails and let the
                /// developer to continue debugging from there on advance.
                /// </summary>
                QDllExport void QE_ASSERT_FAILED()
                {
                    asm("int $3");
                }
            }

            #define QE_ASSERT(expr) (expr) ? (void(0)) : QE_ASSERT_FAILED() // This sentence makes GDB to stop at the failing line and continue the execution later
        #else
            #include <boost/assert.hpp>

            #ifdef BOOST_ASSERT
                #define QE_ASSERT(expr) BOOST_ASSERT(expr)
            #endif
        #endif
    #endif

#endif

#ifndef QE_ASSERT
    #define QE_ASSERT(expr)
#endif


#endif // __INTERNALDEFINITIONS__
