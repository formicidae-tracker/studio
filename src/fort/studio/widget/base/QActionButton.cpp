#include "QActionButton.hpp"

#include <QAction>

QActionButton::QActionButton(QWidget * parent )
	: QPushButton(parent)
	, d_action(nullptr) {
	updateButtonState();
}

QActionButton::~QActionButton() {
}

void QActionButton::setAction(QAction * action) {
	if ( d_action != nullptr ) {
		disconnect(d_action, &QAction::changed,
		           this, &QActionButton::updateButtonState);
		disconnect(this, &QActionButton::clicked,
		           d_action, &QAction::trigger);
	}

	d_action = action;

	updateButtonState();

	if ( d_action != nullptr ) {
		connect(d_action, &QAction::changed,
		        this, &QActionButton::updateButtonState);
		connect(this, &QActionButton::clicked,
		        d_action, &QAction::trigger);
	}
}


void QActionButton::updateButtonState() {
	if ( d_action == nullptr ) {
		auto unconfigured = tr("Non configured action button");
		setText(unconfigured);
		setEnabled(false);
		setStatusTip(unconfigured);
		setToolTip(unconfigured);
		setCheckable(false);
		setChecked(false);
	} else {
		setText(d_action->text());
		setStatusTip(d_action->statusTip());
		setToolTip(d_action->toolTip());
		setIcon(d_action->icon());
		setEnabled(d_action->isEnabled());
		setCheckable(d_action->isCheckable());
		setChecked(d_action->isChecked());
	}
}
