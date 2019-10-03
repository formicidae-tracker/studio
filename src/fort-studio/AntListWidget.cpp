#include "AntListWidget.hpp"
#include "ui_AntListWidget.h"

#include <myrmidon/priv/Ant.hpp>

#include <QtDebug>

AntListWidget::AntListWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::AntListWidget)
	, d_controller(NULL) {
	d_ui->setupUi(this);
	onNewController(NULL);
}

AntListWidget::~AntListWidget() {
    delete d_ui;
}


void AntListWidget::onNewController(ExperimentController * controller) {
	if ( d_controller != NULL ) {
		disconnect(d_controller,
		           SIGNAL(antListModified(const fort::myrmidon::priv::AntByID &)),
		           this,
		           SLOT(onAntListModified(const fort::myrmidon::priv::AntByID &)));
	}
	d_controller = controller;
	if (d_controller == NULL ) {
		d_ui->filterEdit->setEnabled(false);
		d_ui->addButton->setEnabled(false);
		return;
	}
	connect(d_controller,
	        SIGNAL(antListModified(const fort::myrmidon::priv::AntByID &)),
	        this,
	        SLOT(onAntListModified(const fort::myrmidon::priv::AntByID &)));
	onAntListModified(d_controller->experiment().Ants());
	d_ui->filterEdit->setEnabled(true);
	d_ui->addButton->setEnabled(true);

}


QString AntListWidget::format(const fort::myrmidon::priv::Ant & a) {

	QSet<uint32_t> tags;
	for(auto const & i : a.Identifications() ) {
		tags.insert(i->TagValue());
	}
	auto res = tr("%1, tags:").arg(a.FormattedID().c_str());
	if (tags.isEmpty()) {
		return res + tr("<no-tags>");
	}

	QString sep = "";
	for(auto const & t : tags ) {
		res += sep + QString::number(t);
		if (sep.isEmpty()) {
			sep = ",";
		}
	}
	return res;
}


void AntListWidget::onAntListModified(const fort::myrmidon::priv::AntByID & ants ) {
	d_ui->groupBox->setTitle(tr("Ants: %1").arg(ants.size()));

	QSet<uint32_t> notInList;

	for ( auto const & k : d_items.keys() ) {
		notInList.insert(k);
	}

	for ( auto const & [ID,a] : ants ) {
		notInList.remove(ID);
		auto item = d_items.find(ID);
		if ( item != d_items.end() ) {
			(*item)->setText(format(*a));
			continue;
		}
		auto newItem = new QListWidgetItem(format(*a),d_ui->listWidget);
		newItem->setData(Qt::UserRole,a->ID());
		d_items[a->ID()] = newItem;
		d_ui->listWidget->addItem(newItem);
	}

	for ( auto const & k : notInList ) {
		auto item = d_items.find(k);
		d_ui->listWidget->removeItemWidget(item.value());
		delete item.value();
		d_items.remove(k);
	}
}

void AntListWidget::on_listWidget_itemDoubleClicked(QListWidgetItem * item) {
	qInfo() << "double clicked" << item->text() << item->data(Qt::UserRole).toInt();
}


void AntListWidget::on_filterEdit_textChanged(const QString & text) {
	QRegExp rex(text,Qt::CaseInsensitive);
	for( auto const & item : d_items ) {
		item->setHidden(rex.indexIn(item->text()) == -1 );
	}
}


void AntListWidget::on_listWidget_itemSelectionChanged() {
	d_ui->removeButton->setEnabled(!d_ui->listWidget->selectedItems().isEmpty());
}


void AntListWidget::on_addButton_clicked() {
	if ( d_controller == NULL ) {
		return;
	}

	d_controller->createAnt();
}

void AntListWidget::on_removeButton_clicked() {
	if ( d_controller == NULL ) {
		return;
	}

	for ( auto const & item : d_ui->listWidget->selectedItems() ) {
		Error err = d_controller->removeAnt(item->data(Qt::UserRole).toInt());
		if ( err.OK() == false ) {
			qCritical() << err.what();
		}
	}
}
