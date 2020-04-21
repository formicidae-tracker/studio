#pragma once

#include <QWidget>

#include <fort/studio/bridge/ZoneBridge.hpp>

#include "Navigatable.hpp"

namespace Ui {
class ZoningWidget;
}
class ExperimentBridge;
class QAction;
class ZoningWidget : public QWidget , public Navigatable{
	Q_OBJECT
public:
	explicit ZoningWidget(QWidget *parent = 0);
	~ZoningWidget();

	void setup(ExperimentBridge * experiment);

protected:
	void setUp(const NavigationAction & actions) override;
	void tearDown(const NavigationAction & actions) override;

public slots:
	void nextCloseUp();
	void previousCloseUp();

private slots:
	void onCopyTime();

private:
	void display(const std::shared_ptr<ZoneBridge::FullFrame> & fullframe);

	void select(int increment);

	Ui::ZoningWidget * d_ui;
	ZoneBridge       * d_zones;

	std::shared_ptr<ZoneBridge::FullFrame> d_fullframe;
	QAction                              * d_copyAction;
};
