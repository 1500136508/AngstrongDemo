#ifndef SAVEDATA_H
#define SAVEDATA_H

#include <QDialog>

namespace Ui {
class SaveData;
}

class SaveData : public QDialog
{
    Q_OBJECT
public:
	enum ESaveMode
	{
		ESaveMode_3Pix = 0,
		ESaveMode_4Pix,
		ESaveMode_Count,
	};
public:
    explicit SaveData(QWidget *parent = nullptr);
    ~SaveData();

	void BuildConnect();
private slots:
	void on_chose_file_clicked();
	void on_start_clicked();
	void on_stop_clicked();
	void on_model_currentIndexChanged(int nIndex);
signals:
	void SendSaveDataStatus(bool bSave, int eMode, int nSaveCount);
private:
    Ui::SaveData *ui;
	bool m_bIsStart;//开始保存图像标志
	ESaveMode m_eCurSaveMode;
	int m_nSaveCount;
};

#endif // SAVEDATA_H
