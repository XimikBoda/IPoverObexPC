cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("IPoverObexPC")

set(BUILD_SHARED_LIBS FALSE)

if(UNIX)
  include(FetchContent)
  FetchContent_Declare(sdbus-c++
          GIT_REPOSITORY https://github.com/Kistler-Group/sdbus-cpp.git
          GIT_TAG        master
  )
  set(BUILD_EXAMPLES OFF)
  FetchContent_MakeAvailable(sdbus-c++)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Include sub-projects.
set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_GRAPHICS FALSE)
set(SFML_BUILD_WINDOW FALSE)
add_subdirectory(SFML) 

add_subdirectory(main)
add_subdirectory(BT)
add_subdirectory(OBEX)
add_subdirectory(StreamToIP)