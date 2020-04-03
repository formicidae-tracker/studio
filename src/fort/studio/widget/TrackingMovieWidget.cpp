#include "TrackingMovieWidget.hpp"

#include <QOpenGLFunctions>

#include <QFile>
#include <QTextStream>
#include <QDebug>

#include <fort/studio/Format.hpp>

TrackingMovieWidget::TrackingMovieWidget(QWidget * parent)
	: QOpenGLWidget(parent) {
	QSurfaceFormat format;
	format.setVersion(3, 2);
	format.setProfile(QSurfaceFormat::CoreProfile);
	//setFormat(format);
}

TrackingMovieWidget::~TrackingMovieWidget() {
}

QString readAll(const QString & filename) {
	QFile file(filename);
	if (!file.open(QFile::ReadOnly | QFile::Text ) ) {
		qCritical() << "Could not open '" << filename << "'";
		return "";
	}
	QTextStream stream(&file);
	return stream.readAll();
}

QString getShaderInfo(GLuint shaderID,int length) {
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

	std::vector<char> message(length+1);
	f->glGetShaderInfoLog(shaderID, length, NULL, &message[0]);
	return &message[0];
}

QString getProgramInfo(GLuint programID,int length) {
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

	std::vector<char> message(length+1);
	f->glGetProgramInfoLog(programID, length, NULL, &message[0]);
	return &message[0];
}


void TrackingMovieWidget::loadShaders() {
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

	Q_INIT_RESOURCE(resources);

	auto vertexShaderStr = readAll(":movieTracking.vertexshader");
	qDebug() << "Vertex shader:\n" << vertexShaderStr;
	auto fragmentShaderStr = readAll(":movieTracking.fragmentshader");
	qDebug() << "Fragment shader:\n" << fragmentShaderStr;
	char const * vertexSrcPointer = vertexShaderStr.toUtf8().constData();
	char const * fragmentSrcPointer = fragmentShaderStr.toUtf8().constData();

	GLuint vertexShaderID = f->glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = f->glCreateShader(GL_FRAGMENT_SHADER);


	GLint result;
	int infoLogLength;

	f->glShaderSource(vertexShaderID, 1, &vertexSrcPointer , NULL);
	f->glCompileShader(vertexShaderID);

	f->glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	f->glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 0 ){
		qCritical() << "Could not compile vertex shader:\n" << getShaderInfo(vertexShaderID,infoLogLength);
		return;
	}

	f->glShaderSource(fragmentShaderID, 1, &fragmentSrcPointer, NULL);
	f->glCompileShader(fragmentShaderID);

	f->glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
	f->glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 0 ){
		qCritical() << "Could not compile fragment shader:\n" << getShaderInfo(fragmentShaderID,infoLogLength);
		return;
	}

	d_shadersID = f->glCreateProgram();
	f->glAttachShader(d_shadersID, vertexShaderID);
	f->glAttachShader(d_shadersID, fragmentShaderID);
	f->glLinkProgram(d_shadersID);

	f->glGetProgramiv(d_shadersID, GL_LINK_STATUS, &result);
	f->glGetProgramiv(d_shadersID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 0 ){
		auto errors = getProgramInfo(d_shadersID,infoLogLength);
		qCritical() << "Could not link shaders:\n" << errors;
		std::cerr << "Compilation output:" << std::endl
		          << ToStdString(errors) << std::endl;

	}

	f->glDetachShader(d_shadersID, vertexShaderID);
	f->glDetachShader(d_shadersID, fragmentShaderID);

	f->glDeleteShader(vertexShaderID);
	f->glDeleteShader(fragmentShaderID);

}

void TrackingMovieWidget::initializeGL() {
	qDebug() << "Context:" << QOpenGLContext::currentContext()->format();
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

	loadShaders();
	static const GLfloat gVertexBufferData[]
		= {
		   -1.0f, -1.0f, 0.0f,
		   1.0f, -1.0f, 0.0f,
		   0.0f,  1.0f, 0.0f,
	};


	f->glGenBuffers(1, &d_vboID);
	f->glBindBuffer(GL_ARRAY_BUFFER, d_vboID);
	f->glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexBufferData), gVertexBufferData, GL_STATIC_DRAW);

	f->glClearColor(0.0f, 0.0f, 0.1f, 0.0f);

}

void TrackingMovieWidget::paintGL() {

	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

	f->glBindFramebuffer(GL_FRAMEBUFFER, QOpenGLContext::currentContext()->defaultFramebufferObject());

	f->glClear(GL_COLOR_BUFFER_BIT);
	f->glUseProgram(d_shadersID);

	f->glEnableVertexAttribArray(0);
	f->glBindBuffer(GL_ARRAY_BUFFER, d_vboID);
	f->glVertexAttribPointer(0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	                         3,                  // size
	                         GL_FLOAT,           // type
	                         GL_FALSE,           // normalized?
	                         0,                  // stride
	                         (void*)0            // array buffer offset
	                         );
	f->glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
	f->glDisableVertexAttribArray(0);
}

void TrackingMovieWidget::resizeGL(int w, int h) {
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
}
