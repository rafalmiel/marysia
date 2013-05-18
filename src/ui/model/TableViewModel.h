#ifndef TABLEVIEWMODEL_H
#define TABLEVIEWMODEL_H

#include <QAbstractTableModel>
#include <QVector>

class TableManager;
class Row;

class TableViewModel : public QAbstractTableModel
{
public:
    TableViewModel(TableManager *tableManager);

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void loadAllRows();

    void setRow(Row *row);

private:
    TableManager *m_tableManager;

    QVector<Row*> m_rows;

};

#endif // TABLEVIEWMODEL_H
