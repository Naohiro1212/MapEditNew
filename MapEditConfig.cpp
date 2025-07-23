#include "MapEditConfig.h"
#include <windows.h>
#include <cstdlib>   // std::atoi
#include <string>    // std::string
#include <fstream>   // std::ifstream

// 内部ユーティリティ
static int ReadIntFromIni(const std::string& section, const std::string& key, int defaultValue, const std::string& iniPath)
{
    int val = GetPrivateProfileIntA(section.c_str(), key.c_str(), defaultValue, iniPath.c_str());
    return val;
}

MapEditConfig LoadMapEditConfig(const std::string& iniPath)
{
    MapEditConfig efg;
    efg.MAP_WIDTH = ReadIntFromIni("MapEdit", "MAP_WIDTH", 0, iniPath);
    efg.MAP_HEIGHT = ReadIntFromIni("MapEdit", "MAP_HEIGHT", 0, iniPath);
    efg.MAP_IMAGE_SIZE = ReadIntFromIni("MapEdit", "MAP_IMAGE_SIZE", 0, iniPath);
    efg.LEFT_MARGIN = ReadIntFromIni("MapEdit", "LEFT_MARGIN", 0, iniPath);
    efg.TOP_MARGIN = ReadIntFromIni("MapEdit", "TOP_MARGIN", 0, iniPath);
    return efg;
}

const MapEditConfig& GetMapEditConfig()
{
    static MapEditConfig config = LoadMapEditConfig(".\\setting.ini");
    return config;
}
