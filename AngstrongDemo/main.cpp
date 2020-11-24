#include "AngstrongDemo.h"
#include <QtWidgets/QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	HANDLE mutex = CreateMutexA(NULL, FALSE, "MyApp");           //创建一个互斥量对象 防止程序多开！
	if (GetLastError() == ERROR_ALREADY_EXISTS)              	 //如果之前已经打开了一个程序的话！那么显示错误信息并退出
	{
		QMessageBox::information(NULL, "AngstrongDemo",
			"Your AngstrongDemo is already running",
			QMessageBox::Ok);
		return 0;
	}

    AngstrongDemo w;
    w.show();
    return a.exec();
}
