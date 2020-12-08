#ifndef OUTPUTVIEW_H
#define OUTPUTVIEW_H

#include <QWidget>
#include <memory>

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
	OutputView(const OutputView &);
	OutputView& operator=(const OutputView &);
    ~OutputView();
private:
    Ui::OutputView *ui;
	static OutputView *output_view_;
};

#endif // OUTPUTVIEW_H
