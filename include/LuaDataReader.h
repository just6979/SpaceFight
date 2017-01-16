#pragma once

#include <string>

#ifdef __cplusplus
# include <lua.hpp>
#else
# include <lua.h>
# include <lualib.h>
# include <lauxlib.h>
#endif

class LuaDataReader {
public:
    enum ERROR {
        OK,
        FILE_UNAVAILABLE,
        SYNTAX_ERROR,
        MISSING_VALUE,
        OTHER_LUA_ERROR
    };
    LuaDataReader(lua_State* _lua);
    ~LuaDataReader();
    int openFile(const char* _fileName);
    int read();
private:
    lua_State* L;
    const char* fileName;
    bool isOpen;
    char* readString(const char* field);
    lua_Number readNumber(const char* field);
};
