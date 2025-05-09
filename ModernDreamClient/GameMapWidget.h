
#pragma once
#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <QPixmap>
#include <QPair>
#include <QKeyEvent>
#include <QTableWidget>
#include "HttpClient.h"
#include "BulletInfo.h"

class GameMapWidget : public QMainWindow {
    Q_OBJECT
public:
    explicit GameMapWidget(const QString& sessionId, const QString& username, QWidget* parent = nullptr);


public slots:

    void updatePlayerPosition(int x, int y); 


public:
    void syncBullets(const QString& sessionId);
    void shootBullet(const QString& direction);
    void updateWalls();
    void syncPlayers();
    void movePlayer(const QString& direction);
    void fetchAndInitializeMap();
    void setupTextures();
    void setupConnections();
    void displayMap();

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;


private:
    QString sessionId;
    QString username;
    HttpClient* httpClient;
    QVector<QVector<int>> mapData;
    QPixmap wallTexture, bombTexture, bulletTexture;
    QVector<QPixmap> playerTextures;
    QSharedPointer<QVector<BulletInfo>> bullets;
    QString currentDirection;
    QHash<QString, int> playerScores;
    bool isUpdating = false;
    QMutex bulletsMutex;
    QVector<QPair<int, int>> updatedCells;
    QMap<QString, QPoint> playerPositions;
    QTransform transform;
    QMutex mapMutex;

};
