#include "NewColumnDialog.h"
#include "ui_NewColumnDialog.h"

#include <QDebug>
#include <QDateEdit>
#include <QMessageBox>
#include <table/columns/ColumnVarchar.h>
#include <table/columns/ColumnTinyint.h>
#include <table/columns/ColumnSmallint.h>
#include <table/columns/ColumnInteger.h>
#include <table/columns/ColumnDouble.h>
#include <table/columns/ColumnDate.h>
#include <table/columns/ColumnDatetime.h>
#include <table/columns/ColumnBlob.h>

#include "model/CreateTableModel.h"

NewColumnDialog::NewColumnDialog(CreateTableModel *model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewColumnDialog),
    m_createTableModel(model),
    m_defaultValWidget(NULL)
{
    ui->setupUi(this);

    setupControls();
}

void NewColumnDialog::setupControls()
{
    int ci = ui->comboType->currentIndex();

    switch (ci) {
    case TYPE_VARCHAR:
        replaceDefaultValWidget(new QLineEdit());
        ui->spitSize->setValue(32);
        break;
    case TYPE_TINYINT:
        replaceDefaultValWidget(new QSpinBox());
        ui->spitSize->setValue(1);
        break;
    case TYPE_SMALLINT:
        replaceDefaultValWidget(new QSpinBox());
        ui->spitSize->setValue(2);
        break;
    case TYPE_INT:
        replaceDefaultValWidget(new QSpinBox());
        ui->spitSize->setValue(4);
        break;
    case TYPE_DOUBLE:
        replaceDefaultValWidget(new QDoubleSpinBox());
        ui->spitSize->setValue(8);
        break;
    case TYPE_DATE:
        replaceDefaultValWidget(new QDateEdit());
        ui->spitSize->setValue(4);
        break;
    case TYPE_DATETIME:
        replaceDefaultValWidget(new QDateTimeEdit());
        ui->spitSize->setValue(4);
        break;
    case TYPE_BLOB:
        ui->spitSize->setValue(10);
        break;
    }

    if (m_createTableModel->hasAutoincrementPK()) {
        ui->lblAutoincrement->setVisible(false);
        ui->cbAutoincrement->setVisible(false);
        ui->lblPK->setVisible(false);
        ui->cbPK->setVisible(false);
    } else {
        ui->lblAutoincrement->setVisible(ci >= TYPE_TINYINT && ci <= TYPE_INT && !m_createTableModel->hasPK());
        ui->cbAutoincrement->setVisible(ci >= TYPE_TINYINT && ci <= TYPE_INT && !m_createTableModel->hasPK());
    }

    ui->lblDefaultValue->setVisible(ci != TYPE_BLOB);
    m_defaultValWidget->setVisible(ci != TYPE_BLOB);

    ui->lblSize->setVisible(ci != TYPE_BLOB);
    ui->spitSize->setVisible(ci != TYPE_BLOB);
    ui->spitSize->setEnabled(ci == TYPE_VARCHAR);

    if (ui->cbAutoincrement->isChecked()) {
        ui->cbPK->setChecked(true);
    }
}

void NewColumnDialog::replaceDefaultValWidget(QWidget *widget)
{
    QGridLayout *gLayout = qobject_cast<QGridLayout*>(layout());
    if (m_defaultValWidget) {
        gLayout->removeWidget(m_defaultValWidget);
        delete m_defaultValWidget;
    }

    m_defaultValWidget = widget;

    gLayout->addWidget(m_defaultValWidget, 6, 1, 1, 1);
}

NewColumnDialog::~NewColumnDialog()
{
    delete ui;
}

void NewColumnDialog::on_btnAdd_clicked()
{
    if (ui->edName->text().trimmed() == "") {
        QMessageBox::warning(this, "Błąd", "Nazwa tabeli nie może być pusta");
        return;
    }
    if (m_createTableModel->columnNameExists(ui->edName->text().trimmed().toStdString())) {
        QMessageBox::warning(this, "Błąd", "Kolumna o podanej nazwie istnieje");
        return;
    }
    int ci = ui->comboType->currentIndex();
    Column *col = NULL;
    if (ci == TYPE_VARCHAR) {
        QLineEdit *ed = qobject_cast<QLineEdit *>(m_defaultValWidget);
        col = new ColumnVarchar(ui->edName->text().toStdString(),
                                ui->spitSize->value(),
                                ui->cbNullable->isChecked(),
                                ed->text().toStdString());
    }
    else if (ci >= TYPE_TINYINT && ci <= TYPE_INT) {
        QSpinBox *ed = qobject_cast<QSpinBox *>(m_defaultValWidget);
        if (ci == TYPE_TINYINT)
            col = new ColumnTinyint(ui->edName->text().toStdString(),
                                    ui->cbNullable->isChecked(),
                                    ui->cbAutoincrement->isChecked(),
                                    ed->value());
        else if (ci == TYPE_SMALLINT)
            col = new ColumnSmallint(ui->edName->text().toStdString(),
                                     ui->cbNullable->isChecked(),
                                     ui->cbAutoincrement->isChecked(),
                                     ed->value());
        else if (ci == TYPE_INT)
            col = new ColumnInteger(ui->edName->text().toStdString(),
                                    ui->cbNullable->isChecked(),
                                    ui->cbAutoincrement->isChecked(),
                                    ed->value());
    }
    else if (ci == TYPE_DOUBLE) {
        QDoubleSpinBox *ed = qobject_cast<QDoubleSpinBox *>(m_defaultValWidget);
        col = new ColumnDouble(ui->edName->text().toStdString(),
                               ui->cbNullable->isChecked(),
                               ed->value());
    }
    else if (ci == TYPE_DATE) {
        QDateEdit *ed = qobject_cast<QDateEdit *>(m_defaultValWidget);
        QDateTime a();

        col = new ColumnDate(ui->edName->text().toStdString(),
                             ui->cbNullable->isChecked(),
                             QDateTime(ed->date()).toTime_t());
    }
    else if (ci == TYPE_DATETIME) {
        QDateTimeEdit *ed = qobject_cast<QDateTimeEdit *>(m_defaultValWidget);

        col = new ColumnDatetime(ui->edName->text().toStdString(),
                                 ui->cbNullable->isChecked(),
                                 ed->dateTime().toTime_t());
    }
    else if (ci == TYPE_BLOB) {
        col = new ColumnBlob(ui->edName->text().toStdString(),
                             ui->cbNullable->isChecked());
    }

    if (col) {
        m_createTableModel->addColumn(col, ui->cbPK->isChecked());
    }

    close();
}

