#include "AntListWidget.hpp"
#include "ui_AntListWidget.h"

#include <QtDebug>

AntListWidget::AntListWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::AntListWidget)
	, d_experiment(NULL) {
	d_ui->setupUi(this);
}

AntListWidget::~AntListWidget() {
    delete d_ui;
}


void AntListWidget::setExperiment(Experiment * experiment) {
	if ( d_experiment != NULL ) {
		disconnect(d_experiment,SIGNAL(antListModified()),this,SLOT(updateList()));
	}
	d_experiment = experiment;
	connect(d_experiment,SIGNAL(antListModified()),this,SLOT(updateList()));
	updateList();
}

void AntListWidget::updateList() {
	QSet<uint32_t> notInList;

	for ( auto const & k : d_items.keys() ) {
		notInList.insert(k);
	}

	for ( auto const & a : d_experiment->Ants() ) {
		notInList.remove(a->ID());
		auto item = d_items.find(a->ID());
		if ( item != d_items.end() ) {
			(*item)->setText(QString::number(a->ID()));
			continue;
		}
		auto newItem = new QListWidgetItem(tr("%1 tags:").arg(a->ID()),d_ui->listWidget);
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
	qInfo() << "double clicked" << item->text();
}


void AntListWidget::on_filterEdit_textChanged(const QString & text) {
	QRegExp rex(text,Qt::CaseInsensitive);
	for( auto const & item : d_items ) {
		item->setHidden(rex.indexIn(item->text()) == -1 );
	}
}
