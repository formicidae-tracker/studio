#include "ExperimentInfoWidget.hpp"
#include "ui_ExperimentInfoWidget.h"

#include <QFileDialog>
#include <QDebug>
#include <QListWidget>


ExperimentInfoWidget::ExperimentInfoWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::ExperimentInfoWidget)
	, d_experiment(NULL) {
	d_ui->setupUi(this);
	d_ui->addButton->setEnabled(false);
	d_ui->removeButton->setEnabled(false);
}


ExperimentInfoWidget::~ExperimentInfoWidget() {
	delete d_ui;
}


void ExperimentInfoWidget::setExperiment(Experiment * exp) {
	if ( d_experiment != NULL ) {
		disconnect(d_experiment,SIGNAL(pathModified(const QString&)),this,SLOT(onExperimentPathModified(const QString&)));
		disconnect(d_experiment,SIGNAL(dataDirUpdated(QStringList)),this,SLOT(setDataDir(QStringList)));

	}
	d_experiment = exp;
	if (exp == NULL ) {
		return;
	}
	connect(d_experiment,SIGNAL(pathModified(const QString&)),this,SLOT(onExperimentPathModified(const QString&)));
	connect(d_experiment,SIGNAL(dataDirUpdated(QStringList)),this,SLOT(setDataDir(QStringList)));
}

void ExperimentInfoWidget::onExperimentPathModified(const QString & path) {
	d_ui->addButton->setEnabled(path.isEmpty() == false);
}

void ExperimentInfoWidget::on_addButton_clicked() {
	QString dataDir  = QFileDialog::getExistingDirectory(this, tr("Open Tracking Data Directory"),
	                                                     QFileInfo(d_experiment->AbsolutePath()).absolutePath(),
	                                                     QFileDialog::ShowDirsOnly);
	if ( dataDir.isEmpty() ) {
		return;
	}
	QString res;
	Error err = d_experiment->addDataDirectory(dataDir,res);
	if (!err.OK()) {
		qWarning() << err.what();
		return;
	}


	qInfo() << "Added '" << res << "'";

}
void ExperimentInfoWidget::on_removeButton_clicked() {
}


void ExperimentInfoWidget::setDataDir(QStringList data) {
	d_ui->listWidget->clear();
	for(auto const & p : data ) {
		auto item = new QListWidgetItem(p,d_ui->listWidget);
		item->setIcon(QIcon::fromTheme("folder"));
		d_ui->listWidget->addItem(item);
	}
}
