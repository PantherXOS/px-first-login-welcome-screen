/*
 * px-software - Qt-GUI package manager for guix on PantherX OS
 * Copyright (C) 2019, 2020  Hamzeh Nasajpour <h.nasajpour@pantherx.org>
 * Copyright (C) 2019, 2020  Reza Alizadeh Majd <r.majd@pantherx.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#ifndef PX_SOFTWARE_TASKRUNNER_H
#define PX_SOFTWARE_TASKRUNNER_H

#include <QObject>
#include <QProcess>
#include <QUuid>

class AsyncTaskRunner : public QObject {
    Q_OBJECT
public:
    explicit AsyncTaskRunner(QObject *parent = nullptr);
    AsyncTaskRunner(QString app, QStringList args, QObject *parent = nullptr);

    virtual bool asyncRun();
    virtual bool asyncRun(const QString &appName, const QStringList &appArgs);
    bool wait();
    void cancel();
    bool running();

    QUuid Id() const { return m_id; }
    int timeout() const { return m_timeout; }
    void setTimeout(int timeout) { m_timeout = timeout; }
    QString readAllStandardOutput() const { return m_outStr; }
    QString readAllStandardError() const { return m_errStr; }

signals:
    void done(const QString &outData, const QString &errData);
    void failed(const QString &message);
    void canceled();
    void newData(const QString &outData, const QString &errData);

protected:
    QUuid m_id;
    QProcess m_worker;
    bool m_reportFailure;
    QString m_appName;
    QStringList m_appArgs;
    int m_timeout = 30000;
    QString m_outStr;
    QString m_errStr;
    bool m_running = false;
};

#endif //PX_SOFTWARE_TASKRUNNER_H
