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


QString AntListWidget::format(const fort::myrmidon::priv::Ant & a) {

	QMap<uint32_t,bool> tags;
	for(auto const & m : a.Metadata()->marker()) {
		if ( m.has_marker() == false ) {
			continue;
		}
		tags[m.marker().id()] = true;
	}
	auto res = tr("%1, tags:").arg(a.FormatID().c_str());
	if (tags.isEmpty()) {
		return res + tr("<no-tags>");
	}

	QString sep = "";
	for(auto const & t : tags.keys() ) {
		res += sep + QString::number(t);
		if (sep.isEmpty()) {
			sep = ",";
		}
	}
	return res;
}

void AntListWidget::updateList() {
	d_ui->groupBox->setTitle(tr("Ants: %1").arg(d_experiment->Ants().size()));


	QSet<uint32_t> notInList;

	for ( auto const & k : d_items.keys() ) {
		notInList.insert(k);
	}

	for ( auto const & a : d_experiment->Ants() ) {
		notInList.remove(a->ID());
		auto item = d_items.find(a->ID());
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
