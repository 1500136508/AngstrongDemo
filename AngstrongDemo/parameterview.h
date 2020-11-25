#ifndef PARAMETERVIEW_H
#define PARAMETERVIEW_H

#include <QDialog>

enum ECameraStatus
{
	ECameraStatus_Unknow = -1,
	ECameraStatus_Open = 0,
	ECameraStatus_Close,
	ECameraStatus_Live,
	ECameraStatus_Pause,
	ECameraStatus_Stop,
	ECameraStatus_Count,
};

namespace Ui {
class ParameterView;
}

class ParameterView : public QDialog
{
    Q_OBJECT

public:
    explicit ParameterView(QWidget *parent = nullptr);
    ~ParameterView();

private slots:
	void on_open_clicked();
	void on_close_clicked();
	void on_live_clicked();
	void on_pause_clicked();
	void on_stop_clicked();
	void on_createroi01_clicked();
	void on_createroi02_clicked();
	void on_cancelroi01_clicked();
	void on_cancelroi02_clicked();
	void ReceiveCameraStatus(ECameraStatus eStatus = ECameraStatus_Unknow);
signals:
	void SendCameraStatus(ECameraStatus eStatus = ECameraStatus_Unknow);
	void SendCreateAvgArea(int nIndex, bool bIsCreate);
private:
    Ui::ParameterView *ui;

	void BuildConnect();
};

#endif // PARAMETERVIEW_H
