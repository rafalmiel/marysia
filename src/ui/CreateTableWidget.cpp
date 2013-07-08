#include "CreateTableWidget.h"
#include "ui_CreateTableWidget.h"

#include <QInputDialog>
#include <QDebug>
#include <QMessageBox>

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
    delete m_createTableModel;
    delete m_createTableItemDelegate;
    delete ui;
}

void CreateTableWidget::on_btnSave_clicked()
{
    QString tableName = QInputDialog::getText(this, "Nazwa tabeli", QString::fromUtf8("Proszę podać nazwę nowej tabeli"));
    if (tableName != "") {
        if (m_createTableModel->pkColumns().count() == 0) {
            QMessageBox::warning(this, QString::fromUtf8("Błąd zapisu"), QString::fromUtf8("Nie zdefiniowano ani jednej kolumny klucza głównego"));
            return;
        }
        if (!m_tableManager->tableExists(tableName.toStdString())) {
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
            QMessageBox::warning(this, QString::fromUtf8("Sukces"), QString::fromUtf8("Tabela zapisana pomyślnie"));
        } else {
            QMessageBox::warning(this, QString::fromUtf8("Błąd zapisu"), "Tabela o podanej nazwie istnieje");
        }

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
