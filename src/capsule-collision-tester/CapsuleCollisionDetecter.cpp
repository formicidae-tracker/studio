#include "CapsuleCollisionDetecter.hpp"
#include "ui_CapsuleCollisionDetecter.h"

CapsuleCollisionDetecter::CapsuleCollisionDetecter(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::CapsuleCollisionDetecter) {
	d_ui->setupUi(this);
}

CapsuleCollisionDetecter::~CapsuleCollisionDetecter() {
	delete d_ui;
}
