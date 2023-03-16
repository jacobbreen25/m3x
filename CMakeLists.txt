cmake_minimum_required(VERSION 3.0.0)
project(cmake-testing VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
include(FetchContent)
enable_testing()

#FetchContent_Declare(mahi-gui GIT_REPOSITORY https://github.com/mahilab/mahi-gui.git) 
#FetchContent_MakeAvailable(mahi-gui)

add_subdirectory(include/SimpleBLE-main/simpleble ${CMAKE_BINARY_DIR}/simpleble)


add_executable(cmake-testing main.cpp)
target_link_libraries(cmake-testing simpleble::simpleble)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
