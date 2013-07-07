#ifndef TABLEVIEWWIDGET_H
#define TABLEVIEWWIDGET_H

#include <QWidget>

#include <filestruct/TableManager.h>

namespace Ui {
class TableViewWidget;
}

class TableViewModel;

class TableViewWidget : public QWidget
{
	Q_OBJECT
	
public:
    explicit TableViewWidget(TableManager *tableManager, QWidget *parent = 0);
	~TableViewWidget();
	
private slots:
    void on_btnAddRandomRows_clicked();

    void on_btnLoadAll_clicked();

    void on_btnAddRow_clicked();

    void on_btnSearchByKey_clicked();

    void on_btnDelete_clicked();

    void on_btnDeleteAll_clicked();

private:
	Ui::TableViewWidget *ui;

    TableManager *m_tableManager;
    TableViewModel *m_tableViewModel;
};

#endif // TABLEVIEWWIDGET_H
