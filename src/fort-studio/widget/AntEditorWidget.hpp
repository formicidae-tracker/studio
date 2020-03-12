#pragma once

#include <QWidget>

namespace Ui {
class AntEditorWidget;
}

class ExperimentBridge;
class QStandardItemModel;

class AntEditorWidget : public QWidget {
	Q_OBJECT
public:
	explicit AntEditorWidget(QWidget *parent = 0);
	~AntEditorWidget();

	void setup(ExperimentBridge * experiment);



private slots:
	void on_toolBox_currentChanged(int);

	void on_treeView_activated(const QModelIndex & index);

	void onAntSelected(bool);
protected:
	void changeEvent(QEvent * event) override;
private:
	void setShappingMode();
	void setMeasureMode();

	void buildCloseUpList();
	void buildHeaders();

	Ui::AntEditorWidget * d_ui;
	ExperimentBridge    * d_experiment;
	QStandardItemModel  * d_closeUps;
};
