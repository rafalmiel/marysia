#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>

#include "CreateTableWidget.h"
#include "TableViewWidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_tableManager = new TableManager();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionCreateTable_triggered()
{
    if (m_tableManager->isOpened()) m_tableManager->closeTable();
    CreateTableWidget *createTableWidget = new CreateTableWidget(m_tableManager);
    setCentralWidget(createTableWidget);
}

void MainWindow::on_actionOpenTable_triggered()
{
    QString tableName = QInputDialog::getText(this, "Nazwa tabeli", "Nazwa tabeli");
    if (tableName != "") {
        std::string str = tableName.toStdString();
        if (m_tableManager->tableExists(str) || m_tableManager->currentTableName() == tableName.toStdString()) {
            if (m_tableManager->isOpened()) m_tableManager->closeTable();
            m_tableManager->openTable(tableName.toStdString());
            TableViewWidget *tableViewWidget = new TableViewWidget(m_tableManager);
            setCentralWidget(tableViewWidget);
        } else {
            QMessageBox::warning(this, QString::fromUtf8("Błąd"), "Tabela o podanej nazwie nie istnieje");
        }
    }
}
