#include "PasswordDialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QDebug>

auto eyePasswordActionFunc = [](QAction &action, QLineEdit* lineEdit, bool triggered){
    if(triggered){
        action.setIcon(QIcon::fromTheme("hint"));
        lineEdit->setEchoMode(QLineEdit::Normal);
    } else {
        action.setIcon(QIcon::fromTheme("visibility"));
        lineEdit->setEchoMode(QLineEdit::Password);
    }
};

PasswordDialog::PasswordDialog(const QString &username, QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Password settings");
    QFormLayout *lytMain = new QFormLayout(this);

    auto userPasswordLabel = new QLabel("Enter the \"" + username + "\" password:");
    auto userPasswordLineEdit = new QLineEdit(this);
    userPasswordLineEdit->addAction(&eyeUserPasswordAction,QLineEdit::TrailingPosition);
    eyeUserPasswordAction.setCheckable(true);
    eyePasswordActionFunc(eyeUserPasswordAction, userPasswordLineEdit,false);
    connect(&eyeUserPasswordAction, &QAction::toggled,[&,userPasswordLineEdit](bool checked){
        eyePasswordActionFunc(eyeUserPasswordAction, userPasswordLineEdit,checked);
    });
    lytMain->addRow(userPasswordLabel, userPasswordLineEdit);
    lineedits[username] = userPasswordLineEdit;

    auto rootPasswordLabel = new QLabel("Enter the \"root\" password:");
    auto rootPasswordLineEdit = new QLineEdit(this);
    rootPasswordLineEdit->addAction(&eyeRootPasswordAction,QLineEdit::TrailingPosition);
    eyeRootPasswordAction.setCheckable(true);
    eyePasswordActionFunc(eyeRootPasswordAction, rootPasswordLineEdit,false);
    connect(&eyeRootPasswordAction, &QAction::toggled,[&,rootPasswordLineEdit](bool checked){
        eyePasswordActionFunc(eyeRootPasswordAction, rootPasswordLineEdit,checked);
    });
    lytMain->addRow(rootPasswordLabel, rootPasswordLineEdit);
    lineedits["root"] = rootPasswordLineEdit;

    QDialogButtonBox *buttonBox = new QDialogButtonBox
            ( QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
              Qt::Horizontal, this );
    lytMain->addWidget(buttonBox);

    bool conn = connect(buttonBox, &QDialogButtonBox::accepted,
                   this, &PasswordDialog::accept);
    Q_ASSERT(conn);
    conn = connect(buttonBox, &QDialogButtonBox::rejected,
                   this, &PasswordDialog::reject);
    Q_ASSERT(conn);

    setLayout(lytMain);
}

QMap<QString, QString> PasswordDialog::getStrings(const QString &username, QWidget *parent, bool *ok)
{
    PasswordDialog *dialog = new PasswordDialog(username, parent);
    QMap<QString, QString> list;
    const int ret = dialog->exec();
    if (ok)
        *ok = !!ret;
    if (ret) {
        for(auto k : dialog->lineedits.keys()) {
            list[k] = dialog->lineedits[k]->text();
        }
    }
    dialog->deleteLater();
    return list;
}