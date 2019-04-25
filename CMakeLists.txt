cmake_minimum_required (VERSION 2.6)
project (IOPlacement)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Werror")

find_package(Boost COMPONENTS system filesystem program_options REQUIRED)

file(GLOB_RECURSE SRC_FILES src/*.cpp)

add_executable(ioPlacement ${SRC_FILES})
target_include_directories(ioPlacement PUBLIC ${Boost_INCLUDE_DIR})
target_include_directories(ioPlacement PUBLIC 3rdparty)
target_link_libraries(ioPlacement ${Boost_LIBRARIES})
