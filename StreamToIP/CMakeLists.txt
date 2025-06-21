cmake_minimum_required (VERSION 3.8)

set(PROJECT_NAME "StreamToIP")

add_library(${PROJECT_NAME} STATIC 
    StreamToIP.cpp StreamToIP.h
    TCP.cpp TCP.h
    ReaderFromBuf.cpp ReaderFromBuf.h
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
endif()

target_link_libraries(${PROJECT_NAME} SFML::Network)
target_link_libraries(${PROJECT_NAME} DataStream)

target_include_directories(${PROJECT_NAME} PUBLIC ".")