#pragma once
#include <llvmes/graphics/log.h>

#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace llvmes::dbg {

class RecentlyOpened {
   public:
    static void Write(const std::string& str)
    {
        fs::path cache_path(fs::current_path() / ".debugger");

        // Checks if the cache exists on disk
        // If so, create a new one and add the line
        if (!fs::exists(cache_path)) {
            std::ofstream out(cache_path);
            out << str + "\n";
            LLVMES_INFO("Creating new cache");
            return;
        }

        // Checks if the line exists in cache
        // if it does, just return
        std::ifstream cache(cache_path);
        std::string line;
        while (getline(cache, line)) {
            if (line == str)
                return;
        }

        // Otherwise add the new entry to cache
        LLVMES_INFO("New entry added to cache");

        std::ofstream out(cache_path);
        out << str + "\n";
    }

    static std::vector<std::string> GetCache()
    {
        fs::path cache_path(fs::current_path() / ".debugger");
        if (!fs::exists(cache_path))
            return {};

        std::vector<std::string> lines;
        std::ifstream cache(cache_path);
        std::string line;
        while (getline(cache, line)) lines.push_back(line);
        return std::move(lines);
    }
};

}