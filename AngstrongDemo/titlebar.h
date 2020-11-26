#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>

namespace Ui {
class TitleBar;
}

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);
    ~TitleBar();

	void BuildConnect();
public slots:
	void on_TiBar_Clicked();//���Ͻ�������ť����һ���ۺ���
protected:
	//��д����¼�
	virtual void mouseDoubleClickEvent(QMouseEvent *event);// ˫�����������н�������/��ԭ
	virtual void mousePressEvent(QMouseEvent *event);// �����������϶�
	virtual bool eventFilter(QObject *obj, QEvent *event);// ���ý��������ͼ��

	// �����������Ч��ʱ��;
	//QTimer m_titleRollTimer;
private:
	void updateMaximize();// ���/��ԭ
private:
    Ui::TitleBar *ui;
};

#endif // TITLEBAR_H
