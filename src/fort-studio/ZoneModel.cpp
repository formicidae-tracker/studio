#include "ZoneModel.hpp"

#include <myrmidon/priv/TrackingDataDirectory.hpp>

#include <QStandardItemModel>
#include <QDebug>

using namespace fort::myrmidon;

Q_DECLARE_METATYPE(priv::TrackingDataDirectoryConstPtr);
Q_DECLARE_METATYPE(priv::Zone::Ptr);

ZoneModel::ZoneModel( QObject * parent)
	: QObject(parent)
	, d_model(new QStandardItemModel(this) ) {

	connect(d_model,
	        SIGNAL(itemChanged(QStandardItem * )),
	        this,
	        SLOT(on_model_itemChanged(QStandardItem *)));

	d_group = std::make_shared<priv::Zone::Group>();

	using TDD = priv::TrackingDataDirectory;

	auto z1 = priv::Zone::Group::Create(d_group,"foo");

	auto z2 = priv::Zone::Group::Create(d_group,"foobar");

	z1->AddTrackingDataDirectory(TDD::Create("foo.0000",
	                                         "/tmp/foo.0000",
	                                         0,
	                                         100,
	                                         Time::FromTimeT(0),
	                                         Time::FromTimeT(100),
	                                         std::make_shared<TDD::TrackingIndex>(),
	                                         std::make_shared<TDD::MovieIndex>(),
	                                         std::make_shared<TDD::FrameReferenceCache>()));

	z1->AddTrackingDataDirectory(TDD::Create("foo.0001",
	                                         "/tmp/foo.0001",
	                                         101,
	                                         200,
	                                         Time::FromTimeT(101),
	                                         Time::FromTimeT(200),
	                                         std::make_shared<TDD::TrackingIndex>(),
	                                         std::make_shared<TDD::MovieIndex>(),
	                                         std::make_shared<TDD::FrameReferenceCache>()));

	z2->AddTrackingDataDirectory(TDD::Create("bar.0000",
	                                         "/tmp/bar.0000",
	                                         0,
	                                         198,
	                                         Time::FromTimeT(0),
	                                         Time::FromTimeT(198),
	                                         std::make_shared<TDD::TrackingIndex>(),
	                                         std::make_shared<TDD::MovieIndex>(),
	                                         std::make_shared<TDD::FrameReferenceCache>()));

	BuildAll(d_group->Zones());
}



QAbstractItemModel * ZoneModel::model() {
	return d_model;
}

void ZoneModel::on_model_itemChanged(QStandardItem * item) {
	if ( item->data(Qt::UserRole+1).toInt() != ZONE_TYPE || item->column() != 0 ) {
		qDebug() << "Invalid item changed";
		return;
	}

	auto z = item->data(Qt::UserRole+2).value<priv::Zone::Ptr>();
	if (item->text() == z->URI().c_str()) {
		return;
	}

	try {
		z->SetName(item->text().toUtf8().data());
	} catch (const std::exception & e) {
		qDebug() << "Could not change name: " << e.what();
		item->setText(z->URI().c_str());
	}
}


QList<QStandardItem*> ZoneModel::BuildTDD(const priv::TrackingDataDirectoryConstPtr & tdd) {
	auto formatFrameID = [](priv::FrameID FID) -> QString {
		                     return std::to_string(FID).c_str();
	                     };
	auto formatTime = [](const Time & t) -> QString {
		                  std::ostringstream oss;
		                  oss << t;
		                  return oss.str().c_str();
	                  };
	auto uri = new QStandardItem(tdd->URI().c_str());
	auto path = new QStandardItem(tdd->AbsoluteFilePath().c_str());
	auto start = new QStandardItem(formatFrameID(tdd->StartFrame()));
	auto end = new QStandardItem(formatFrameID(tdd->EndFrame()));
	auto startDate = new QStandardItem(formatTime(tdd->StartDate()));
	auto endDate = new QStandardItem(formatTime(tdd->EndDate()));

	QList<QStandardItem*> res = {uri,path,start,end,startDate,endDate};
	for(const auto i : res) {
		i->setEditable(false);
		i->setData(TDD_TYPE,Qt::UserRole+1);
		i->setData(QVariant::fromValue(tdd),Qt::UserRole+2);
	}
	return res;
}

QList<QStandardItem*> ZoneModel::BuildZone(const priv::Zone::Ptr & z) {
	auto zoneItem = new QStandardItem(z->URI().c_str());
	zoneItem->setEditable(true);
	zoneItem->setData(ZONE_TYPE,Qt::UserRole+1);
	zoneItem->setData(QVariant::fromValue(z),Qt::UserRole+2);
	for ( const auto & tdd : z->TrackingDataDirectories() ) {
		auto tddItem = BuildTDD(tdd);
		zoneItem->appendRow(tddItem);
	}
	QList<QStandardItem*> res = {zoneItem};
	for(size_t i = 0 ; i < 5; ++i) {
		auto dummyItem = new QStandardItem("");
		dummyItem->setEditable(false);
		dummyItem->setData(ZONE_TYPE,Qt::UserRole+1);
		dummyItem->setData(QVariant::fromValue(z),Qt::UserRole+2);
		res.push_back(dummyItem);
	}
	return res;
}

void ZoneModel::BuildAll(const std::vector<priv::Zone::Ptr> & zones) {
	d_model->clear();
	d_model->setColumnCount(6);
	auto labels = {tr("URI"),tr("Filepath"),tr("Start Frame"),tr("End Frame"),tr("Start Date"),tr("End Date")};
	d_model->setHorizontalHeaderLabels(labels);
	for (const auto & z : zones) {
		d_model->invisibleRootItem()->appendRow(BuildZone(z));
	}
}
