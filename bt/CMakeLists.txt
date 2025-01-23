cmake_minimum_required (VERSION 3.8)

set(PROJECT_NAME "bt")

add_library(${PROJECT_NAME} STATIC 
    bt.h 
	bt_Windows.cpp)

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
endif()

if(WIN32)
	target_link_libraries(${PROJECT_NAME} windowsapp)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ".")