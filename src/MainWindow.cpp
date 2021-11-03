#include "MainWindow.h"
#include <vector>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QCheckBox>
#include <QIcon>
#include <QPixmap>
#include <QSize>
#include <QGuiApplication>

#include "px-settings-service/RpcSettingsClient.h"

string themeModuleName = "theme";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){
    setWindowTitle("PantherX - First Login Welcome Screen");
    setWindowIcon(QIcon::fromTheme("panther"));
    setMinimumSize(QSize(1000,750));
    showMaximized();

    RPCSettingsClient rpcSettingsClient;
    vector<string> errors;
    rpcSettingsClient.getModuleSections(themeModuleName, sections, errors);

    auto mainLayout = new QVBoxLayout();
    mainLayout->addLayout(createTopLayout());
    mainLayout->addStretch();
    mainLayout->addLayout(createContentLayout());
    mainLayout->addStretch();
    mainLayout->addLayout(createBottomLayout());
    mainLayout->setContentsMargins(70,70,70,70);
    mainLayout->setAlignment(Qt::AlignTop);

    auto widget = new QWidget();
    widget->setLayout(mainLayout);
    // setLayout(mainLayout);
    setCentralWidget(widget);
    installEventFilter(this);
}


QBoxLayout *MainWindow::createTopLayout(){
    auto titleLabel = new QLabel("<font size=+8>Thank you for installing PantherX</font><br><font size=+2>We're almost there, just 3 more steps.</font>");
    auto font = titleLabel->font();
    font.setPointSize(17);
    titleLabel->setFont(font);

    auto layout = new QHBoxLayout();
    layout->addWidget(titleLabel);
    return layout;
}

QBoxLayout *MainWindow::createContentLayout(){
    // password widgets
    auto passwordsLabel = new QLabel("Set a user and root password", this);
    auto font = passwordsLabel->font();
    font.setPointSize(18);
    passwordsLabel->setFont(font);
    
    passwordsLabel->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
    auto passwordButton = new QPushButton("Run", this);
    connect(passwordButton, &QPushButton::released,[&](){
        passwordTaskRunner.asyncRun("sh", QStringList() << "-c" << "qterminal -e passwd \"$(whoami)\"");
    });
    connect(&passwordTaskRunner, &AsyncTaskRunner::done,[&](const QString &outData, const QString &errData){
        rootPasswordTaskRunner.asyncRun("sh", QStringList() << "-c" << "qterminal -e sudo passwd \"root\"");
        qDebug() << "DONE: " << outData << errData;
    });
    auto passwordsLayout = new QHBoxLayout();
    passwordsLayout->addWidget(passwordsLabel);
    passwordsLayout->addWidget(passwordButton);
    // update system widgets
    auto updateSystemLabel = new QLabel("Update your system", this);
    updateSystemLabel->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
    updateSystemLabel->setFont(font);

    auto updateSystemButton = new QPushButton("Run", this);
    connect(updateSystemButton, &QPushButton::released, [&](){
        if(!softwareUpdateTaskRunner.running()) 
            softwareUpdateTaskRunner.asyncRun("px-software", QStringList() << "-i" << "system_updates");
    });
    // theme widgets
    auto themeLabel = new QLabel("Switch Theme", this);
    themeLabel->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
    themeLabel->setFont(font);
    auto themeCheckBox = new QCheckBox(this);
    connect(themeCheckBox, &QCheckBox::toggled,[&](int checked){
        for(auto &sec: sections){
            if(sec.name==themeModuleName){
                for(auto &par : sec.params){
                    if(par.key==themeModuleName) {
                        par.value.clear();
                        par.value.push_back((checked?"bright":"dark"));
                    }
                }
                RPCSettingsClient settingsClient;
                SettingsObjects::SettingsWriteResult result = settingsClient.addToSection(themeModuleName,sec.name,sec);

                if(result.written){
                    vector<string> errors;
                    settingsClient.apply(themeModuleName,errors);
                }
            }
        }
    });
    for(auto &sec: sections){
        if(sec.name==themeModuleName){
            for(auto &par : sec.params){
                if(par.key==themeModuleName) {
                    if(par.value.size() && par.value.at(0) == "bright")
                        themeCheckBox->setChecked(true);
                }
            }
        }
    }
    auto themeCheckBoxLayout = new QHBoxLayout();
    themeCheckBoxLayout->setAlignment(Qt::AlignCenter);
    themeCheckBoxLayout->addWidget(themeCheckBox);
    // reboot widgets
    auto rebootLabel = new QLabel("Reboot", this);
    rebootLabel->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
    rebootLabel->setFont(font);
    rebootButton = new QPushButton("Run", this);
    connect(rebootButton, &QPushButton::released,[&](){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Reboot", "Are you sure?                               ",
                                        QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            rebootButton->setDisabled(true);
            rebootButton->setText("Wait ...");
            rebootTaskRunner.asyncRun("guix", QStringList() << "package" << "-i" << "xdg-utils");
        }
    });
    connect(&rebootTaskRunner, &AsyncTaskRunner::done,[&](const QString &outData, const QString &errData){
        reboot();
        qDebug() << "DONE: " << outData << errData;
    });
    connect(&rebootTaskRunner, &AsyncTaskRunner::failed,[&](const QString &message){
        rebootButton->setEnabled(true);
        rebootButton->setText("Run");
        qDebug() << "Failed: " << message;
    });
    auto leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(20);
    leftLayout->addWidget(passwordsLabel);
    leftLayout->addWidget(updateSystemLabel);
    leftLayout->addWidget(themeLabel);
    leftLayout->addWidget(rebootLabel);

    auto rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(20);
    rightLayout->setAlignment(Qt::AlignCenter);
    rightLayout->addWidget(passwordButton);
    rightLayout->addWidget(updateSystemButton);
    rightLayout->addLayout(themeCheckBoxLayout);
    rightLayout->addWidget(rebootButton);
    //
    auto settingsLayout = new QHBoxLayout();
    settingsLayout->addLayout(leftLayout);
    settingsLayout->addSpacing(200);
    settingsLayout->addLayout(rightLayout);

    settingsWidget = new QWidget(this);
    settingsWidget->setLayout(settingsLayout);
    settingsWidget->setContentsMargins(50,50,50,50);
    refreshStylesheet();

    auto contentLabel = new QLabel(tr("<font size=+2>After you complete these steps, issues like opening folders<br>from the menu and panel will be resolved permanently.</font>"));

    auto vlayout = new QVBoxLayout();
    vlayout->addWidget(settingsWidget);
    vlayout->addWidget(contentLabel);

    auto mainLayout = new QHBoxLayout();
    mainLayout->addLayout(vlayout);
    mainLayout->addStretch(1);
    mainLayout->setContentsMargins(70,0,70,0);

    return mainLayout;
}

