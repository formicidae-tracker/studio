#pragma once

#include <QWidget>

#include <fort/studio/MyrmidonTypes/TagCloseUp.hpp>

class TagCloseUpBridge;
class QLabel;
class QAbstractSlider;

class CloseUpExplorer : public QWidget {
	Q_OBJECT
	Q_PROPERTY(fmp::TagCloseUp::ConstPtr currentCloseUp
	           READ currentCloseUp
	           NOTIFY currentCloseUpChanged);
public:
	explicit CloseUpExplorer(QWidget * parent = nullptr);
	virtual ~CloseUpExplorer();

	virtual void setUp(TagCloseUpBridge * bridge) = 0;

	const fmp::TagCloseUp::ConstPtr & currentCloseUp() const;

public slots:
	void setCloseUps(uint32_t objectID, const QVector<fmp::TagCloseUp::ConstPtr> & closeUps);

	void next();
	void previous();

protected slots:
	void onCloseUpsChanged(uint32_t objectID,const QVector<fmp::TagCloseUp::ConstPtr> & closeUps);

private slots:
	void onSliderValueChanged(int position);

signals:
	void currentCloseUpChanged(const fmp::TagCloseUp::ConstPtr & closeUp);



private:
	typedef QVector<fmp::TagCloseUp::ConstPtr> CloseUpList;

	void updateWidgets();

	CloseUpList                  d_closeUps;
	CloseUpList::const_iterator  d_currentCloseUp;
	uint32_t                     d_currentID;

	QLabel          * d_label;
	QAbstractSlider * d_slider;

};


class AntCloseUpExplorer : public CloseUpExplorer {
public:
	explicit AntCloseUpExplorer(QWidget * parent = nullptr);
	virtual ~AntCloseUpExplorer();


	void setUp(TagCloseUpBridge * bridge) override;
};

class TagCloseUpExplorer : public CloseUpExplorer {
public:
	explicit TagCloseUpExplorer(QWidget * parent = nullptr);
	virtual ~TagCloseUpExplorer();


	void setUp(TagCloseUpBridge * bridge) override;
};
