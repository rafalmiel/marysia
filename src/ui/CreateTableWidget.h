#ifndef CREATETABLEWIDGET_H
#define CREATETABLEWIDGET_H

#include <QWidget>

namespace Ui {
class CreateTableWidget;
}

class CreateTableModel;
class CreateTableItemDelegate;

class CreateTableWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit CreateTableWidget(QWidget *parent = 0);
    ~CreateTableWidget();
    
private slots:
    void on_btnSave_clicked();

    void on_btnAddColumn_clicked();

signals:
    void tableCreated(QString);

private:
    Ui::CreateTableWidget *ui;

    CreateTableModel *m_createTableModel;
    CreateTableItemDelegate *m_createTableItemDelegate;
};

#endif // CREATETABLEWIDGET_H
