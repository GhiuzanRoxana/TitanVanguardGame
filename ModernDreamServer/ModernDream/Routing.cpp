#include "Routing.h"
#include <vector>
#include <regex>
#include <iostream>


using namespace http;



Routing::Routing()
    : m_db(DatabaseManager()) {}

void Routing::Run(DatabaseManager& storage) {

    CROW_ROUTE(m_app, "/login/<string>")
        ([this](const crow::request& req, std::string username) {
        return LoginRoute(req, username);
            });


    CROW_ROUTE(m_app, "/register")
        .methods("POST"_method)([this](const crow::request& req) {
        return RegisterRoute(req);
            });


    CROW_ROUTE(m_app, "/game/create").methods("POST"_method)([this](const crow::request& req) {
        return CreateSessionRoute(req);
        });

    CROW_ROUTE(m_app, "/game/join").methods("POST"_method)([this](const crow::request& req) {
        return JoinSessionRoute(req);
        });

    CROW_ROUTE(m_app, "/game/leave").methods("POST"_method)([this](const crow::request& req) {
        return LeaveSessionRoute(req);
        });

    
    CROW_ROUTE(m_app, "/game/status/<string>")
        .methods("GET"_method)([this](std::string sessionId) {
        return GetSessionStatusRoute(sessionId);
            });

    CROW_ROUTE(m_app, "/generateMap")
        .methods("POST"_method)([this](const crow::request& req) {
        return GenerateMapRoute(req); 
            });

    CROW_ROUTE(m_app, "/game/move").methods("POST"_method)([this](const crow::request& req) {
        return MovePlayerRoute(req);
        });

  

    CROW_ROUTE(m_app, "/game/syncBullets").methods("POST"_method)([this](const crow::request& req) {
        return SyncBulletsRoute(req, m_gameSessionManager);
        });


    CROW_ROUTE(m_app, "/game/shoot").methods("POST"_method)([this](const crow::request& req) {
        return ShootBulletRoute(req, m_gameSessionManager);
        });

    CROW_ROUTE(m_app, "/game/updateWalls").methods("POST"_method)([this](const crow::request& req) {
        return UpdateWallsRoute(req, m_gameSessionManager);
        });


    std::thread matchingThread([this]() {
        while (true) {
            m_gameSessionManager.MatchPlayers();
            std::this_thread::sleep_for(std::chrono::seconds(1));  
        }
        });
    matchingThread.detach();

    CROW_ROUTE(m_app, "/game/joinQueue").methods("POST"_method)([this](const crow::request& req) {
         return  UpdateJoinQueueRoute(req, m_gameSessionManager);
        });


    CROW_ROUTE(m_app, "/matchmaking/queue").methods("POST"_method)([this](const crow::request& req) {
        return HandleQueueRoute(req, m_gameSessionManager);
        });



    CROW_ROUTE(m_app, "/matchmaking/status/<string>").methods("GET"_method)([this](const std::string& sessionId) {
        return HandleMatchmakingStatusRoute(sessionId, m_gameSessionManager);
        });
    
 


    CROW_ROUTE(m_app, "/game/syncPlayers").methods("POST"_method)([this](const crow::request& req) {
         return HandleSyncPlayersRoute(req, m_gameSessionManager);
    });




    m_app.port(8080).multithreaded().run();
}

crow::response Routing::LoginRoute(const crow::request& req, const std::string& username) {
    try {
        auto user = m_db.GetUser(username);

        if (!user) {
            crow::json::wvalue errorResponse;
            errorResponse["error"] = "User not found";
            errorResponse["username"] = username;
            return crow::response(404, errorResponse);
        }

        crow::json::wvalue response;
        response["username"] = user->username;
        response["score"] = user->score;
        return crow::response(response);
    }
    catch (const std::system_error& e) {
        crow::json::wvalue errorResponse;
        errorResponse["error"] = "Internal Server Error";
        errorResponse["details"] = e.what();
        return crow::response(500, errorResponse);
    }
}

crow::response Routing::RegisterRoute(const crow::request& req) {
    crow::json::rvalue data = crow::json::load(req.body);
    if (!data) {
        std::cout << "Invalid JSON received.\n";
        return crow::response(400, "Invalid JSON\n");
    }

    std::string username = data["username"].s();
    std::cout << "Attempting to register username: " << username << std::endl;

    auto existingUser = m_db.GetUser(username);
    if (existingUser) {
        std::cout << "Username already exists: " << username << std::endl;
        return crow::response(409, "Username Already Exists\n");
    }

    DataUser newUser{ username };
    m_db.AddUser(newUser);
    std::cout << "User registered successfully: " << username << std::endl;
    return crow::response(200, "User registered successfully\n");
}

