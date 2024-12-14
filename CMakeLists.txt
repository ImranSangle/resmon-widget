
cmake_minimum_required(VERSION 3.5)

project(resmon-widget)

enable_testing()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
add_definitions(-DDEBUG)
endif()

include_directories(include)

file(GLOB_RECURSE RES ${CMAKE_CURRENT_SOURCE_DIR}/resources/*)

file(COPY ${RES} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/build/resources)

make_directory(${CMAKE_CURRENT_SOURCE_DIR}/objectfiles)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/objectfiles/icon.o
    COMMAND windres ${CMAKE_CURRENT_SOURCE_DIR}/src/icon.rs -o ${CMAKE_CURRENT_SOURCE_DIR}/objectfiles/icon.o
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/icon.rs
    COMMENT "Generating application icon" 
)

add_executable(resmon-widget src/main.cpp src/draw.cpp src/workers.cpp src/hardwareinfo.cpp src/calculations.cpp objectfiles/icon.o)

target_link_libraries(resmon-widget PRIVATE
gdi32
pdh
)

install(TARGETS resmon-widget DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)

install(DIRECTORY resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/resources)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  set_target_properties(resmon-widget PROPERTIES WIN32_EXECUTABLE true)
endif()
