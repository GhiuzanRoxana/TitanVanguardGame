#include "GameSessionManager.h"
#include <stdexcept>
#include <random>
#include <iostream>
#include "Game.h"




std::string GameSessionManager::CreateSession(int requiredPlayers) {
    requiredPlayers = 2;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000000);
    std::string sessionId = std::to_string(dis(gen));  

    std::uniform_int_distribution<> seedDis(1, 1000000); 
    int seed = seedDis(gen);


    std::shared_ptr<GameSession> session = std::make_shared<GameSession>(sessionId, requiredPlayers);

    sessions[sessionId] = session;

    std::cout << "Created session with ID: " << sessionId << " and " << requiredPlayers << " required players.\n";
    return sessionId;
}


bool GameSessionManager::JoinSession(const std::string& sessionId, const std::string& username) {
    auto it = sessions.find(sessionId);
    if (it != sessions.end() && !it->second->isReady) {
        auto session = it->second;

        if (session->players.contains(username)) {
            std::cerr << std::format("Player {} is already in the session.\n", username);
            return false;
        }

        auto player = std::make_unique<Player>(username, std::make_unique<Weapon>(), std::make_pair(0, 0));
        session->players[username] = std::move(player);

        session->lastJoinedPlayer = username;
        session->lastLeftPlayer.clear();

        std::cout << std::format("Player {} joined session {}\n", username, sessionId);

        if (session->players.size() >= session->requiredPlayers) {
            session->isReady = true;
        }
        return true;
    }
        else {
            std::cerr << "Jucătorul " << username << " este deja în sesiune.\n";
        }
    
    std::cerr << std::format("Session {} not found or already full.\n", sessionId);
    return false;
   
}




void GameSessionManager::CreateMatch(std::array<std::unique_ptr<WaitingPlayer>, 4> players) {
    std::string sessionId = CreateSession(players.size());
    auto session = GetSession(sessionId);

    std::array<std::unique_ptr<Player>, 4> gamePlayers;
    int index = 0;

    for (auto& player : players) {
        if (player) {
            JoinSession(sessionId, player->username);
            auto weapon = std::make_unique<Weapon>();
            gamePlayers[index++] = std::make_unique<Player>(player->username, std::move(weapon), std::make_pair(0, 0));
        }
    }

    session->game = Game(Map(), session->players);




    session->isReady = true;

    std::cout << "Game session " << sessionId << " created with players: ";
    for (const auto& player : players) {
        if (player) {
            std::cout << player->username << " ";
        }
    }
    std::cout << std::endl;
}


void GameSessionManager::LeaveSession(const std::string& sessionId, const std::string& username) {
    auto it = sessions.find(sessionId);
    if (it != sessions.end()) {
        auto session = it->second;

        session->players.erase(username);
        session->lastLeftPlayer = username;
        session->lastJoinedPlayer = "";

        std::cout << std::format("Player {} left session {}\n", username, sessionId);

        if (session->players.size() < session->requiredPlayers) {
            session->isReady = false;
        }
    }
}


std::shared_ptr<GameSession> GameSessionManager::GetSession(const std::string& sessionId) {
    auto it = sessions.find(sessionId);
    if (it != sessions.end()) {
        return it->second;
    }
    return nullptr;
}


const GameSession& GameSessionManager::GetSessionStatus(const std::string& sessionId) const {
    auto it = sessions.find(sessionId);
    if (it != sessions.end()) {
        return *(it->second);
    }
    throw std::out_of_range("Session not found\n");
}

std::map<std::string, std::shared_ptr<GameSession>>& GameSessionManager::GetSessions()
{
    return sessions;
}

void GameSessionManager::AddToQueue(const std::string& username, int score) {
    auto player = std::make_unique<WaitingPlayer>(username, score); 
    waitingQueue.push_back(std::move(player));
    std::cout << "Player " << username << " added to queue with score: " << score << std::endl;
}


void GameSessionManager::MatchPlayers() {
    auto now = std::chrono::steady_clock::now();

    while (waitingQueue.size() >= 2) {
        std::array<std::unique_ptr<WaitingPlayer>, 4> selectedPlayers = { nullptr, nullptr, nullptr, nullptr };
        size_t index = 0;

        for (auto it = waitingQueue.begin(); it != waitingQueue.end() && index < 4; ) {
            if (std::chrono::duration_cast<std::chrono::seconds>(now - (*it)->joinTime).count() >= 30 || index < 4) {
                std::cout << "[INFO] Adding player to session: " << (*it)->username << std::endl;
                selectedPlayers[index++] = std::move(*it); 
                it = waitingQueue.erase(it);
            }
            else {
                ++it;
            }
        }

        if (index >= 2) {
            CreateMatch(std::move(selectedPlayers));
        }
    }
}


void GameSessionManager::ManageSession(const std::string& sessionId) {
    std::shared_ptr<GameSession> session;
    {
        std::lock_guard<std::mutex> lock(sessionMutex);
        auto it = sessions.find(sessionId);
        if (it == sessions.end()) {
            std::cerr << "Session " << sessionId << " not found." << std::endl;
            return;
        }
        session = it->second;
    }

    std::cout << "Game session " << sessionId << " started." << std::endl;
    auto& game = session->GetGame();

    game.DetermineWinner();

    {
        std::lock_guard<std::mutex> lock(sessionMutex);
        sessions.erase(sessionId);
    }

    std::cout << "Game session " << sessionId << " ended." << std::endl;
}

std::string GameSessionManager::FindOrCreateSession(const std::string& username, int score) {
    auto now = std::chrono::steady_clock::now();

    for (auto& [id, session] : sessions) {
        if (!session->isReady && session->players.size() < session->requiredPlayers) {
            JoinSession(id, username);
            return id;
        }
    }

    std::string newSessionId = CreateSession(4);  
    JoinSession(newSessionId, username);
    return newSessionId;
}

