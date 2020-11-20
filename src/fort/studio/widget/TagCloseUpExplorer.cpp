#include "TagCloseUpExplorer.hpp"
#include "ui_TagCloseUpExplorer.h"

#include <fort/studio/bridge/TagCloseUpBridge.hpp>

CloseUpFilterModel::CloseUpFilterModel(QObject * parent)
	: QSortFilterProxyModel(parent)
	, d_removeUsed(false) {
	d_whitelist = -1;
}

CloseUpFilterModel::~CloseUpFilterModel() {
}

void CloseUpFilterModel::setFilter(const QString & filter) {
	if ( d_filter.pattern() == filter ) {
		return;
	}
	d_filter.setPattern(filter);
	invalidateFilter();
}

void CloseUpFilterModel::whitelist(fm::TagID tagID) {
	if ( d_whitelist == tagID ) {
		return;
	}
	d_whitelist = tagID;
	invalidateFilter();
}

void CloseUpFilterModel::blacklist(fm::TagID tagID) {
	if ( d_blacklist.insert(tagID).second == true ) {
		invalidateFilter();
	}
}

void CloseUpFilterModel::clearWhitelist() {
	whitelist(-1);
}

void CloseUpFilterModel::clearBlacklist() {
	if ( d_blacklist.empty() == true ) {
		return;
	}
	d_blacklist.clear();
	invalidateFilter();
}


void CloseUpFilterModel::setRemoveUsed(bool removeUsed) {
	if ( d_removeUsed == removeUsed ) {
		return;
	}
	d_removeUsed = removeUsed;
	invalidateFilter();
}


bool CloseUpFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const {
	auto index = sourceModel()->index(sourceRow,0,sourceParent);
	fm::TagID tagID = sourceModel()->data(index,Qt::UserRole+1).toInt();
	auto formattedTagID = sourceModel()->data(index,Qt::DisplayRole).toString();
	if ( tagID == d_whitelist ) {
		return true;
	}
	if ( d_blacklist.count(tagID) != 0 ) {
		return false;
	}

	bool matchFilter = true;
	if ( d_filter.pattern().isEmpty() == false) {
		matchFilter = d_filter.match(formattedTagID).hasMatch();
	}
	bool matchCount = true;
	if ( d_removeUsed == true ) {
		auto totalCount = sourceModel()->data(sourceModel()->index(sourceRow,1,sourceParent),Qt::DisplayRole).toInt();
		auto usedCount = sourceModel()->data(sourceModel()->index(sourceRow,2,sourceParent),Qt::DisplayRole).toInt();
		matchCount = totalCount > usedCount;
	}
	return matchCount && matchFilter;
}


TagCloseUpExplorer::TagCloseUpExplorer(QWidget *parent)
	: QWidget(parent)
	, d_sortedFilteredModel(new CloseUpFilterModel(this))
	, d_ui(new Ui::TagCloseUpExplorer) {
	d_ui->setupUi(this);

	d_ui->closeUpView->setModel(d_sortedFilteredModel);
    d_ui->closeUpView->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ui->closeUpView->setSelectionBehavior(QAbstractItemView::SelectRows);


    connect(d_ui->closeUpsScroller,
            &TagCloseUpScroller::currentCloseUpChanged,
            this,
            [this](const fmp::TagCloseUp::ConstPtr & closeUp) {
	            emit currentCloseUpChanged(closeUp);
            });


    connect(d_ui->closeUpFilterEdit,
            &QLineEdit::textChanged,
            d_sortedFilteredModel,
            &CloseUpFilterModel::setFilter);

        connect(d_ui->hideUsedTagBox,
            &QCheckBox::stateChanged,
            [this](int state) {
	            d_sortedFilteredModel->setRemoveUsed(state == Qt::Checked);
            });
    d_sortedFilteredModel->setRemoveUsed(d_ui->hideUsedTagBox->checkState() == Qt::Checked);

    d_ui->closeUpView->setContextMenuPolicy(Qt::DefaultContextMenu);
}


void TagCloseUpExplorer::initialize(TagCloseUpBridge * tagCloseUps) {
	d_tagCloseUps = tagCloseUps;

	d_sortedFilteredModel->setSourceModel(d_tagCloseUps->tagModel());
	d_ui->closeUpView->setModel(d_sortedFilteredModel);
	d_ui->closeUpView->setSortingEnabled(true);
	d_ui->closeUpView->sortByColumn(0,Qt::AscendingOrder);

	connect(d_tagCloseUps,
	        &Bridge::activated,
	        this,
	        [this]() {
		        d_ui->closeUpView->horizontalHeader()->setSortIndicatorShown(true);
		        d_ui->closeUpView->sortByColumn(0,Qt::AscendingOrder);
		        showAllTags();
		        nextTag();
	        });

	d_ui->closeUpsScroller->setUp(d_tagCloseUps);

}

