#ifndef SETUPVIEW_H
#define SETUPVIEW_H

#include <QDialog>
#include "ConfigInstance.h"

namespace Ui {
class SetupView;
}

class SetupView : public QDialog
{
    Q_OBJECT

public:
    explicit SetupView(QWidget *parent = nullptr);
    ~SetupView();

	void BuildConnect();
	void InitParameter();
private slots:
	void on_save_parameter_clicked();
private:
    Ui::SetupView *ui;

	QString stylesheet;							// QSSÑùÊ½±í
};

#endif // SETUPVIEW_H
