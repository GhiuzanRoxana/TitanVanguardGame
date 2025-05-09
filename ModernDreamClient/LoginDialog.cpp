#include "LoginDialog.h"
#include <QPixmap>
#include <QPalette>
#include <QBrush>
#include <QPainter>
#include <QMessageBox>
#include <QCoreApplication>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <qframe.h>
#include "HttpClient.h"
#include "ModernDreamClient.h"


LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent), httpClient(new HttpClient(this))
{ 
    
    modernDreamClient = new ModernDreamClient(nullptr);
     
    setWindowTitle("Titan Vanguard - Login");
    resize(850, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);

    stackedWidget = new QStackedWidget(this);

    QWidget *loginPage = new QWidget();
    QVBoxLayout *loginLayout = new QVBoxLayout();
    loginLayout->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);

    QFrame *loginFrame = new QFrame(this);
    loginFrame->setFixedSize(350, 120);
    loginFrame->setStyleSheet(
        "QFrame {"
        "    background-color: rgba(30, 30, 30, 0.8);"
        "    border-radius: 15px;"
        "    border: 2px solid rgba(50, 50, 50, 0.9);"
        "}");

    QVBoxLayout *frameLayout = new QVBoxLayout(loginFrame);
    frameLayout->setAlignment(Qt::AlignCenter);

    QLabel *usernameLabel = new QLabel("USERNAME:", this);
    usernameLabel->setStyleSheet("font-size: 14px; color: white; font-weight: bold; background: none; border: none; font-style: italic; letter-spacing: 2px;");
    usernameLabel->setAlignment(Qt::AlignCenter);

    usernameEdit = new QLineEdit(this);
    usernameEdit->setFixedWidth(300);
    usernameEdit->setFixedHeight(25);
    usernameEdit->setStyleSheet("background: rgba(255, 255, 255, 0.7); border-radius: 5px; padding: 5px;");
    usernameEdit->setAlignment(Qt::AlignCenter);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    loginButton = new QPushButton("LOGIN", this);
    registerButton = new QPushButton("REGISTER", this);

    loginButton->setFixedSize(100, 40);
    registerButton->setFixedSize(100, 40);

    loginButton->setStyleSheet(
        "QPushButton {"
        "    background-color: black;"
        "    color: #BF00FF;"
        "    border: 2px solid #BF00FF;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "    font-size: 12px;"
        "    font-family: 'Italic';"
        "    font-weight: bold;"
        "}");
    registerButton->setStyleSheet(
        "QPushButton {"
        "    background-color: black;"
        "    color: #BF00FF;"
        "    border: 2px solid #BF00FF;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "    font-size: 12px;"
        "    font-family: 'Italic';"
        "    font-weight: bold;"
        "}");

    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(registerButton);
    buttonLayout->setAlignment(Qt::AlignCenter);

    frameLayout->addWidget(usernameLabel);
    frameLayout->addWidget(usernameEdit);
    frameLayout->addLayout(buttonLayout);

    loginLayout->addWidget(loginFrame, 0, Qt::AlignBottom | Qt::AlignHCenter);

    loginPage->setLayout(loginLayout);

    QWidget *menuPage = new QWidget();
    QVBoxLayout *menuLayout = new QVBoxLayout();
    menuLayout->setAlignment(Qt::AlignCenter);

    QFrame *menuFrame = new QFrame(this);
    QVBoxLayout *menuFrameLayout = new QVBoxLayout(menuFrame);
    menuFrameLayout->setAlignment(Qt::AlignCenter);

    QPushButton *singlePlayer = new QPushButton("SINGLE PLAYER", this);
    QPushButton *multiPlayer = new QPushButton("MULTIPLAYER", this);
    QPushButton *backToLoginButton = new QPushButton("BACK", this);

    singlePlayer->setFixedSize(200, 50);
    multiPlayer->setFixedSize(200, 50);
    backToLoginButton->setFixedSize(100, 50);

    singlePlayer->setStyleSheet(
        "QPushButton {"
        "    background-color: black;"
        "    color: #BF00FF;"
        "    border: 2px solid #BF00FF;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "    font-size: 14px;"
        "    font-family: 'Italic';"
        "    font-weight: bold;"
        "}");
    multiPlayer->setStyleSheet(
        "QPushButton {"
        "    background-color: black;"
        "    color: #BF00FF;"
        "    border: 2px solid #BF00FF;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "    font-size: 14px;"
        "    font-family: 'Italic';"
        "    font-weight: bold;"
        "}");
    backToLoginButton->setStyleSheet(
        "QPushButton {"
        "    background-color: black;"
        "    color: #BF00FF;"
        "    border: 2px solid #BF00FF;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "    font-size: 12px;"
        "    font-family: 'Italic';"
        "    font-weight: bold;"
        "}");

    menuFrameLayout->addWidget(singlePlayer);
    menuFrameLayout->addWidget(multiPlayer);
    menuFrameLayout->addWidget(backToLoginButton);

    menuLayout->addStretch();
    menuLayout->addWidget(menuFrame);
    menuLayout->addStretch();

    menuPage->setLayout(menuLayout);

    QWidget *mapOptionsPage = new QWidget();
    QVBoxLayout *mapOptionsLayout = new QVBoxLayout();
    mapOptionsLayout->setAlignment(Qt::AlignCenter);

    QFrame *mapOptionsFrame = new QFrame(this);
    QVBoxLayout *mapOptionsFrameLayout = new QVBoxLayout(mapOptionsFrame);
    mapOptionsFrameLayout->setAlignment(Qt::AlignCenter);

    QPushButton *helicopterButton = new QPushButton(this);
    QPushButton *boatButton = new QPushButton(this);
    QPushButton *carButton = new QPushButton(this);
    QPushButton *backToMenuButton = new QPushButton("BACK", this);

    helicopterButton->setFixedSize(450, 220);
    boatButton->setFixedSize(450, 220);
    carButton->setFixedSize(450, 220);
    backToMenuButton->setFixedSize(100, 50);

    helicopterButton->setStyleSheet(
        "QPushButton {"
        "    background-image: url(../ModernDreamImages/helicopterMap.jpeg);"
        "    background-color: transparent;"
        "    border: 2px solid #BF00FF;"
        "    border-radius: 5px;"
        "    font-size: 14px;"
        "    font-family: 'Italic';"
        "    font-weight: bold;"
        "    text-align: center;"
        "    color: #BF00FF;"
        "    background-size: contain;"
        "    background-repeat: no-repeat;"
        "    background-position: center;"
        "} "
        "QPushButton:hover {"
        "    background-color: rgba(191, 0, 255, 0.2);"
        "} "
        "QPushButton:pressed {"
        "    background-color: rgba(191, 0, 255, 0.4);"
        "}");
    boatButton->setStyleSheet(
        "QPushButton {"
        "    background-image: url(../ModernDreamImages/boatMap2.jpg);"
        "    background-color: transparent;"
        "    border: 2px solid #BF00FF;"
        "    border-radius: 5px;"
        "    font-size: 14px;"
        "    font-family: 'Italic';"
        "    font-weight: bold;"
        "    text-align: center;"
        "    color: #BF00FF;"
        "    background-size: contain;"
        "    background-repeat: no-repeat;"
        "    background-position: center;"
        "} "
        "QPushButton:hover {"
        "    background-color: rgba(191, 0, 255, 0.2);"
        "} "
        "QPushButton:pressed {"
        "    background-color: rgba(191, 0, 255, 0.4);"
        "}");
    carButton->setStyleSheet(
        "QPushButton {"
        "    background-image: url(../ModernDreamImages/carMap.jpeg);"
        "    background-color: transparent;"
        "    border: 2px solid #BF00FF;"
        "    border-radius: 5px;"
        "    font-size: 14px;"
        "    font-family: 'Italic';"
        "    font-weight: bold;"
        "    text-align: center;"
        "    color: #BF00FF;"
        "    background-size: contain;"
        "    background-repeat: no-repeat;"
        "    background-position: center;"
        "} "
        "QPushButton:hover {"
        "    background-color: rgba(191, 0, 255, 0.2);"
        "} "
        "QPushButton:pressed {"
        "    background-color: rgba(191, 0, 255, 0.4);"
        "}");
    backToMenuButton->setStyleSheet(
        "QPushButton {"
        "    background-color: black;"
        "    color: #BF00FF;"
        "    border: 2px solid #BF00FF;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "    font-size: 12px;"
        "    font-family: 'Italic';"
        "    font-weight: bold;"
        "}");

    mapOptionsFrameLayout->addWidget(helicopterButton);
    mapOptionsFrameLayout->addWidget(boatButton);
    mapOptionsFrameLayout->addWidget(carButton);
    mapOptionsFrameLayout->addWidget(backToMenuButton);

    mapOptionsLayout->addStretch();
    mapOptionsLayout->addWidget(mapOptionsFrame);
    mapOptionsLayout->addStretch();

    mapOptionsPage->setLayout(mapOptionsLayout);

    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(menuPage);
    stackedWidget->addWidget(mapOptionsPage);

    mainLayout->addWidget(stackedWidget);
    setLayout(mainLayout);

    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::OnLogin);
    connect(registerButton, &QPushButton::clicked, this, &LoginDialog::OnRegister);

    connect(singlePlayer, &QPushButton::clicked, [=]()
            { stackedWidget->setCurrentIndex(2); });
    connect(multiPlayer, &QPushButton::clicked, [=]()
            { stackedWidget->setCurrentIndex(2); });
    connect(backToLoginButton, &QPushButton::clicked, [=]()
            { stackedWidget->setCurrentWidget(loginPage); });

    connect(helicopterButton, &QPushButton::clicked, this, &LoginDialog::onHelicopterSelected);
    connect(boatButton, &QPushButton::clicked, this, &LoginDialog::onBoatSelected);
    connect(carButton, &QPushButton::clicked, this, &LoginDialog::onCarSelected);
    connect(backToMenuButton, &QPushButton::clicked, [=]()
            { stackedWidget->setCurrentWidget(menuPage); });

    connect(httpClient, &HttpClient::loginSuccess, this, &LoginDialog::onLoginSuccess);
    connect(httpClient, &HttpClient::registerSuccess, this, &LoginDialog::onRegisterSuccess);
    connect(httpClient, &HttpClient::loginFailure, this, &LoginDialog::onLoginFailure);
    connect(httpClient, &HttpClient::registerFailure, this, &LoginDialog::onRegisterFailure);
    connect(this, &LoginDialog::switchToWaitingRoom, modernDreamClient, &ModernDreamClient::onJoinGameSuccess);
    
   
}

