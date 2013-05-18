#ifndef NEWROWDIALOG_H
#define NEWROWDIALOG_H

#include <QDialog>
#include <QVector>

namespace Ui {
class NewRowDialog;
}

class TableManager;
class ColumnValueProber;
class Row;
class KeyValue;

class NewRowDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit NewRowDialog(TableManager *tableManager, bool keyValue = false, QWidget *parent = 0);
	~NewRowDialog();
	Row *row() const;
	KeyValue *keyValue() const;

	bool ok() const {return m_ok;}
	
private slots:
	void on_btnAddRow_clicked();

private:
	Ui::NewRowDialog *ui;

	TableManager *m_tableManager;

	QVector<ColumnValueProber *> m_valueProbers;

	bool m_keyValue;
	bool m_ok;
};

#endif // NEWROWDIALOG_H
