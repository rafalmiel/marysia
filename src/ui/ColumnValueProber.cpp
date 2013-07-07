#include "ColumnValueProber.h"
#include "ui_ColumnValueProber.h"

#include <limits>

#include <QLineEdit>
#include <QSpinBox>
#include <QDateEdit>

#include <table/Column.h>
#include <table/Value.h>

#include <table/values/ValueBlob.h>
#include <table/values/ValueDate.h>
#include <table/values/ValueDatetime.h>
#include <table/values/ValueDouble.h>
#include <table/values/ValueInteger.h>
#include <table/values/ValueSmallint.h>
#include <table/values/ValueTinyint.h>
#include <table/values/ValueVarchar.h>

ColumnValueProber::ColumnValueProber(Column *column, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColumnValueProber),
    m_valueWidget(NULL),
    m_column(column)
{
	ui->setupUi(this);

    initColumn(m_column);
}

Value *ColumnValueProber::ColumnValueProber::value() const
{
    uint32_t size = 0;
    uint8_t *data;
    switch (m_column->type())
    {
    case VARCHAR_TYPE:
        return new ValueVarchar(static_cast<QLineEdit*>(m_valueWidget)->text().toStdString().c_str());
        break;
    case INT_TYPE:
        return new ValueInteger(static_cast<QSpinBox*>(m_valueWidget)->value());
        break;
    case SMALLINT_TYPE:
        return new ValueSmallint(static_cast<QSpinBox*>(m_valueWidget)->value());
        break;
    case TINYINT_TYPE:
        return new ValueTinyint(static_cast<QSpinBox*>(m_valueWidget)->value());
        break;
    case DATETIME_TYPE:
        return new ValueDatetime(static_cast<QDateTimeEdit*>(m_valueWidget)->dateTime().toTime_t());
        break;
    case DATE_TYPE:
        return new ValueDate(QDateTime(static_cast<QDateEdit*>(m_valueWidget)->date()).toTime_t());
        break;
    case DOUBLE_TYPE:
        return new ValueDouble(static_cast<QDoubleSpinBox*>(m_valueWidget)->value());
        break;
    case BLOB_TYPE:
        size = static_cast<QLineEdit*>(m_valueWidget)->text().length();
        data = new uint8_t[size];
        memcpy(data, static_cast<QLineEdit*>(m_valueWidget)->text().toStdString().data(), size);
        return new ValueBlob(data, size);
        break;
    }
}

void ColumnValueProber::initColumn(Column *column)
{
    m_column = column;

    switch (m_column->type())
    {
    case VARCHAR_TYPE:
        m_valueWidget = new QLineEdit();
        static_cast<QLineEdit *>(m_valueWidget)->setMaxLength(m_column->typeLen());
        break;
    case INT_TYPE:
        m_valueWidget = new QSpinBox();
        static_cast<QSpinBox *>(m_valueWidget)->setMinimum(std::numeric_limits<qint32>().min());
        static_cast<QSpinBox *>(m_valueWidget)->setMaximum(std::numeric_limits<qint32>().max());
        break;
    case SMALLINT_TYPE:
        m_valueWidget = new QSpinBox();
        static_cast<QSpinBox *>(m_valueWidget)->setMinimum(std::numeric_limits<qint16>().min());
        static_cast<QSpinBox *>(m_valueWidget)->setMaximum(std::numeric_limits<qint16>().max());
        break;
    case TINYINT_TYPE:
        m_valueWidget = new QSpinBox();
        static_cast<QSpinBox *>(m_valueWidget)->setMinimum(std::numeric_limits<qint8>().min());
        static_cast<QSpinBox *>(m_valueWidget)->setMaximum(std::numeric_limits<qint8>().max());
        break;
    case DATETIME_TYPE:
        m_valueWidget = new QDateTimeEdit();
        break;
    case DATE_TYPE:
        m_valueWidget = new QDateEdit();
        break;
    case DOUBLE_TYPE:
        m_valueWidget = new QDoubleSpinBox();
        break;
    case BLOB_TYPE:
        m_valueWidget = new QLineEdit();
        break;
    }

    ui->gridLayout->addWidget(m_valueWidget, 0, 0);
}



ColumnValueProber::~ColumnValueProber()
{
	delete ui;
}
