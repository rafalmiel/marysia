#ifndef COLUMNVALUEPROBER_H
#define COLUMNVALUEPROBER_H

#include <QWidget>

namespace Ui {
class ColumnValueProber;
}

class Column;
class Value;

class ColumnValueProber : public QWidget
{
	Q_OBJECT
	
public:
    explicit ColumnValueProber(Column *column, QWidget *parent = 0);
	~ColumnValueProber();

    void initColumn(Column *column);

    Value *value() const;
	
private:
	Ui::ColumnValueProber *ui;
    QWidget *m_valueWidget;

    Column *m_column;
};

#endif // COLUMNVALUEPROBER_H
