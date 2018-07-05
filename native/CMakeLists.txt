cmake_minimum_required(VERSION 3.0.0)
project(UnityPlugin)

set(UNITY_PLUGIN_HEADERS )
set(CMAKE_INSTALL_LIBDIR ${CMAKE_CURRENT_SOURCE_DIR}/../Assets)

if (UNITY_PLUGIN_HEADERS)

	include_directories(${UNITY_PLUGIN_HEADERS})

else(UNITY_PLUGIN_HEADERS)

	message(FATAL_ERROR "The CMAKE_INSTALL_LIBDIR variable must be set (like cmake -DCMAKE_INSTALL_LIBDIR=/path) to the PluginAPI directory for your version of Unity. On the Mac this is inside of the Unity.app/, on Windows it is in Program Files.")

endif(UNITY_PLUGIN_HEADERS) 

add_library(UnityPlugin SHARED
	main.cpp
)

install(TARGETS UnityPlugin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

