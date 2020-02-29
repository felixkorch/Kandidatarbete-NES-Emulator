#include "llvmes/graphics/log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

namespace llvmes {

std::shared_ptr<spdlog::logger> Log::s_logger;

void Log::Init()
{
    LLVMES_ASSERT(s_logger == nullptr,
                  "Trying to initialize the global logger a second time");

    spdlog::set_pattern("%^[%T] %n: %v%$");
    s_logger = spdlog::stdout_color_mt("LLVMES");
    s_logger->set_level(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger> Log::GetLogger()
{
    LLVMES_ASSERT(s_logger != nullptr,
                  "Trying to get logger without initializing it, initialize by "
                  "calling Log::Init() at the start of the program");
    return s_logger;
}

}  // namespace llvmes