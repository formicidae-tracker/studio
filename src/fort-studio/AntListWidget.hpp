#pragma once

#include <QWidget>
#include <QListWidget>
#include "Experiment.hpp"

namespace Ui {
class AntListWidget;
}

class AntListWidget : public QWidget {
    Q_OBJECT
public:
	explicit AntListWidget(QWidget *parent = nullptr);
    ~AntListWidget();

	void setExperiment(Experiment * experiment);

public slots:
	void updateList();
	void on_listWidget_itemDoubleClicked(QListWidgetItem * item);
	void on_filterEdit_textChanged(const QString & text);

signals:
	void antSelected(uint32_t);

private:
    Ui::AntListWidget *d_ui;

	Experiment * d_experiment;

	QMap<uint32_t,QListWidgetItem*> d_items;

	QString format(const fort::myrmidon::priv::Ant & a);
};