LoginDialog::~LoginDialog()
{
     
}

void LoginDialog::switchToMenu()
{
    stackedWidget->setCurrentIndex(1);
}

void LoginDialog::OnLogin()
{
    QString username = usernameEdit->text();

    if (username.isEmpty())
    {
        showMessageDialog("Username cannot be empty", "red");
        return;
    }

    httpClient->login(username);
}

void LoginDialog::OnRegister()
{
    QString username = usernameEdit->text();

    if (username.isEmpty())
    {
        showMessageDialog("Username cannot be empty", "red");
        return;
    }

    httpClient->registerUser(username);
}


void LoginDialog::onHelicopterSelected()
{
    QString username = usernameEdit->text();
    if (username.isEmpty()) {
        showMessageDialog("Username cannot be empty", "red");
        return;
    }

    if (!modernDreamClient) {
        qDebug() << "Error: modernDreamClient is nullptr!";
        return;
    }

    connect(httpClient, &HttpClient::gameReady, modernDreamClient, &ModernDreamClient::onGameReady);

    modernDreamClient->OnStartGame(GameMap::HELICOPTER, username);
    modernDreamClient->show();
    this->hide();
}


void LoginDialog::onBoatSelected()
{
    QString username = usernameEdit->text();
    if (username.isEmpty()) {
        showMessageDialog("Username cannot be empty", "red");
        return;
    }

    if (!modernDreamClient) {
        qDebug() << "Error: modernDreamClient is nullptr!";
        return;
    }

    connect(httpClient, &HttpClient::gameReady, modernDreamClient, &ModernDreamClient::onGameReady);

    modernDreamClient->OnStartGame(GameMap::BOAT, username);
    modernDreamClient->show();
    this->hide();

}

