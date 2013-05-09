#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class CreateTableWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_actionCreateTable_triggered();

private:
    Ui::MainWindow *ui;

    CreateTableWidget *m_createTableWidget;
};

#endif // MAINWINDOW_H
