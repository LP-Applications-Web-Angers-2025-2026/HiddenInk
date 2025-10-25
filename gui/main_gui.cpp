#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    app.setApplicationName("HiddenInk");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("HiddenInk Project");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
