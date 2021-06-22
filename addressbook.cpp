#include "addressbook.h"
#include "ui_addressbook.h"
#include <QMessageBox>

addressbook::addressbook(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::addressbook)
{
    ui->setupUi(this);
    setWindowTitle(tr("地址簿"));

    nameLine = new QLineEdit;
    nameLine->setReadOnly(true);

    addressText = new QTextEdit;
    addressText->setReadOnly(true);

    m_gLayout = new QGridLayout(this);
    m_vLayout = new QVBoxLayout(this);
    m_hLayout = new QHBoxLayout(this);

    Name = new QLabel("姓名：",this);
    Address = new QLabel("地址：",this);

    AddBtn = new QPushButton("添加",this);
    EditBtn = new QPushButton("编辑",this);
    RemoveBtn = new QPushButton("移除",this);
    FindBtn = new QPushButton("查找",this);
    SubmitBtn = new QPushButton("提交",this);
    CancelBtn = new QPushButton("取消",this);
    LoadBtn = new QPushButton("载入",this);
    SaveBtn = new QPushButton("保存",this);
    PreviousBtn = new QPushButton("上一个",this);
    NextBtn = new QPushButton("下一个",this);

    connect(AddBtn, SIGNAL(clicked()), this, SLOT(add()));
    connect(SubmitBtn, SIGNAL(clicked()), this, SLOT(submit()));
    connect(CancelBtn, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(EditBtn,SIGNAL(clicked()),this,SLOT(edit()));
    connect(RemoveBtn,SIGNAL(clicked()),this,SLOT(remove()));
    connect(FindBtn, SIGNAL(clicked()), this, SLOT(find()));
    connect(SaveBtn, SIGNAL(clicked()), this, SLOT(save()));
    connect(LoadBtn, SIGNAL(clicked()), this, SLOT(load()));
    connect(PreviousBtn, SIGNAL(clicked()), this, SLOT(previous()));
    connect(NextBtn, SIGNAL(clicked()), this, SLOT(next()));

    m_vLayout->addWidget(AddBtn, Qt::AlignTop);
    m_vLayout->addWidget(EditBtn);
    m_vLayout->addWidget(RemoveBtn);
    m_vLayout->addWidget(FindBtn);
    m_vLayout->addWidget(SubmitBtn);
    m_vLayout->addWidget(CancelBtn);
    m_vLayout->addWidget(LoadBtn);
    m_vLayout->addWidget(SaveBtn);
    m_vLayout->addStretch();

    m_hLayout->addWidget(PreviousBtn);
    m_hLayout->addWidget(NextBtn);

    m_gLayout->addWidget(Name, 0, 0);
    m_gLayout->addWidget(nameLine, 0, 1);
    m_gLayout->addWidget(Address, 1, 0, Qt::AlignTop);
    m_gLayout->addWidget(addressText, 1, 1);
    m_gLayout->addLayout(m_vLayout, 1, 2);
    m_gLayout->addLayout(m_hLayout, 2, 1);

    dialog = new FindDialog(this);

}

addressbook::~addressbook()
{
    delete ui;
}

void addressbook::add()
{
    oldName = nameLine->text();
    oldAddress = addressText->toPlainText();

    nameLine->clear();
    addressText->clear();

    nameLine->setReadOnly(false);
    nameLine->setFocus(Qt::OtherFocusReason);
    addressText->setReadOnly(false);

    AddBtn->setEnabled(false);
    SubmitBtn->show();
    CancelBtn->show();

    updateInterface(AddingMode);
}

void addressbook::submit()
{
    QString name = nameLine->text();
    QString address = addressText->toPlainText();

    if(name.isEmpty()||address.isEmpty())
    {
        QMessageBox::information(this, tr("空白"),
            tr("请输入一个姓名和地址。"));
        return;
    }

    if (currentMode == AddingMode) {

        if (!contacts.contains(name)) {
            contacts.insert(name, address);
            QMessageBox::information(this, tr("添加成功"),
                tr("\"%1\" 已成功加入地址簿。").arg(name));
        } else {
            QMessageBox::information(this, tr("添加失败"),
                tr(" 添加失败，\"%1\" 已在地址簿中。").arg(name));
        }
    } else if (currentMode == EditingMode) {

        if (oldName != name) {
            if (!contacts.contains(name)) {
                QMessageBox::information(this, tr("编辑成功"),
                    tr("\"%1\" 已被成功编辑。").arg(oldName));
                contacts.remove(oldName);
                contacts.insert(name, address);
            } else {
                QMessageBox::information(this, tr("编辑失败"),
                    tr("编辑失败, \"%1\" 已在地址簿中。").arg(name));
            }
        } else if (oldAddress != address) {
            QMessageBox::information(this, tr("编辑成功"),
                tr("\"%1\" 已被成功编辑。").arg(name));
            contacts[name] = address;
        }
    }

    updateInterface(NavigationMode);

    nameLine->setReadOnly(true);
    addressText->setReadOnly(true);

    AddBtn->setEnabled(true);
}

void addressbook::cancel()
{
    nameLine->setText(oldName);
    addressText->setText(oldAddress);
    updateInterface(NavigationMode);
}

void addressbook::edit()
{
    QString name = nameLine->text();
    QString address = addressText->toPlainText();

    if((name.isEmpty()||address.isEmpty())==false)
    {
        nameLine->setReadOnly(false);
        addressText->setReadOnly(false);
    }
    else
        QMessageBox::information(this, tr("空白"),
            tr("请先使用添加。"));

    updateInterface(EditingMode);
}

void addressbook::remove()
{

    QString name = nameLine->text();
    QString address = addressText->toPlainText();

    if (contacts.contains(name)) {

        int button = QMessageBox::question(this,
            tr("确认移除"),
            tr("确定要移除 \"%1\"?").arg(name),
            QMessageBox::Yes | QMessageBox::No);

        if (button == QMessageBox::Yes) {

            previous();
            contacts.remove(name);

            QMessageBox::information(this, tr("移除成功"),
                tr("\"%1\" 已从地址簿中移除。").arg(name));
        }
    }

    updateInterface(NavigationMode);

    AddBtn->setEnabled(true);
}

void addressbook::find()
{
    dialog->show();

    if (dialog->exec() == 1) {
        QString contactName = dialog->getFindText();

        if (contacts.contains(contactName)) {
            nameLine->setText(contactName);
            addressText->setText(contacts.value(contactName));
        } else {
            QMessageBox::information(this, tr("未找到"),
                tr("\"%1\" 不在地址簿中。").arg(contactName));
            return;
        }
    }

    updateInterface(NavigationMode);
}

void addressbook::load()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("打开地址簿"), "",
        tr("Address Book (*.abk);;All Files (*)"));

    if (fileName.isEmpty())
        return;
    else {
        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("打开文件失败"),
                file.errorString());
            return;
        }

        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_4_3);
        in >> contacts;

        QMap<QString, QString>::iterator i = contacts.begin();
        nameLine->setText(i.key());
        addressText->setText(i.value());
    }

    updateInterface(NavigationMode);
}

