cmake_minimum_required (VERSION 3.8)

set(PROJECT_NAME "BT")

add_library(${PROJECT_NAME} STATIC 
    BTSock.h BTSock_Windows.cpp BTSock_Linux.cpp
	BTSockListener.h BTSockListener_Windows.cpp BTSockListener_Linux.cpp
	BTAddress.h BTAddress.cpp
    BluezProfile.h BluezProfile.cpp
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
endif()

if(WIN32)
	target_link_libraries(${PROJECT_NAME} windowsapp)
elseif(UNIX)
	target_link_libraries(${PROJECT_NAME} -l:libbluetooth.a sdbus-c++)
	set(CMAKE_CXX_FLAGS "-fpermissive")
endif()


target_include_directories(${PROJECT_NAME} PUBLIC ".")
