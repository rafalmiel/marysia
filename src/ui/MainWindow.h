#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <filestruct/TableManager.h>

namespace Ui {
class MainWindow;
}

class CreateTableWidget;
class TableViewWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_actionCreateTable_triggered();

    void on_actionOpenTable_triggered();

private:
    Ui::MainWindow *ui;


    TableManager *m_tableManager;
};

#endif // MAINWINDOW_H
