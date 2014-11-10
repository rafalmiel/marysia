#include "TableViewModel.h"

#include <filestruct/TableManager.h>
#include <filestruct/Tableinfo.h>
#include <table/Column.h>
#include <table/Value.h>
#include <table/Row.h>

TableViewModel::TableViewModel(TableManager *tableManager): m_tableManager(tableManager)
{
}

void TableViewModel::setRow(Row *row)
{
    beginResetModel();
    foreach (Row *row, m_rows) delete row;
    m_rows.clear();
    m_rows.push_back(row);
    endResetModel();
}

void TableViewModel::removeAllRows()
{
    QVector<Row *> keys;
    beginResetModel();
    foreach (Row *row, m_rows) delete row;
    m_rows.clear();
    m_tableManager->readInit("primary_key");
    Row *row = NULL;
    while (row = m_tableManager->readNext(), row != NULL) {
        keys.push_back(row);
    }
    m_tableManager->readClose();

    foreach (Row *kv, keys) {
        m_tableManager->deleteRow(kv->createKeyValueByName("primary_key"));
    }

    foreach (Row *r, keys) {
        delete r;
    }

    endResetModel();
}

void TableViewModel::loadAllRows()
{
    beginResetModel();
    foreach (Row *row, m_rows) delete row;
    m_rows.clear();
    m_tableManager->readInit("primary_key");
    Row *row = NULL;
    while (row = m_tableManager->readNext(), row != NULL) {
        m_rows.push_back(row);
    }
    endResetModel();
}

int TableViewModel::rowCount(const QModelIndex &parent) const
{
    return m_rows.size();
}

int TableViewModel::columnCount(const QModelIndex &parent) const
{
    return m_tableManager->meta()->columnCount();
}

QVariant TableViewModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();

    int row = index.row();
    int column = index.column();

    if (row >= 0 && row < m_rows.size() && column >= 0 && column < columnCount()) {
        return m_rows[row]->valueAt(column)->toString().c_str();
    }

    return QVariant();
}

QVariant TableViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();

    return m_tableManager->meta()->columnByNum(section)->columnName().c_str();
}


