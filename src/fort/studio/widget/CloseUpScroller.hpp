#pragma once

#include <QWidget>

#include <fort/studio/MyrmidonTypes/TagCloseUp.hpp>

class TagCloseUpBridge;
class QLabel;
class QAbstractSlider;

class CloseUpScroller : public QWidget {
	Q_OBJECT
	Q_PROPERTY(fmp::TagCloseUp::ConstPtr currentCloseUp
	           READ currentCloseUp
	           NOTIFY currentCloseUpChanged);
public:
	explicit CloseUpScroller(QWidget * parent = nullptr);
	virtual ~CloseUpScroller();

	virtual void setUp(TagCloseUpBridge * bridge) = 0;

	const fmp::TagCloseUp::ConstPtr & currentCloseUp() const;

	void setCloseUps(uint32_t objectID,
	                 const QVector<fmp::TagCloseUp::ConstPtr> & closeUps,
	                 QVector<fmp::TagCloseUp::ConstPtr>::const_iterator currentCloseUp);

public slots:

	void next();
	void previous();

protected slots:
	void onCloseUpsChanged(uint32_t objectID,const QVector<fmp::TagCloseUp::ConstPtr> & closeUps);

	void clear();

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


class AntCloseUpScroller : public CloseUpScroller {
public:
	explicit AntCloseUpScroller(QWidget * parent = nullptr);
	virtual ~AntCloseUpScroller();


	void setUp(TagCloseUpBridge * bridge) override;
};

class TagCloseUpScroller : public CloseUpScroller {
public:
	explicit TagCloseUpScroller(QWidget * parent = nullptr);
	virtual ~TagCloseUpScroller();


	void setUp(TagCloseUpBridge * bridge) override;
};
