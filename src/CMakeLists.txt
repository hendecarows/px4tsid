cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
	${PROJECT_NAME}
	chset.cpp
	config.cpp
	convert.cpp
	px4_device.cpp
	tsid_scan.cpp
	main.cpp
)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
	${CMAKE_SOURCE_DIR}/json/single_include/nlohmann
)
