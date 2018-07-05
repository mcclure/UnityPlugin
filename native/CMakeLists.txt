cmake_minimum_required(VERSION 3.0.0)
project(UnityPlugin)

set(CMAKE_INSTALL_LIBDIR ${CMAKE_CURRENT_SOURCE_DIR}/../Assets)

add_library(UnityPlugin SHARED
	main.cpp
)

install(TARGETS UnityPlugin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

