#include <QDebug>
#include <QMouseEvent>
#include "cameraview.h"

CameraView::CameraView(QWidget *parent /* = nullptr */)
	:QTreeView(parent)
{
	//初始化
	model = nullptr;
	m_mpCameraList.clear();

	model = new QStandardItemModel(1, 1);
	model->setHeaderData(0, Qt::Horizontal, "Camera");
	//model->setHeaderData(1, Qt::Horizontal, "Details");

	/*QStandardItem *item1 = new QStandardItem("CameraList");
	item1->setIcon(QIcon(":/AngstrongDemo/image_ico/camera.jpg"));*/

	//QStandardItem *item2 = new QStandardItem("bluetooth");
	//item2->setIcon(QIcon(":/btn_bright_02"));
	//QStandardItem *item3 = new QStandardItem("crond");
	//item3->setIcon(QIcon(":/btn_bright_03"));
	//QStandardItem *item4 = new QStandardItem("cups");
	//item4->setIcon(QIcon(":/btn_bright_04"));

	//model->setItem(0, 0, item1);
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
	model->removeRow(0);
}

void CameraView::iteratorOverItems()
{
	QList<QStandardItem *> list = this->returnTheItems();
	foreach(QStandardItem *item, list)
	{
#if DEBUG
		qDebug() << item->text();
#endif
	}
}

CameraView::~CameraView()
{
	//Qt内存管理机制,不需要delete释放内存？？？？
	/*if (model)
	{
		delete model;
		model = nullptr;
	}*/

	if (m_mpCameraList.size() > 0)
	{
		/*for (auto iter = m_mpCameraList.begin(); iter != m_mpCameraList.end(); ++iter)
		{
			if (iter->second)
			{
				delete iter->second;
				iter->second = nullptr;
			}
		}*/
		m_mpCameraList.clear();
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
		auto iter = m_mpCameraList.find(index.data().toString());
		if (iter != m_mpCameraList.end())
		{
			emit SelectCamera(iter->first, iter->second.first);
		}
	}
}

void CameraView::DetectCameraUSB(bool bUSB,QString qstrUSBName, int nIndex)
{
	if (bUSB)
	{
		if (nIndex >= 0)
		{
			m_mpCameraList.insert(valType(qstrUSBName,std::pair<int, QStandardItem*>(nIndex,new QStandardItem(qstrUSBName))));
			m_mpCameraList[qstrUSBName].second->setIcon(QIcon(":/AngstrongDemo/image_ico/camera.jpg"));
			model->setItem(nIndex, 0, m_mpCameraList[qstrUSBName].second);
		}
	}
	else
	{
		int count = model->rowCount();
		for (int i = 0; i < count;++i)
		{
			model->removeRow(i);
		}
		m_mpCameraList.clear();
	}
}