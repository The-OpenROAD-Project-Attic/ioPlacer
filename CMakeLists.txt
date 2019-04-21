cmake_minimum_required (VERSION 2.6)
project (IOPlacement)

find_package(Boost COMPONENTS system filesystem program_options REQUIRED)

file(GLOB_RECURSE SRC_FILES src/*.cpp)

add_executable(ioPlacement ${SRC_FILES})
target_include_directories(ioPlacement PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(ioPlacement ${Boost_LIBRARIES})