#ifndef PASSWORD_DIALOG_H
#define PASSWORD_DIALOG_H

#include <QDialog>
#include <QMap>
#include <QAction>

class QLineEdit;
class QLabel;

class PasswordDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PasswordDialog(const QString &username, QWidget *parent = nullptr);

    static QMap<QString, QString> getStrings(const QString &username, QWidget *parent, bool *ok = nullptr);

private:
    QMap<QString, QLineEdit*> lineedits;
    QAction eyeRootPasswordAction;
    QAction eyeUserPasswordAction;
};

#endif // PASSWORD_DIALOG_H
