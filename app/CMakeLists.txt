#############################################################################
# Author:      Alexandre CARPENTIER
# Modified by:
# Created:     06/11/25
# Copyright:   (c) Alexandre CARPENTIER
# Licence:     LGPL-2.1-or-later
#############################################################################

#go modern
cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 23)

##########################################################################
# main application executable
##########################################################################
project(cppDaqStatic)
enable_language(C CXX)

message("[*] Building " ${PROJECT_NAME})
file(GLOB_RECURSE APPSRC *.cpp *.h)

include_directories(src)
include_directories(src/driver/ni_daqmx_driver/)
include_directories("${MY_ROOT}/dep/daqmx/include/")

##########################################################################
# Build a library .lib
##########################################################################
message("[*] " ${PROJECT_NAME}" building .lib")
add_library(cppDaqStatic STATIC ${APPSRC} "src/dbgprint.h" "src/dbgprint.cpp")

# Librairie inc
if(APPLE)
message("[*] " ${PROJECT_NAME} " target Apple Silicon includes")
elseif(WIN32)
message("[*] " ${PROJECT_NAME} " target Windows daqmx includes")
target_include_directories(cppDaqStatic PUBLIC "${MY_ROOT}/dep/daqmx/include/")
endif()
# Librairie link correct static lib arch
if(IAM_64BIT)
message("[*] " ${PROJECT_NAME} " target 64bit arch")

if(APPLE)
message("[*] " ${PROJECT_NAME} " target Apple lib")
elseif(WIN32)
message("[*] " ${PROJECT_NAME} " target Windows lib")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
message("[*] " ${PROJECT_NAME} " arch is 64bit")
message("${MY_ROOT}/dep/daqmx/lib64/msvc/NIDAQmx.lib")
target_link_libraries(cppDaqStatic PUBLIC "${MY_ROOT}/dep/daqmx/lib64/msvc/NIDAQmx.lib")
endif()

endif()

endif()

if(IAM_32BIT)
message("[*] " ${PROJECT_NAME} " arch is 32bit")
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
target_link_libraries(cppDaqStatic PUBLIC "${MY_ROOT}/dep/daqmx/lib32/msvc/NIDAQmx.lib")
endif()
endif()

##########################################################################
# Build a shared library .dll .so
##########################################################################
message("[*] daq building .dll/.so")
add_library(cppDaq SHARED ${APPSRC} "src/dbgprint.h" "src/dbgprint.cpp")

# Shared inc
if(APPLE)
message("[*] daq target Apple Silicon includes")
elseif(WIN32)
message("[*] daq target Windows daqmx includes")
target_include_directories(cppDaq PUBLIC "${MY_ROOT}/dep/daqmx/include/")
endif()

# Shared link correct static lib arch
if(IAM_64BIT)

if(APPLE)
message("[*] daq target Apple Silicon lib")
elseif(WIN32)
message("[*] daq target 64bit arch")
target_link_libraries(cppDaq PUBLIC "${MY_ROOT}/dep/daqmx/lib64/msvc/NIDAQmx.lib")
endif()

endif()

if(IAM_32BIT)
message("[*] daq target 32bit arch")
target_link_libraries(cppDaq PUBLIC "${MY_ROOT}/dep/daqmx/lib32/msvc/NIDAQmx.lib")
endif()
message("[*] " ${PROJECT_NAME}"Built.")

