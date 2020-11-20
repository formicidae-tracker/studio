#pragma once

#include <QWidget>

#include <fort/studio/MyrmidonTypes/Identification.hpp>

class IdentifierBridge;
class QSortFilterProxyModel;

namespace Ui {
class IdentificationListWidget;
}

class IdentificationListWidget : public QWidget {
	Q_OBJECT
public:
	explicit IdentificationListWidget(QWidget *parent = nullptr);
	virtual ~IdentificationListWidget();


	void initialize(IdentifierBridge * identifier);

private slots:
	void on_identificationsView_activated(const QModelIndex &);
	void onIdentificationsViewSelectionChanged();

	void on_removeButton_clicked();

signals:
	void identificationSelected(const fmp::Identification::ConstPtr & identification);

private:
	Ui::IdentificationListWidget * d_ui;
	IdentifierBridge             * d_identifier;
	QSortFilterProxyModel        * d_sortedModel;
};
