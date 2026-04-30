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
project(DataAcquisitionProject)

# Set the current base folder here with abs path: ex C:\\...\\taget_folder, ex C:/.../taget_folder,...
if(EXISTS "C:/DEV/cppDaq")
set(MY_ROOT "C:/DEV/cppDaq")
message("[*] We are at IRAM.")
endif()

if(EXISTS "G:/CODE/cppDaq")
set(MY_ROOT "G:/CODE/cppDaq")
message("[*] We are at Home.")
endif()

if(EXISTS "/Users/alex/Desktop/01-Code/cppDaq")
set(MY_ROOT "/Users/alex/Desktop/01-Code/cppDaq")
message("[*] We are flying on Macbook air.")
endif()

message("[*] Root folder: ${MY_ROOT}")

# Determine the current target arch
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
set(IAM_64BIT ON)
endif()

if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
set(IAM_32BIT ON)
endif()

message("[*] Building" ${CMAKE_PROJECT_NAME})

# Build production app
message("[*] Add app.")
add_subdirectory(app)

#Build unit test app
message("[*] Add test.")
add_subdirectory(test)

message("[*] "  ${CMAKE_PROJECT_NAME}"built.")