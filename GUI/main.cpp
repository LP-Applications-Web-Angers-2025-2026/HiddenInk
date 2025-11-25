#include <QApplication>
#include "mainwindow.h"
#include <locale>
#include <clocale>

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef _WIN32
    // Ensure the Windows console (if shown) uses UTF-8 so accented characters print correctly
    SetConsoleOutputCP(CP_UTF8);
    // Also set C locale to the user's default to help Qt/local functions
    std::setlocale(LC_ALL, "");
#endif

    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
