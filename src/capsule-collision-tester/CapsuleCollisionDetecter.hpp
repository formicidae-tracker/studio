#pragma once

#include <QWidget>

class QGraphicsScene;

namespace Ui {
class CapsuleCollisionDetecter;
}

class CapsuleCollisionDetecter : public QWidget {
	Q_OBJECT
public:
	explicit CapsuleCollisionDetecter(QWidget *parent = 0);
	~CapsuleCollisionDetecter();

public slots:
	void onAnyValueChanged();

private:
	Ui::CapsuleCollisionDetecter * d_ui;
	QGraphicsScene               * d_scene;
};