crow::response Routing::CreateSessionRoute(const crow::request& req) {
    crow::json::rvalue data = crow::json::load(req.body);
    if (!data) {
        return crow::response(400, "Invalid JSON");
    }

    int requiredPlayers = data["requiredPlayers"].i();
    std::string sessionId = m_gameSessionManager.CreateSession(requiredPlayers);

    crow::json::wvalue response;
    response["sessionId"] = sessionId;
    response["requiredPlayers"] = requiredPlayers;

    return crow::response(200, response);
}




crow::response Routing::JoinSessionRoute(const crow::request& req) {
    auto json = crow::json::load(req.body);

    if (!json) {
        CROW_LOG_ERROR << "Invalid JSON format";
        return crow::response(400, "Invalid JSON format");
    }

    if (!json.has("username") || !json.has("sessionId")) {
        CROW_LOG_ERROR << "Missing required parameters: username or sessionId";
        return crow::response(400, "Missing required parameters");
    }

    std::string username = json["username"].s();
    std::string sessionId = json["sessionId"].s();

    CROW_LOG_INFO << "Join session called for username: " << username << ", sessionId: " << sessionId;

    if (m_gameSessionManager.JoinSession(sessionId, username)) {
        CROW_LOG_INFO << "Player joined session successfully: " << sessionId;

        crow::json::wvalue response;
        response["message"] = "Player joined successfully";
        response["sessionId"] = sessionId;
        response["username"] = username;

        return crow::response(200, response);
    }

    CROW_LOG_ERROR << "Failed to join session for username: " << username;
    return crow::response(400, "Failed to join session");
}



crow::response Routing::LeaveSessionRoute(const crow::request& req) {
    crow::json::rvalue data = crow::json::load(req.body);
    if (!data) {
        return crow::response(400, "Invalid JSON");
    }

    std::string sessionId = data["sessionId"].s();
    std::string username = data["username"].s();

    m_gameSessionManager.LeaveSession(sessionId, username);

    crow::json::wvalue response;
    response["message"] = "Player left successfully";
    response["sessionId"] = sessionId;
    response["username"] = username;

    return crow::response(200, response);
}


crow::response Routing::GetSessionStatusRoute(const std::string& sessionId) {
    try {
        
        const auto& session = m_gameSessionManager.GetSessionStatus(sessionId);

        crow::json::wvalue response;
        response["sessionId"] = session.sessionId;
        response["requiredPlayers"] = session.requiredPlayers;
        response["currentPlayers"] = session.players.size();


        response["status"] = session.isReady ? "ready" : "waiting";

        std::vector<std::string> playerList;
        for (const auto& [username, player] : session.players) {
            playerList.push_back(username);
        }
        response["players"] = std::move(playerList);

        if (session.lastJoinedPlayer != "") {
            response["lastJoined"] = session.lastJoinedPlayer;
        }
        if (session.lastLeftPlayer != "") {
            response["lastLeft"] = session.lastLeftPlayer;
        }

        return crow::response(200, response);
    }
    catch (const std::out_of_range&) {
        return crow::response(404, "Session not found");
    } 
}



crow::response Routing::GenerateMapRoute(const crow::request& req) {
    crow::json::rvalue data = crow::json::load(req.body);
    if (!data) {
        return crow::response(400, "Invalid JSON");
    }

    if (!data.has("sessionId")) {
        return crow::response(400, "Missing required key: 'sessionId'");
    }

    std::string sessionId = data["sessionId"].s();

    auto session = m_gameSessionManager.GetSession(sessionId);
    if (!session) {
        return crow::response(404, "Session not found");
    }

    const Map& map = session->GetGame().GetMap();

    crow::json::wvalue response;
    std::vector<crow::json::wvalue> mapJson;
    for (const auto& row : map.GetMapMatrix()) {
        std::vector<crow::json::wvalue> rowJson;
        for (int val : row) {
            rowJson.push_back(crow::json::wvalue(val));
        }
        mapJson.push_back(crow::json::wvalue(rowJson));
    }

    response["map"] = crow::json::wvalue(mapJson);

    auto startPositions = map.GetPlayerStartPositions();
    int i = 0;
    for (auto& player : session->players) {
        if (i < startPositions.size()) {
            player.second->SetPosition(startPositions[i]);
            std::cout << "Player " << player.first << " placed at ("
                << startPositions[i].first << ", " << startPositions[i].second << ")\n";
            ++i;
        }
    }

    std::cout << "JSON response size: " << mapJson.size() << " rows, "
        << (mapJson.empty() ? 0 : mapJson[0].size()) << " columns\n";

    return crow::response(200, response);
}




