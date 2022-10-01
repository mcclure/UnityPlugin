extern "C" {

#include "luax.h"

}

#include "IUnityInterface.h"
#include <stdio.h>
#include "game/game.h"
#include "game/jumpman.h"
#include <physfs.h>
#include "game/hook.h"
// #include "game/jumpman_unity_bridge.h"

// Unity plugin load event
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
    UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
}

// Unity plugin unload event
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
    UnityPluginUnload()
{
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
    UnityPluginCustomTest()
{
    return 3;
}

static lua_State* L;
static bool died;

extern "C" {
void lovrInit(lua_State* L);
void lovrStep(lua_State* L);
}

FEEDSTRING printFunction;
CREATEMESH CreateMesh;
RESIZEMESH ResizeMesh;
GETBUFFERPOINTER GetBufferPointer;
FLUSHMESH FlushMesh;

extern "C" void printError(const char *tag) {
    ostringstream str;
    str << tag << ":\n" << luaL_checkstring(L, -1);
    lua_pop(L, 1);
    printFunction(str.str().c_str());
}

static bool inited = false;

// THIS FUNCTION IS SUBSTANTIALLY BASED ON luaB_print FROM LUA SOURCE AND SHOULD BE MOVED TO ITS OWN FILE AS IT IS LIKELY LICENSE ENCUMBERED
static int luaB_print_override (lua_State *L) {
  ostringstream str;
  int n = lua_gettop(L);  /* number of arguments */
  int i;
  lua_getglobal(L, "tostring");
  for (i=1; i<=n; i++) {
    const char *s;
    lua_pushvalue(L, -1);  /* function to be called */
    lua_pushvalue(L, i);   /* value to print */
    lua_call(L, 1, 1);
    s = lua_tostring(L, -1);  /* get result */
    if (s == NULL)
      return luaL_error(L, LUA_QL("tostring") " must return a string to "
                           LUA_QL("print"));
    if (i>1) str << "\t";
    str << s;
    lua_pop(L, 1);  /* pop result */
  }
  printFunction(str.str().c_str());
  return 0;
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
    UnityPluginInit(const char *_path, FEEDSTRING _printFunction, CREATEMESH _CreateMesh, RESIZEMESH _ResizeMesh, GETBUFFERPOINTER _GetBufferPointer, FLUSHMESH _FlushMesh)
{
    printFunction = _printFunction;
    printFunction("Booted.");
    string path = _path;
    path += "/game";

    CreateMesh = _CreateMesh; ResizeMesh = _ResizeMesh; GetBufferPointer = _GetBufferPointer; FlushMesh = _FlushMesh;
    

    #if 1
    if (L)
		lua_close(L);
	
    L = luaL_newstate();
    
    if (!L) {
        printFunction("Failed to launch LuaJIT.");
        died = true;
        return;
    }

    luaL_openlibs(L);

    // Install custom print
    static const struct luaL_Reg printHack [] = {
        {"print", luaB_print_override},
        {NULL, NULL} /* end of array */
    };
    lua_getglobal(L, "_G");
    luaL_register(L, NULL, printHack); // "for Lua versions < 5.2"
    //luaL_setfuncs(L, printlib, 0);  // "for Lua versions 5.2 or greater"
    lua_pop(L, 1);

    // Setup args
    // arg global
    lua_newtable(L);
    lua_pushstring(L, path.c_str());
    lua_rawseti(L, -2, 1);
    lua_setglobal(L, "arg");

    //luax_preloadmodule(L, "game", l_gameInit);

    lovrInit(L);
#endif
#if 0
    if (!inited) {
        if (!PHYSFS_init(NULL)) {
            printFunction("PhysFS failed!");
            died = true;
            return;
        }
        PHYSFS_mount("/", "/", 0); // Do I need to also lovrFilesystemInit?
        PHYSFS_mount("/Users/mcc/work/gh/lovr-jumpman/game/lua", "/", 0);

        inited = true;
    }
#else
#if 0
    // This is from a version of this repo that loaded code from a game and then extracted a 3D level mesh to display. 
    PHYSFS_mount("/", "/", 0);
    PHYSFS_mount(path.c_str(), "/", 0);
    #endif
    #if 0
    path += "/Intro";
    gameInit(path.c_str());
//    loadGame(FromInternal, "/Users/mcc/work/gh/lovr-jumpman/game/lua/Main.jmp", 0);

    quadpile &q = level[0].staticVertex;
    int x = CreateMesh();
    ResizeMesh(x, q.size()/12); // Not correct with textures
    float *pos = (float *)GetBufferPointer(x, 0);
    float *color = (float *)GetBufferPointer(x, 1);
    for(int c = 0; c < q.size(); c+=3) {
        int idx = c/3;
        pos[idx*3] = q[c];
        pos[idx*3+1] = q[c+1];
        pos[idx*3+2] = 0;

        unsigned char *b = (unsigned char *)&q[c+2]; // NOT ENDIAN SAFE
        color[idx*4  ] = b[0]/255.0f;
        color[idx*4+1] = b[1]/255.0f;
        color[idx*4+2] = b[2]/255.0f;
        color[idx*4+3] = b[3]/255.0f;
    }
    FlushMesh(x);
    #endif
    #endif
    printFunction("Loaded");
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
    UnityPluginUpdate()
{
    if (died)
        return;

	lovrStep(L);
}
