cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BOOST_INCLUDE_DIR "/usr/include/boost" )

include_directories( 
	${BOOST_INCLUDE_DIR}
)

# Habilitamos sólo el testing en proyectos .hpp
enable_testing()
add_subdirectory(test)