crow::response Routing::MovePlayerRoute(const crow::request& req) {
    auto json = crow::json::load(req.body);
    if (!json || !json.has("sessionId") || !json.has("username") || !json.has("direction")) {
        return crow::response(400, "Invalid request: Missing required fields.");
    }

    std::string sessionId = json["sessionId"].s();
    std::string username = json["username"].s(); 
    std::string directionStr = json["direction"].s();
    char direction = directionStr[0];

    auto session = m_gameSessionManager.GetSession(sessionId);
    if (!session) {
        return crow::response(404, "Session not found.");
    }

    auto player = session->GetPlayerByUsername(username);
    if (!player) {
        return crow::response(404, "Player not found.");
    }

    player->Movement(session->GetMap(), direction);
    auto newPosition = player->GetPosition();
    session->game.UpdatePlayerPosition(username, newPosition.first, newPosition.second);

    crow::json::wvalue response;
    crow::json::wvalue::list players_list; 

    for (const auto& [name, playerObj] : session->players) {
        crow::json::wvalue player_data;
        player_data["username"] = name;
        player_data["x"] = playerObj->GetPosition().first;
        player_data["y"] = playerObj->GetPosition().second;

        players_list.push_back(std::move(player_data)); 
    }

    response["players"] = std::move(players_list); 

    return crow::response(200, response);
}


crow::response Routing::SyncBulletsRoute(const crow::request& req, GameSessionManager& gameSessionManager) {
    try {
        auto body = crow::json::load(req.body);

        if (!body || !body.has("sessionId")) {
            return crow::response(400, "Invalid request: Missing sessionId.");
        }

        std::string sessionId = body["sessionId"].s();
        auto session = gameSessionManager.GetSession(sessionId);
        if (!session) {
            return crow::response(404, "Session not found.");
        }

        session->game.UpdateBullets();

        crow::json::wvalue response;
        size_t bulletIndex = 0;
        for (const auto& bullet : session->game.GetBullets()) {
            response["bullets"][bulletIndex]["x"] = bullet.GetPosition().first;
            response["bullets"][bulletIndex]["y"] = bullet.GetPosition().second;
            response["bullets"][bulletIndex]["direction"] = std::string(1, bullet.GetDirection());
            ++bulletIndex;
        }

        return crow::response(200, response);
    }
    catch (const std::exception& e) {
        CROW_LOG_ERROR << "Error in /game/syncBullets: " << e.what();
        return crow::response(500, "Error in /game/syncBullets: " + std::string(e.what()));
    }
}


crow::response Routing::ShootBulletRoute(const crow::request& req, GameSessionManager& gameSessionManager) {
    try {
        auto body = crow::json::load(req.body);

        if (!body || !body.has("sessionId") || !body.has("username") || !body.has("direction")) {
            return crow::response(400, "Invalid request: Missing required fields.");
        }

        std::string sessionId = body["sessionId"].s();
        std::string username = body["username"].s();

        std::string directionString = body["direction"].s();
        if (directionString.empty()) {
            return crow::response(400, "Invalid request: Direction is empty.");
        }

        char direction = directionString[0];

        auto session = gameSessionManager.GetSession(sessionId);
        if (!session) {
            return crow::response(404, "Session not found.");
        }

        Player* player = session->GetPlayerByUsername(username);
        if (!player) {
            return crow::response(404, "Player not found.");
        }

        int startX = player->GetPosition().first;
        int startY = player->GetPosition().second;

        session->game.ShootBullet(*player);

        crow::json::wvalue response;
        response["startX"] = startX;
        response["startY"] = startY;
        response["direction"] = std::string(1, direction);

        return crow::response(200, response);
    }
    catch (const std::exception& e) {
        CROW_LOG_ERROR << "Error in /game/shoot: " << e.what();
        return crow::response(500, "Error: " + std::string(e.what()));
    }
}


crow::response Routing::UpdateWallsRoute(const crow::request& req, GameSessionManager& gameSessionManager) {
    try {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("sessionId")) {
            return crow::response(400, "Invalid request: Missing sessionId.");
        }

        std::string sessionId = body["sessionId"].s();
        auto session = gameSessionManager.GetSession(sessionId);
        if (!session) {
            return crow::response(404, "Session not found.");
        }

        crow::json::wvalue response;
        response["updatedCells"] = crow::json::wvalue::list();

        size_t index = 0;
        CROW_LOG_INFO << "[DEBUG] Updated walls: ";
        for (const auto& cell : session->game.GetUpdatedCells()) {
            CROW_LOG_INFO << "Cell: (" << cell.first << ", " << cell.second << ")";
        }
        for (const auto& [x, y] : session->game.GetUpdatedCells()) {
            response["updatedCells"][index]["x"] = x;
            response["updatedCells"][index]["y"] = y;
            ++index;
        }

        session->game.ClearUpdatedCells();

        return crow::response(200, response);
    }
    catch (const std::exception& e) {
        CROW_LOG_ERROR << "Error in /game/updateWalls: " << e.what();
        return crow::response(500, "Error: " + std::string(e.what()));
    }
}


