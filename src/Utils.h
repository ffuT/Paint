#pragma once

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <lua.hpp>
#include <vector>

struct vec2f{
    float x;
    float y;
};

inline void luaerror(lua_State* L, const char* fmt, ...){
    va_list argp;
    va_start(argp, fmt);
    vfprintf(stderr, fmt, argp);
    va_end(argp);
}

inline lua_State* loadconfig(const char* path){
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    if(luaL_loadfile(L, path) || lua_pcall(L, 0, 0, 0)){
        luaerror(L, "cant load config file: %s", lua_tostring(L, -1));
        lua_close(L);
        exit(EXIT_FAILURE);
    }

    luaL_dostring(L, "print('config loaded succesfully')");
    return L;
};

inline bool isnil(lua_State* L, const char* var_name){
    lua_getglobal(L, var_name);
    if(lua_isnil(L, -1)){
        luaerror(L, "variable not found: %s\n", var_name);
        return true;
    } 
    return false;
}

inline void loadcolors(lua_State* L, const char* var_name, std::vector<unsigned int>& var){
    if(isnil(L, var_name))
        return;
    lua_getglobal(L, var_name);
    if(!lua_istable(L, -1)){
        luaerror(L, "variable is not a table: %s\n", var_name);
        return;
    }
    lua_pushnil(L);
    while(lua_next(L, -2) != 0) {
        if (!lua_isnumber(L, -1)){
            luaerror(L, "variable is not a number %s\n", lua_tostring(L, -2));
            continue;
        } 
        var.push_back((unsigned int) lua_tointeger(L, -1));       
        lua_pop(L, 1);
    }
}

// modifies var to the loaded lua value if it is an int, else give error
inline void loadint(lua_State* L, const char* var_name, int& var){
    if(isnil(L, var_name))
        return;
    if(!lua_isinteger(L, -1))
        luaerror(L, "variable is not a number: %s\n", var_name);
    var = lua_tointeger(L, -1);
}

// modifies var to the loaded lua value if it is a float, else give error
inline void loadfloat(lua_State* L, const char* var_name, float& var){
    if(isnil(L, var_name))
        return;
    if(!lua_isnumber(L, -1))
        luaerror(L, "variable is not a number: %s\n", var_name);
    var = (float) lua_tonumber(L, -1);
}

// modifies var to the loaded lua value if it is a bool, else give error
inline void loadbool(lua_State* L, const char* var_name, bool& var){
    if(isnil(L, var_name))
        return;
    if(!lua_isboolean(L, -1))
        luaerror(L, "variable is not a number: %s\n", var_name);
    var = lua_toboolean(L, -1);
}