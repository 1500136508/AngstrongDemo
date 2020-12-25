#ifndef XMVIEW_H
#define XMVIEW_H

#include <QDialog>
#include <mutex>

namespace Ui {
class XMView;
}

class XMView : public QDialog
{
    Q_OBJECT

public:
    explicit XMView(QWidget *parent = nullptr);
    ~XMView();
private slots:
	void on_choose_clicked();
	void on_upload_cliecked();
	void on_write_command_clicked();
	void on_read_command_clicked();
	void ReceiveSN(QString sn);
private:
	void BuildConnect();
	void CreateHalerThread();
private:
    Ui::XMView *ui;
	std::string log_msg_;
	std::mutex mutex_;
	bool quite_program = false;
};

#endif // XMVIEW_H
