#pragma once

#include <QWidget>
#include <QListWidget>
#include "MainWindow.hpp"
#include "ExperimentController.hpp"

namespace Ui {
class AntListWidget;
}

class AntListWidget : public QWidget {
    Q_OBJECT
public:
	explicit AntListWidget(QWidget *parent = nullptr);
    ~AntListWidget();

public slots:
	void onNewController(ExperimentController * controller);

	void onAntListModified(const fort::myrmidon::priv::Experiment::AntByID & );

	void on_listWidget_itemDoubleClicked(QListWidgetItem * item);
	void on_filterEdit_textChanged(const QString & text);

signals:
	void antSelected(uint32_t);

private:
    Ui::AntListWidget *d_ui;

	ExperimentController * d_controller;

	QMap<uint32_t,QListWidgetItem*> d_items;

	QString format(const fort::myrmidon::priv::Ant & a);
};
