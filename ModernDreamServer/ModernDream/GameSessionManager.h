#pragma once
#include <string>
#include <map>
#include <memory>
#include <mutex>
#include "Game.h"
#include <chrono>


struct GameSession {
    std::string sessionId;
    int requiredPlayers;
    std::map<std::string, std::unique_ptr<Player>> players;
    bool isReady = false;

    std::string lastJoinedPlayer;
    std::string lastLeftPlayer;

    Game game; 
    GameSession(const std::string& id, int reqPlayers)
        : sessionId(id), requiredPlayers(reqPlayers), game() {
        game.GenerateMap(requiredPlayers);
    }

    GameSession(GameSession&& other) noexcept = default;
    GameSession& operator=(GameSession&& other) noexcept = default;


    const Map& GetMap() const {
        return game.GetMap(); 
    }

    Game& GetGame() { return game; }

    Player* GetPlayerByUsername(const std::string& username) {
        if (auto it = players.find(username); it != players.end()) {
            std::cout << std::format("[DEBUG] Player found: {}\n", username);
            return it->second.get();
        }
        std::cerr << std::format("[DEBUG] Player not found: {}\n", username);
        return nullptr;
    }


};

struct WaitingPlayer {
    std::string username;
    int score;
    std::chrono::time_point<std::chrono::steady_clock> joinTime;
    WaitingPlayer(const std::string& name, int scr)
        : username(name), score(scr), joinTime(std::chrono::steady_clock::now()) {
    }
};

class GameSessionManager {
private:
    std::map<std::string, std::shared_ptr<GameSession>> sessions;
    std::deque<std::unique_ptr<WaitingPlayer>> waitingQueue;

    std::mutex sessionMutex;

public:
    GameSessionManager() = default;

    std::string CreateSession(int requiredPlayers);

    bool JoinSession(const std::string& sessionId, const std::string& username);

    void LeaveSession(const std::string& sessionId, const std::string& username);

    std::shared_ptr<GameSession> GetSession(const std::string& sessionId);

    const GameSession& GetSessionStatus(const std::string& sessionId) const;

    std::map<std::string, std::shared_ptr<GameSession>>& GetSessions();
    void MatchPlayers();
    void CreateMatch(std::array<std::unique_ptr<WaitingPlayer>, 4> players);
    void AddToQueue(const std::string& username, int score);
    void ManageSession(const std::string& sessionId);
    std::string FindOrCreateSession(const std::string& username, int score);

    
};