TagCloseUpExplorer::~TagCloseUpExplorer() {
	delete d_ui;
}

void TagCloseUpExplorer::on_closeUpView_clicked(const QModelIndex & index) {
	const auto & tcus = d_tagCloseUps->closeUpsForIndex(d_sortedFilteredModel->mapToSource(index));
	d_ui->closeUpView->selectionModel()->clearSelection();
	if ( tcus.isEmpty() == true ) {
		clearCurrentTag();
		return;
	}
	auto tagID = tcus[0]->TagValue();
	d_ui->closeUpView->selectionModel()->select(index,QItemSelectionModel::Select | QItemSelectionModel::Rows );
	d_ui->closeUpsScroller->setCloseUps(tagID,tcus,tcus.begin());
	d_sortedFilteredModel->whitelist(tagID);
	emit currentTagIDChanged(tagID);
}

void TagCloseUpExplorer::nextTag() {
	moveIndex(1);
}

void TagCloseUpExplorer::nextTagCloseUp() {
	d_ui->closeUpsScroller->next();
}

void TagCloseUpExplorer::previousTag() {
	moveIndex(-1);
}


std::pair<int,fm::TagID> TagCloseUpExplorer::currentTag() {
	if ( d_ui->closeUpView->selectionModel()->hasSelection() == false ) {
		return {-1,-1};
	}
	auto index = d_ui->closeUpView->selectionModel()->selectedRows()[0];
	auto tagID = d_tagCloseUps->tagIDFromIndex(d_sortedFilteredModel->mapToSource(index));
	return {index.row(),tagID};
}


void TagCloseUpExplorer::moveIndex(int direction) {
	auto [row,tagID]  = currentTag();
	if ( row == - 1 ) {
		row = direction > 0 ? 0 : d_sortedFilteredModel->rowCount() - 1;
	} else {
		row += direction;
	}
	if ( row < 0 || row >= d_sortedFilteredModel->rowCount() ) {
		return;
	}
	auto index = d_sortedFilteredModel->index(row,0);
	on_closeUpView_clicked(index);
	d_ui->closeUpView->scrollTo(index);
}

void TagCloseUpExplorer::previousTagCloseUp() {
	d_ui->closeUpsScroller->previous();
}


void TagCloseUpExplorer::selectCloseUpForIdentification(const fmp::Identification::ConstPtr & identification) {
	if (!identification) {
		return;
	}

	auto tagID = identification->TagValue();
	auto closeUps = d_tagCloseUps->closeUpsForTag(tagID);
	auto fi = std::find_if(closeUps.begin(),
	                       closeUps.end(),
	                       [&identification](const fmp::TagCloseUp::ConstPtr & closeUp) ->bool {
		                       return identification->IsValid(closeUp->Frame().Time());
	                       });
	d_ui->closeUpsScroller->setCloseUps(identification->TagValue(),
	                                    closeUps,
	                                    fi);

	auto selectionModel = d_ui->closeUpView->selectionModel();
	d_sortedFilteredModel->whitelist(tagID);

	selectionModel->clearSelection();
	auto index = d_sortedFilteredModel->mapFromSource(d_tagCloseUps->indexForTag(tagID));
	selectionModel->select(index,QItemSelectionModel::Select | QItemSelectionModel::Rows );
	d_ui->closeUpView->scrollTo(index);
	emit currentTagIDChanged(tagID);
}

void TagCloseUpExplorer::clearCurrentTag() {
	QVector<fmp::TagCloseUp::ConstPtr> tcus;
	d_ui->closeUpsScroller->setCloseUps(-1,tcus,tcus.end());
	d_sortedFilteredModel->clearWhitelist();
	emit currentTagIDChanged(-1);
}


void TagCloseUpExplorer::hideTag(fm::TagID tagID) {
	const auto & [row,current] = currentTag();
	if ( current == tagID ) {
		if ( row < d_sortedFilteredModel->rowCount() - 1 ) {
			nextTag();
		} else if ( row != 0 ) {
			previousTag();
		} else {
			clearCurrentTag();
		}
	}
	d_sortedFilteredModel->blacklist(tagID);
}

void TagCloseUpExplorer::showAllTags() {
	d_ui->hideUsedTagBox->setCheckState(Qt::Unchecked);
	d_ui->closeUpFilterEdit->clear();
	d_sortedFilteredModel->clearBlacklist();
}
