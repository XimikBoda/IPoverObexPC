cmake_minimum_required (VERSION 3.8)

set(PROJECT_NAME "main")

add_executable (${PROJECT_NAME} main.cpp)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
endif()

target_link_libraries(${PROJECT_NAME} SFML::Network BT OBEX)

target_include_directories(${PROJECT_NAME} PUBLIC ".")