#pragma once

#include <QWidget>

#include <fort/studio/MyrmidonTypes/TagCloseUp.hpp>

class ExperimentBridge;
class AntGlobalModel;
class QSortFilterProxyModel;

class QVBoxLayout;
class QHBoxLayout;
class QTableView;
class QLabel;
class ColorComboBox;
class QLineEdit;
class QPushButton;
class QToolButton;
class AntCloseUpScroller;


class AntListWidget : public QWidget {
	Q_OBJECT
	Q_PROPERTY(quint32 selectedAntID
	           WRITE selectAnt
	           READ selectedAntID
	           NOTIFY antSelected)
public:
	explicit AntListWidget(QWidget * widget = nullptr);
	virtual ~AntListWidget();

	quint32 selectedAntID() const;

	void initialize(ExperimentBridge * experiment);

public slots:
	void selectAnt(quint32 antID);

protected slots:
	void onSelectionChanged();

private slots:
	void updateNumber();

signals:
	void antSelected(quint32 antID);

protected:
	virtual AntGlobalModel * sourceModel() = 0;
	virtual void initializeChild(ExperimentBridge * experiment) = 0;


	QVBoxLayout * d_verticalLayout;
	QLabel      * d_antLabel;
	QHBoxLayout * d_horizontalLayoutFilter;
	QLabel      * d_filterLabel;
	QLineEdit   * d_filterEdit;
    QTableView  * d_tableView;

	ExperimentBridge  * d_experiment;

	QSortFilterProxyModel * d_sortedModel;
	quint32                 d_selectedAnt;
private :
	void setUpUI();
};


class AntSimpleListWidget : public AntListWidget {
	Q_OBJECT
public:
	explicit AntSimpleListWidget(QWidget * parent);
	virtual ~AntSimpleListWidget();

protected:
	AntGlobalModel * sourceModel() override;
	void initializeChild(ExperimentBridge * experiment) override;

private slots:
	void onSelectionChanged();
	void onAddButtonClicked();
	void onDeleteButtonClicked();
private:
	void setUpUI();

	QHBoxLayout * d_actionsLayout;
	QToolButton * d_addButton;
	QToolButton * d_deleteButton;
};


class AntDisplayListWidget : public AntListWidget {
    Q_OBJECT
public:
	explicit AntDisplayListWidget(QWidget *parent = nullptr);
    virtual ~AntDisplayListWidget();


protected:
	AntGlobalModel * sourceModel() override;
	void initializeChild(ExperimentBridge * experiment) override;

private slots:
	void onColorBoxColorChanged(const QColor & color);

	void updateShowAll();
	void updateUnsoloAll();
private:
	void setUpUI();

	QHBoxLayout   * d_colorLayout;
	QLabel        * d_colorLabel;
	ColorComboBox * d_colorBox;
	QHBoxLayout   * d_actionsLayout;
	QPushButton   * d_showAllButton;
	QPushButton   * d_unsoloAllButton;
};



class AntCloseUpExplorer : public AntListWidget {
	Q_OBJECT
public:
	AntCloseUpExplorer(QWidget *parent = nullptr);
    virtual ~AntCloseUpExplorer();

public slots:
	void nextAnt();
	void previousAnt();
	void nextAntCloseUp();
	void previousAntCloseUp();

signals:
	void currentCloseUpChanged(const fmp::TagCloseUp::ConstPtr &);

protected:
	void initializeChild(ExperimentBridge * experiment) override;

private slots:
	void onAntSelected(quint32 antID);
private :
	void moveIndex(int increment);
	void setUpUI();

	AntCloseUpScroller * d_closeUpScroller;
};


class AntMeasurementListWidget : public AntCloseUpExplorer {
public:
	explicit AntMeasurementListWidget(QWidget * parent = nullptr);
    virtual ~AntMeasurementListWidget();
protected:
	AntGlobalModel * sourceModel() override;
};


class AntShapeListWidget : public AntCloseUpExplorer {
public:
	explicit AntShapeListWidget(QWidget * parent = nullptr);
    virtual ~AntShapeListWidget();

protected:
	AntGlobalModel * sourceModel() override;

};
