#pragma once

#include <QOpenGLWidget>
#include <QOpenGLTextureBlitter>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <memory>

class TrackingVideoFrame;
class QOpenGLFramebufferObject;

class TrackingVideoWidget : public QOpenGLWidget {
	Q_OBJECT
public:
	explicit TrackingVideoWidget(QWidget * parent = nullptr);
	~TrackingVideoWidget();

public slots:
	void display(TrackingVideoFrame * frame);

protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int w, int h) override;

private :
	std::shared_ptr<QOpenGLFramebufferObject> d_fbo;
	QOpenGLTextureBlitter                     d_blitter;
	QOpenGLTexture                            d_texture;
	quint32                                  d_triangle;
};
