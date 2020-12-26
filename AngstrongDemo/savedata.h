#ifndef SAVEDATA_H
#define SAVEDATA_H

#include <vector>
#include <QDialog>
#include <QRadioButton>
#include "definition_save_data.h"

namespace Ui {
class SaveData;
}

class SaveData : public QDialog
{
    Q_OBJECT
public:
    explicit SaveData(QWidget *parent = nullptr);
    ~SaveData();

	void BuildConnect();
private slots:
	void on_chose_file_clicked();
	void on_start_clicked();
	void on_stop_clicked();
	void on_model_currentIndexChanged(int nIndex);
	void ReceiveSaveImageInfo(QString qstrInfo);
	void on_save_parameter_changed(bool is_check);
	void on_combo_save_mode_currentIndexChanged(int nIndex);
	void on_get_current_choose_clicked();
signals:
	void SendSaveDataStatus(bool bSave, int eMode, int nSaveCount,QString qstrPath);
private:
	void InitUI();
	void InitButtonArray();
	int get_accessory_current_check_id();
	int get_distanct_current_check_id();
	int get_face_action_current_check_id();

    Ui::SaveData *ui;
	bool m_bIsStart;//开始保存图像标志
	EGrabMode m_eCurSaveMode;
	ESaveMode save_data_mode_;
	int m_nSaveCount;
	std::string log_msg_;
	std::vector<QRadioButton*> radio_button_accessory_mode_;
	std::vector<QRadioButton*> radio_button_distance_mode_;
	std::vector<QRadioButton*> radio_button_face_action_mode_;
	std::string current_save_path_;
};

#endif // SAVEDATA_H
