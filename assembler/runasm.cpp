#include <chrono>
#include <thread>

#include "cxxopts.hpp"
#include "lua.hpp"

#define ASM_SCRIPT "asm6502.lua"

void AssembleThread(const std::string& filename)
{
    int status;
    lua_State* L;

    L = luaL_newstate();
    if (!L) {
        printf("Failed to initialize lua\n");
        return;
    }

    luaJIT_setmode(L, 0, LUAJIT_MODE_ENGINE | LUAJIT_MODE_ON);

    luaL_openlibs(L);
    status = luaL_loadfile(L, ASM_SCRIPT);

    lua_pushstring(L, filename.c_str());

    int ret = lua_pcall(L, 1, 0, 0);
    if (ret != 0) {
        fprintf(stderr, "%s\n", lua_tostring(L, -1));
        return;
    }

    lua_close(L);
}

using namespace std::chrono;

int main(int argc, char** argv)
try {
    cxxopts::Options options("Assembler", "Assemble one or more .lst files");

    options.add_options()("f,positional", "File",
                          cxxopts::value<std::vector<std::string>>())(
        "o,out", "Out Directory", cxxopts::value<std::string>())(
        "d,debug", "Enable debugging", cxxopts::value<bool>())("h,help",
                                                               "Print usage");

    options.parse_positional({"positional"});
    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    if (result.count("out")) {
        auto out = result["out"].as<std::string>();
        std::cout << "TODO: Move the files" << std::endl;
    }

    auto input = result["positional"].as<std::vector<std::string>>();

    std::vector<std::thread> workers;

    for (auto const& f : input) {
        std::thread t(AssembleThread, std::ref(f));
        workers.push_back(std::move(t));
    }

    auto start = high_resolution_clock::now();

    for (auto& w : workers)
        w.join();

    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);

    std::cout << "Assembled " << workers.size() << " programs in "
              << (double)duration.count() / (double)1000000 << "s" << std::endl;

    return 0;
}
catch (std::exception& e) {
    std::cout << e.what() << std::endl;
}