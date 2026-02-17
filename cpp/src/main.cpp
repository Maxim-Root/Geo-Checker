#include "mainwindow.hpp"
#include <QApplication>
#include <QIcon>
#include <QStyleFactory>
#ifdef _WIN32
#include <windows.h>
#include <cstdlib>
#include <string>
#endif

int main(int argc, char* argv[]) {
#ifdef _WIN32
    // Set QT_PLUGIN_PATH to exe directory so Qt finds plugins
    // even inside Enigma Virtual Box virtual filesystem
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string exeDir(exePath);
    auto pos = exeDir.find_last_of("\\/");
    if (pos != std::string::npos) exeDir.resize(pos);
    _putenv_s("QT_PLUGIN_PATH", exeDir.c_str());
#endif
    QApplication app(argc, argv);
    app.setApplicationName("Geo Checker");
    app.setApplicationVersion("1.0");
#ifdef GEOCHECKER_HAS_APP_ICON
    app.setWindowIcon(QIcon(":/icons/app_icon.ico"));
#endif

    geochecker::MainWindow w;
    w.show();
    return app.exec();
}