QBoxLayout *MainWindow::createBottomLayout(){
    auto bottomLabel = new QLabel("Find more information on our Wiki at <a href=\"wiki.pantherx.org\">wiki.pantherx.org</a><br>\
                                    or get help on the forum at <a href=\"community.pantherx.org\">community.pantherx.org</a>.");
    auto font = bottomLabel->font();
    font.setPointSize(22);
    bottomLabel->setFont(font);
    bottomLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    bottomLabel->setAlignment(Qt::AlignBottom);

    QIcon qicon = QIcon::fromTheme("panther");
    QSize iconSize(128,128);
    QPixmap pixmap = qicon.pixmap(iconSize, QIcon::Normal, QIcon::On);
    auto iconLabel = new QLabel;
    iconLabel->setAttribute(Qt::WA_TranslucentBackground);
    iconLabel->setPixmap(pixmap);
    iconLabel->setFixedSize(iconSize);

    auto bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(bottomLabel);
    bottomLayout->addWidget(iconLabel);
    bottomLayout->setAlignment(Qt::AlignBottom);
    return bottomLayout;
}

int MainWindow::reboot(){
    QDBusInterface logind{"org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", QDBusConnection::systemBus()};
    const auto message = logind.callWithArgumentList(QDBus::Block, "CanReboot", {});
    QDBusPendingReply< QString > canReboot = message;
    Q_ASSERT(canReboot.isFinished());
    if (canReboot.isError()) {
        const auto error = canReboot.error();
        qDebug() << "Asynchronous call finished with error: " + error.name() + " " + error.message();
        return -1;
    }
    if (canReboot.value() == "yes") {
        QDBusPendingReply<> reboot = logind.callWithArgumentList(QDBus::Block, "Reboot", {true, });
        Q_ASSERT(reboot.isFinished());
        if (reboot.isError()) {
            const auto error = reboot.error();
            qDebug() << "Asynchronous call finished with error: " + error.name() + " " + error.message();
            return -1;
        }
    } else {
        qDebug() << "Can't power off: CanReboot() result is " + canReboot.value();
        return -1;
    }
    return 0;
}

void MainWindow::refreshStylesheet(){    
    auto bg = QGuiApplication::palette().color(QPalette::Active, QPalette::Window).darker(110);
    auto fgcolor = QGuiApplication::palette().color(QPalette::Active, QPalette::Text);
    settingsWidget->setStyleSheet("QWidget {background-color: " + bg.name() + ";" + "color: " +  fgcolor.name() + "}");   
}

bool MainWindow::eventFilter(QObject* object, QEvent* event) {
    if(event->type() == QEvent::PaletteChange) {
        refreshStylesheet();
    }
    return QMainWindow::eventFilter(object, event);
}