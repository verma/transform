#
# top-level CMake configuration file for TRANSFORM
#
# (based originally on the libLAS files copyright Mateusz Loskot)

if(WIN32)
cmake_minimum_required(VERSION 2.8.7)
else()
cmake_minimum_required(VERSION 2.6.0)
endif()

project(TRANSFORM)

#------------------------------------------------------------------------------
# internal cmake settings
#------------------------------------------------------------------------------

set(CMAKE_COLOR_MAKEFILE ON)

# Allow advanced users to generate Makefiles printing detailed commands
mark_as_advanced(CMAKE_VERBOSE_MAKEFILE)

# Path to additional CMake modules
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})


#------------------------------------------------------------------------------
# PDAL general settings
#------------------------------------------------------------------------------



# the next three lines are the ONLY place in the entire pdal system where
# the version info is hard-coded
SET(TRANSFORM_VERSION_MAJOR "0")
SET(TRANSFORM_VERSION_MINOR "0")
SET(TRANSFORM_VERSION_PATCH "1")

set(TRANSFORM_VERSION_STRING "${TRANSFORM_VERSION_MAJOR}.${TRANSFORM_VERSION_MINOR}.${TRANSFORM_VERSION_PATCH}")
SET(TRANSFORM_LIB_SOVERSION ${TRANSFORM_VERSION_MAJOR})
SET(TRANSFORM_C_LIB_SOVERSION ${TRANSFORM_VERSION_MAJOR})


# Name of C++ library
set(TRANSFORM_LIB_NAME transform)

set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)



#------------------------------------------------------------------------------
# misc user options
#------------------------------------------------------------------------------

# Choose package components
set(WITH_TESTS TRUE CACHE BOOL "Choose if TRANSFORM unit tests should be built")


#------------------------------------------------------------------------------
# test harness settings
#------------------------------------------------------------------------------

# Name of test suite runner
set(TRANSFORM_UNIT_TEST transform_tests)




#------------------------------------------------------------------------------
# General build settings
#------------------------------------------------------------------------------

# note we default to debug mode
if(NOT MSVC_IDE)
  if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING 
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel" FORCE)
  endif()
  message(STATUS "Setting TRANSFORM build type - ${CMAKE_BUILD_TYPE}")
endif()

set(PROJCL_BUILD_TYPE ${CMAKE_BUILD_TYPE})


#------------------------------------------------------------------------------
# Platform and compiler specific settings
#------------------------------------------------------------------------------

if(WIN32)
  if (MSVC)
    if (MSVC80 OR MSVC90 OR MSVC10 OR MSVC11)
      add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
      add_definitions(-D_CRT_SECURE_NO_WARNINGS)
      add_definitions(-D_CRT_NONSTDC_NO_WARNING)
      add_definitions(-D_SCL_SECURE_NO_WARNINGS)

      set(TRANSFORM_COMPILER_MSVC 1)
      if (MSVC11)
        set(TRANSFORM_COMPILER_VC11 1)
      elseif (MSVC10)
        set(TRANSFORM_COMPILER_VC10 1)
      elseif (MSVC9)
        set(TRANSFORM_COMPILER_VC9 1)
      elseif (MSVC8)
        set(TRANSFORM_COMPILER_VC8 1)
      endif()
    endif()
    
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
      string(REGEX REPLACE "/W[0-4]" "/W4"
        CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
  endif()
  
  set(TRANSFORM_PLATFORM_WIN32 1)

else()
  # not WIN32

  # Recommended C++ compilation flags
  # -Weffc++
  set(TRANSFORM_COMMON_CXX_FLAGS
          "-std=c++11 -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wpointer-arith -Wcast-align -Wcast-qual -Wfloat-equal -Wredundant-decls -Wno-long-long -fPIC")

  if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TRANSFORM_COMMON_CXX_FLAGS}")
    if (CMAKE_COMPILER_IS_GNUCXX)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()
    
    set(TRANSFORM_COMPILER_GCC 1)
    
  elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER}" MATCHES "clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TRANSFORM_COMMON_CXX_FLAGS}")
    set(TRANSFORM_COMPILER_CLANG 1)
  endif()

  # Compiler-specific C++11 activation.
  if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
	  execute_process(
		  COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
	  if (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
		  message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater.")
	  endif ()
  elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  else ()
	  message(FATAL_ERROR "Your C++ compiler does not support C++11.")
  endif ()
endif(WIN32)

if (APPLE)
  set(SO_EXT dylib)
  set(CMAKE_FIND_FRAMEWORK "LAST")
elseif(WIN32)
  set(SO_EXT dll)
else()
  set(SO_EXT so)
endif(APPLE)

# setup dependent libraries
if(WIN32 AND MSVC11)
	set(BOOST_ROOT "C:/local")
endif(WIN32 AND MSVC11)

set(BOOST_USE_STATIC_LIBS OFF)
set(BOOST_USE_MULTITHREADED ON)
set(BOOST_USE_STATIC_RUNTIME OFF)

find_package(Boost "1.54.0" COMPONENTS unit_test_framework)
if(NOT Boost_FOUND)
	message(FATAL_ERROR "boost is required")
endif(NOT Boost_FOUND)

set(TRANSFORM_DEPENDENT_LIBRARIES ${Boost_LIBRARIES})

find_package(PROJ4 4.8.0)
if(NOT PROJ4_FOUND)
	message(WARNING "PROJ4 library is missing, proj4 backend and calculation tests will not be available")
else(NOT PROJ4_FOUND)
	set(TRANSFORM_HAVE_PROJ4 1)
	add_definitions(-DHAVE_PROJ4)
	include_directories(${PROJ4_INCLUDE_DIR})
	set(TRANSFORM_DEPENDENT_LIBRARIES ${TRANSFORM_DEPENDENT_LIBRARIES} ${PROJ4_LIBRARIES})
endif(NOT PROJ4_FOUND)

# find opencl
if(APPLE)
	FIND_LIBRARY(OPENCL_LIBRARY OpenCL)
	MARK_AS_ADVANCED(OPENCL_LIBRARY)

	set(TRANSFORM_DEPENDENT_LIBRARIES ${TRANSFORM_DEPENDENT_LIBRARIES} ${OPENCL_LIBRARY})
endif()

include_directories(${Boost_INCLUDE_DIRS})

set(TRANSFORM_LIBRARY transform)

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(benchmarks)

if(WITH_TESTS)
	add_subdirectory(test)
endif()

