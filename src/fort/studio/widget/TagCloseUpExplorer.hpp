#pragma once

#include <QSortFilterProxyModel>

#include <QWidget>
#include <QRegularExpression>

#include <fort/studio/MyrmidonTypes/TagCloseUp.hpp>
#include <fort/studio/MyrmidonTypes/Identification.hpp>

namespace Ui {
class TagCloseUpExplorer;
}

class TagCloseUpBridge;

class CloseUpFilterModel : public QSortFilterProxyModel {
	Q_OBJECT
public:
	CloseUpFilterModel(QObject * parent = nullptr);
	virtual ~CloseUpFilterModel();

public slots:
	void setFilter(const QString & filter);
	void setRemoveUsed(bool removeUsed);
	void whitelist(fm::TagID tagID);
	void blacklist(fm::TagID tagID);
	void clearWhitelist();
	void clearBlacklist();
protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:

	QRegularExpression  d_filter;
	fm::TagID           d_whitelist;
	std::set<fm::TagID> d_blacklist;
	bool                d_removeUsed;
};

class TagCloseUpExplorer : public QWidget {
	Q_OBJECT
public:
	explicit TagCloseUpExplorer(QWidget *parent = nullptr);
	virtual ~TagCloseUpExplorer();

	void initialize(TagCloseUpBridge * tagCloseUps);

public slots:

	void hideTag(fm::TagID tagID);
	void showAllTags();

	void selectCloseUpForIdentification(const fmp::Identification::ConstPtr & identification);


	void nextTag();
	void nextTagCloseUp();
	void previousTag();
	void previousTagCloseUp();

signals:
	void currentTagIDChanged(int tagID);
	void currentCloseUpChanged(const fmp::TagCloseUp::ConstPtr &);


private slots:
	void on_closeUpView_clicked(const QModelIndex & index);


private:
	void moveIndex(int direction);

	void clearCurrentTag();

	std::pair<int,fm::TagID> currentTag();

	Ui::TagCloseUpExplorer * d_ui;

	TagCloseUpBridge   * d_tagCloseUps;
	CloseUpFilterModel * d_sortedFilteredModel;

};
