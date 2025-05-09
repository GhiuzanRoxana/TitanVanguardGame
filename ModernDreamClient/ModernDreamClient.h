
#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QTabWidget>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QListWidget>
#include <QLabel>
#include <QString>
#include "HttpClient.h"
#include "GameMapWidget.h"
enum class GameMap {
    CAR,
    HELICOPTER,
    BOAT
};

class ModernDreamClient : public QMainWindow {
    Q_OBJECT

public:
    explicit ModernDreamClient(QWidget* parent = nullptr);
    ~ModernDreamClient();



public:
    void setupWaitingRoom();


public slots:
    void OnStartGame(GameMap mapType, const QString& username);
    void onJoinGameSuccess(const QString& sessionId, int current, int required);
    void onPlayerJoined(const QString& username, int current, int required);
    void onPlayerLeft(const QString& username, int current, int required);
    void onGameReady(const QString& sessionId, const QJsonArray& players);
    void onLeaveGame();
    void updateWaitingRoom(int current, int required);
    void startMatchmaking(const QString& username, int score, GameMap mapType);

private:
    QTabWidget* tabWidget;          
    QSpinBox* playerCountSpinBox;   
    QComboBox* mapComboBox;        
    QPushButton* startGameButton;  

      
    QWidget* waitingRoomWidget;               

    QLabel* waitingStatusLabel;  
    QProgressBar* playerProgress;   
    QListWidget* playerList;        

    QString currentUsername;
    QString currentSessionId;       
    GameMap currentMap;             

    HttpClient* httpClient;        

    QStackedWidget* mainStack;
    GameMapWidget* gameMapWidget;
    QString currentDirection;

};