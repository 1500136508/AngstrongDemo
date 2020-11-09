#include <QDebug>
#include <QMouseEvent>
#include "cameraview.h"

CameraView::CameraView(QWidget *parent /* = nullptr */)
	:QTreeView(parent)
{
	model = new QStandardItemModel(1, 1);
	model->setHeaderData(0, Qt::Horizontal, "Camera");
	//model->setHeaderData(1, Qt::Horizontal, "Details");

	QStandardItem *item1 = new QStandardItem("CameraList");
	item1->setIcon(QIcon(":/AngstrongDemo/image_ico/camera.jpg"));
	//QStandardItem *item2 = new QStandardItem("bluetooth");
	//item2->setIcon(QIcon(":/btn_bright_02"));
	//QStandardItem *item3 = new QStandardItem("crond");
	//item3->setIcon(QIcon(":/btn_bright_03"));
	//QStandardItem *item4 = new QStandardItem("cups");
	//item4->setIcon(QIcon(":/btn_bright_04"));

	model->setItem(0, 0, item1);
	/*model->setItem(1, 0, item2);
	model->setItem(2, 0, item3);
	model->setItem(3, 0, item4);*/

	//QStandardItem *item5 = new QStandardItem("fifth");
	//item4->appendRow(item5);

	//QModelIndex myparent;
	//for (int i = 0; i < 4; i++)
	//{
	//	myparent = model->index(0, 0, myparent);
	//	model->insertRows(0, 1, myparent);  // 添加一行
	//	model->insertColumns(0, 1, myparent);   // 添加一列
	//	QModelIndex index = model->index(0, 0, myparent);
	//	model->setData(index, i);
	//}

	this->setModel(model);
}

void CameraView::iteratorOverItems()
{
	QList<QStandardItem *> list = this->returnTheItems();
	foreach(QStandardItem *item, list)
	{
		qDebug() << item->text();
	}
}

QList<QStandardItem*> CameraView::returnTheItems()
{
	return model->findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
}

void CameraView::mouseDoubleClickEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		QModelIndex index = currentIndex();
		if (index.row() == 0)
		{
			qDebug() << index.data().toString();
		}
	}
}
