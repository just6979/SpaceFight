#include <logog.hpp>

#include <LuaDataReader.h>

LuaDataReader::LuaDataReader(lua_State* _lua) {
    L = _lua;
}

LuaDataReader::~LuaDataReader() {
}

int LuaDataReader::openFile(const char* _fileName) {
    int result;
    fileName = _fileName;
    INFO("Opening '%s' as a Lua script", fileName);
    result = luaL_loadfile(L, fileName);
    if (result == LUA_OK) {
        isOpen = true;
    }
    return OK;
}

int LuaDataReader::read() {
    int result;

    if (!isOpen) {
        return FILE_UNAVAILABLE;
    }

    result = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (result != LUA_OK) {
        if (result == LUA_ERRSYNTAX) {
            return SYNTAX_ERROR;
        } else {
            return OTHER_LUA_ERROR;
        }
    }

    const char* type = readString("type");
    INFO("\ttype = %s", type);

    const char* name = readString("name");
    INFO("\tname = %s", name);

    lua_Number size_int = readNumber("size");
    INFO("size = %f", size_int);

    INFO("Vertexes");
    lua_getglobal(L, "verts");
    lua_pushnil(L);
    int i = 1;
    while (lua_next(L, -2)) {
        lua_pushnil(L);
        lua_next(L, -2);
        lua_Number x = lua_tonumber(L, -1);
        lua_pop(L, 1);
        lua_next(L, -2);
        lua_Number y = lua_tonumber(L, -1);
        lua_pop(L, 1);
        lua_next(L, -2);
        INFO("(%f, %f)", x, y);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    INFO("Colors");
    lua_getglobal(L, "colors");
    lua_pushnil(L);
    i = 1;
    while (lua_next(L, -2)) {
        lua_Number rgb[3];
        int j = 0;
        lua_pushnil(L);
        while (lua_next(L, -2)) {
            rgb[j++] = lua_tonumber(L, -1);
            lua_pop(L, 1);
        }
        INFO("(%f, %f, %f)", rgb[0], rgb[1], rgb[2]);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    return OK;
}

char* LuaDataReader::readString(const char* field) {
    lua_getglobal(L, field);
    const char* value = lua_tostring(L, -1);
    lua_pop(L, -1);
    char* result = new char[255];
    strncpy(result, value, 254);
    return result;
}

lua_Number LuaDataReader::readNumber(const char* field) {
    lua_getglobal(L, field);
    lua_Number value = lua_tonumber(L, -1);
    lua_pop(L, -1);
    return value;
}
