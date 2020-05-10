#include "debugger.h"

int main()
try {
    Debugger ui;
    ui.Run();
}
catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
}
