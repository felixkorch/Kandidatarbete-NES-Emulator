#pragma once
#include <iostream>

#include <spdlog/spdlog.h>

namespace llvmes {

// Singleton class that contains the global instance of the logger
// Gets created on call to "Init" so make sure it's called somewhere in the
// start of the program

// TODO: Add client logger
class Log {
   public:
    static std::shared_ptr<spdlog::logger>& GetLogger();
    static void Init();

   private:
    static std::shared_ptr<spdlog::logger> s_logger;
};

}  // namespace llvmes

#define LLVMES_WARN(...) ::llvmes::Log::GetLogger()->warn(__VA_ARGS__)
#define LLVMES_ERROR(...) ::llvmes::Log::GetLogger()->error(__VA_ARGS__)
#define LLVMES_TRACE(...) ::llvmes::Log::GetLogger()->trace(__VA_ARGS__)
#define LLVMES_INFO(...) ::llvmes::Log::GetLogger()->info(__VA_ARGS__)
#define LLVMES_FATAL(...) ::llvmes::Log::GetLogger()->fatal(__VA_ARGS__)

// Assert if debug mode otherwise don't.
#ifndef NDEBUG
#define LLVMES_ASSERT(x, ...)                                   \
    {                                                       \
        if (!(x)) {                                         \
            LLVMES_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
            __debugbreak();                                 \
        }                                                   \
    }
#else
#define LLVMES_ASSERT(x, ...)
#endif