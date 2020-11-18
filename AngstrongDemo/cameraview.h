#pragma execution_character_set("utf-8")
#pragma once
#include <QTreeView>
#include <QStandardItem>
#include <map>
#include <memory>
#include <utility>
#include <QString>

typedef std::map<QString, std::pair<unsigned, QStandardItem*>>::value_type valType;

class CameraView : public QTreeView
{
	Q_OBJECT
public:
	explicit CameraView(QWidget *parent = nullptr);
	~CameraView();
	void iteratorOverItems();
	QList<QStandardItem *> returnTheItems();
	void mouseDoubleClickEvent(QMouseEvent *event);

public slots:
	void DetectCameraUSB(bool bUSB, QString qstrUSBName,int nIndex);
signals:
	void SelectCamera(QString qstrUSBName,int nIndex);
private:
	QStandardItemModel *model;
	std::map<QString, std::pair<int,QStandardItem*>> m_mpCameraList;
};

