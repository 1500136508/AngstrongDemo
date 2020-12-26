#include "AngstrongDemo.h"
#include <QtWidgets/QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	HANDLE mutex = CreateMutexA(NULL, FALSE, "MyApp");           //����һ������������ ��ֹ����࿪��
	if (GetLastError() == ERROR_ALREADY_EXISTS)              	 //���֮ǰ�Ѿ�����һ������Ļ�����ô��ʾ������Ϣ���˳�
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
