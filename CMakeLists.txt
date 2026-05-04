#############################################################################
# Author:      Alexandre CARPENTIER
# Modified by:
# Created:     06/11/25
# Copyright:   (c) Alexandre CARPENTIER
# Licence:     LGPL-2.1-or-later
#############################################################################

# Go modern
cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 23)

# Project to make a wrapper over daq librairies
<<<<<<< HEAD
project(cppDaqProjet)
=======
project(DataAcquisitionProject)

# Use CMake's built-in variable for the project root
set(MY_ROOT "${CMAKE_SOURCE_DIR}")

message("[*] Root folder: ${MY_ROOT}")
>>>>>>> 5ab85af (fix:(cmake) MY_ROOT point to CMAKE_SOURCE_DIR)

# Determine the current target arch
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
set(IAM_64BIT ON)
endif()

if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
set(IAM_32BIT ON)
endif()

message("[*] Building " ${CMAKE_PROJECT_NAME})

# Build production app
message("[*] Add app.")
add_subdirectory(app)

#Build unit test app
message("[*] Add test.")
add_subdirectory(test)

message("[*] " ${CMAKE_PROJECT_NAME} " built.")