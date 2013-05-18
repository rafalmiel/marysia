#include "TableViewWidget.h"
#include "ui_TableViewWidget.h"

#include <table/Row.h>
#include <filestruct/Tableinfo.h>
#include <utils/RandGenerator.h>

#include "model/TableViewModel.h"
#include "NewRowDialog.h"

#include <QInputDialog>
#include <QMessageBox>
TableViewWidget::TableViewWidget(TableManager *tableManager, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableViewWidget),
    m_tableManager(tableManager)
{
	ui->setupUi(this);
    m_tableViewModel = new TableViewModel(m_tableManager);
    ui->tableView->setModel(m_tableViewModel);
}

TableViewWidget::~TableViewWidget()
{
	delete ui;
}

void TableViewWidget::on_btnAddRandomRows_clicked()
{
    int cnt = QInputDialog::getInt(this, QString::fromUtf8("Liczba losowych rekordów"), "Proszę podać liczbę losowych rekordów",
                             1, 1);
    int resCount = 0;
    for (int i = 0; i < cnt; ++i) {
        Row *row = new Row();
        for (int j = 0; j < m_tableManager->meta()->columnCount(); ++j) {
            row->addValue(RandGenerator::randomValue(m_tableManager->meta()->columnByNum(j)));
        }
        if (m_tableManager->insertRow(row)) resCount++;
    }

    QMessageBox::information(this,
                             "Wstawiono nowe rekordy",
                             QString::fromUtf8("Pomyślnie wstawiono ") +
                             QString::number(resCount) +
                             QString::fromUtf8(" rekordów"));
}

void TableViewWidget::on_btnLoadAll_clicked()
{
    m_tableViewModel->loadAllRows();
}

void TableViewWidget::on_btnAddRow_clicked()
{
    NewRowDialog dialog(m_tableManager);
    dialog.exec();
}

void TableViewWidget::on_btnSearchByKey_clicked()
{
    NewRowDialog dialog(m_tableManager, true);
    dialog.exec();
    if (dialog.ok()) {
        Row *row = m_tableManager->read("primary_key", dialog.keyValue());

        if (row) {
            m_tableViewModel->setRow(row);
            QMessageBox::information(this,
                                     QString::fromUtf8("Znaleciono rekord"),
                                     QString::fromUtf8("Znaleziono rekord o podanym kluczu"));
        } else {
            QMessageBox::information(this,
                                     QString::fromUtf8("Nie znaleciono rekordu"),
                                     QString::fromUtf8("Nie Znaleziono rekordu o podanym kluczu"));
        }
    }
}
