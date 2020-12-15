#ifndef PARAMETERVIEW_H
#define PARAMETERVIEW_H

#include <QDialog>
#include <mutex>
#include "definition_camera.h"

namespace Ui {
class ParameterView;
}

class ParameterView : public QDialog
{
    Q_OBJECT

public:
    explicit ParameterView(QWidget *parent = nullptr);
    ~ParameterView();

	void FindAllPort();
public slots:
	void on_open_clicked();
	void on_close_clicked();
	void on_live_clicked();
	void on_pause_clicked();
	void on_stop_clicked();
	void on_createroi01_clicked();
	void on_createroi02_clicked();
	void on_cancelroi01_clicked();
	void on_cancelroi02_clicked();
	void on_get_current_camera_id_index_change(int camera_id);
	void on_get_current_diplay_mode_index_change(int image_display_mode);
	void ReceiveCameraStatus(ECameraStatus eStatus = ECameraStatus_Unknow);
	void ReceiveAddCameraUSBString(bool bUSB, QString qstrUSBName, int nIndex);
signals:
	void SendCameraStatus(ECameraStatus eStatus,int camera_index);
	void SendCreateAvgArea(int nIndex, bool bIsCreate);
	void SendImageDisplayMode(EDisplayMode image_display_mode);
	void SendXMData(QString read_data);
private:
    Ui::ParameterView *ui;

	void BuildConnect();
	void CreateHalerThread();
private:
	int current_camera_index_;
	ECameraStatus current_camera_status_ = ECameraStatus_Unknow;
	std::mutex mutex_;
	bool quite_program_ = false;
};

#endif // PARAMETERVIEW_H
