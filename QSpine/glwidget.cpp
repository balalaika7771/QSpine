// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "glwidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>

bool GLWidget::m_transparent = false;

GLWidget::GLWidget( QWidget* parent )
	: QOpenGLWidget(parent)
{
	m_core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
	// --transparent causes the clear color to be transparent. Therefore, on systems that
	// support it, the widget will become transparent apart from the logo.
	if (m_transparent) {
		QSurfaceFormat fmt = format();
		fmt.setAlphaBufferSize(8);
		setFormat(fmt);
	}
	
}

GLWidget::~GLWidget()
{
	cleanup();
}

void GLWidget::setSpine(std::vector<pozvonok> _spine)
{
	spine = _spine;
}

QSize GLWidget::minimumSizeHint() const
{
	return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
	return QSize(800, 800);
}

pozvonok GLWidget::getCurSpine()
{

	return spine[num_pozvonok];
}

static void qNormalizeAngle(int& angle)
{
	while (angle < 0)
		angle += 360 * 16;
	while (angle > 360 * 16)
		angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != m_xRot) {
		m_xRot = angle;
		emit xRotationChanged(angle);
		update();
	}
}

void GLWidget::setYRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != m_yRot) {
		m_yRot = angle;
		emit yRotationChanged(angle);
		update();
	}
}

void GLWidget::setZRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != m_zRot) {
		m_zRot = angle;
		emit zRotationChanged(angle);
		update();
	}
}


void GLWidget::cleanup()
{
	if (m_program == nullptr)
		return;
	makeCurrent();
	m_spineVbo.destroy();
	delete m_program;
	m_program = nullptr;
	doneCurrent();
	QObject::disconnect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);
}

//Vertex shader source code for OpenGL Core Profile
static const char* vertexShaderSourceCore =
"#version 150\n" //Specify the version of GLSL used
"in vec4 vertex;\n" //Input vertex data from the vertex buffer object
"in vec3 normal;\n" //Input normal data from the vertex buffer object
"out vec3 vert;\n" //Output vertex data to the fragment shader
"out vec3 vertNormal;\n" //Output normal data to the fragment shader
"uniform mat4 projMatrix;\n" //Projection matrix uniform
"uniform mat4 mvMatrix;\n" //Model-view matrix uniform
"uniform mat3 normalMatrix;\n" //Normal matrix uniform
"void main() {\n" //Main function of the vertex shader
"   vert = vertex.xyz;\n" //Set the output vertex data to the input vertex data
"   vertNormal = normalMatrix * normal;\n" //Transform the normal data by the normal matrix
"   gl_Position = projMatrix * mvMatrix * vertex;\n" //Set the gl_Position output to the transformed vertex data
"}\n";

static const char* fragmentShaderSourceCore =
"#version 150\n"
"in highp vec3 vert;\n"
"in highp vec3 vertNormal;\n"
"out highp vec4 fragColor;\n"
"uniform highp vec3 lightPos;\n"
"void main() {\n"
"   highp vec3 L = normalize(lightPos - vert);\n"
"   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
"   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
"   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
"   fragColor = vec4(col, 1.0);\n"
"}\n";

static const char* vertexShaderSource =
"attribute vec4 vertex;\n"
"attribute vec3 normal;\n"
"varying vec3 vert;\n"
"varying vec3 vertNormal;\n"
"uniform mat4 projMatrix;\n"
"uniform mat4 mvMatrix;\n"
"uniform mat3 normalMatrix;\n"
"void main() {\n"
"   vert = vertex.xyz;\n"
"   vertNormal = normalMatrix * normal;\n"
"   gl_Position = projMatrix * mvMatrix * vertex;\n"
"}\n";

static const char* fragmentShaderSource =
"varying highp vec3 vert;\n"
"varying highp vec3 vertNormal;\n"
"uniform highp vec3 lightPos;\n"
"void main() {\n"
"   highp vec3 L = normalize(lightPos - vert);\n"
"   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
"   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
"   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
"   gl_FragColor = vec4(col, 1.0);\n"
"}\n";

//This code is part of the initializeGL() function in the GLWidget class. This function is responsible for initializing the OpenGL functions and setting up the shader
// program. It also loads the spine data from a text file and creates a vertex buffer object for the spine. Finally, it sets up the camera and light

