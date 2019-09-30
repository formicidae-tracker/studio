#pragma once

#include <QWidget>

#include "ExperimentController.hpp"


namespace Ui {
class ExperimentInfoWidget;
}

class ExperimentInfoWidget : public QWidget {
	Q_OBJECT
public:
	explicit ExperimentInfoWidget(QWidget *parent = nullptr);
	~ExperimentInfoWidget();


signals:
	void addTriggered();
	void deleteTriggered(const QString & path);

public slots:
	void onNewController(ExperimentController * controller);

	void onDataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath & );

	void on_addButton_clicked();
	void on_removeButton_clicked();
	void on_nameEdit_textEdited(const QString & text);
	void on_authorEdit_textEdited(const QString & text);
	void on_commentEdit_textChanged();




private:
	Ui::ExperimentInfoWidget * d_ui;
	ExperimentController     * d_controller;
};
