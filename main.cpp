#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    try {
            QApplication a(argc, argv);
            FloatingBall w;
            w.show();
            QObject::connect(&a, &QApplication::lastWindowClosed, [&w]() {
                qDebug() << "Last window closed, showing FloatingBall";
                w.show();
            });
            return a.exec();
        } catch (const std::exception& e) {
            qDebug() << "Unhandled exception in main:" << e.what();
        } catch (...) {
            qDebug() << "Unknown unhandled exception in main";
        }
        return 1;
}
