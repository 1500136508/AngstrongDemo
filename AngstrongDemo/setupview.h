#ifndef SETUPVIEW_H
#define SETUPVIEW_H

#include <QDialog>

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
	void on_ok_clicked();
private:
	void Save();

    Ui::SetupView *ui;

	QString stylesheet;							// QSSÑùÊ½±í
};

#endif // SETUPVIEW_H
