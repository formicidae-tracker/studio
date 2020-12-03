#pragma once

#include <QWidget>

namespace Ui {
class AntListWidget;
}

class ExperimentBridge;

class QSortFilterProxyModel;

class AntListWidget : public QWidget {
    Q_OBJECT
public:
	explicit AntListWidget(QWidget *parent = nullptr);
    virtual ~AntListWidget();

	void setup(ExperimentBridge * experiment);

public slots:


private slots:
	void onSelectionChanged();
	void updateNumber();


	void on_colorBox_colorChanged(const QColor & color);
	void on_addButton_clicked();
	void on_deleteButton_clicked();

	void onDoubleClicked(const QModelIndex & index);

	void updateShowAll();
	void updateUnsoloAll();
private:

	friend class IdentifierUTest_AntListWidgetTest_Test;
    Ui::AntListWidget * d_ui;
	ExperimentBridge  * d_experiment;

	QSortFilterProxyModel * d_sortedModel;
};
