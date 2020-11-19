#include "TagCloseUpExplorer.hpp"
#include "ui_TagCloseUpExplorer.h"

#include <fort/studio/bridge/TagCloseUpBridge.hpp>

CloseUpFilterModel::CloseUpFilterModel(QObject * parent)
	: QSortFilterProxyModel(parent)
	, d_removeUsed(false) {
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

void CloseUpFilterModel::setWhiteList(const QString & formattedTagID) {
	if ( d_whiteList == formattedTagID ) {
		return;
	}
	d_whiteList = formattedTagID;
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
	auto formattedTagID = sourceModel()->data(sourceModel()->index(sourceRow,0,sourceParent),Qt::DisplayRole).toString();
	if ( formattedTagID == d_whiteList ) {
		return true;
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
		emit currentTagIDChanged(-1);
		d_ui->closeUpsScroller->setCloseUps(-1,tcus);
		d_sortedFilteredModel->setWhiteList("");
		return;
	}
	auto tagID = tcus[0]->TagValue();
	d_ui->closeUpView->selectionModel()->select(index,QItemSelectionModel::Select | QItemSelectionModel::Rows );
	d_ui->closeUpsScroller->setCloseUps(tagID,tcus);
	d_sortedFilteredModel->setWhiteList(fm::FormatTagID(tagID).c_str());
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


void TagCloseUpExplorer::moveIndex(int direction) {
	int row = -1;
	if ( d_ui->closeUpView->selectionModel()->hasSelection() == false ) {
		row = direction > 0 ? 0 : d_sortedFilteredModel->rowCount() - 1;
	} else {
		row = d_ui->closeUpView->selectionModel()->selectedRows()[0].row() + direction;
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