void LoginDialog::onCarSelected()
{

    QString username = usernameEdit->text();
    if (username.isEmpty()) {
        showMessageDialog("Username cannot be empty", "red");
        return;
    }

    if (!modernDreamClient) {
        qDebug() << "Error: modernDreamClient is nullptr!";
        return;
    }

    connect(httpClient, &HttpClient::gameReady, modernDreamClient, &ModernDreamClient::onGameReady);

    modernDreamClient->OnStartGame(GameMap::CAR, username);
    modernDreamClient->show();
    this->hide();
    
}

void LoginDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QPixmap backgroundImage;
    if (stackedWidget->currentIndex() == 0)
    {
        backgroundImage.load("../ModernDreamImages/Titans1.jpg");
    }
    else if (stackedWidget->currentIndex() == 1)
    {
        backgroundImage.load("../ModernDreamImages/meniu1.jpeg");
    }
    else if (stackedWidget->currentIndex() == 2)
    {
        backgroundImage.load("../ModernDreamImages/meniu1.jpeg");
    }

    if (!backgroundImage.isNull())
    {
        painter.drawPixmap(0, -90, width(), height() + 90, backgroundImage.scaled(width(), height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }
}

void LoginDialog::onLoginSuccess(const QString &username, int score)
{
    showMessageDialog("Welcome back!", "#7fff00");
    switchToMenu();
}

void LoginDialog::onRegisterSuccess()
{
    showMessageDialog("Successfully registered!", "#7fff00");
}

void LoginDialog::onLoginFailure(const QString &error)
{
    showMessageDialog("Login failed: " + error, "red");
}

void LoginDialog::onRegisterFailure(const QString &error)
{
    showMessageDialog("Registration failed: " + error, "red");
}

void LoginDialog::showMessageDialog(const QString &message, const QString &color)
{
    QDialog messageDialog(this);
    messageDialog.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    messageDialog.setStyleSheet("background-color: black;");

    QHBoxLayout *layout = new QHBoxLayout(&messageDialog);

    QLabel *messageLabel = new QLabel(message, &messageDialog);
    messageLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    messageLabel->setStyleSheet("color: " + color + "; font-weight: bold; font-size: 14px;");

    QPushButton *closeButton = new QPushButton("OK", &messageDialog);
    closeButton->setStyleSheet(
        "QPushButton {"
        "    background-color: white;"
        "    color: black;"
        "    border: 2px solid black;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "    font-family: 'Italic';"
        "    font-weight: bold;"
        "}");

    QObject::connect(closeButton, &QPushButton::clicked, &messageDialog, &QDialog::accept);

    layout->addWidget(messageLabel);
    layout->addWidget(closeButton, 0, Qt::AlignCenter);

    messageDialog.setLayout(layout);

    messageDialog.exec();
}
