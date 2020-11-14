#pragma once

#include "Workspace.hpp"

namespace Ui {
class AntMetadataWorkspace;
}

class AntMetadataBridge;

class AntMetadataWorkspace : public Workspace {
	Q_OBJECT
public:
	explicit AntMetadataWorkspace(QWidget *parent = 0);
	virtual ~AntMetadataWorkspace();


protected:
	void initialize(ExperimentBridge * experiment) override;
	void setUp(QMainWindow * main,const NavigationAction & actions) override;
	void tearDown(QMainWindow * maina,const NavigationAction & actions) override;


private slots:
	void on_dataView_activated(const QModelIndex & index);
	void onAddButtonClicked();
	void onRemoveButtonClicked();

	void onSelectedAntID(quint32 ID);


private:
	void initialize(AntMetadataBridge * metadata);


	Ui::AntMetadataWorkspace * d_ui;
	AntMetadataBridge  * d_metadata;
};
