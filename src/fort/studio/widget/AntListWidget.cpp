#include "AntListWidget.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableView>
#include <QPushButton>
#include <QToolButton>
#include <QHeaderView>
#include <QIcon>
#include "CloseUpScroller.hpp"
#include "base/ColorComboBox.hpp"

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/AntDisplayBridge.hpp>
#include <fort/studio/bridge/AntShapeBridge.hpp>
#include <fort/studio/bridge/AntMeasurementBridge.hpp>
#include <fort/studio/bridge/TagCloseUpBridge.hpp>
#include <fort/studio/bridge/AntGlobalModel.hpp>

#include <QDebug>
#include <QSortFilterProxyModel>


void AntListWidget::setUpUI() {
	if (objectName().isEmpty()) {
		setObjectName(QString::fromUtf8("AntListWidget"));
	}

	d_verticalLayout = new QVBoxLayout(this);
	d_verticalLayout->setObjectName("verticalLayout");
	d_antLabel = new QLabel(tr("Number of ants: %1").arg(0),this);
	d_antLabel->setObjectName("antLabel");

	d_verticalLayout->addWidget(d_antLabel);

	d_horizontalLayoutFilter = new QHBoxLayout();
	d_horizontalLayoutFilter->setObjectName("horizontalLayoutFilter");

	d_filterLabel = new QLabel(tr("Filter:"),this);
	d_filterLabel->setObjectName("filterLabel");
	d_horizontalLayoutFilter->addWidget(d_filterLabel);

	d_filterEdit = new QLineEdit(this);
	d_filterEdit->setObjectName("filterEdit");
	d_horizontalLayoutFilter->addWidget(d_filterEdit);

	d_verticalLayout->addLayout(d_horizontalLayoutFilter);

	d_tableView = new QTableView(this);
	d_tableView->setObjectName("tableView");
	d_tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	d_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	d_tableView->setSortingEnabled(true);
	d_tableView->verticalHeader()->setVisible(false);

	d_verticalLayout->addWidget(d_tableView);
}

AntListWidget::AntListWidget(QWidget * parent)
	: QWidget(parent)
	, d_experiment(nullptr)
	, d_sortedModel(new QSortFilterProxyModel(this))
	, d_selectedAnt(0) {


	setUpUI();

	updateNumber();

	d_filterEdit->setEnabled(false);

	d_tableView->setModel(d_sortedModel);
	auto header = d_tableView->horizontalHeader();
	header->setSectionResizeMode(QHeaderView::ResizeToContents);


	d_sortedModel->setFilterKeyColumn(0);
	connect(d_filterEdit,
	        &QLineEdit::textChanged,
	        d_sortedModel,
	        static_cast<void (QSortFilterProxyModel::*)(const QString &)>(&QSortFilterProxyModel::setFilterRegExp));


	connect(this,&AntListWidget::antSelected,
	        this,[this](quint32 antID ) { qWarning() << "Ant sleected: " << antID ;});

}

AntListWidget::~AntListWidget() {
}

quint32 AntListWidget::selectedAntID() const {
	return d_selectedAnt;
}


void AntListWidget::initialize(ExperimentBridge * experiment) {
	d_experiment = experiment;
	updateNumber();
	d_filterEdit->clear();
	d_sortedModel->setSourceModel(sourceModel());

	connect(d_tableView->selectionModel(),&QItemSelectionModel::selectionChanged,
	        this,&AntListWidget::onSelectionChanged);

	connect(d_experiment,
	        &Bridge::activated,
	        d_filterEdit,
	        &QLineEdit::setEnabled);

	connect(d_experiment,
	        &ExperimentBridge::antCreated,
	        this,
	        &AntDisplayListWidget::updateNumber);

	connect(d_experiment,
	        &ExperimentBridge::antDeleted,
	        this,
	        &AntDisplayListWidget::updateNumber);
	connect(d_tableView->selectionModel(),
	        &QItemSelectionModel::selectionChanged,
	        [this]() {
		        if ( d_tableView->selectionModel()->hasSelection() == false ) {
			        selectAnt(0);
		        }
	        });


	initializeChild(experiment);
}



void AntListWidget::updateNumber() {
	size_t n = 0;
	auto sourceModel = d_sortedModel->sourceModel();
	if ( sourceModel != nullptr ) {
		n = sourceModel->rowCount();
	}
	d_antLabel->setText(tr("Number of ants: %1").arg(n));
}

