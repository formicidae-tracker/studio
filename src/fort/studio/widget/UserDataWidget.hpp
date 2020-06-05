#pragma once

#include <QWidget>

namespace Ui {
class UserDataWidget;
}

class AntMetadataBridge;

class UserDataWidget : public QWidget {
	Q_OBJECT
public:
	explicit UserDataWidget(QWidget *parent = 0);
	virtual ~UserDataWidget();

	void setup(AntMetadataBridge * metadata);

private slots:
	void on_dataView_activated(const QModelIndex & index);
	void onAddButtonClicked();
	void onRemoveButtonClicked();

	void onSelectedAntID(quint32 ID);


private:
	Ui::UserDataWidget * d_ui;
	AntMetadataBridge  * d_metadata;
};
