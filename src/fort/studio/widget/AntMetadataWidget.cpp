#include "AntMetadataWidget.hpp"
#include "ui_AntMetadataWidget.h"

#include <fort/studio/bridge/AntMetadataBridge.hpp>

#include <QStyledItemDelegate>
#include <QDebug>


class MetadataTypeDelegate : public QStyledItemDelegate {
public:
	MetadataTypeDelegate(AntMetadataWidget * parent)
		: QStyledItemDelegate(parent)
		, d_widget(parent) {
	};

protected:

	QWidget * createEditor( QWidget *parent,
	                        const QStyleOptionViewItem &option,
	                        const QModelIndex &index ) const override {
		auto res = new QComboBox(parent);
		d_widget->buildTypeCombo(res);
		return res;
	}

	void setEditorData ( QWidget *editor, const QModelIndex &index ) const override {
		auto column = index.data(Qt::UserRole+1).value<fmp::AntMetadata::Column::Ptr>();
		auto combo = qobject_cast<QComboBox*>(editor);
		if ( combo == nullptr || !column == true ) {
			return;
		}

		combo->setCurrentIndex(int(column->MetadataType()));
	}

	void setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const override {
		auto combo = qobject_cast<QComboBox*>(editor);
		if ( combo == nullptr ) {
			return;
		}
		model->setData(index,combo->currentData(Qt::UserRole+1),Qt::UserRole+2);
	}
private :
	AntMetadataWidget * d_widget;
};

AntMetadataWidget::AntMetadataWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::AntMetadataWidget)
	, d_metadata(nullptr) {
	d_ui->setupUi(this);
	d_ui->addButton->setEnabled(false);
	d_ui->removeButton->setEnabled(false);
	d_ui->comboBox->setEnabled(false);
}

void AntMetadataWidget::setup(AntMetadataBridge * metadata) {
	d_metadata = metadata;
	d_ui->tableView->setModel(d_metadata->columnModel());
	connect(d_metadata,
	        &AntMetadataBridge::activated,
	        d_ui->addButton,
	        &QToolButton::setEnabled);

	connect(d_metadata,
	        &AntMetadataBridge::activated,
	        d_ui->comboBox,
	        &QComboBox::setEnabled);
	auto sModel = d_ui->tableView->selectionModel();

	connect(sModel,
	        &QItemSelectionModel::selectionChanged,
	        [this]() {
		        d_ui->removeButton->setEnabled(d_ui->tableView->selectionModel()->selectedRows().size() == 1);
	        });

	d_ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	d_ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

	auto hHeader = d_ui->tableView->horizontalHeader();
	hHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
	d_ui->tableView->setItemDelegateForColumn(1,new MetadataTypeDelegate(this));
	buildTypeCombo(d_ui->comboBox);
}

AntMetadataWidget::~AntMetadataWidget() {
	delete d_ui;
}

void AntMetadataWidget::buildTypeCombo(QComboBox *  combo) {
	combo->setModel(d_metadata->typeModel());
}


void AntMetadataWidget::on_addButton_clicked() {
	auto newName = tr("Column %1").arg(d_ui->tableView->model()->rowCount()+1);
	d_metadata->addMetadataColumn(newName,d_ui->comboBox->currentData(Qt::UserRole+1).toInt());
	d_ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}


void AntMetadataWidget::on_removeButton_clicked() {
	auto rows = d_ui->tableView->selectionModel()->selectedRows();
	if ( rows.size() != 1 ) {
		qDebug() << "[AntMetadataWidget]: Invalid selection " << rows.size();
		return;
	}

	d_metadata->removeMetadataColumn(rows[0].data(Qt::DisplayRole).toString());
}
