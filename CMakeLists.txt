cmake_minimum_required(VERSION 3.10)

#project name
SET(PROJECTNAME ber_tlv)

project("${PROJECTNAME}")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set (CMAKE_VERBOSE_MAKEFILE ON)

ENABLE_LANGUAGE(C CXX)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)



#executable
add_executable(${PROJECTNAME} 
                             ${CMAKE_SOURCE_DIR}/src/main.cpp
                             )
