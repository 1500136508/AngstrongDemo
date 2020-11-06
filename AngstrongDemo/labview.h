#ifndef LABVIEW_H
#define LABVIEW_H

#include <QLabel>
#include <vector>
#include <core.hpp>
#include <highgui.hpp>
#include <opencv.hpp>

class LabView : public QLabel
{
	Q_OBJECT
public:
	explicit LabView(QWidget *pParent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	explicit LabView(const QString &text, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~LabView();
public:
	bool Open();
	bool Save();
	bool Close();
protected:
	void contextMenuEvent(QContextMenuEvent *ev);
private slots:
	void on_Open_triggle();
	void on_Save_triggle();
	void on_Close_triggle();

public:
	cv::Mat m_Image;
};

#endif // LABVIEW_H