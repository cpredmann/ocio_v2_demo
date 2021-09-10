cmake_minimum_required(VERSION 3.12)

project(ocio_v2_demo)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenColorIO)

add_executable(
	DemoProgram
	DemoProgram.cpp
)

target_link_libraries(
	DemoProgram
	PRIVATE
	OpenColorIO::OpenColorIO
)
