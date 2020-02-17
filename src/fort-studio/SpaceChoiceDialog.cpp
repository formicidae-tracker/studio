#include "SpaceChoiceDialog.hpp"
#include "ui_SpaceChoiceDialog.h"

#include "UniverseBridge.hpp"

#include <QPushButton>

SpaceChoiceDialog::SpaceChoiceDialog(UniverseBridge * universe,
                                     QWidget *parent)
	: QDialog(parent)
	, d_ui(new Ui::SpaceChoiceDialog) {
	setWindowModality(Qt::ApplicationModal);
	d_ui->setupUi(this);
	const auto & m = universe->model();
	for (size_t i = 0; i < m->rowCount(); ++i) {
		auto sName = m->data(m->index(i,0),Qt::DisplayRole).toString();
		d_ui->comboBox->addItem(sName,sName);
	}
	d_ui->comboBox->setCurrentIndex(-1);
	d_ui->comboBox->setEditable(true);
	d_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	connect(d_ui->comboBox,
	        &QComboBox::currentTextChanged,
	        [this](const QString & value) {
		        auto enabled = value.isEmpty() == false;
		        d_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enabled);
	        });
}

SpaceChoiceDialog::~SpaceChoiceDialog() {
	delete d_ui;
}


QString SpaceChoiceDialog::Get(UniverseBridge * universe,
                               QWidget * parent) {
	SpaceChoiceDialog dialog(universe,parent);
	QEventLoop loop;
	connect(&dialog, &QDialog::finished,&loop, &QEventLoop::quit);
	dialog.open();
	loop.exec();
	if ( dialog.result() == QDialog::Rejected ) {
		return "";
	}
	return dialog.d_ui->comboBox->currentText();
}
