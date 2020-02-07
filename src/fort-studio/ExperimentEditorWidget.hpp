#pragma once

#include <QWidget>

#include "ExperimentController.hpp"


namespace Ui {
class ExperimentEditorWidget;
}

class ZoneAndTDDBridge;

class ExperimentEditorWidget : public QWidget {
	Q_OBJECT
public:
	explicit ExperimentEditorWidget(QWidget *parent = nullptr);
	~ExperimentEditorWidget();


signals:
	void addTriggered();
	void deleteTriggered(const QString & path);

public slots:
	void onNewController(ExperimentController * controller);

	void onFamilyChanged(fort::tags::Family f);

	void on_nameEdit_textEdited(const QString & text);
	void on_authorEdit_textEdited(const QString & text);
	void on_commentEdit_textChanged();
	void on_familySelector_currentIndexChanged(int index);
	void on_tagSizeEdit_valueChanged(double value);


private:
	Ui::ExperimentEditorWidget * d_ui;
	ExperimentController       * d_controller;
	ZoneAndTDDBridge           * d_zones;
};
