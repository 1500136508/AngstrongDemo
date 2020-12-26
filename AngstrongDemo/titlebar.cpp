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
	QPushButton *pBtn = qobject_cast<QPushButton *>(sender());//如果在由信号激活的槽中调用，则返回指向发送信号的对象的指针; 否则它返回0.
	/*  返回此窗口小部件的窗口，即具有（或可能具有）窗口系统框架的下一个祖先窗口小部件。 如果窗口小部件是窗口，则返回窗口小部件本身。*/
	QWidget *pWindow = this->window();
	if (pWindow->isWindow())//如果窗口小部件是独立窗口，则返回true，否则返回false。
	{
		if (pBtn == ui->m_btn_min)
		{
			pWindow->showMinimized();//最小化窗口小部件，作为图标。调用此函数仅影响窗口。
		}
		else if (pBtn == ui->m_btn_max)
		{
			pWindow->isMaximized() ? pWindow->showNormal() : pWindow->showMaximized();//放大缩小
		}
		else if (pBtn == ui->m_btn_close)
		{
			pWindow->close();//关闭窗口
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