void addressbook::save()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("保存地址簿"), "",
        tr("Address Book (*.abk);;All Files (*)"));

    if (fileName.isEmpty())
        return;
    else {
        QFile file(fileName);

        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("打开文件失败"),
                file.errorString());
            return;
        }

        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_4_3);
        out << contacts;
    }

    updateInterface(NavigationMode);
}

void addressbook::previous()
{
    QString name = nameLine->text();
    QMap<QString, QString>::iterator i = contacts.find(name);

    if (i == contacts.end()) {
        nameLine->clear();
        addressText->clear();
        return;
    }

    if (i == contacts.begin())
        i = contacts.end();

    i--;
    nameLine->setText(i.key());
    addressText->setText(i.value());
}

void addressbook::next()
{
    QString name = nameLine->text();
    QMap<QString, QString>::iterator i = contacts.find(name);

    if (i != contacts.end())
        i++;

    if (i == contacts.end())
        i = contacts.begin();

    nameLine->setText(i.key());
    addressText->setText(i.value());
}

void addressbook::updateInterface(Mode mode)
{
    currentMode = mode;

    switch (currentMode) {

    case AddingMode:
    case EditingMode:

        nameLine->setReadOnly(false);
        nameLine->setFocus(Qt::OtherFocusReason);
        addressText->setReadOnly(false);

        AddBtn->setEnabled(false);
        EditBtn->setEnabled(false);
        RemoveBtn->setEnabled(false);

        NextBtn->setEnabled(false);
        PreviousBtn->setEnabled(false);

        SubmitBtn->show();
        CancelBtn->show();

        LoadBtn->setEnabled(false);
        SaveBtn->setEnabled(false);

        break;

    case NavigationMode:

        if (contacts.isEmpty()) {
            nameLine->clear();
            addressText->clear();
        }

        nameLine->setReadOnly(true);
        addressText->setReadOnly(true);
        AddBtn->setEnabled(true);

        int number = contacts.size();
        EditBtn->setEnabled(number >= 1);
        RemoveBtn->setEnabled(number >= 1);
        FindBtn->setEnabled(number > 2);
        NextBtn->setEnabled(number > 1);
        PreviousBtn->setEnabled(number > 1);

        LoadBtn->setEnabled(true);
        SaveBtn->setEnabled(number >= 1);
        break;
    }
}
