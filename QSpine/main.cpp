#include "QSpine.h"
#include <QtWidgets>
#include <QtWidgets/QApplication>
#include <QVBoxLayout>
#include "glwidget.h"
#include <QTimer>
#include <QSharedPointer>
void Timer(QSharedPointer<GLWidget> glWidget, QSharedPointer<QLabel> text);



int main(int argc, char *argv[])
{
    
    QApplication app(argc, argv);
    QMainWindow mainWindow;
    QSharedPointer<GLWidget> glWidget = QSharedPointer<GLWidget>(new GLWidget(&mainWindow));
    QSharedPointer<QLabel> text = QSharedPointer<QLabel>(new QLabel(&mainWindow));
    QSharedPointer<QToolBar> toolBar = QSharedPointer<QToolBar>(new QToolBar(&mainWindow));
    QSharedPointer<QLabel> text3 = QSharedPointer<QLabel>(new QLabel(&mainWindow));

    parserFile pr("SDL_VBL_1.txt");
    glWidget->setSpine(pr.get_spine());
    text->setText("eawde");
    // Добавление действий на панель инструментов
    QAction* action1 = new QAction("Action 1", &mainWindow);
    QAction* action2 = new QAction("Action 2", &mainWindow);
    toolBar->addAction(action1);
    toolBar->addAction(action2);

    // Создание вертикального компоновщика и добавление в него виджетов
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(toolBar.get());
    mainLayout->addWidget(glWidget.get());

    // Создание горизонтального компоновщика для текстового поля и добавление в него текстового поля
    QHBoxLayout* textLayout = new QHBoxLayout();
    textLayout->addWidget(text.get());

    // Создание главного виджета и добавление в него вертикального компоновщика и горизонтального компоновщика для текстового поля
    QWidget* mainWidget = new QWidget(&mainWindow);
    mainWidget->setLayout(mainLayout);
    mainLayout->addLayout(textLayout);
    mainWindow.setCentralWidget(mainWidget);
    mainWindow.show();

    QTimer* timer = new QTimer(&app);
    QObject::connect(timer, &QTimer::timeout, [&glWidget, &text]() { Timer(glWidget, text); });
    timer->start(100);

    return app.exec();
}

void Timer(QSharedPointer<GLWidget> glWidget, QSharedPointer<QLabel> text) {
    pozvonok pz = glWidget->getCurSpine();
    text->setText(QString::fromUtf8(pz.spine_name));
}