#include "IdentifierBridge.hpp"

#include <QDebug>

#include <myrmidon/priv/Identifier.hpp>


IdentifierBridge::IdentifierBridge(QObject * parent)
	: Bridge(parent)
	, d_model(new QStandardItemModel(this)) {

	connect(d_model,
	        &QStandardItemModel::itemChanged,
	        this,
	        &IdentifierBridge::onItemChanged);

}

bool IdentifierBridge::isActive() const {
	return d_experiment.get() != NULL;
}


QAbstractItemModel * IdentifierBridge::antModel() const {
	return d_model;
}

void IdentifierBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
	setModified(false);
	d_model->clear();

	if ( d_experiment ) {
		d_experiment->Identifier()
			.SetAntPositionUpdateCallback([](const fmp::Identification::Ptr & i) {
			                              });
	}

	d_experiment = experiment;
	if ( !d_experiment ) {
		emit activated(false);
		return;
	}
	d_experiment->Identifier()
		.SetAntPositionUpdateCallback([=](const fmp::Identification::Ptr & ident) {
			                              emit identificationAntPositionModified(ident);
		                              });

	for ( const auto & [AID,a] : d_experiment->Identifier().Ants() ) {
		d_model->invisibleRootItem()->appendRow(buildAnt(a));
	}

	emit activated(true);
}

fmp::Ant::Ptr IdentifierBridge::createAnt() {
	if ( !d_experiment ) {
		return fmp::Ant::Ptr();
	}
	fmp::Ant::Ptr ant;
	try {
		ant = d_experiment->Identifier().CreateAnt();
	} catch ( const std::exception & e) {
		qWarning() << "Could not create Ant: " << e.what();
		return fmp::Ant::Ptr();
	}

	d_model->invisibleRootItem()->appendRow(buildAnt(ant));

	setModified(true);
	emit antCreated(ant);
	return ant;
}

void IdentifierBridge::removeAnt(fm::Ant::ID AID) {
	auto item = findAnt(AID);
	if ( !d_experiment || item == NULL) {
		return;
	}


	try {
		d_experiment->Identifier().DeleteAnt(AID);
	} catch (const std::exception & e) {
		qWarning() << "Could not delete Ant '" <<  fmp::Ant::FormatID(AID).c_str()
		           << "': " << e.what();
		return;
	}

	d_model->removeRows(item->row(),1);
	setModified(true);
	emit antDeleted(AID);
}

fmp::Identification::Ptr IdentifierBridge::addIdentification(fm::Ant::ID AID,
                                                             fmp::TagID TID,
                                                             fm::Time::ConstPtr & start,
                                                             fm::Time::ConstPtr & end) {

	static auto formatTime= [](const fm::Time::ConstPtr & t, const QString & prefix) -> QString {
		                        if (t) {
			                        std::ostringstream os;
			                        os << t;
			                        return os.str().c_str();
		                        }
		                        return prefix + "∞";
	                        };

	auto item = findAnt(AID);

	if ( !d_experiment || item == NULL ) {
		return fmp::Identification::Ptr();
	}

	fmp::Identification::Ptr identification;
	try {
		identification = d_experiment->Identifier().AddIdentification(AID,TID,start,end);
	} catch (const std::exception & e) {
		qWarning() << "Could not create Identification " << fmp::Ant::FormatID(AID).c_str()
		           << "↤" << TID
		           << " [" << formatTime(start,"-")
		           << ";" << formatTime(end,"+")
		           << "]: " << e.what();
		return fmp::Identification::Ptr();
	}

	item->setText(formatAntName(item->data().value<fmp::Ant::Ptr>()));

	emit identificationCreated(identification);
	setModified(true);
	return identification;
}

void IdentifierBridge::deleteIdentification(const fmp::Identification::Ptr & identification) {
	auto item = findAnt(identification->Target()->ID());
	if ( !d_experiment || item == NULL) {
		return ;
	}

	try {
		d_experiment->Identifier().DeleteIdentification(identification);
	} catch (const std::exception & e ) {
		std::ostringstream os;
		os << *identification;
		qWarning() << "Could not delete identification " << os.str().c_str()
		           << ": " <<  e.what();
		return;
	}

	item->setText(formatAntName(item->data().value<fmp::Ant::Ptr>()));
	setModified(true);
	emit identificationDeleted(identification);
}

