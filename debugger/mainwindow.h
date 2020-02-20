#pragma once
#include <QMainWindow>
#include <QFutureWatcher>

#include <llvmes/NES/CPU.h>

#include <sstream>
#include <iomanip>

#include "debugger.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void Reset();
    void Step();
    void Run();
    void RunBP();
    void RunFinished();
    void Browse();

private:
    Ui::MainWindow* m_ui;
    std::shared_ptr<Debugger> m_debugger;
    bool m_good_state;

private:
    void DisplayRegisters();

    template<typename T>
    std::string ToHexString(T i)
    {
        std::stringstream stream;
        stream << "$"
               << std::uppercase
               << std::setfill ('0')
               << std::setw(sizeof(T)*2)
               << std::hex << (unsigned)i;
        return stream.str();
    }

    static constexpr const char* TITLE = "LLVMES - Debugger";
};
