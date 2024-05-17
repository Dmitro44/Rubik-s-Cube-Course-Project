#ifndef SOLCUBDIALOG_H
#define SOLCUBDIALOG_H

#include <QDialog>

namespace Ui {
class SolCubDialog;
}

class SolCubDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SolCubDialog(QWidget *parent = nullptr);
    ~SolCubDialog();

public slots:
    void SaveEvent();
    void CancelEvent();

signals:
    void SaveSolution();
    void CancelSolution();

private:
    Ui::SolCubDialog *ui;
};

#endif // SOLCUBDIALOG_H
