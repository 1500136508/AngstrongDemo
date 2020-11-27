#ifndef DISPLAYVIEW_H
#define DISPLAYVIEW_H

#include <QDialog>

namespace Ui {
class DisplayView;
}

class DisplayView : public QDialog
{
    Q_OBJECT

public:
    explicit DisplayView(QWidget *parent = nullptr);
    ~DisplayView();
private slots:
	void ReceiveLocationDepth(int x, int y, float depth);
	void ReceiveImageGray(int r, int g, int b);
	void ReceiveAvgDepth(float avg0, float avg1);
private:
    Ui::DisplayView *ui;
};

#endif // DISPLAYVIEW_H
