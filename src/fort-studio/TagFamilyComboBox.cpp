#include "TagFamilyComboBox.hpp"



TagFamilyComboBox::TagFamilyComboBox(QWidget * parent)
	: QComboBox(parent)
	, d_family(fort::tags::Family::Undefined) {

	insertItem(0,"36h11",(int)fort::tags::Family::Tag36h11);
    insertItem(1,"36ARTag",(int)fort::tags::Family::Tag36ARTag);
    insertItem(2,"36h10",(int)fort::tags::Family::Tag36h10);
    insertItem(3,"Standard41h12",(int)fort::tags::Family::Standard41h12);
    insertItem(4,"16h5",(int)fort::tags::Family::Tag16h5);
    insertItem(5,"25h9",(int)fort::tags::Family::Tag25h9);
    insertItem(6,"Circle21h7",(int)fort::tags::Family::Circle21h7);
    insertItem(7,"Circle49h12",(int)fort::tags::Family::Circle49h12);
    insertItem(8,"Custom48h12",(int)fort::tags::Family::Custom48h12);
    insertItem(9,"Standard52h13",(int)fort::tags::Family::Standard52h13);
    setCurrentIndex(-1);

    connect(this,
            SIGNAL(QComboBox::currentIndexChanged(int)),
            this,
            SLOT(TagFamilyComboBox::onCurrentIndexChanged(int)));
}

fort::tags::Family TagFamilyComboBox::family() const {
	if ( currentIndex() < 0 || currentIndex() > 9 ) {
		return fort::tags::Family::Undefined;
	}
	return fort::tags::Family(currentData().toInt());
}


void TagFamilyComboBox::setFamily(fort::tags::Family f) {
	int index = findData(int(f));
	if ( f == d_family || index < 0 ) {
		return;
	}
	setCurrentIndex(index);
}


void TagFamilyComboBox::onCurrentIndexChanged(int index) {
	d_family = fort::tags::Family(currentData().toInt());
	emit familyModified(d_family);
}
