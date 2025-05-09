
#include "ModernDreamClient.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QJsonArray>
#include <QKeyEvent>
#include <QDockWidget>

ModernDreamClient::ModernDreamClient(QWidget* parent)
    : QMainWindow(parent), mainStack(new QStackedWidget(this)), httpClient(new HttpClient(this)) {
    resize(1400, 800);

    setupWaitingRoom();
    qDebug() << "setupWaitingRoom initialized waitingRoomWidget:" << waitingRoomWidget;
    
    tabWidget = new QTabWidget(this);
    QWidget* gameSetupTab = new QWidget();
    QVBoxLayout* setupLayout = new QVBoxLayout(gameSetupTab);

    playerCountSpinBox = new QSpinBox(this);
    playerCountSpinBox->setRange(1, 4);

    startGameButton = new QPushButton("Start Game", this);

    setupLayout->addWidget(playerCountSpinBox);
    setupLayout->addWidget(startGameButton);

    tabWidget->addTab(gameSetupTab, "Game Setup");
    mainStack->addWidget(tabWidget);
    qDebug() << "tabWidget created and added to mainStack:" << tabWidget;

    
    mainStack->setCurrentWidget(tabWidget);
    setCentralWidget(mainStack);

    qDebug() << "HttpClient instance:" << httpClient;

    
    connect(startGameButton, &QPushButton::clicked, this, [this]() {
        OnStartGame(currentMap, currentUsername);
        });

    
    connect(httpClient, &HttpClient::joinGameSuccess, this, &ModernDreamClient::onJoinGameSuccess);
    connect(httpClient, &HttpClient::gameReady, this, &ModernDreamClient::onGameReady); 
    connect(httpClient, &HttpClient::playerJoined, this, &ModernDreamClient::onPlayerJoined);
    connect(httpClient, &HttpClient::playerLeft, this, &ModernDreamClient::onPlayerLeft);


    connect(httpClient, &HttpClient::queueJoinedSuccess, this, [this](const QString& sessionId) {
        currentSessionId = sessionId;
        qDebug() << "[DEBUG] User connected to session:" << sessionId;
        if (sessionId.isEmpty()) {
            qDebug() << "[ERROR] Received empty session ID.";
            return;
        }
        mainStack->setCurrentWidget(waitingRoomWidget);
        waitingStatusLabel->setText("Waiting for players...");
        });
    qDebug() << "ModernDreamClient initialized successfully.";
}


ModernDreamClient::~ModernDreamClient() {
    qDebug() << "ModernDreamClient destroyed.";
  
}

void ModernDreamClient::setupWaitingRoom() {
    waitingRoomWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(waitingRoomWidget);
    layout->setAlignment(Qt::AlignCenter);

 
    QFrame* waitingFrame = new QFrame();
    waitingFrame->setStyleSheet(
        "QFrame {"
        "    background-color: rgba(30, 30, 30, 0.8);"
        "    border: 2px solid #BF00FF;"
        "    border-radius: 15px;"
        "    padding: 20px;"
        "}");
    QVBoxLayout* frameLayout = new QVBoxLayout(waitingFrame);


    waitingStatusLabel = new QLabel("Waiting for players...");
    waitingStatusLabel->setAlignment(Qt::AlignCenter);
    waitingStatusLabel->setStyleSheet(
        "color: #BF00FF;"
        "font-size: 24px;"
        "font-weight: bold;"
        "margin-bottom: 20px;"
    );


    playerProgress = new QProgressBar();
    playerProgress->setStyleSheet(
        "QProgressBar {"
        "    border: 2px solid #BF00FF;"
        "    border-radius: 5px;"
        "    text-align: center;"
        "    height: 25px;"
        "    margin: 10px 0px;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #BF00FF;"
        "}"
    );


    playerList = new QListWidget();
    playerList->setStyleSheet(
        "QListWidget {"
        "    border: 2px solid #BF00FF;"
        "    border-radius: 5px;"
        "    background: rgba(0, 0, 0, 0.7);"
        "    min-height: 200px;"
        "}"
        "QListWidget::item {"
        "    color: white;"
        "    padding: 10px;"
        "    margin: 5px;"
        "    background: rgba(191, 0, 255, 0.2);"
        "    border-radius: 5px;"
        "}"
    );


    QPushButton* leaveButton = new QPushButton("Leave Game");
    leaveButton->setStyleSheet(
        "QPushButton {"
        "    background-color: black;"
        "    color: #BF00FF;"
        "    border: 2px solid #BF00FF;"
        "    border-radius: 5px;"
        "    padding: 10px 20px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    margin-top: 20px;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(191, 0, 255, 0.2);"
        "}"
    );

    frameLayout->addWidget(waitingStatusLabel);
    frameLayout->addWidget(playerProgress);
    frameLayout->addWidget(playerList);
    frameLayout->addWidget(leaveButton);

    layout->addWidget(waitingFrame);

    connect(leaveButton, &QPushButton::clicked, this, &ModernDreamClient::onLeaveGame);

    mainStack->addWidget(waitingRoomWidget);
}

