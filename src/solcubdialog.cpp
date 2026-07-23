#include "solcubdialog.h"
#include "ui_solcubdialog.h"

#include <QPixmap>

SolCubDialog::SolCubDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SolCubDialog)
{
    ui->setupUi(this);
    setWindowTitle("Congratulations!");

    QPalette pal = this->palette();
    pal.setColor(QPalette::Window, QColor(179,255,179));
    this->setPalette(pal);

    QPixmap pix(":/pictures/party-popper.png");
    QPixmap reversedPix(":/pictures/reversedparty-popper.png");
    ui->pic_label->setPixmap(pix.scaled(100, 100, Qt::KeepAspectRatio));
    ui->reversePic_label->setPixmap(reversedPix.scaled(100, 100, Qt::KeepAspectRatio));

    connect(ui->save_button, SIGNAL(clicked()), this, SLOT(SaveEvent()));
    connect(ui->cancel_button, SIGNAL(clicked()), this, SLOT(CancelEvent()));
}

SolCubDialog::~SolCubDialog()
{
    delete ui;
}

void SolCubDialog::SaveEvent()
{
    emit SaveSolution();
}

void SolCubDialog::CancelEvent()
{
    emit CancelSolution();
}

