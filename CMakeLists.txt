cmake_minimum_required (VERSION 2.6)
project (ioPlacement)

set(BIN_NAME "ioPlacement")
set(LIB_NAME "ioPlace")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Werror -O3 -fopenmp")

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

find_package(Boost COMPONENTS system filesystem program_options REQUIRED)

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

if(DEFINED ENV{TYPE_CALL})
        message("-- Will generate lib")

        list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/Main.cpp)
        list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/DEFParser.cpp)

        add_definitions(-DLIBRARY_MODE)
        add_LIBRARY(${LIB_NAME} STATIC ${SRC_FILES})

        target_include_directories(${LIB_NAME} PUBLIC ${Boost_INCLUDE_DIR})
        target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
        target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
        target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/def)
        target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/lef)

        target_link_libraries(${LIB_NAME} def lef)
        target_link_libraries(${LIB_NAME} ${Boost_LIBRARIES})
else()
        message("-- Will generate binary")

        add_definitions(-DSTANDALONE_MODE)
        add_executable(${BIN_NAME} ${SRC_FILES})

        target_include_directories(${BIN_NAME} PUBLIC ${Boost_INCLUDE_DIR})
        target_include_directories(${BIN_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
        target_include_directories(${BIN_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
        target_include_directories(${BIN_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/def)
        target_include_directories(${BIN_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/lef)

        target_link_libraries(${BIN_NAME} def lef)
        target_link_libraries(${BIN_NAME} ${Boost_LIBRARIES})
endif()