void ModernDreamClient::OnStartGame(GameMap mapType, const QString& username) {
    currentUsername = username;
    currentMap = mapType;

    QString mapTypeStr;
    switch (mapType) {
    case GameMap::CAR: mapTypeStr = "car"; break;
    case GameMap::HELICOPTER: mapTypeStr = "helicopter"; break;
    case GameMap::BOAT: mapTypeStr = "boat"; break;
    }
   
    mainStack->setCurrentWidget(waitingRoomWidget);
    waitingStatusLabel->setText("Waiting for players...");
    playerProgress->setValue(0);

   
    httpClient->joinQueue(username, 100);  
    startMatchmaking(username, 100, mapType);
}


void ModernDreamClient::onJoinGameSuccess(const QString& sessionId, int current, int required) {
    qDebug() << "onJoinGameSuccess called with sessionId:" << sessionId
        << "current:" << current << "required:" << required;
    currentSessionId = sessionId;
    updateWaitingRoom(current, required);
    playerList->addItem(currentUsername + " (You)");
    if (!waitingRoomWidget) {
        qDebug() << "Error: waitingRoomWidget is nullptr!";
        return;
    }

    
    if (mainStack->indexOf(waitingRoomWidget) == -1) {
        qDebug() << "Adding waitingRoomWidget to mainStack.";
        mainStack->addWidget(waitingRoomWidget);
    }
    mainStack->setCurrentWidget(waitingRoomWidget);
}

void ModernDreamClient::onPlayerJoined(const QString& username, int current, int required) {
    qDebug() << "Player joined:" << username;
    bool exists = false;
    for (int i = 0; i < playerList->count(); ++i) {
        if (i >= playerList->count()) {
            qDebug() << "[ERROR] Index out of range: i =" << i;
            continue;
        }
        if (playerList->item(i)->text() == username) {
            exists = true;
            break;
        }

        if (!exists) {
            playerList->addItem(username);
        }
        updateWaitingRoom(current, required);

    }
}

void ModernDreamClient::onPlayerLeft(const QString& username, int current, int required) {
    updateWaitingRoom(current, required);
    for (int i = 0; i < playerList->count(); i++) {
        if (playerList->item(i)->text().startsWith(username)) {
            delete playerList->takeItem(i);
            break;
        }
    }
}

void ModernDreamClient::updateWaitingRoom(int current, int required) {
    waitingStatusLabel->setText(QString("Waiting for players... (%1/%2)").arg(current).arg(required));
    playerProgress->setMaximum(required);
    playerProgress->setValue(current);
}

void ModernDreamClient::onGameReady(const QString& sessionId, const QJsonArray& players) {
    static bool gameStarted = false;
    if (gameStarted) {
        qDebug() << "Game already started, ignoring additional calls.";
        return;
    }
    gameStarted = true;

    qDebug() << "Game is ready. Session ID:" << sessionId;

   
    if (!gameMapWidget)
    {
        gameMapWidget = new GameMapWidget(sessionId, currentUsername, this);
    }
    gameMapWidget->show();

    
    this->hide();   
  
}


void ModernDreamClient::onLeaveGame() 
{
    if (!currentSessionId.isEmpty()) {
        httpClient->leaveGame(currentSessionId);
        mainStack->setCurrentWidget(tabWidget);
        playerList->clear();
        waitingStatusLabel->setText("Waiting for players...");
        playerProgress->setValue(0);
    }
}




void ModernDreamClient::startMatchmaking(const QString& username, int score, GameMap mapType) {
    QString mapTypeStr;
    switch (mapType) {
    case GameMap::CAR: mapTypeStr = "car"; break;
    case GameMap::HELICOPTER: mapTypeStr = "helicopter"; break;
    case GameMap::BOAT: mapTypeStr = "boat"; break;
    }

    qDebug() << "[DEBUG] Starting matchmaking for username:" << username
        << ", score:" << score
        << ", mapType:" << mapTypeStr;

    QTimer* matchmakingTimer = new QTimer(this);
    connect(matchmakingTimer, &QTimer::timeout, [this, matchmakingTimer]() {
        QJsonObject status = httpClient->checkMatchStatus(currentSessionId);

        QString sessionId = currentSessionId;  
        if (!status["sessionId"].toString().isEmpty()) {
            sessionId = status["sessionId"].toString();
        }
        if (status.contains("players")) {
            QJsonArray players = status["players"].toArray();

            playerList->clear();
            for (const auto& player : players) {
                playerList->addItem(player.toString());
            }
        }

        if (status.contains("status") && status["status"].toString() == "ready") {
            matchmakingTimer->stop();
            if (sessionId.isEmpty()) {
                qDebug() << "[ERROR] Received empty sessionId in matchmaking status.";
                return;
            }
            QString sessionId = status["sessionId"].toString();
            onGameReady(sessionId, status["players"].toArray());
        }
        else {
            qDebug() << "[DEBUG] Matchmaking status:" << status;
        }
        });
    matchmakingTimer->start(5000);  

}





