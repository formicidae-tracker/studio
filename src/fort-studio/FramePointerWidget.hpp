#pragma once

#include <QWidget>

namespace Ui {
class FramePointerWidget;
}

class FramePointerWidget : public QWidget {
    Q_OBJECT
public:
    explicit FramePointerWidget(QWidget *parent = 0);
    ~FramePointerWidget();

private:
    Ui::FramePointerWidget *d_ui;
};
