#ifndef XMVIEW_H
#define XMVIEW_H

#include <QDialog>

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
private:
	void BuildConnect();
private:
    Ui::XMView *ui;
};

#endif // XMVIEW_H
