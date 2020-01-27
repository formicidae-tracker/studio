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
		           SIGNAL(antCreated(const fort::myrmidon::priv::AntPtr &)),
		           this,
		           SLOT(onAntCreated(const fort::myrmidon::priv::AntPtr &)));
		disconnect(d_controller,
		           SIGNAL(antModified(const fort::myrmidon::priv::AntPtr &)),
		           this,
		           SLOT(onAntModified(const fort::myrmidon::priv::AntPtr &)));
		disconnect(d_controller,
		           SIGNAL(antDeleted(const fort::myrmidon::priv::AntPtr &)),
		           this,
		           SLOT(onAntDeleted(const fort::myrmidon::priv::AntPtr &)));

		disconnect(d_controller,
		           SIGNAL(identificationCreated(const fort::myrmidon::priv::IdentificationPtr &)),
		           this,
		           SLOT(onIdentificationCreated(const fort::myrmidon::priv::IdentificationPtr &)));

		disconnect(d_controller,
		           SIGNAL(identificationDeleted(const fort::myrmidon::priv::IdentificationPtr &)),
		           this,
		           SLOT(onIdentificationDeleted(const fort::myrmidon::priv::IdentificationPtr &)));


	}
	d_controller = controller;
	if (d_controller == NULL ) {
		d_ui->filterEdit->setEnabled(false);
		d_ui->addButton->setEnabled(false);
		return;
	}

	connect(d_controller,
	        SIGNAL(antCreated(const fort::myrmidon::priv::AntPtr &)),
	        this,
	        SLOT(onAntCreated(const fort::myrmidon::priv::AntPtr &)));
	connect(d_controller,
	        SIGNAL(antModified(const fort::myrmidon::priv::AntPtr &)),
	        this,
	        SLOT(onAntModified(const fort::myrmidon::priv::AntPtr &)));
	connect(d_controller,
	        SIGNAL(antDeleted(const fort::myrmidon::priv::AntPtr &)),
	        this,
	        SLOT(onAntDeleted(const fort::myrmidon::priv::AntPtr &)));

	connect(d_controller,
	        SIGNAL(identificationCreated(const fort::myrmidon::priv::IdentificationPtr &)),
	        this,
	        SLOT(onIdentificationCreated(const fort::myrmidon::priv::IdentificationPtr &)));

	connect(d_controller,
	        SIGNAL(identificationDeleted(const fort::myrmidon::priv::IdentificationPtr &)),
	        this,
	        SLOT(onIdentificationDeleted(const fort::myrmidon::priv::IdentificationPtr &)));

	setupList();
	d_ui->filterEdit->setEnabled(true);
	d_ui->addButton->setEnabled(true);

}


QString AntListWidget::format(const fort::myrmidon::priv::Ant & a) {

	QSet<fort::myrmidon::priv::TagID> tags;
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


void AntListWidget::setupList() {
	d_ui->listWidget->clear();
	d_items.clear();

	auto & ants = d_controller->experiment().ConstIdentifier().Ants();

	if (!d_controller) {
		d_ui->groupBox->setTitle(tr("Ants: %1").arg(ants.size()));
		return;
	}

	for ( auto const & [ID,a] : ants ) {
		onAntCreated(a);
	}
}


void AntListWidget::on_filterEdit_textChanged(const QString & text) {
	QRegExp rex(text,Qt::CaseInsensitive);
	for( auto const & [ID,item] : d_items ) {
		item->setHidden(rex.indexIn(item->text()) == -1 );
	}
}


void AntListWidget::on_listWidget_itemSelectionChanged() {
	auto items = d_ui->listWidget->selectedItems();
	switch (items.size()) {
	case 0:
		d_ui->removeButton->setEnabled(false);
		emit antSelected(fort::myrmidon::Ant::NO_ANT);
		break;
	case 1 :
		emit antSelected(items[0]->data(Qt::UserRole).toInt());
	default:
		d_ui->removeButton->setEnabled(true);
	}
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


void AntListWidget::onAntCreated(const fort::myrmidon::priv::AntPtr &a) {
	auto newItem = new QListWidgetItem(format(*a),d_ui->listWidget);
	newItem->setData(Qt::UserRole,a->ID());
	d_items[a->ID()] = newItem;
}

void AntListWidget::onAntDeleted(const fort::myrmidon::priv::AntPtr & a) {
	auto fi = d_items.find(a->ID());
	if ( fi == d_items.end() ) {
		return;
	}
	delete fi->second;
	d_items.erase(fi);
}

void AntListWidget::onAntModified(const fort::myrmidon::priv::AntPtr& a) {
	auto fi = d_items.find(a->ID());
	if ( fi == d_items.end() ) {
		return;
	}
	fi->second->setText(format(*a));
}

void AntListWidget::onIdentificationCreated(const fort::myrmidon::priv::IdentificationPtr & i) {
	onAntModified(i->Target());
}

void AntListWidget::onIdentificationDeleted(const fort::myrmidon::priv::IdentificationPtr & i) {
	onAntModified(i->Target());
}
