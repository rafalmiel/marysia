#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "CreateTableWidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_createTableWidget = new CreateTableWidget();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionCreateTable_triggered()
{
    setCentralWidget(m_createTableWidget);
}
