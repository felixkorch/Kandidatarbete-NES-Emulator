#include "llvmes-gui/log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

namespace llvmes {

std::shared_ptr<spdlog::logger> Log::s_logger;

void Log::Init()
{
    spdlog::set_pattern("%^[%T] %n: %v%$");
    s_logger = spdlog::stdout_color_mt("LLVMES");
    s_logger->set_level(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger>& Log::GetLogger()
{
    assert(s_logger != nullptr &&
           "Trying to use Log-macros without initializing, call Log::Init()"
           "at the start of the program");
    return s_logger;
}

}  // namespace llvmes
