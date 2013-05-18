#include "NewRowDialog.h"
#include "ui_NewRowDialog.h"

#include <QLabel>
#include <QMessageBox>

#include <filestruct/TableManager.h>
#include <table/Column.h>
#include <table/KeyValue.h>
#include <table/KeyInfo.h>
#include <table/KeyPart.h>
#include <table/Row.h>
#include <filestruct/Tableinfo.h>

#include "ColumnValueProber.h"

NewRowDialog::NewRowDialog(TableManager *tableManager, bool keyValue, QWidget *parent) :
    QDialog(parent),
	ui(new Ui::NewRowDialog),
	m_tableManager(tableManager),
	m_keyValue(keyValue),
	m_ok(false)
{
	ui->setupUi(this);

	if (!m_keyValue) {
		for (int i = 0; i < m_tableManager->meta()->columnCount(); ++i) {
			QLabel *label = new QLabel(QString::fromUtf8(m_tableManager->meta()->columnByNum(i)->columnName().c_str()));
			ColumnValueProber *valueProber = new ColumnValueProber(m_tableManager->meta()->columnByNum(i));
			ui->gridLayout->addWidget(label, m_valueProbers.size(), 0);
			static_cast<QGridLayout *>(ui->proberWidget->layout())->addWidget(label, m_valueProbers.size(), 0);
			static_cast<QGridLayout *>(ui->proberWidget->layout())->addWidget(valueProber, m_valueProbers.size(), 1);
			m_valueProbers.push_back(valueProber);
		}
	} else {
		ui->btnAddRow->setText("Szukaj");
		KeyInfo *keyInfo = m_tableManager->meta()->getTableInfo()->keyByName("primary_key");
		for (int i = 0; i < keyInfo->keyColumnNum(); ++i) {
			QLabel *label = new QLabel(QString::fromUtf8(keyInfo->keyPart(i)->columnName().c_str()));
			ColumnValueProber *valueProber = new ColumnValueProber(keyInfo->keyPart(i)->column());
			ui->gridLayout->addWidget(label, m_valueProbers.size(), 0);
			static_cast<QGridLayout *>(ui->proberWidget->layout())->addWidget(label, m_valueProbers.size(), 0);
			static_cast<QGridLayout *>(ui->proberWidget->layout())->addWidget(valueProber, m_valueProbers.size(), 1);
			m_valueProbers.push_back(valueProber);
		}
	}
}

Row *NewRowDialog::row() const
{
	Row *row = new Row();
	foreach (const ColumnValueProber *prober, m_valueProbers) {
		row->addValue(prober->value());
	}

	return row;
}

KeyValue *NewRowDialog::keyValue() const
{
	KeyValue *keyValue = new KeyValue(m_tableManager->meta()->getTableInfo()->keyByName("primary_key"));
	foreach (const ColumnValueProber *prober, m_valueProbers) {
		keyValue->addValue(prober->value());
	}
	return keyValue;
}

NewRowDialog::~NewRowDialog()
{
	delete ui;
}

void NewRowDialog::on_btnAddRow_clicked()
{
	m_ok = true;
	if (m_keyValue) {
		close();
	} else {
		if (m_tableManager->insertRow(row())) {
			QMessageBox::information(this, "Dodano rekord", QString::fromUtf8("Nowy rekord dodany pomyślnie"));
			close();
		} else {
			QMessageBox::warning(this, QString::fromUtf8("Błąd"), QString::fromUtf8("Rekord z podanym kluczem głównym istnieje"));
		}
	}
}
