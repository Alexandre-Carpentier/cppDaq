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

# main application executable project name
project(test_api)
message("[*] Building ${PROJECT_NAME}")

message("[*] Add the google tests framework")
enable_testing()
add_subdirectory(googletest)

#create executable
include_directories("${MY_ROOT}/app/src/")
include_directories("${MY_ROOT}/app/src/driver/ni_daqmx_driver/")
include_directories("${MY_ROOT}/dep/daqmx/include/")
include_directories("${MY_ROOT}/dep/daqmx/googletest/")

#add_executable(test_api "main.cpp" "test.h" "test.cpp" "example.h" "example.cpp")
add_executable(run_test  "test.cpp" )
add_executable(run_example1 "example.h" "example1-cpp-api-basic-read-write.cpp" )
add_executable(run_example2 "example2-cpp-api-device-select.cpp")
add_executable(run_example3 "example3-cpp-api-thread-read-write.cpp")
add_executable(run_example4 "example4-c-api.cpp")
add_executable(run_example5 "example5-c-api-r208driver-usb6001.cpp")
add_executable(run_example6 "example6-c-api-analog-pwm.cpp")
add_executable(run_example7 "example7-c-api-digital-output.cpp")
add_executable(run_example8 "example8-cpp-api-analog-pwm.cpp")
add_executable(run_example9 "example9-cpp-api-analog-input-read.cpp")
add_executable(run_example10 "example10-cpp-api-create-destroy-cycling.cpp")
add_executable(run_example11 "example11-c-api-create-destroy-cycling.cpp")
add_executable(run_example12 "example-12-cpp-api-double-free.cpp")
add_executable(run_example13 "example13-c-api-double-free.cpp")


# Link correct static lib arch
if(IAM_64BIT)
message("[*] " ${PROJECT_NAME} "target 64bit arch")
if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
target_link_libraries(cppDaqStatic PUBLIC "${MY_ROOT}/dep/daqmx/lib64/msvc/NIDAQmx.lib")
endif()
endif()
if(IAM_32BIT)
message("[*] " ${PROJECT_NAME} "target 32bit arch")
if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
target_link_libraries(cppDaqStatic PUBLIC "${MY_ROOT}/dep/daqmx/lib32/msvc/NIDAQmx.lib")
endif()
endif()

# Link the built lib
target_link_libraries(run_test PRIVATE cppDaqStatic)
# Link google test framework
target_link_libraries(run_test PRIVATE gtest_main)

# Link other modules like run_test above
target_link_libraries(run_example1 PRIVATE cppDaqStatic gtest_main)
target_link_libraries(run_example2 PRIVATE cppDaqStatic gtest_main)
target_link_libraries(run_example3 PRIVATE cppDaqStatic gtest_main)
target_link_libraries(run_example4 PRIVATE cppDaqStatic gtest_main)
target_link_libraries(run_example5 PRIVATE cppDaqStatic gtest_main)
target_link_libraries(run_example6 PRIVATE cppDaqStatic gtest_main)
target_link_libraries(run_example7 PRIVATE cppDaqStatic gtest_main)
target_link_libraries(run_example8 PRIVATE cppDaqStatic gtest_main)
target_link_libraries(run_example9 PRIVATE cppDaqStatic gtest_main)
target_link_libraries(run_example10 PRIVATE cppDaqStatic gtest_main)
target_link_libraries(run_example11 PRIVATE cppDaqStatic gtest_main)
target_link_libraries(run_example12 PRIVATE cppDaqStatic gtest_main)
target_link_libraries(run_example13 PRIVATE cppDaqStatic gtest_main)


# Add google test
add_test(
	NAME run_test
	COMMAND run_test
)

message("[*] ${PROJECT_NAME} Built.")

