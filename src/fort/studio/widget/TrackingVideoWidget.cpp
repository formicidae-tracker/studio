#include "TrackingVideoWidget.hpp"
#include "TrackingVideoFrame.hpp"
#include <QOpenGLFunctions>

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QSurface>

#include <QOpenGLFramebufferObject>
#include <QOpenGLBuffer>
#include <QRect>

#include <fort/studio/Format.hpp>

TrackingVideoWidget::TrackingVideoWidget(QWidget * parent)
	: QOpenGLWidget(parent)
	, d_texture(QOpenGLTexture::Target2D) {
}

TrackingVideoWidget::~TrackingVideoWidget() {
	makeCurrent();
	d_fbo.reset();
	d_blitter.destroy();
	doneCurrent();
}


void TrackingVideoWidget::initializeGL() {
	d_blitter.create();

	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();


	f->glClearColor(0.4f, 0.0f, 0.0f, 0.0f);

	static const GLfloat g_vertex_buffer_data[] =
		{
		 -1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
		};
	f->glGenBuffers(1, &d_triangle);
	// The following commands will talk about our 'vertexbuffer' buffer
	f->glBindBuffer(GL_ARRAY_BUFFER, d_triangle);
	// Give our vertices to OpenGL.
	f->glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	f->glGenTextures(1,&d_texture);
}

void TrackingVideoWidget::paintGL() {
	std::cerr << "Painting" << std::endl;
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

	f->glClear(GL_COLOR_BUFFER_BIT);

	if ( !d_fbo ) {
		std::cerr << "No FBO" << std::endl;
		return;
	}

	auto size = d_fbo->size();
	size.scale(width(),height(),Qt::KeepAspectRatio);
	QRect targetRect(QPoint(0,0),2*size);
	targetRect.translate(2*rect().center() - targetRect.center());
	qWarning() << context()->surface()->size();

	QOpenGLFramebufferObject::blitFramebuffer(nullptr,
	                                          targetRect,
	                                          d_fbo.get(),
	                                          QRect(QPoint(0,0),d_fbo->size()));

}

void TrackingVideoWidget::resizeGL(int w, int h) {
}


void TrackingVideoWidget::display(TrackingVideoFrame * frame) {
	std::cerr << "Displaying TrackingVideoFrame " << frame << " with buffer " << frame->Buffer << std::endl;
	makeCurrent();
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

	if ( frame == nullptr ) {
		d_fbo.reset();
		doneCurrent();
		update();
		return;
	}

	if ( !d_fbo
	     || d_fbo->width() != frame->Width
	     || d_fbo->height() != frame->Height ) {
		std::cerr << "Creating FBO" <<std::endl;
		d_fbo = std::make_shared<QOpenGLFramebufferObject>(frame->Width,frame->Height);
		std::cerr << "FBO valid:" << std::boolalpha << d_fbo->isValid() << std::endl;
	}


	d_fbo->bind();
	f->glViewport(0,0,d_fbo->width(),d_fbo->height());
	f->glClear(GL_COLOR_BUFFER_BIT);

	std::cerr << "Binding texture from buffer" << std::endl;
	std::cerr <<  std::boolalpha << frame->Buffer->bind() << std::endl;
	f->glBindTexture(GL_TEXTURE_2D,d_texture);
	f->glTexImage2D(GL_TEXTURE_2D,
	                0,
	                GL_RGB,
	                frame->Width,
	                frame->Height,
	                0,
	                GL_RGB,
	                GL_UNSIGNED_BYTE,
	                0);
	frame->Buffer->release();

	d_blitter.bind();
	auto target = QOpenGLTextureBlitter::targetTransform(QRect(QPoint(0,0),d_fbo->size()),
	                                                     QRect(QPoint(0,0),d_fbo->size()));
	d_blitter.blit(d_texture, target,QOpenGLTextureBlitter::OriginBottomLeft);
	d_blitter.release();

	//TODO: draw display data
	f->glEnableVertexAttribArray(0);

	f->glBindBuffer(GL_ARRAY_BUFFER,d_triangle);
	f->glVertexAttribPointer(0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	                         3,                  // size
	                         GL_FLOAT,           // type
	                         GL_FALSE,           // normalized?
	                         0,                  // stride
	                         (void*)0            // array buffer offset
	                         );
	f->glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
	f->glDisableVertexAttribArray(0);

	d_fbo->release();

	doneCurrent();
	std::cerr << "Scheduling an update" << std::endl;
	update();
}
