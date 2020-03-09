#include "TaggingWidget.hpp"
#include "ui_TaggingWidget.h"


#include <QSortFilterProxyModel>

#include <fort-studio/bridge/GlobalPropertyBridge.hpp>
#include <fort-studio/bridge/MeasurementBridge.hpp>
#include <fort-studio/bridge/IdentifierBridge.hpp>

#include <fort-studio/Format.hpp>
#include <fort-studio/widget/vectorgraphics/VectorialScene.hpp>



TaggingWidget::TaggingWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::TaggingWidget)
	, d_sortedModel ( new QSortFilterProxyModel(this) )
	, d_measurements(nullptr)
	, d_vectorialScene(new VectorialScene) {
	d_sortedModel->setSortRole(Qt::UserRole+2);
    d_ui->setupUi(this);
    d_ui->treeView->setModel(d_sortedModel);
    d_ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    d_ui->vectorialView->setScene(d_vectorialScene);
    connect(d_ui->vectorialView,
            &VectorialView::zoomed,
            d_vectorialScene,
            &VectorialScene::onZoomed);
}

TaggingWidget::~TaggingWidget() {
    delete d_ui;
}


void TaggingWidget::setup(GlobalPropertyBridge * globalProperties,
                          MeasurementBridge * measurements,
                          IdentifierBridge * identifier) {
	connect(globalProperties,
	        &GlobalPropertyBridge::activated,
	        d_ui->familySelector,
	        &TagFamilyComboBox::setEnabled);

	connect(globalProperties,
	        &GlobalPropertyBridge::tagFamilyChanged,
	        d_ui->familySelector,
	        &TagFamilyComboBox::setFamily);

	connect(d_ui->familySelector,
	        &TagFamilyComboBox::familyModified,
	        globalProperties,
	        &GlobalPropertyBridge::setTagFamily);


	connect(globalProperties,
	        &GlobalPropertyBridge::activated,
	        d_ui->thresholdBox,
	        &QSpinBox::setEnabled);

	connect(globalProperties,
	        &GlobalPropertyBridge::thresholdChanged,
	        d_ui->thresholdBox,
	        &QSpinBox::setValue);

	connect(d_ui->thresholdBox,
	        &QSpinBox::editingFinished,
	        [globalProperties,this] () {
		        globalProperties->setThreshold(d_ui->thresholdBox->value());
	        });

	connect(d_sortedModel,
	        &QAbstractItemModel::rowsInserted,
	        [this](const QModelIndex & parent, int first, int last) {
		        if ( parent.isValid() == true ) {
			        return;
		        }
		        for ( int i = first; i <= last; ++i) {
			        d_ui->treeView->expand(d_sortedModel->index(i,0,parent));
		        }
	        });


	d_sortedModel->setSourceModel(measurements->tagCloseUpModel());
	d_ui->treeView->sortByColumn(0,Qt::AscendingOrder);
	connect(measurements,
	        &MeasurementBridge::progressChanged,
	        [this](size_t done, size_t toDo) {
		        d_ui->tagCloseUpLoadingProgress->setMaximum(toDo);
		        d_ui->tagCloseUpLoadingProgress->setValue(done);
	        });
	d_measurements = measurements;

}


void TaggingWidget::on_addIdentButton_clicked() {
	qWarning() << "implements me!";
}

void TaggingWidget::on_newAntButton_clicked() {
	qWarning() << "implements me!";
}

void TaggingWidget::on_deletePoseButton_clicked() {
	qWarning() << "implements me!";
}

void TaggingWidget::onIdentificationAntPositionChanged(fmp::IdentificationConstPtr) {
	qWarning() << "implements me!";
}


void TaggingWidget::on_treeView_activated(const QModelIndex & index) {
	if ( index.parent().isValid() == false || d_measurements == nullptr ) {
		return;
	}
	auto tcu = d_measurements->fromTagCloseUpModelIndex(d_sortedModel->mapToSource(index));
	if ( !tcu ) {
		return;
	}
	qInfo() << "Loading " << ToQString(tcu->URI()) << " image " << ToQString(tcu->AbsoluteFilePath());

	d_vectorialScene->setBackgroundPicture(ToQString(tcu->AbsoluteFilePath().string()));
	d_ui->vectorialView->resetZoom();
	auto & tagPosition = tcu->TagPosition();
	d_ui->vectorialView->centerOn(QPointF(tagPosition.x(),tagPosition.y()));
	d_vectorialScene->setStaticPolygon(tcu->Corners(),QColor(255,0,0));
}
