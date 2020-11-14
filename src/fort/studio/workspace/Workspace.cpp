#include "Workspace.hpp"

Workspace::Workspace(bool showAntSelector,
                     QWidget * parent)
	: QWidget(parent)
	, d_showAntSelector(showAntSelector) {
}

Workspace::~Workspace() {
}


bool Workspace::showAntSelector() const {
	return d_showAntSelector;
}
