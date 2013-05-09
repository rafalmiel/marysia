#ifndef CREATETABLEMODEL_H
#define CREATETABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>

class Column;

class CreateTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Columns{
        COLUMN_NAME = 0,
        COLUMN_TYPE = 1,
        COLUMN_SIZE = 2,
        COLUMN_NULLABLE = 3,
        COLUMN_AUTOINCREMENT = 4,
        COLUMN_PK = 5,
        COLUMN_DEFAULT_VAL = 6
    };

    explicit CreateTableModel(QObject *parent = 0);

    void addColumn(Column *column, bool is_pk = false);

    bool hasAutoincrementPK() const;
    bool hasPK() const;
    bool columnNameExists(const QString &name) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    const QVector<Column*> &columns() const;
    const QVector<Column*> &pkColumns() const;
    
signals:
    
public slots:

private:
    bool m_has_ai;
    QVector<Column*> m_columns;
    QVector<Column*> m_pk_columns;
};

#endif // CREATETABLEMODEL_H