QString IdentifierBridge::formatAntName(const fmp::Ant::Ptr & ant) {
	QString res = fmp::Ant::FormatID(ant->ID()).c_str();
	if ( ant->Identifications().empty() ) {
		return res + ", <no-tags>";
	}
	std::set<fmp::TagID> tags;
	for ( const auto & i : ant->Identifications() ) {
		tags.insert(i->TagValue());
	}
	QString prefix = ", tags:";
	for ( const auto & t : tags ) {
		res += prefix + QString::number(t);
		prefix = ",";
	}
	return res;
}

QList<QStandardItem*> IdentifierBridge::buildAnt(const fmp::Ant::Ptr & ant) {
	auto data = QVariant::fromValue(ant);
	auto label = new QStandardItem(formatAntName(ant));
	label->setEditable(false);
	label->setData(data);

	auto color = new QStandardItem("");
	color->setEditable(false);
	color->setIcon(antDisplayColor(ant));
	color->setData(data);

	auto hidden = new QStandardItem("");
	hidden->setCheckable(true);
	hidden->setData(data);

	auto solo = new QStandardItem("");
	solo->setCheckable(true);
	solo->setData(data);

	switch(ant->DisplayStatus()) {
	case fmp::Ant::DisplayState::VISIBLE:
		hidden->setCheckState(Qt::Unchecked);
		solo->setCheckState(Qt::Unchecked);
	case fmp::Ant::DisplayState::HIDDEN:
		hidden->setCheckState(Qt::Checked);
		solo->setCheckState(Qt::Unchecked);
	case fmp::Ant::DisplayState::SOLO:
		hidden->setCheckState(Qt::Unchecked);
		solo->setCheckState(Qt::Checked);
	}

	return {label,color,hidden,solo};
}

QIcon IdentifierBridge::antDisplayColor(const fmp::Ant::Ptr & ant) {
	auto c = ant->DisplayColor();
	QColor color(std::get<0>(c),std::get<1>(c),std::get<2>(c));
	QPixmap pixmap(20,20);
	pixmap.fill(color);
	return pixmap;
}

QString IdentifierBridge::formatIdentification(const fmp::Identification::Ptr & ident) {
	std::ostringstream os;
	os << "↤ " << ident->TagValue();
	if ( ident->Start() ) {
		os << "[" << ident->Start()->DebugString();
	} else {
		os << "]-∞";
	}
	if ( ident->End() ) {
		os << ";" << ident->End()->DebugString() << "[";
	} else {
		os << "+∞[";
	}
	return os.str().c_str();
}


QStandardItem * IdentifierBridge::findAnt(fm::Ant::ID AID) const {
	auto items = d_model->findItems(fmp::Ant::FormatID(AID).c_str(), Qt::MatchStartsWith);
	if ( items.size() != 1 ) {
		return NULL;
	}
	return items[0];
}


void IdentifierBridge::onItemChanged(QStandardItem * item) {
	if ( item->column() < 2 ) {
		return;
	}

	auto ant = item->data().value<fmp::Ant::Ptr>();
	switch ( item->column() ) {
	case 2:
		if ( item->checkState() == Qt::Checked ){
			ant->SetDisplayStatus(fmp::Ant::DisplayState::HIDDEN);
			d_model->item(item->row(),3)->setCheckState(Qt::Unchecked);
			setModified(true);
			emit antDisplayChanged(ant->ID(),ant->DisplayColor(),ant->DisplayStatus());
		} else if (d_model->item(item->row(),3)->checkState() == Qt::Unchecked ) {
			ant->SetDisplayStatus(fmp::Ant::DisplayState::VISIBLE);
			setModified(true);
			emit antDisplayChanged(ant->ID(),ant->DisplayColor(),ant->DisplayStatus());
		}
	case 3:
		if ( item->checkState() == Qt::Checked ) {
			ant->SetDisplayStatus(fmp::Ant::DisplayState::SOLO);
			d_model->item(item->row(),2)->setCheckState(Qt::Unchecked);
			setModified(true);
			emit antDisplayChanged(ant->ID(),ant->DisplayColor(),ant->DisplayStatus());
		} else if ( d_model->item(item->row(),2)->checkState() == Qt::Unchecked) {
			ant->SetDisplayStatus(fmp::Ant::DisplayState::VISIBLE);
			setModified(true);
			emit antDisplayChanged(ant->ID(),ant->DisplayColor(),ant->DisplayStatus());
		}
	}

}
