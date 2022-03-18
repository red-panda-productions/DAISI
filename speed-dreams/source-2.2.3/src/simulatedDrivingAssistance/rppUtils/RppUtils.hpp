#pragma once
#include <string>
#include <fstream>

inline float stringToFloat(std::string s)
{
    try { return std::stof(s); }
    catch (std::exception& e) { return NAN; }
}

inline bool FindFileDirectory(std::string& p_path, const std::string& fileToFind)
{
    std::string cd = "";

    for (int i = 0; i < 50; i++)
    {
        std::ifstream file(cd + fileToFind);
        if (file.good())
        {
            file.close();
            p_path = cd + p_path;
            return true;
        }

        cd += "../";
    }

    return false;
}