void AntListWidget::selectAnt(quint32 antID) {
	if ( d_selectedAnt == antID ) {
		return;
	}
	d_selectedAnt = antID;

	emit antSelected(antID);

	auto selectionModel =  d_tableView->selectionModel();
	auto selectedRows = selectionModel->selectedRows();
	if ( selectedRows.size() == 1
	     && sourceModel()->antIDFromIndex(d_sortedModel->mapToSource(selectedRows[0])) == d_selectedAnt ) {
		return;
	}
	selectionModel->clearSelection();

	auto item = sourceModel()->itemFromAntID(antID);
	if ( item == nullptr ) {
		return;
	}
	auto index = d_sortedModel->mapFromSource(item->index());
	selectionModel->select(index,QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void AntSimpleListWidget::setUpUI() {
	setMaximumSize(QSize(250,16777215));

	d_tableView->setMinimumSize(QSize(180,0));
	d_tableView->setMaximumSize(QSize(300,16777215));

	d_actionsLayout = new QHBoxLayout();
	auto spacer = new QSpacerItem(40,20,QSizePolicy::Expanding, QSizePolicy::Minimum);
	d_actionsLayout->addItem(spacer);

	d_addButton = new QToolButton(this);
	d_addButton->setObjectName("addButton");
	d_addButton->setIcon(QIcon::fromTheme("list-add"));
	d_actionsLayout->addWidget(d_addButton);

	d_deleteButton = new QToolButton(this);
	d_deleteButton->setObjectName("deleteButton");
	d_deleteButton->setIcon(QIcon::fromTheme("list-remove"));
	d_actionsLayout->addWidget(d_deleteButton);

	d_verticalLayout->insertLayout(d_verticalLayout->count()-1,d_actionsLayout);

}

void AntListWidget::onSelectionChanged() {
	auto selection = d_tableView->selectionModel();
	if ( selection->hasSelection() == false ) {
		selectAnt(0);
		return;
	}

	auto rows = selection->selectedRows();
	if ( rows.size() != 1 ) {
		return;
	}

	auto antID = sourceModel()->antIDFromIndex(d_sortedModel->mapToSource(rows[0]));
	selectAnt(antID);
}

AntSimpleListWidget::AntSimpleListWidget(QWidget * parent)
	: AntListWidget(parent) {

	setUpUI();

	d_addButton->setEnabled(false);
	d_deleteButton->setEnabled(false);

	connect(d_addButton,&QAbstractButton::clicked,
	        this,&AntSimpleListWidget::onAddButtonClicked);

	connect(d_deleteButton,&QAbstractButton::clicked,
	        this,&AntSimpleListWidget::onDeleteButtonClicked);

}

AntSimpleListWidget::~AntSimpleListWidget() {
}

AntGlobalModel * AntSimpleListWidget::sourceModel() {
	return dynamic_cast<AntGlobalModel*>(d_experiment->antDisplay()->model());
}

void AntSimpleListWidget::initializeChild(ExperimentBridge * experiment) {
	connect(d_experiment,
	        &Bridge::activated,
	        d_addButton,
	        &QToolButton::setEnabled);

	connect(experiment->antDisplay(),
	        &Bridge::activated,
	        this,
	        [this] () {
		        d_tableView->horizontalHeader()->setSortIndicatorShown(true);
		        d_tableView->sortByColumn(0,Qt::AscendingOrder);
		        d_tableView->setColumnHidden(1,true);
		        d_tableView->setColumnHidden(2,true);
	        });

	connect(d_tableView->selectionModel(),
	        &QItemSelectionModel::selectionChanged,
	        this,
	        &AntSimpleListWidget::onSelectionChanged);
}

void AntSimpleListWidget::onSelectionChanged() {
	const auto selection  = d_tableView->selectionModel();
	d_deleteButton->setEnabled(selection->selectedRows().size() != 0);
}

void AntSimpleListWidget::onAddButtonClicked() {
	d_experiment->createAnt();
	auto header = d_tableView->horizontalHeader();
	header->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void AntSimpleListWidget::onDeleteButtonClicked() {
	auto rows = d_tableView->selectionModel()->selectedRows();

	std::set<fm::Ant::ID> toDelete;
	for ( const auto & r : rows ) {
		auto antID = sourceModel()->antIDFromIndex(d_sortedModel->mapToSource(r));
		toDelete.insert(antID);
	}

	for ( const auto & antID : toDelete ) {
		d_experiment->deleteAnt(antID);
	}
}


void AntDisplayListWidget::setUpUI() {
	setMaximumSize(QSize(250,16777215));

	d_tableView->setMinimumSize(QSize(180,0));
	d_tableView->setMaximumSize(QSize(300,16777215));

	d_colorLayout = new QHBoxLayout();

	d_colorLabel = new QLabel(tr("Display Color:"),this);
	d_colorLabel->setObjectName("colorLabel");
	d_colorLayout->addWidget(d_colorLabel);

	d_colorBox = new ColorComboBox(this);
	d_colorBox->setObjectName("colorBox");
	d_colorLayout->addWidget(d_colorBox);

	d_verticalLayout->insertLayout(d_verticalLayout->count()-1,d_colorLayout);

	d_actionsLayout = new QHBoxLayout();

	d_showAllButton = new QPushButton(tr("Show All"),this);
	d_showAllButton->setObjectName("showAllButton");
	d_actionsLayout->addWidget(d_showAllButton);

	d_unsoloAllButton = new QPushButton(tr("Unsolo All"),this);
	d_unsoloAllButton->setObjectName("unsoloAllButton");
	d_actionsLayout->addWidget(d_unsoloAllButton);

	d_verticalLayout->insertLayout(d_verticalLayout->count()-1,d_actionsLayout);

}

AntDisplayListWidget::AntDisplayListWidget(QWidget *parent)
	: AntListWidget(parent) {
	setUpUI();

	d_colorBox->setEnabled(false);
	d_colorBox->setCurrentIndex(-1);
	d_showAllButton->setEnabled(false);
	d_unsoloAllButton->setEnabled(false);
}

AntDisplayListWidget::~AntDisplayListWidget() {
}


AntGlobalModel * AntDisplayListWidget::sourceModel() {
	return dynamic_cast<AntGlobalModel*>(d_experiment->antDisplay()->model());
}

void AntDisplayListWidget::initializeChild(ExperimentBridge * experiment) {
	auto antDisplay = experiment->antDisplay();

	connect(experiment->antDisplay(),
	        &Bridge::activated,
	        this,
	        [this] () {
		        d_tableView->horizontalHeader()->setSortIndicatorShown(true);
		        d_tableView->sortByColumn(0,Qt::AscendingOrder);
		        d_experiment->antDisplay()->showAll();
	        });

	connect(d_showAllButton,
	        &QPushButton::clicked,
	        antDisplay,
	        &AntDisplayBridge::showAll);

	connect(d_unsoloAllButton,
	        &QPushButton::clicked,
	        antDisplay,
	        &AntDisplayBridge::unsoloAll);

	connect(antDisplay,
	        &AntDisplayBridge::numberHiddenAntChanged,
	        this,
	        &AntDisplayListWidget::updateShowAll);

	connect(antDisplay,
	        &AntDisplayBridge::numberSoloAntChanged,
	        this,
	        &AntDisplayListWidget::updateShowAll);

	connect(antDisplay,
	        &AntDisplayBridge::numberSoloAntChanged,
	        this,
	        &AntDisplayListWidget::updateUnsoloAll);

	connect(d_tableView->selectionModel(),
	        &QItemSelectionModel::selectionChanged,
	        this,
	        [this]() {
		        const auto selection  = d_tableView->selectionModel();
		        if ( selection->hasSelection() == false ) {
			        d_colorBox->setCurrentIndex(-1);
			        d_colorBox->setEnabled(false);
			        return;
		        }
		        d_colorBox->setCurrentIndex(-1);
		        d_colorBox->setEnabled(true);
	        });

	connect(d_colorBox,&ColorComboBox::colorChanged,
	        this,&AntDisplayListWidget::onColorBoxColorChanged);

	updateShowAll();
	updateUnsoloAll();
}


void AntDisplayListWidget::onColorBoxColorChanged(const QColor & color) {
	if ( color.isValid() == false ) {
		return;
	}

	auto rows = d_tableView->selectionModel()->selectedRows();

	if (rows.isEmpty() == true ) {
		return;
	}

	for ( const auto & index : rows ) {
		d_experiment->antDisplay()->setAntDisplayColor(d_sortedModel->mapToSource(index),
		                                               color);
	}
}


void AntDisplayListWidget::updateShowAll() {
	bool enabled = d_experiment != nullptr && (d_experiment->antDisplay()->numberSoloAnt() > 0
	                                        || d_experiment->antDisplay()->numberHiddenAnt() > 0);
	d_showAllButton->setEnabled(enabled);
}

void AntDisplayListWidget::updateUnsoloAll() {
	bool enabled = d_experiment != nullptr && d_experiment->antDisplay()->numberSoloAnt() > 0;
	d_unsoloAllButton->setEnabled(enabled);
}

void AntCloseUpExplorer::setUpUI() {
	setMaximumSize(QSize(500,16777215));

	d_tableView->setMinimumSize(QSize(360,0));
	d_tableView->setMaximumSize(QSize(550,16777215));

	d_closeUpScroller->setObjectName("closeUpScroller");
	d_verticalLayout->insertWidget(d_verticalLayout->count()-1,d_closeUpScroller);
}

AntCloseUpExplorer::AntCloseUpExplorer(QWidget *parent)
	: AntListWidget(parent)
	, d_closeUpScroller(new AntCloseUpScroller(this)) {

	setUpUI();

	connect(d_closeUpScroller,
	        &CloseUpScroller::currentCloseUpChanged,
	        this,
	        [this](const fmp::TagCloseUp::ConstPtr & closeUp) {
		        emit currentCloseUpChanged(closeUp);
	        });

	connect(this,
	        &AntListWidget::antSelected,
	        this,
	        &AntCloseUpExplorer::onAntSelected);
}

AntCloseUpExplorer::~AntCloseUpExplorer() {
}

void AntCloseUpExplorer::nextAnt() {
	moveIndex(1);
}
void AntCloseUpExplorer::previousAnt() {
	moveIndex(-1);
}

void AntCloseUpExplorer::nextAntCloseUp() {
	d_closeUpScroller->next();
}

void AntCloseUpExplorer::previousAntCloseUp() {
	d_closeUpScroller->previous();
}

void AntCloseUpExplorer::initializeChild(ExperimentBridge * experiment) {
	d_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	connect(experiment,
	        &Bridge::activated,
	        this,
	        [this]() {
		        d_tableView->horizontalHeader()->setSortIndicatorShown(true);
		        d_tableView->sortByColumn(0,Qt::AscendingOrder);
	        });
	d_closeUpScroller->setUp(experiment->tagCloseUps());
}


void AntCloseUpExplorer::moveIndex(int direction) {
	int row = -1;
	auto selectedRows = d_tableView->selectionModel()->selectedRows();
	if ( selectedRows.size() == 1 ) {
		row = selectedRows[0].row();
	}

	if ( row == -1 ) {
		row = direction > 0 ? 0 : d_sortedModel->rowCount() - 1;
	} else {
		row += direction;
	}
	if ( row < 0 || row >= d_sortedModel->rowCount() ) {
		return;
	}
	auto index  = d_sortedModel->index(row,0);
	auto antID = sourceModel()->antIDFromIndex(d_sortedModel->mapToSource(index));
	selectAnt(antID);
	d_tableView->scrollTo(index);
}

void AntCloseUpExplorer::onAntSelected(quint32 antID) {
	auto closeUps = d_experiment->tagCloseUps()->closeUpsForAnt(antID);
	if ( closeUps.isEmpty() == true ) {
		d_closeUpScroller->setCloseUps(-1,closeUps,closeUps.end());
		return;
	}
	d_closeUpScroller->setCloseUps(antID,closeUps,closeUps.begin());
}

AntMeasurementListWidget::AntMeasurementListWidget(QWidget * parent)
	: AntCloseUpExplorer(parent) {
}

AntMeasurementListWidget::~AntMeasurementListWidget() {
}

AntGlobalModel * AntMeasurementListWidget::sourceModel() {
	return dynamic_cast<AntGlobalModel*>(d_experiment->antMeasurements()->model());
}

AntShapeListWidget::AntShapeListWidget(QWidget * parent)
	: AntCloseUpExplorer(parent) {
}

AntShapeListWidget::~AntShapeListWidget() {
}

AntGlobalModel * AntShapeListWidget::sourceModel() {
	return dynamic_cast<AntGlobalModel*>(d_experiment->antShapes()->model());
}
