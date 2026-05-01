cmake_minimum_required(VERSION 3.16)

project(TroneOfValor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(TroneOfValor
    source/main.cpp
)

target_include_directories(TroneOfValor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/headers)

file(GLOB LEVEL_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/levels/*.txt"
)

foreach(level_file ${LEVEL_FILES})
    get_filename_component(level_file_name ${level_file} NAME)
    configure_file(${level_file}
                   ${CMAKE_CURRENT_BINARY_DIR}/${level_file_name}
                   COPYONLY)
endforeach()