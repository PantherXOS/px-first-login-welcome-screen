#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBoxLayout>
#include <QWidget>

#include "px-settings-service/SettingsObjects.h"

class MainWindow : public QMainWindow {
    public:
        MainWindow(QWidget *parent = nullptr);
    protected:
        virtual bool eventFilter(QObject* object, QEvent* event);
    private:
        QBoxLayout *createTopLayout();
        QBoxLayout *createContentLayout();
        QBoxLayout *createBottomLayout();
        int reboot();
        void        refreshStylesheet();

        SettingsObjects::SettingsSectionList sections;
        QWidget *settingsWidget;
};

#endif