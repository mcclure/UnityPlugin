# CMake file for Unity plugin

cmake_minimum_required(VERSION 3.0.0)
project(UnityPlugin)

###### "JUMPMAN"

include_directories(include static_lib lovr)
set(LOVR_SOURCE
  lovr/lovr.c
  lovr/luax.c
  lovr/util.c
  lovr/lib/map/map.c
  lovr/api/filesystem.c
  lovr/filesystem/blob.c
  lovr/filesystem/file.c
  lovr/filesystem/filesystem.c
  lovr/lib/lua-cjson/fpconv.c
  lovr/lib/lua-cjson/lua_cjson.c
  lovr/lib/lua-cjson/strbuf.c
  lovr/api/types/blob.c
  lovr/lib/lodepng.cpp
)

###### Lua

#macro (install)
#endmacro ()

if (USE_LUAJIT)
  if(WIN32)
    add_subdirectory(dep/luajit luajit)
    include_directories(dep/luajit/src ${CMAKE_BINARY_DIR}/luajit)
    set(LIB_LUA liblua)
  else()
    if (APPLE)
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pagezero_size 10000 -image_base 100000000")
    endif()
    find_package(PkgConfig REQUIRED)
    pkg_search_module(LUAJIT REQUIRED luajit)
    include_directories(${LUAJIT_INCLUDE_DIRS})
    set(LIB_LUA ${LUAJIT_LIBRARIES})
  endif()
else(USE_LUAJIT)
  #option(LUA_USE_RELATIVE_LOADLIB OFF)
  #option(LUA_USE_ULONGJMP OFF)
#  add_subdirectory(dep/lua lua)
#  set_target_properties(lua luac liblua liblua_static PROPERTIES EXCLUDE_FROM_ALL 1)
#  include_directories(dep/lua/src ${CMAKE_BINARY_DIR}/lua)
#  set(LIB_LUA liblua_static)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(LUA REQUIRED lua5.1)
    include_directories(${LUA_INCLUDE_DIRS})
    set(LIB_LUA ${LUA_LIBRARIES})
endif(USE_LUAJIT)

#macro (install)
#  _install(${ARGV})
#endmacro(install)

###### PhysFS

if(WIN32)
  add_subdirectory(dep/physfs physfs)
  include_directories(dep/physfs/src)
  set(LIB_PHYSFS physfs)
else()
  find_package(PhysFS REQUIRED)
  include_directories(${PHYSFS_INCLUDE_DIR})
  set(LIB_PHYSFS ${PHYSFS_LIBRARY})
endif()

###### Unity

# Settings

set(UNITY_PLUGIN_HEADERS )
set(CMAKE_INSTALL_LIBDIR ${CMAKE_CURRENT_SOURCE_DIR}/../Assets/Plugins)
set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_LIBDIR})

# Parse UNITY_PLUGIN_HEADERS setting, or print an error if it has not been set

if (UNITY_PLUGIN_HEADERS)

	include_directories(${UNITY_PLUGIN_HEADERS})

else(UNITY_PLUGIN_HEADERS)

	message(FATAL_ERROR "The CMAKE_INSTALL_LIBDIR variable must be set (like cmake -DCMAKE_INSTALL_LIBDIR=/path) to the PluginAPI directory for your version of Unity. On the Mac this is inside of the Unity.app/, on Windows it is in Program Files.")

endif(UNITY_PLUGIN_HEADERS) 

# Build library

add_library(UnityPlugin MODULE
	main.cpp
	${JUMPMAN_SOURCE}
)

target_link_libraries(UnityPlugin ${LIB_LUA} ${LIB_PHYSFS})

# On OS X, CMake will want to name the library "libUnityPlugin.so", but Unity requires it to be "UnityPlugin.bundle"

if (UNIX AND APPLE) # TODO: iPhone
	set_target_properties(UnityPlugin PROPERTIES
		PREFIX ""
		SUFFIX ".bundle"
	)
endif(UNIX AND APPLE)

# Install library

install(TARGETS UnityPlugin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} RENAME ${UNITY_PLUGIN_NAME}
)
