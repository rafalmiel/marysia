#include "CreateTableItemDelegate.h"

#include <QApplication>
#include <QPainter>

#include "CreateTableModel.h"

CreateTableItemDelegate::CreateTableItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void CreateTableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index) const
{
    if (index.column() == CreateTableModel::COLUMN_NULLABLE ||
        index.column() == CreateTableModel::COLUMN_AUTOINCREMENT) {
        /*painter->setClipRect(option.rect);

        if (option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());*/

        QStyleOptionButton checkboxstyle;
        QRect checkbox_rect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkboxstyle);
        checkboxstyle.rect = option.rect;
        checkboxstyle.rect.setLeft(option.rect.x() +
                option.rect.width()/2 - checkbox_rect.width()/2);

        if (index.model()->data(index,Qt::DisplayRole).toBool())
        {
            checkboxstyle.state = QStyle::State_On | QStyle::State_Enabled;
        }
        else
        {
            checkboxstyle.state = QStyle::State_Off | QStyle::State_Enabled;
        }

        if (option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());

        QApplication::style()->drawControl(QStyle::CE_CheckBox,
                                            &checkboxstyle, painter);
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize CreateTableItemDelegate::sizeHint(const QStyleOptionViewItem &option,
               const QModelIndex &index) const
{
    return QSize(option.rect.width(), option.rect.height());
}
