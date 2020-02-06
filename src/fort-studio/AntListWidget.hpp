#pragma once

#include <QWidget>
#include <QListWidget>
#include "MainWindow.hpp"
#include "ExperimentController.hpp"

#include <myrmidon/Ant.hpp>

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

	//void on_listWidget_itemDoubleClicked(QListWidgetItem * item);
	void on_listWidget_itemSelectionChanged();
	void on_filterEdit_textChanged(const QString & text);

	void on_addButton_clicked();
	void on_removeButton_clicked();

	void onAntCreated(const fort::myrmidon::priv::AntPtr &);
	void onAntDeleted(const fort::myrmidon::priv::AntPtr &);
	void onAntModified(const fort::myrmidon::priv::AntPtr&);

	void onIdentificationCreated(const fort::myrmidon::priv::IdentificationPtr &);
	void onIdentificationDeleted(const fort::myrmidon::priv::IdentificationPtr &);


signals:
	void antSelected(fort::myrmidon::Ant::ID);

private:
	void setupList();
	void updateNumber();

    Ui::AntListWidget *d_ui;

	ExperimentController * d_controller;

	std::map<fort::myrmidon::Ant::ID,QListWidgetItem*> d_items;

	QString format(const fort::myrmidon::priv::Ant & a);
};
