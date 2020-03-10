#include "Bridge.hpp"



Bridge::Bridge(QObject * parent)
	: d_modified(false) {
}

bool Bridge::isModified() const {
	return d_modified;
}

void Bridge::setModified(bool modifiedValue) {
	if ( d_modified == modifiedValue ){
		return;
	}
	d_modified = modifiedValue;
	emit modified(d_modified);
}
