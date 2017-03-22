#ifndef MEMORYAPPLICATION_H
#define MEMORYAPPLICATION_H

#include <QMainWindow>

namespace Ui {
class MemoryApplication;
}

class MemoryApplication : public QMainWindow
{
    Q_OBJECT

public:
    explicit MemoryApplication(QWidget *parent = 0);
    ~MemoryApplication();

private slots:
    void on_btnNewProcess_clicked();

    void on_btnTerminate_clicked();

private:
    Ui::MemoryApplication *ui;
    void UpdateMemoryList();
};

#endif // MEMORYAPPLICATION_H
