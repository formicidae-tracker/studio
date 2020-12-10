#pragma once

#include <QDialog>

#include <memory>

namespace Ui {
class CloneShapeDialog;
}

class ExperimentBridge;

class CloneShapeDialog : public QDialog {
	Q_OBJECT
public:
	explicit CloneShapeDialog(ExperimentBridge * experiment,
	                          quint32 sourceID,
	                          QWidget *parent = 0);
	virtual ~CloneShapeDialog();

	struct CloneOptions {
		bool OverwriteShapes;
		bool ScaleToSize;
	};

	static std::shared_ptr<CloneOptions> get(ExperimentBridge * experiment,
	                                         QWidget *parent);

private:
	Ui::CloneShapeDialog * d_ui;
};
