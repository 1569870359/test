#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QString>
#include <QMap>
#include "finddialog.h"

namespace Ui {
class addressbook;
}

class addressbook : public QWidget
{
    Q_OBJECT

public:
    explicit addressbook(QWidget *parent = 0);
    ~addressbook();
    enum Mode { NavigationMode, AddingMode, EditingMode };

public slots:
    void add();
    void submit();
    void cancel();
    void edit();
    void remove();
    void find();
    void load();
    void save();
    void previous();
    void next();

private:
    Ui::addressbook *ui;

    void updateInterface(Mode mode);

    QPushButton *AddBtn;
    QPushButton *EditBtn;
    QPushButton *RemoveBtn;
    QPushButton *FindBtn;
    QPushButton *SubmitBtn;
    QPushButton *CancelBtn;
    QPushButton *LoadBtn;
    QPushButton *SaveBtn;
    QPushButton *PreviousBtn;
    QPushButton *NextBtn;

    QVBoxLayout *m_vLayout;
    QHBoxLayout *m_hLayout;
    QGridLayout *m_gLayout;

    QLabel *Name;
    QLabel *Address;

    QLineEdit *nameLine;
    QTextEdit *addressText;

    QString oldName;
    QString oldAddress;
    QMap<QString, QString> contacts;
    Mode currentMode;
    FindDialog *dialog;

};

#endif // ADDRESSBOOK_H
