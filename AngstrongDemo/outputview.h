#ifndef OUTPUTVIEW_H
#define OUTPUTVIEW_H

#include <QWidget>
#include <mutex>

namespace Ui {
class OutputView;
}

class OutputView : public QWidget
{
    Q_OBJECT
public:
	static OutputView *GetInstance();
	static void WriteText(QString output_msg);
private:
    explicit OutputView(QWidget *parent = nullptr);
    ~OutputView();
private:
    Ui::OutputView *ui;
	static std::mutex mutex_;
	static OutputView *output_view_;
};

#endif // OUTPUTVIEW_H
