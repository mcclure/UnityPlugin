This is a proof-of-concept project for a Unity project which incorporates [LOVR](https://lovr.org/). It originally incorporated files from a propreitary game project, which have been removed, which may mean this does not work out of the box. It is posted here for Public Interest.

To use this you must install [CMake](https://cmake.org/) to compile the C parts.

This repo was created by Andi McClure <<andi.m.mcclure@gmail.com>> sometime in 2018 incorporating parts of LOVR 0.9.1. The license and provenance information from LOVR were not retained. You can take this repo as being under the [MIT license](https://opensource.org/licenses/MIT) as long as you also follow the license requirements of LOVR.

For more information about how Unity native plugins work, see the official documentation [here](https://docs.unity3d.com/Manual/Plugins.html).

# Building

Build the C plugin before you open the Unity project.

## On windows

Run this from a cmd window. You must `cd` to the "unitynativepluginexample" directory first.

	mkdir build
	cd build
	cmake.exe ../native -DCMAKE_GENERATOR_PLATFORM=x64 -DUNITY_PLUGIN_HEADERS="C:/Program Files/Unity/Hub/Editor/2017.4.6f1/Editor/Data/PluginAPI"
	cmake.exe --build . --target install

If you are not using 2017.4.6f1, you will need to replace "C:/Program Files..." with the correct path for your version of Unity.

## On mac

Run this from a Terminal window. You must `cd` to the "unitynativepluginexample" directory first.

	mkdir -p build && cd build
	cmake ../native -DUNITY_PLUGIN_HEADERS=/Applications/Unity/Hub/Editor/2017.4.6f1/Unity.app/Contents/PluginAPI
	cmake --build . --target install

If you are not using 2017.4.6f1, you will need to replace "/Applications/Unity/Hub..." with the correct path for your version of Unity.

# Editing

Once you have built the C plugin once, you can open the project in Unity and run the "Test" scene. It is supposed to run the file game/main.lua.

The C code is in "native/main.cpp". Every time you change it, you will need to re-run `cmake --build . --target install` from the "build" directory, then **quit and re-open** Unity. You do not need to re-run the other steps.

In "native/main.cpp", there are `UnityPluginLoad` and `UnityPluginUnload` functions that are called. You can also add your own functions by duplicating `UnityPluginCustomTest` and changing the return type (leave the "extern C" and "UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API" bits). If you add a function, you will need to add its signature to the Plugin class in "Assets/Plugin.cs" so you can call it from C# code. To learn more about how this works google "P/Invoke".

