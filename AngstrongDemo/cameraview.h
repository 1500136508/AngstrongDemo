#pragma execution_character_set("utf-8")
#pragma once
#include <QTreeView>
#include <QStandardItem>
#include <map>
#include <QString>

class CameraView : public QTreeView
{
	Q_OBJECT
public:
	explicit CameraView(QWidget *parent = nullptr);
	void iteratorOverItems();
	QList<QStandardItem *> returnTheItems();
	void mouseDoubleClickEvent(QMouseEvent *event);

public slots:
	void DetectCameraUSB(bool bUSB);
signals:
	void SelectCamera(int nIndex);
private:
	QStandardItemModel *model;
	std::map<QString, QStandardItem*> m_mpCameraList;
};

