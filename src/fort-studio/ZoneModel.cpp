#include "ZoneModel.hpp"

#include <myrmidon/priv/TrackingDataDirectory.hpp>

#include <QDebug>

using namespace fort::myrmidon;


ZoneModel::ZoneModel( QObject * parent)
	: QAbstractItemModel(parent) {

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
}

std::vector<priv::Zone::Ptr>::const_iterator findURIInZone(const std::vector<priv::Zone::Ptr> & zones,
                                                     const fs::path & URI) {
	return std::find_if(zones.cbegin(),
	                    zones.cend(),
	                    [&URI](const priv::Zone::Ptr & z) {
		                    return z->URI() == URI;
	                    });
}

QModelIndex ZoneModel::index(int row, int column,
                             const QModelIndex & parent) const {
	if ( hasIndex(row, column, parent) == false ){
		return QModelIndex();
	}

	if ( parent.isValid() == false ) {
		//looking up in zones
		if ( row >= d_group->Zones().size() ) {
			return QModelIndex();
		}
		auto z = d_group->Zones()[row];
		return createIndex(row,column,z.get());
	} else {
		//looking up in a zone

		auto ptr = static_cast<priv::Identifiable*>(parent.internalPointer());

		auto fi = findURIInZone(d_group->Zones(),ptr->URI());
		if ( fi == d_group->Zones().cend() ) {
			return QModelIndex();
		}

		if ( row >= (*fi)->TrackingDataDirectories().size() ) {
			return QModelIndex();
		}

		const priv::Identifiable * tdd = (*fi)->TrackingDataDirectories()[row].get();
		return createIndex(row,column,const_cast<priv::Identifiable*>(tdd));
	}
}

QModelIndex ZoneModel::parent(const QModelIndex &index) const {
	if ( index.isValid() == false ) {
		return QModelIndex();
	}

	auto path = static_cast<const priv::Identifiable*>(index.internalPointer())->URI();

	auto fi = findURIInZone(d_group->Zones(),path);
	if ( fi != d_group->Zones().cend() ) {
		return QModelIndex();
	}

	size_t row = 0;
	for (const auto & z : d_group->Zones()) {
		for ( const auto & tdd : z->TrackingDataDirectories() ) {
			if ( tdd->URI() == path ) {
				return createIndex(row, 0, z.get());
			}
		}
		++row;
	}
	return QModelIndex();
}

int ZoneModel::rowCount(const QModelIndex & parent) const {
	if ( parent.column() > 0 ) {
		return 0;
	}

	if ( parent.isValid() == false ) {
		return d_group->Zones().size();
	}

	auto path  = static_cast<const priv::Identifiable*>(parent.internalPointer())->URI();

	auto fi = findURIInZone(d_group->Zones(),path);
	if ( fi != d_group->Zones().cend() ) {
		return (*fi)->TrackingDataDirectories().size();
	}

	return 0;
}

int ZoneModel::columnCount(const QModelIndex &parent) const {
	return 6;
}

Qt::ItemFlags ZoneModel::flags(const QModelIndex & index) const {
	if (index.parent().isValid() ) {
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
	} else if ( index.column() == 0 ) {
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
	} else {
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable ;
	}

	return QAbstractItemModel::flags(index);
}

bool ZoneModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (index.isValid() == false || index.parent().isValid() == true || role != Qt::EditRole) {
		return false;
	}

	if ( index.column() != 0 || value.toString().isEmpty() ) {
		return false;
	}

	auto path = static_cast<const priv::Identifiable*>(index.internalPointer())->URI();
	auto fi = findURIInZone(d_group->Zones(),path);
	if ( fi == d_group->Zones().cend() ) {
		return false;
	}
	try {
		(*fi)->SetName(value.toString().toUtf8().data());
		emit dataChanged(index,index);
	} catch ( const std::exception & e) {
		return false;
	}

	return true;
}

QVariant ZoneModel::headerData(int section,
                               Qt::Orientation orientation,
                               int role) const {
	if ( role != Qt::DisplayRole || orientation != Qt::Horizontal ) {
		return QVariant();
	}

	switch(section) {
	case 0:
		return tr("URI");
	case 1:
		return tr("Filepath");
	case 2:
		return tr("Start FrameID");
	case 3:
		return tr("End FrameID");
	case 4:
		return tr("Start Date");
	case 5:
		return tr("End Date");
	}

	return QVariant();

}


QVariant ZoneModel::data(const QModelIndex &index, int role) const {
	if ( index.isValid() == false || role != Qt::DisplayRole ) {
		return QVariant();
	}
	auto path = static_cast<const priv::Identifiable*>(index.internalPointer())->URI();

	auto zi = findURIInZone(d_group->Zones(),path);
	if ( zi != d_group->Zones().end() ) {
		switch (index.column()) {
		case 0:
			return (*zi)->URI().c_str();
		default:
			return QVariant();
		}
	}

	auto tddi = d_group->TrackingDataDirectories().find(path);
	if ( tddi == d_group->TrackingDataDirectories().end() ) {
		return QVariant();
	}
	auto tdd = tddi->second;

	std::ostringstream oss;
	switch( index.column() ) {
	case 0:
		return tdd->URI().c_str();
	case 1:
		return tdd->AbsoluteFilePath().c_str();
	case 2:
		return qulonglong(tdd->StartFrame());
	case 3:
		return qulonglong(tdd->EndFrame());
	case 4:
		oss << tdd->StartDate();
		return oss.str().c_str();
	case 5:
		oss << tdd->EndDate();
		return oss.str().c_str();
	}


	return QVariant();
}
