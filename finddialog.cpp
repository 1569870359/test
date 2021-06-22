#include <QtWidgets>
#include "finddialog.h"

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
{
    QLabel *findLabel = new QLabel(tr("输入姓名:"));
    lineEdit = new QLineEdit;

    findButton = new QPushButton(tr("&查找"));
    findText = "";

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(findLabel);
    layout->addWidget(lineEdit);
    layout->addWidget(findButton);

    setLayout(layout);
    setWindowTitle(tr("查找"));
    connect(findButton, SIGNAL(clicked()), this, SLOT(findClicked()));
    connect(findButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void FindDialog::findClicked()
{
    QString text = lineEdit->text();

    if (text.isEmpty()) {
        QMessageBox::information(this, tr("空白"),
            tr("请输入姓名。"));
        return;
    } else {
        findText = text;
        lineEdit->clear();
    }
}

QString FindDialog::getFindText()
{
    return findText;
}
