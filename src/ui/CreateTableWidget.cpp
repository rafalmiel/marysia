#include "CreateTableWidget.h"
#include "ui_CreateTableWidget.h"

#include <QInputDialog>
#include <QDebug>

#include <table/Column.h>

#include "NewColumnDialog.h"

#include "model/CreateTableModel.h"
#include "model/CreateTableItemDelegate.h"

CreateTableWidget::CreateTableWidget(TableManager *tableManager, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateTableWidget),
    m_tableManager(tableManager)
{
    ui->setupUi(this);

    m_createTableModel = new CreateTableModel();
    m_createTableItemDelegate = new CreateTableItemDelegate();

    ui->tableNewTable->setModel(m_createTableModel);
    //ui->tableNewTable->setItemDelegate(m_createTableItemDelegate);
}

CreateTableWidget::~CreateTableWidget()
{
    delete ui;
}

void CreateTableWidget::on_btnSave_clicked()
{
    QString tableName = QInputDialog::getText(this, "Nazwa tabeli", "Proszę podać nazwę nowej tabeli");
    if (tableName != "") {
        m_tableManager->createTable(tableName.toStdString());
        const QVector<Column *> &columns = m_createTableModel->columns();
        const QVector<Column *> &pk_columns = m_createTableModel->pkColumns();

        foreach (Column *col, columns) {
            m_tableManager->addColumn(col);
        }
        bool first = true;
        QString pkColNames = "";
        foreach (Column *col, pk_columns) {
            if (!first) pkColNames += ";";
            first = false;
            pkColNames += col->columnName().c_str();
        }

        m_tableManager->addKey("primary_key", pkColNames.toStdString(), true, true);
        //m_tableManager->closeTable();

        //ui->tableNewTable->setModel(NULL);
        //delete m_createTableModel;

        //emit tableCreated(tableName);
    }
}

void CreateTableWidget::on_btnAddColumn_clicked()
{
    NewColumnDialog d(m_createTableModel);
    d.exec();
}
