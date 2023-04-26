#include "QSpine.h"
#include <QtWidgets/QApplication>
#include <QVBoxLayout>
#include "glwidget.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    GLWidget widget;
    widget.resize(640, 480);
    widget.show();

    return app.exec();
}
