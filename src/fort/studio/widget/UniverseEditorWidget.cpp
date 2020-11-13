#include "UniverseEditorWidget.hpp"
#include "ui_UniverseEditorWidget.h"

#include <fort/studio/bridge/UniverseBridge.hpp>

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

#include "SpaceChoiceDialog.hpp"

#include "TrackingDataDirectoryLoader.hpp"

UniverseEditorWidget::UniverseEditorWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::UniverseEditorWidget) {
	d_ui->setupUi(this);

	d_ui->addButton->setEnabled(false);
	d_ui->deleteButton->setEnabled(false);

	d_ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

}

UniverseEditorWidget::~UniverseEditorWidget() {
	delete d_ui;
}


void UniverseEditorWidget::setup(UniverseBridge * universe) {
	d_universe = universe;
	connect(d_universe,
	        &UniverseBridge::activated,
	        d_ui->addButton,
	        &QToolButton::setEnabled);

	d_ui->treeView->setModel(d_universe->model());

	connect(d_ui->treeView->selectionModel(),
	        &QItemSelectionModel::selectionChanged,
	        this,
	        &UniverseEditorWidget::onSelectionChanged);

	connect(d_universe->model(),
	        &QAbstractItemModel::rowsInserted,
	        [this](const QModelIndex & parent, int first, int last) {
		        if (parent.isValid() == true) {
			        return;
		        }
		        for(; first <= last; ++first) {
			        d_ui->treeView->expand(d_universe->model()->index(first,0,parent));
		        }
	        });

}


void UniverseEditorWidget::on_addButton_clicked() {
	if ( d_universe == NULL ) {
		return;
	}

	auto tddFilePath = QFileDialog::getExistingDirectory(this, tr("Open Tracking Data Directory"),
	                                                     d_universe->basepath(),
	                                                     QFileDialog::ShowDirsOnly);
	fmp::TrackingDataDirectory::Ptr tdd;
	try {
		tdd = fmp::TrackingDataDirectory::Open(tddFilePath.toUtf8().constData(),
		                                       d_universe->basepath().toUtf8().constData());
		TrackingDataDirectoryLoader::EnsureLoaded({tdd},this);
	} catch ( const std::exception & e ) {
		qCritical() << "Could not open TrackingDataDirectory"
		            << tddFilePath << ": " << e.what();
		QMessageBox::warning(this,tr("Data Error"),
		                     tr("Could not open Tracking Data Directory '%1'.\n"
		                        "Error: %2").arg(tddFilePath,e.what()),
		                     QMessageBox::Ok     );
		return;
	}

	auto space = SpaceChoiceDialog::Get(d_universe,this);

	if ( space.isEmpty() ) {
		qDebug() << "[UniverseEditorWidget]: TDD addition aborded by user";
		return;
	}
	// Will log errors, but who cares
	d_universe->addSpace(space);


	d_universe->addTrackingDataDirectoryToSpace(space,tdd);
}

void UniverseEditorWidget::on_deleteButton_clicked() {
	if ( !d_universe ) {
		return;
	}

	auto selected = d_ui->treeView->selectionModel()->selectedIndexes();
	if ( selected.isEmpty() ) {
		return;
	}

	d_universe->deleteSelection(selected);
}


void UniverseEditorWidget::onSelectionChanged(const QItemSelection & selection) {
	if ( selection.isEmpty() ) {
		d_ui->deleteButton->setEnabled(false);
		return;
	}
	const auto & currentSelection  =d_ui->treeView->selectionModel()->selectedIndexes();
	d_ui->deleteButton->setEnabled(d_universe->isDeletable(currentSelection));
}
