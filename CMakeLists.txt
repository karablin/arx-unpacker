cmake_minimum_required(VERSION 2.8)
project(arx-unpacker)

set(SRC_DIR src)

include_directories(
	include
)

file(GLOB SOURCES ${SRC_DIR}/*.c*)


#build
add_executable(arx-unpacker ${SOURCES})

#link
#target_link_libraries(arx-unpacker)