void GLWidget::initializeGL()
{

	connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

	initializeOpenGLFunctions();
	glClearColor(0, 0, 0, m_transparent ? 0 : 1);

	m_program = new QOpenGLShaderProgram;
	m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, m_core ? vertexShaderSourceCore : vertexShaderSource);
	m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, m_core ? fragmentShaderSourceCore : fragmentShaderSource);
	m_program->bindAttributeLocation("vertex", 0);
	m_program->bindAttributeLocation("normal", 1);
	m_program->link();

	m_program->bind();
	m_projMatrixLoc = m_program->uniformLocation("projMatrix");
	m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
	m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
	m_lightPosLoc = m_program->uniformLocation("lightPos");

	m_vao.create();
	QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

	
	for (size_t i = 0; i < spine.size(); i++)
	{
		QString str = QString::fromUtf8(spine[i].spine_name + ".obj");
		m_spine.loadObj(str, spine[i].position, spine[i].rotation);
	}

	// Setup our vertex buffer object.
	m_spineVbo.create();
	m_spineVbo.bind();
	m_spineVbo.allocate(m_spine.constData(), m_spine.count() * sizeof(GLfloat));
	// Store the vertex attribute bindings for the program.
	setupVertexAttribs();

	m_camera.setToIdentity();
	m_camera.translate(0, 0, -50);

	// Light position is fixed.
	m_program->setUniformValue(m_lightPosLoc, QVector3D(-100, -50, 40));

	m_program->release();
}
// Rewritten code with comments

void GLWidget::setupVertexAttribs()
{
	// Bind the spineVbo
	m_spineVbo.bind();

	// Get the current context's functions
	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();

	// Enable vertex attribute 0 and 1
	f->glEnableVertexAttribArray(0);
	f->glEnableVertexAttribArray(1);

	// Set the vertex attribute 0 to 3 floats, not normalized, and 6 bytes apart
	f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
		nullptr);

	// Set the vertex attribute 1 to 3 floats, not normalized, and 6 bytes apart, starting 3 bytes in
	f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
		reinterpret_cast<void*>(3 * sizeof(GLfloat)));

	// Release the spineVbo
	m_spineVbo.release();
}
// Rewritten code with comments

void GLWidget::paintGL()
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable depth testing and face culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Set the world matrix to the identity matrix
	m_world.setToIdentity();

	// Rotate the world matrix based on the x, y, and z rotation values
	m_world.rotate(180.0f - (m_xRot / 16.0f), 1, 0, 0);
	m_world.rotate(m_yRot / 16.0f, 0, 1, 0);
	m_world.rotate(m_zRot / 16.0f, 0, 0, 1);

	// Translate the world matrix based on the x, y, and z translation values
	m_world.translate(m_xTrans, m_yTrans, m_zTrans);

	// Bind the vertex array object
	QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

	// Bind the shader program
	m_program->bind();
	m_camera.setToIdentity();
	m_camera.translate(0, 0, zoom);
	// Set the projection and model-view matrices
	m_program->setUniformValue(m_projMatrixLoc, m_proj);
	m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);

	// Calculate the normal matrix
	QMatrix3x3 normalMatrix = m_world.normalMatrix();
	m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);

	// Draw the triangles
	glDrawArrays(GL_TRIANGLES, 0, m_spine.vertexCount());

	// Release the shader program
	m_program->release();
}

void GLWidget::resizeGL(int w, int h)
{
	m_proj.setToIdentity();
	m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 500.0f);
}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
	m_lastPos = event->position().toPoint();
}

void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
	int dx = event->position().toPoint().x() - m_lastPos.x();
	int dy = event->position().toPoint().y() - m_lastPos.y();

	if (event->buttons() & Qt::LeftButton) {
		setXRotation(m_xRot + 8 * dy);
		setYRotation(m_yRot + 8 * dx);
	}
	else if (event->buttons() & Qt::RightButton) {
		setXRotation(m_xRot + 8 * dy);
		setZRotation(m_zRot + 8 * dx);
	}
	m_lastPos = event->position().toPoint();
}

//This code handles a wheel event, checking if the left or right mouse button is pressed. If so, it calculates the number of steps the wheel has moved and adjusts the num_pozvonok variable accordingly. It then sets the m_xTrans, m_yTrans, and m_zTrans variables to the corresponding values in the sp array and emits a zRotationChanged signal. 
void GLWidget::wheelEvent(QWheelEvent* event) {
	if (!(event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton)) { //Check if left or right mouse button is pressed
		return;
	}


	int numSteps = event->angleDelta().toPointF().y() / 15; //Calculate number of steps wheel has moved
	auto h = event->modifiers();
	if (h & Qt::ShiftModifier) {
		zoom -= numSteps / 3;
		if (zoom > 0) {
			zoom = 0;
		}
		else if (zoom <= -500) {
			zoom = -500;
		}

		return;
	}
	num_pozvonok -= numSteps / 8; //Adjust num_pozvonok variable
	if (num_pozvonok < 0) {
		num_pozvonok = 0;
	}
	else if (num_pozvonok >= spine.size()) {
		num_pozvonok = spine.size() - 1;
	}

	m_xTrans = -spine[num_pozvonok].position.x(); //Set m_xTrans, m_yTrans, and m_zTrans to corresponding values in sp array
	m_yTrans = -spine[num_pozvonok].position.y();
	m_zTrans = -spine[num_pozvonok].position.z();


}
