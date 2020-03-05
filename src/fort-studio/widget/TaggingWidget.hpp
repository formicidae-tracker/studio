#pragma once

#include <QWidget>
#include <QTreeWidgetItem>

#include <myrmidon/priv/ForwardDeclaration.hpp>

class GlobalPropertyBridge;
class MeasurementBridge;
class IdentifierBridge;

namespace fmp = fort::myrmidon::priv;

namespace Ui {
class TaggingWidget;
}

class TaggingWidget : public QWidget {
    Q_OBJECT
public:
    explicit TaggingWidget(QWidget *parent = 0);
	~TaggingWidget();


	void setup(GlobalPropertyBridge * globalProperties,
	           MeasurementBridge * measurements,
	           IdentifierBridge * identifier);


public slots:

	void on_addIdentButton_clicked();
	void on_newAntButton_clicked();
	void on_deletePoseButton_clicked();

	void onIdentificationAntPositionChanged(fmp::IdentificationConstPtr);

private:
	Ui::TaggingWidget * d_ui;
};
