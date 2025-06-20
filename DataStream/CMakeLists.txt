cmake_minimum_required (VERSION 3.8)

set(PROJECT_NAME "DataStream")

add_library(${PROJECT_NAME} STATIC 
    DataStream.h
    Reader.h Reader.cpp
    ReaderAgent.h ReaderAgent.cpp
    Writer.h Writer.cpp
    WriterAgent.h WriterAgent.cpp
    Stream.h Stream.cpp
    StreamAgent.h StreamAgent.cpp
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ".")