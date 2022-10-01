#include "lovr.h"
#include "lib/lua-cjson/lua_cjson.h"
#include "filesystem/filesystem.h"
#include "resources/boot.lua.h"

typedef void  ( __stdcall *FEEDSTRING )( const char * );
extern FEEDSTRING printFunction;
extern void printError(const char *);

#define LOVR_VERSION_MAJOR 0
#define LOVR_VERSION_MINOR 9
#define LOVR_VERSION_PATCH 1
#define LOVR_VERSION_ALIAS "Unity Test"

// api.h

int l_lovrFilesystemInit(lua_State* L);

// lovr.c

static int stepRef = 0;

static int lovrGetOS(lua_State* L) {
  lua_pushstring(L, "Unity");
  return 1;
}

static int lovrGetVersion(lua_State* L) {
  lua_pushnumber(L, LOVR_VERSION_MAJOR);
  lua_pushnumber(L, LOVR_VERSION_MINOR);
  lua_pushnumber(L, LOVR_VERSION_PATCH);
  return 3;
}

static int lovrSetConf(lua_State* L) {
  luax_setconf(L);
  return 0;
}

static int getStackTrace(lua_State* L) {
  const char* message = luaL_checkstring(L, -1);
  lua_getglobal(L, "debug");
  lua_getfield(L, -1, "traceback");
  lua_pushstring(L, message);
  lua_pushinteger(L, 2);
  lua_call(L, 2, 1);
  return 1;
}

static bool died = true;

static void lovrDestroy(int exitCode) {
  died = true;
}

static void handleError(lua_State* L, const char* message) {
  lua_getglobal(L, "lovr");
  lua_getfield(L, -1, "errhand");
  if (lua_isfunction(L, -1)) {
    lua_pushstring(L, message);
    lua_pcall(L, 1, 1, 0);
  } else {
    fprintf(stderr, "%s\n", message);
  }
  if (!(lua_type(L, -1) == LUA_TSTRING && 0 == strcmp("restart", lua_tostring(L, -1))))
    lovrDestroy(1);
}

void lovrInit(lua_State* L) {
  died = false;

#if 0
  glfwSetTime(0);
  glfwSetErrorCallback(onGlfwError);

  if (!glfwAlreadyInit && !glfwInit()) {
    lovrThrow("Error initializing glfw");
  }
  glfwAlreadyInit = true;
#endif
  // arg global is set before this is called

  // lovrInit
//  lua_newtable(L);
  //if (argc > 0) {
  //  lua_pushstring(L, argv[0]);
  //  lua_rawseti(L, -2, -2);
  //}

 // lua_pushstring(L, "lovr");
  //lua_rawseti(L, -2, -1);
  //for (int i = 1; i < argc; i++) {
  //  lua_pushstring(L, argv[i]);
  //  lua_rawseti(L, -2, i);
  //}

  //lua_setglobal(L, "arg");

  // lovr
  lua_newtable(L);
  lua_pushcfunction(L, lovrSetConf);
  lua_setfield(L, -2, "_setConf");
  lua_pushcfunction(L, lovrGetOS);
  lua_setfield(L, -2, "getOS");
  lua_pushcfunction(L, lovrGetVersion);
  lua_setfield(L, -2, "getVersion");
  lua_pushnil(L);
  //lua_pushlstring(L, (const char*) logo_png, logo_png_len);
  lua_setfield(L, -2, "_logo");
  lua_setglobal(L, "lovr");

  // Preload modules
  //luax_preloadmodule(L, "lovr.audio", l_lovrAudioInit);
  //luax_preloadmodule(L, "lovr.data", l_lovrDataInit);
  //luax_preloadmodule(L, "lovr.event", l_lovrEventInit);
  luax_preloadmodule(L, "lovr.filesystem", l_lovrFilesystemInit);
  //luax_preloadmodule(L, "lovr.graphics", l_lovrGraphicsInit);
  //luax_preloadmodule(L, "lovr.headset", l_lovrHeadsetInit);
  //luax_preloadmodule(L, "lovr.math", l_lovrMathInit);
  //luax_preloadmodule(L, "lovr.physics", l_lovrPhysicsInit);
  //luax_preloadmodule(L, "lovr.timer", l_lovrTimerInit);
  //luax_preloadmodule(L, "game", l_gameInit);

  // Preload libraries
  luax_preloadmodule(L, "json", luaopen_cjson);
//  luax_preloadmodule(L, "enet", luaopen_enet);

  lua_pushcfunction(L, getStackTrace);
  if (luaL_loadbuffer(L, (const char*) boot_lua, boot_lua_len, "boot.lua") || lua_pcall(L, 0, 0, -2)) {
    handleError(L, lua_tostring(L, -1));
  }

  // Emscripten lovrRun
  // lovr.load
  lua_getglobal(L, "lovr");
  if (!lua_isnil(L, -1)) {
    lua_getfield(L, -1, "load");
    if (!lua_isnil(L, -1)) {
      if (0 != lua_pcall(L, 0, 0, 0)) // MODIFIED
        printError("INIT FAIL");
        died = true;
        return; // Don't save step
    } else {
      lua_pop(L, 1);
    }

    lua_getfield(L, -1, "step");
    stepRef = luaL_ref(L, LUA_REGISTRYINDEX);
  }
}

// emscriptenLoop()
void lovrStep(lua_State* L) {
  if (died) return;
  lua_rawgeti(L, LUA_REGISTRYINDEX, stepRef);
  if (0 != lua_pcall(L, 0, 0, 0)) { // MODIFIED
    printError("STEP FAIL");
  }
}
