#pragma once

#include <QWidget>

namespace Ui {
class CapsuleCollisionDetecter;
}

class CapsuleCollisionDetecter : public QWidget {
	Q_OBJECT
public:
	explicit CapsuleCollisionDetecter(QWidget *parent = 0);
	~CapsuleCollisionDetecter();

private:
	Ui::CapsuleCollisionDetecter * d_ui;
};
