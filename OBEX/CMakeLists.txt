cmake_minimum_required (VERSION 3.8)

set(PROJECT_NAME "OBEX")

add_library(${PROJECT_NAME} STATIC 
    OBEX.h
    OBEXServer.h OBEXServer.cpp
    OBEXClient.h OBEXClient.cpp
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ".")