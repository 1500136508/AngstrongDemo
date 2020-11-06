#pragma execution_character_set("utf-8")
#pragma once
#include <QTreeView>
#include <QStandardItem>

class CameraView : public QTreeView
{
	Q_OBJECT
public:
	explicit CameraView(QWidget *parent = nullptr);
	void iteratorOverItems();
	QList<QStandardItem *> returnTheItems();
	void mouseDoubleClickEvent(QMouseEvent *event);

signals:

public slots:

private:
	QStandardItemModel *model;
};

