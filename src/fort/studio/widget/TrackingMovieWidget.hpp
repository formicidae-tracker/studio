#pragma once

#include <QOpenGLWidget>

class TrackingMovieWidget : public QOpenGLWidget {
	Q_OBJECT
public:
	explicit TrackingMovieWidget(QWidget * parent = nullptr);
	~TrackingMovieWidget();

protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int w, int h) override;

private :
	void loadShaders();

	GLuint d_shadersID;
	GLuint d_vaoID,d_vboID;

};
