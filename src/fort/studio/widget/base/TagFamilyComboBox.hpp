#pragma once

#include <QComboBox>
#include <fort/studio/MyrmidonTypes.hpp>

class TagFamilyComboBox : public QComboBox {
	Q_OBJECT
	Q_PROPERTY(fort::tags::Family family
	           READ family WRITE setFamily
	           NOTIFY familyModified)
public :

	TagFamilyComboBox(QWidget * parent);
	virtual ~TagFamilyComboBox();

	fort::tags::Family family() const;

signals:
	void familyModified(fort::tags::Family f);
public slots:
	void setFamily(fort::tags::Family f);

private slots:

	void onCurrentIndexChanged(int index);

private:
	fort::tags::Family d_family;
};
