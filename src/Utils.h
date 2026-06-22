#pragma once

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <lua.hpp>

struct vec2f{
    float x;
    float y;
};

inline void luaerror(lua_State* L, const char* fmt, ...){
    va_list argp;
    va_start(argp, fmt);
    vfprintf(stderr, fmt, argp);
    va_end(argp);
    lua_close(L);
    exit(EXIT_FAILURE);
}

inline lua_State* loadconfig(const char* path){
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    if(luaL_loadfile(L, path) || lua_pcall(L, 0, 0, 0))
        luaerror(L, "cant load config file: %s", lua_tostring(L, -1));   

    luaL_dostring(L, "print('config loaded succesfully')");
    return L;
};

// modifies var to the loaded lua value if it is an int, else give error
inline void loadint(lua_State* L, const char* var_name, int& var){
    lua_getglobal(L, var_name);
    if(lua_isnil(L, -1)) 
        luaerror(L, "variable not found: %s", var_name);
    if(!lua_isinteger(L, -1))
        luaerror(L, "variable is not a number : %s", var_name);
    var = lua_tointeger(L, -1);
}

// modifies var to the loaded lua value if it is a bool, else give error
inline void loadbool(lua_State* L, const char* var_name, bool& var){
    lua_getglobal(L, var_name);
    if(lua_isnil(L, -1)) 
        luaerror(L, "variable not found: %s", var_name);
    if(!lua_isboolean(L, -1))
        luaerror(L, "variable is not a number : %s", var_name);
    var = lua_toboolean(L, -1);
}