crow::response Routing::UpdateJoinQueueRoute(const crow::request& req, GameSessionManager& gameSessionManager) {
    try {
        auto json = crow::json::load(req.body);
        if (!json.has("username") || !json.has("score")) {
            return crow::response(400, "Invalid request: Missing username or score.");
        }

        std::string username = json["username"].s();
        int score = json["score"].i();

        std::cout << "[INFO] Received joinQueue request. Username: " << username << ", Score: " << score << std::endl;

        gameSessionManager.AddToQueue(username, score);
        return crow::response(200, "Player added to queue.");
    }
    catch (const std::exception& e) {
        CROW_LOG_ERROR << "Error in /game/joinQueue: " << e.what();
        return crow::response(500, "Error: " + std::string(e.what()));
    }
}


crow::response Routing::HandleQueueRoute(const crow::request& req, GameSessionManager& gameSessionManager) {
    try {
        auto json = crow::json::load(req.body);

        if (!json.has("username") || !json.has("score")) {
            return crow::response(400, "Invalid request: Missing username or score.");
        }

        std::string username = json["username"].s();
        int score = json["score"].i();

        std::string sessionId = gameSessionManager.FindOrCreateSession(username, score);

        crow::json::wvalue response;
        response["status"] = "success";
        response["sessionId"] = sessionId;

        CROW_LOG_INFO << "Player " << username << " added to session " << sessionId;
        return crow::response(200, response);
    }
    catch (const std::exception& e) {
        CROW_LOG_ERROR << "Error in /matchmaking/queue: " << e.what();
        return crow::response(500, "Error: " + std::string(e.what()));
    }
}



crow::response Routing::HandleMatchmakingStatusRoute(const std::string& sessionId, GameSessionManager& gameSessionManager) {
    auto session = gameSessionManager.GetSession(sessionId);
    if (!session) {
        CROW_LOG_ERROR << "Session not found: " << sessionId;
        return crow::response(404, "Session not found.");
    }

    crow::json::wvalue response;
    response["status"] = session->isReady ? "ready" : "waiting";
    response["sessionId"] = sessionId;
    response["currentPlayers"] = session->players.size();
    response["requiredPlayers"] = session->requiredPlayers;

    crow::json::wvalue::list playersList;
    for (const auto& [username, player] : session->players) {
        playersList.push_back(username);
    }
    response["players"] = std::move(playersList);

    return crow::response(200, response);
}



crow::response Routing::HandleSyncPlayersRoute(const crow::request& req, GameSessionManager& gameSessionManager) {
    try {
        auto body = crow::json::load(req.body);

        if (!body || !body.has("sessionId")) {
            return crow::response(400, "Invalid request: Missing sessionId.");
        }

        std::string sessionId = body["sessionId"].s();
        auto session = gameSessionManager.GetSession(sessionId);
        if (!session) {
            return crow::response(404, "Session not found.");
        }

        auto& gamePlayers = session->game.GetPlayers();
        size_t index = 0;

        for (const auto& [name, playerObj] : session->players) {
            if (playerObj && index < gamePlayers.size()) {
                if (!gamePlayers[index]) {
                    gamePlayers[index] = std::make_unique<Player>(
                        playerObj->GetName(),
                        std::make_unique<Weapon>(),
                        playerObj->GetPosition()
                    );
                    CROW_LOG_INFO << "[DEBUG] Player added to Game: " << playerObj->GetName();
                }
                else {
                    gamePlayers[index]->SetPosition(playerObj->GetPosition());
                    CROW_LOG_INFO << "[DEBUG] Player updated in Game: " << playerObj->GetName();
                }
                ++index;
            }
        }

        for (size_t i = 0; i < gamePlayers.size(); ++i) {
            if (gamePlayers[i]) {
                const std::string& playerName = gamePlayers[i]->GetName();
                if (gamePlayers[i]->IsEliminated()) {
                    session->players.erase(playerName);
                    CROW_LOG_INFO << "[DEBUG] Removed eliminated player from session: " << playerName;
                }
            }
        }

        crow::json::wvalue response;
        crow::json::wvalue::list players_list;

        for (const auto& [name, playerObj] : session->players) {
            if (playerObj) {
                crow::json::wvalue player_data;
                player_data["username"] = name;
                player_data["x"] = playerObj->GetPosition().first;
                player_data["y"] = playerObj->GetPosition().second;
                player_data["score"] = playerObj->GetPoints();

                players_list.push_back(std::move(player_data));
            }
        }

        response["players"] = std::move(players_list);

        return crow::response(200, response);
    }
    catch (const std::exception& e) {
        CROW_LOG_ERROR << "Error in /game/syncPlayers: " << e.what();
        return crow::response(500, "Error: " + std::string(e.what()));
    }
}