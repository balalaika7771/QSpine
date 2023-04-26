#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QSpine.h"
//#define QT_OPENGL_SUPPORT
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
class QSpine : public QMainWindow
{
    Q_OBJECT

public:
    QSpine(QWidget *parent = nullptr);
    ~QSpine();

private:
    Ui::QSpineClass ui;
};
