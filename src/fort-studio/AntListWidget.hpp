#pragma once

#include <QWidget>

namespace Ui {
class AntListWidget;
}

class IdentifierBridge;

class QSortFilterProxyModel;

class AntListWidget : public QWidget {
    Q_OBJECT
public:
	explicit AntListWidget(QWidget *parent = nullptr);
    ~AntListWidget();

	void setup(IdentifierBridge *);

public slots:


private slots:
	void onSelectionChanged();
	void updateNumber();


	void on_colorBox_colorChanged(const QColor & color);
	void on_addButton_clicked();
	void on_deleteButton_clicked();

	void onDoubleClicked(const QModelIndex & index);
private:

	friend class IdentifierUTest_AntListWidgetTest_Test;
    Ui::AntListWidget * d_ui;
	IdentifierBridge  * d_identifier;

	QSortFilterProxyModel * d_sortedModel;
};
