#ifndef HISTORY_H
#define HISTORY_H

#include <QDialog>

namespace Ui {
class History;
}

class History : public QDialog
{
    Q_OBJECT

public:
    explicit History(QWidget *parent = nullptr);
    ~History();

public slots:
    void addRow(QString time, QString scramble, QString solution);

    void addInfoToFile(QString time, QString scramble, QString solution);

    void showHistory();

    void clearHistory();

private:
    Ui::History *ui;
};

#endif // HISTORY_H
