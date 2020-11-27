#include "widgetui.h"
#include "ui_widgetui.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <windows.h>

WidgetUI::WidgetUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetUI)
{
    ui->setupUi(this);

	//嵌入qss进行美化
	QFile file("black.qss");
	file.open(QFile::ReadOnly);
	QTextStream filetext(&file);
	stylesheet = filetext.readAll();
	setStyleSheet(stylesheet);
	file.close();
}

WidgetUI::~WidgetUI()
{
    delete ui;
}
