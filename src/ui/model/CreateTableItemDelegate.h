#ifndef CREATETABLEITEMDELEGATE_H
#define CREATETABLEITEMDELEGATE_H

#include <QStyledItemDelegate>

class CreateTableItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CreateTableItemDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;
    
signals:
    
public slots:
    
};

#endif // CREATETABLEITEMDELEGATE_H
