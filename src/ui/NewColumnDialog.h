#ifndef NEWCOLUMNDIALOG_H
#define NEWCOLUMNDIALOG_H

#include <QDialog>

namespace Ui {
class NewColumnDialog;
}

class CreateTableModel;

class NewColumnDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewColumnDialog(CreateTableModel *model, QWidget *parent = 0);
    ~NewColumnDialog();
    
private slots:
    void setupControls();

    void on_btnAdd_clicked();

private:
    enum TypeIndex {
        TYPE_VARCHAR = 0,
        TYPE_TINYINT = 1,
        TYPE_SMALLINT = 2,
        TYPE_INT = 3,
        TYPE_DOUBLE = 4,
        TYPE_DATE = 5,
        TYPE_DATETIME = 6,
        TYPE_BLOB = 7
    };

    void replaceDefaultValWidget(QWidget *widget);

    Ui::NewColumnDialog *ui;

    CreateTableModel *m_createTableModel;
    QWidget *m_defaultValWidget;
};

#endif // NEWCOLUMNDIALOG_H
