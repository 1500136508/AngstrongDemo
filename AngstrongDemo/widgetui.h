#ifndef WIDGETUI_H
#define WIDGETUI_H

#include <QWidget>

namespace Ui {
class WidgetUI;
}

class WidgetUI : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetUI(QWidget *parent = nullptr);
    ~WidgetUI();

private:
    Ui::WidgetUI *ui;
	QString stylesheet;
};

#endif // WIDGETUI_H
