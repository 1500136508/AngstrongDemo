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
	void on_TiBar_Clicked();//右上角三个按钮共用一个槽函数
protected:
	//重写鼠标事件
	virtual void mouseDoubleClickEvent(QMouseEvent *event);// 双击标题栏进行界面的最大化/还原
	virtual void mousePressEvent(QMouseEvent *event);// 进行鼠界面的拖动
	virtual bool eventFilter(QObject *obj, QEvent *event);// 设置界面标题与图标

	// 标题栏跑马灯效果时钟;
	//QTimer m_titleRollTimer;
private:
	void updateMaximize();// 最大化/还原
private:
    Ui::TitleBar *ui;
};

#endif // TITLEBAR_H
