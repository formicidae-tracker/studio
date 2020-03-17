#pragma once

#include <QDialog>

#include <fort/myrmidon/priv/FrameReference.hpp>


namespace Ui {
class FrameFinderDialog;
}


namespace fmp = fort::myrmidon::priv;

class UniverseBridge;

class FrameFinderDialog : public QDialog {
	Q_OBJECT
public:
	explicit FrameFinderDialog(UniverseBridge * universe,
	                           QWidget *parent = 0);
	~FrameFinderDialog();

	static fmp::FrameReference::ConstPtr Get(UniverseBridge * universe,
	                                         QWidget * parent);
private slots :

	void on_comboBox_currentIndexChanged(int);

private:
	Ui::FrameFinderDialog * d_ui;
};
