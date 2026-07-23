#include "history.h"
#include "ui_history.h"

#include <QFile>
#include <QTextStream>

History::History(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::History)
{
    ui->setupUi(this);
    setWindowTitle("History of solutions");

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableWidget->show();

    connect(ui->clear_history_btn, SIGNAL(clicked()), this, SLOT(clearHistory()));
}

History::~History()
{
    delete ui;
}

void History::addRow(QString time, QString scramble, QString solution)
{
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    QTableWidgetItem *item = new QTableWidgetItem(time);
    ui->tableWidget->setItem(row, 0, item);

    item = new QTableWidgetItem(scramble);
    ui->tableWidget->setItem(row, 1, item);

    item = new QTableWidgetItem(solution);
    ui->tableWidget->setItem(row, 2, item);

    ui->tableWidget->show();
}

void History::addInfoToFile(QString time, QString scramble, QString solution)
{
    QFile file("C:/Users/Dima/Documents/Rubik-s-Cube-Course-Project/history.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
        return;

    QTextStream out(&file);
    out << time << "\n" << scramble << "\n" << solution << "\n";
    file.close();
}

void History::showHistory()
{
    QFile file("C:/Users/Dima/Documents/Rubik-s-Cube-Course-Project/history.txt");
    if (!file.open(QIODevice::ReadOnly))
        return;

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString time = in.readLine();
        QString scramble = in.readLine();
        QString solution = in.readLine();
        addRow(time, scramble, solution);
    }
    file.close();
}

void History::clearHistory()
{
    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        ui->tableWidget->removeRow(i);
    }
    ui->tableWidget->setRowCount(0);
    QFile file("C:/Users/Dima/Documents/Rubik-s-Cube-Course-Project/history.txt");
    file.remove();
}


