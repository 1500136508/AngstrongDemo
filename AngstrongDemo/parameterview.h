#ifndef PARAMETERVIEW_H
#define PARAMETERVIEW_H

#include <QDialog>

namespace Ui {
class ParameterView;
}

class ParameterView : public QDialog
{
    Q_OBJECT

public:
    explicit ParameterView(QWidget *parent = nullptr);
    ~ParameterView();

private:
    Ui::ParameterView *ui;
};

#endif // PARAMETERVIEW_H
