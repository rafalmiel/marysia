#include "CreateTableModel.h"

#include <QDebug>

#include <table/Column.h>
#include <table/columns/ColumnVarchar.h>
#include <table/columns/ColumnDouble.h>
#include <table/columns/ColumnInteger.h>
#include <table/columns/ColumnTinyint.h>
#include <table/columns/ColumnDatetime.h>
#include <table/columns/ColumnDate.h>
#include <table/columns/ColumnBlob.h>
#include <table/Value.h>

CreateTableModel::CreateTableModel(QObject *parent) :
    QAbstractTableModel(parent),
    m_has_ai(false)
{
    /*m_columns.push_back(new ColumnVarchar(
                            "user_name",
                             16,
                             true)
    );*/

//    m_columns.push_back( new ColumnDouble(
//                            "rate",
//                            false,
//                            3.2 ) );

//    m_columns.push_back( new ColumnInteger(
//                            "user_id",
//                            false,
//                            false)
//    );

//    m_columns.push_back( new ColumnTinyint(
//                            "user_age",
//                            false,
//                            false)
//    );

//    m_columns.push_back( new ColumnVarchar(
//                            "city",
//                             16,
//                             true,
//                             "lublin" )
//    );

//    m_columns.push_back( new ColumnDatetime(
//                            "birth_date",
//                            false )
//    );

    /*m_columns.push_back( new ColumnBlob(
                            "picture",
                            false )
    );*/
}

const QVector<Column*> &CreateTableModel::columns() const
{
    return m_columns;
}

const QVector<Column*> &CreateTableModel::pkColumns() const
{
    return m_pk_columns;
}

bool CreateTableModel::hasAutoincrementPK() const
{
    return m_has_ai;
}

bool CreateTableModel::hasPK() const
{
    return !m_pk_columns.empty();
}

bool CreateTableModel::columnNameExists(const QString &name) const
{
    foreach (Column *col, m_columns) {
        if (col->columnName().std_str() == name.toStdString()) return true;
    }

    return false;
}

void CreateTableModel::addColumn(Column *column, bool is_pk)
{
    if (!m_has_ai || !column->isAutoincrement()) {
        beginInsertRows(QModelIndex(), m_columns.size(), m_columns.size());
        if (column->isAutoincrement()) m_has_ai = true;
        m_columns.push_back(column);
        endInsertRows();

        if (is_pk) {
            m_pk_columns.push_back(column);
        }
    }
}

int CreateTableModel::rowCount(const QModelIndex &parent) const
{
    return m_columns.size();
}

int CreateTableModel::columnCount(const QModelIndex &parent) const
{
    return 7;
}

QVariant CreateTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case COLUMN_NAME: return QString("Nazwa");
        case COLUMN_TYPE: return QString("Typ");
        case COLUMN_SIZE: return QString("Rozmiar");
        case COLUMN_NULLABLE: return QString("Nullable");
        case COLUMN_AUTOINCREMENT: return QString("Autoincrement");
        case COLUMN_DEFAULT_VAL: return QString::fromUtf8("Domyślna wartość");
        case COLUMN_PK: return QString("PK");
        default:
            return QVariant();
        }
    }
    return QVariant();
}

QVariant CreateTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        Column *col = m_columns[index.row()];
        switch (index.column()) {
        case COLUMN_NAME: return QString(col->columnName().c_str());
        case COLUMN_TYPE: return col->type();
        case COLUMN_SIZE: return col->typeLen();
        case COLUMN_NULLABLE: return col->isNullable();
        case COLUMN_AUTOINCREMENT: return col->isAutoincrement();
        case COLUMN_DEFAULT_VAL:
            if (col->type() != BLOB_TYPE)
                return col->defaultValue()->toString().c_str();
        case COLUMN_PK:
            return m_pk_columns.contains(col);
        }
    }
    return QVariant();
}
