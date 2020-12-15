#include "titlebar.h"
#include "ui_titlebar.h"
#include <qt_windows.h>
#include <QMouseEvent>

TitleBar::TitleBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TitleBar)
{
    ui->setupUi(this);
	BuildConnect();
}

TitleBar::~TitleBar()
{
    delete ui;
}

void TitleBar::BuildConnect()
{
	connect(ui->m_btn_min, SIGNAL(clicked()), this, SLOT(on_TiBar_Clicked()));
	connect(ui->m_btn_max, SIGNAL(clicked()), this, SLOT(on_TiBar_Clicked()));
	connect(ui->m_btn_close, SIGNAL(clicked()), this, SLOT(on_TiBar_Clicked()));
}

void TitleBar::on_TiBar_Clicked()
{
	QPushButton *pBtn = qobject_cast<QPushButton *>(sender());//��������źż���Ĳ��е��ã��򷵻�ָ�����źŵĶ����ָ��; ����������0.
	/*  ���ش˴���С�����Ĵ��ڣ������У�����ܾ��У�����ϵͳ��ܵ���һ�����ȴ���С������ �������С�����Ǵ��ڣ��򷵻ش���С��������*/
	QWidget *pWindow = this->window();
	if (pWindow->isWindow())//�������С�����Ƕ������ڣ��򷵻�true�����򷵻�false��
	{
		if (pBtn == ui->m_btn_min)
		{
			pWindow->showMinimized();//��С������С��������Ϊͼ�ꡣ���ô˺�����Ӱ�촰�ڡ�
		}
		else if (pBtn == ui->m_btn_max)
		{
			pWindow->isMaximized() ? pWindow->showNormal() : pWindow->showMaximized();//�Ŵ���С
		}
		else if (pBtn == ui->m_btn_close)
		{
			pWindow->close();//�رմ���
		}
	}
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent * event)
{
	Q_UNUSED(event);
	emit ui->m_btn_max->clicked();
}

void TitleBar::mousePressEvent(QMouseEvent * event)
{
	if (ReleaseCapture())
	{
		QWidget *pWindow = this->window();
		if (pWindow->isWindow())
		{
			SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
		}
	}
	event->ignore();
}

bool TitleBar::eventFilter(QObject * obj, QEvent * event)
{
	switch (event->type())
	{
	case QEvent::WindowTitleChange:
	{
		QWidget *pWidget = qobject_cast<QWidget *>(obj);
		if (pWidget)
		{
			//ui->m_lab_title->setText(pWidget->windowTitle());
			return true;
		}
	}
	case QEvent::WindowIconChange:
	{
		QWidget *pWidget = qobject_cast<QWidget *>(obj);
		if (pWidget)
		{
			QIcon icon = pWidget->windowIcon();
			ui->m_lab_icon->setPixmap(icon.pixmap(ui->m_lab_icon->size()));
			return true;
		}
	}
	case QEvent::WindowStateChange:
	case QEvent::Resize:
		//updateMaximize();
		return true;
	default:
		return false;
	}
	return QWidget::eventFilter(obj, event);
}

void TitleBar::updateMaximize()
{
	QWidget *pWindow = this->window();
	if (pWindow->isTopLevel())
	{
		bool bMaximize = pWindow->isMaximized();
		if (bMaximize)
		{
			ui->m_btn_max->setToolTip(tr("Restore"));
			ui->m_btn_max->setProperty("maximizeProperty", "restore");
		}
		else
		{
			ui->m_btn_min->setProperty("maximizeProperty", "maximize");
			ui->m_btn_min->setToolTip(tr("Maximize"));
		}
		ui->m_btn_max->setStyle(QApplication::style());
	}
}
