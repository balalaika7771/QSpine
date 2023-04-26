// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "Spine.h"
#include <qmath.h>

Spine::Spine()
{
   
}

void Spine::add(const QVector3D &v, const QVector3D &n)
{
    m_data.resize(m_count + 6);
    GLfloat *p = m_data.data() + m_count;
    *p++ = v.x();
    *p++ = v.y();
    *p++ = v.z();
    *p++ = n.x();
    *p++ = n.y();
    *p++ = n.z();
    m_count += 6;
}

void Spine::loadObj(const QString& filename) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename.toStdString(),
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    if (!scene) {
        //qWarning() << "Failed to load OBJ file:" << importer.GetErrorString();
        return;
    }

    for (unsigned int meshIdx = 0; meshIdx < scene->mNumMeshes; meshIdx++) {
        const aiMesh* mesh = scene->mMeshes[meshIdx];
        for (unsigned int faceIdx = 0; faceIdx < mesh->mNumFaces; faceIdx++) {
            const aiFace& face = mesh->mFaces[faceIdx];
            for (unsigned int vertexIdx = 0; vertexIdx < face.mNumIndices; vertexIdx++) {
                const aiVector3D& vertex = mesh->mVertices[face.mIndices[vertexIdx]];
                const aiVector3D& normal = mesh->mNormals[face.mIndices[vertexIdx]];
                add(QVector3D(vertex.x, vertex.y, vertex.z), QVector3D(normal.x, normal.y, normal.z));
            }
        }
    }
}
void Spine::loadObj(const QString filename, const QVector3D& position, const QVector3D& rotation) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename.toStdString(),
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    if (!scene) {
       // qWarning() << "Failed to load OBJ file:" << importer.GetErrorString();
        return;
    }

    for (unsigned int meshIdx = 0; meshIdx < scene->mNumMeshes; meshIdx++) {
        const aiMesh* mesh = scene->mMeshes[meshIdx];
        for (unsigned int faceIdx = 0; faceIdx < mesh->mNumFaces; faceIdx++) {
            const aiFace& face = mesh->mFaces[faceIdx];
            for (unsigned int vertexIdx = 0; vertexIdx < face.mNumIndices; vertexIdx++) {
                const aiVector3D& vertex = mesh->mVertices[face.mIndices[vertexIdx]];
                const aiVector3D& normal = mesh->mNormals[face.mIndices[vertexIdx]];
                QVector3D v(vertex.x, vertex.y, vertex.z);
                QVector3D n(normal.x, normal.y, normal.z);

                // ѕримен€ем смещение и поворот к вершине и нормали
                v = QQuaternion::fromEulerAngles(rotation) * v + position;
                n = QQuaternion::fromEulerAngles(rotation) * n;

                add(v, n);
            }
        }
    }
}

void Spine::quad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4)
{
    QVector3D n = QVector3D::normal(QVector3D(x4 - x1, y4 - y1, 0.0f), QVector3D(x2 - x1, y2 - y1, 0.0f));

    add(QVector3D(x1, y1, -0.05f), n);
    add(QVector3D(x4, y4, -0.05f), n);
    add(QVector3D(x2, y2, -0.05f), n);

    add(QVector3D(x3, y3, -0.05f), n);
    add(QVector3D(x2, y2, -0.05f), n);
    add(QVector3D(x4, y4, -0.05f), n);

    n = QVector3D::normal(QVector3D(x1 - x4, y1 - y4, 0.0f), QVector3D(x2 - x4, y2 - y4, 0.0f));

    add(QVector3D(x4, y4, 0.05f), n);
    add(QVector3D(x1, y1, 0.05f), n);
    add(QVector3D(x2, y2, 0.05f), n);

    add(QVector3D(x2, y2, 0.05f), n);
    add(QVector3D(x3, y3, 0.05f), n);
    add(QVector3D(x4, y4, 0.05f), n);
}

void Spine::extrude(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    QVector3D n = QVector3D::normal(QVector3D(0.0f, 0.0f, -0.1f), QVector3D(x2 - x1, y2 - y1, 0.0f));

    add(QVector3D(x1, y1, +0.05f), n);
    add(QVector3D(x1, y1, -0.05f), n);
    add(QVector3D(x2, y2, +0.05f), n);

    /*add(QVector3D(x2, y2, -0.05f), n);
    add(QVector3D(x2, y2, +0.05f), n);
    add(QVector3D(x1, y1, -0.05f), n);*/
}
