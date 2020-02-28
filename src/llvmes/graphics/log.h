#pragma once
#include <iostream>

#include <spdlog/spdlog.h>

namespace llvmes {

// Singleton class that contains the global instance of the logger
// Gets created on call to "Init" so make sure it's called somewhere in the
// start of the program
class Log {
   public:
    static std::shared_ptr<spdlog::logger> GetLogger();
    static void Init();

   private:
    static std::shared_ptr<spdlog::logger> s_logger;
};

}  // namespace llvmes

#define LLVMES_WARN(...) Log::GetLogger()->warn(__VA_ARGS__)
#define LLVMES_ERROR(...) Log::GetLogger()->error(__VA_ARGS__)
#define LLVMES_TRACE(...) Log::GetLogger()->trace(__VA_ARGS__)
#define LLVMES_INFO(...) Log::GetLogger()->info(__VA_ARGS__)
#define LLVMES_FATAL(...) Log::GetLogger()->fatal(__VA_ARGS__)

// This is a macro that will do an assert and will work in both
// debug and release builds. In release mode it will print an error
// and terminate the program instead of a normal assert.
#ifndef NDEBUG
#define LLVMES_ASSERT(condition, message)                                      \
    do {                                                                       \
        if (!(condition)) {                                                    \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__   \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate();                                                  \
        }                                                                      \
    } while (false)
#else
#define LLVMES_ASSERT(condition, message)
#endif