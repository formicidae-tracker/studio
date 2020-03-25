#pragma once

#include <QWidget>


namespace Ui {
class AntMetadataWidget;
}

class AntMetadataBridge;
class QComboBox;


class AntMetadataWidget : public QWidget {
	Q_OBJECT
public:
	explicit AntMetadataWidget(QWidget *parent = nullptr);
	virtual ~AntMetadataWidget();

	void setup(AntMetadataBridge * metadata);

	void buildTypeCombo(QComboBox * combo);

private slots:
	void on_addButton_clicked();
	void on_removeButton_clicked();

private:
	Ui::AntMetadataWidget * d_ui;
	AntMetadataBridge     * d_metadata;
};
