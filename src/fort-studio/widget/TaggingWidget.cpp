#include "TaggingWidget.hpp"
#include "ui_TaggingWidget.h"

#include <QSortFilterProxyModel>

#include <fort-studio/bridge/GlobalPropertyBridge.hpp>
#include <fort-studio/bridge/MeasurementBridge.hpp>
#include <fort-studio/bridge/IdentifierBridge.hpp>

using namespace fort::myrmidon::priv;


TaggingWidget::TaggingWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::TaggingWidget)
	, d_sortedModel ( new QSortFilterProxyModel(this) ) {
	d_sortedModel->setSortRole(Qt::UserRole+2);
    d_ui->setupUi(this);
    d_ui->treeView->setModel(d_sortedModel);
    d_ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
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
