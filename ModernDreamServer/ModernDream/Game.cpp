
#include "Game.h"


Game::Game(Map map, const std::map<std::string, std::unique_ptr<Player>>& sessionPlayers)
    : map(std::move(map)) {

    size_t index = 0;
    for (const auto& [username, playerPtr] : sessionPlayers) {
        if (index < players.size()) {
            players[index] = std::make_unique<Player>(playerPtr->GetName(),
                std::make_unique<Weapon>(),
                playerPtr->GetPosition());
            std::cout << "[DEBUG] Player added to Game: " << playerPtr->GetName() << "\n";
            ++index;
        }
        else {
            std::cerr << "[WARNING] Too many players in session. Ignoring player: " << username << "\n";
        }
    }
}


const Map& Game::GetMap() const {
    return map;
}

std::array<std::unique_ptr<Player>, 4>& Game::GetPlayers() {
    return players;
}

void Game::DetermineWinner() {
    if (std::all_of(players.begin(), players.end(), [](const std::unique_ptr<Player>& player) { return !player; })) {
        std::cout << "No players in the game." << std::endl;
        return;
    }

    std::vector<Player*> sortedPlayers;
    for (const auto& player : players) {
        if (player) {
            sortedPlayers.push_back(player.get());
        }
    }

    std::sort(sortedPlayers.begin(), sortedPlayers.end(), [](const Player* a, const Player* b) {
        return a->GetPoints() > b->GetPoints();
        });

    WinGame();
    if (sortedPlayers.size() > 1) {
        FinishSecond();
    }

    std::cout << "Game Results:" << std::endl;
    for (const auto* player : sortedPlayers) {
        std::cout << "Player " << player->GetName()
            << ": Points = " << player->GetPoints()
            << ", Score = " << player->GetScore() << std::endl;
    }
}

void Game::WinGame() {
    auto maxPlayer = std::max_element(players.begin(), players.end(),
        [](const std::unique_ptr<Player>& a, const std::unique_ptr<Player>& b) {
            return a ? a->GetScore() < (b ? b->GetScore() : 0) : false;
        });

    if (maxPlayer != players.end() && *maxPlayer) {
        (*maxPlayer)->SetPoints((*maxPlayer)->GetPoints() + 200);
        (*maxPlayer)->SetScore((*maxPlayer)->GetScore() + 2);

        std::cout << "Player " << (*maxPlayer)->GetName()
            << " won the game and received 200 bonus points and 2 score points. "
            << "Total points: " << (*maxPlayer)->GetPoints()
            << ", Total score: " << (*maxPlayer)->GetScore() << std::endl;
    }
}

void Game::FinishSecond()
{
    auto first = std::max_element(players.begin(), players.end(),
        [](const std::unique_ptr<Player>& a, const std::unique_ptr<Player>& b)
        {
            return a ? a->GetScore() < (b ? b->GetScore() : 0) : false;
        });

    auto second = std::max_element(players.begin(), players.end(),
        [&](const std::unique_ptr<Player>& a, const std::unique_ptr<Player>& b)
        {
            if (a == *first) return true;
            if (b == *first) return false;

            return a ? a->GetScore() < (b ? b->GetScore() : 0) : false;
        });

    if (second != players.end() && *second && *second != *first)
    {
        (*second)->SetScore((*second)->GetScore() + 1);
        std::cout << "Player " << (*second)->GetName()
            << " finished second and received 1 score point. "
            << "Total score: " << (*second)->GetScore() << std::endl;
    }
}

void Game::GenerateMap(uint8_t numPlayers) {
    std::cout << "[DEBUG] GenerateMap called with " << numPlayers << " players.\n";
    MapGenerator generator;
    generator.GenerateMap(numPlayers);
    map = Map(generator);  

    
    auto startPositions = map.GetPlayerStartPositions();

    int i = 0;
    for (auto& playerPtr : players) {
        if (playerPtr && i < startPositions.size()) {
            playerPtr->SetPosition(startPositions[i]);  
            playerPtr->SetInitialPosition(startPositions[i]);
            std::cout << "Jucătorul " << playerPtr->GetName() << " este plasat la ("
                << startPositions[i].first << ", " << startPositions[i].second << ")\n";
            ++i;
        }
    }
}


void Game::ShootBullet(const Player& player)
{
    auto [x, y] = player.GetPosition();
    char direction = player.GetDirection();

    Bullet newBullet({ x, y }, direction);
    bullets.push_back(newBullet);  
    std::cout << "The player " << player.GetName() << " shoot a bullet at ("
        << newBullet.GetPosition().first << ", " << newBullet.GetPosition().second << ")\n";
    bulletCounter++; 
    bulletToPlayerMap[bulletCounter] = player.GetName();

}

void Game::CheckAndApplyWeaponUpgrade()
{
    for (auto& playerPtr : players)
    {
        if (playerPtr && playerPtr->GetScore() >= 10 && !playerPtr->IsSpeedBoostUsed())
        {
            playerPtr->GetWeapon().UpgradeWaitingTime(0.5f);
            playerPtr->SetSpeedBoostUsed(true);

            std::cout << "Player " << playerPtr->GetName()
                << " has reached 10 score points and upgraded their weapon: "
                << "increased bullet speed and reduced waiting time!" << std::endl;
        }
    }
}

const std::deque<Bullet>& Game::GetBullets() const {
    return bullets;
}

const std::vector<std::pair<std::uint16_t, std::uint16_t>>& Game::GetUpdatedCells() const {
    return updatedCells;
}

void Game::ClearUpdatedCells() {
    updatedCells.clear();
}


std::map<std::string, std::pair<std::uint16_t, std::uint16_t>> Game::GetPlayerPositions() const {
    std::map<std::string, std::pair<std::uint16_t, std::uint16_t>> positions;
    for (const auto& player : players) {
        if (player) {
            positions[player->GetName()] = player->GetPosition();
            std::cout << "[DEBUG] Player: " << player->GetName()
                << " Position: (" << player->GetPosition().first
                << ", " << player->GetPosition().second << ")\n";
        }
    }
    return positions;
}

void Game::UpdatePlayerPosition(const std::string& username, std::uint16_t x, std::uint16_t y) {
    for (auto& player : players) {
        if (player && player->GetName() == username) {
            auto oldPosition = player->GetPosition();
            map.SetCellValue(oldPosition.first, oldPosition.second, MapGenerator::FreeSpace);  
            map.SetCellValue(x, y, MapGenerator::PlayerPosition);  
            player->SetPosition(std::make_pair(x, y));
            return;
        }
    }
}



void Game::UpdateBullets() {
    int mapHeight = map.GetHeight();
    int mapWidth = map.GetWidth();

    for (auto it = bullets.begin(); it != bullets.end();) {
        it->Movement(mapHeight, mapWidth);

        auto [x, y] = it->GetPosition();

        
        if (!it->GetIsActive() || x < 0 || x >= mapHeight || y < 0 || y >= mapWidth) {
            std::cout << "Bullet out of bounds at: (" << x << ", " << y << ")\n";
            it = bullets.erase(it);
            continue;
        }

        int cellValue = map.GetCellValue(x, y);
        bool bulletHandled = false;

       
        for (const auto& player : players) {
            if (player && player->GetPosition() == std::make_pair(x, y)) {
                std::cout << "Bullet hit player: " << player->GetName() << " at: (" << x << ", " << y << ")\n";
                player->Hit();

                int bulletId = std::distance(bullets.begin(), it) + 1;
                if (bulletToPlayerMap.count(bulletId)) {
                    std::string shooterName = bulletToPlayerMap[bulletId];
                    auto shooter = std::find_if(players.begin(), players.end(), [&shooterName](const auto& p) {
                        return p && p->GetName() == shooterName;
                        });
                    if (shooter != players.end() && *shooter) {
                        (*shooter)->AddPoints(100);
                        std::cout << "Shooter " << shooterName << " now has " << (*shooter)->GetScore() << " points.\n";
                    }
                    bulletToPlayerMap.erase(bulletId);
                }

                
                if (player->IsEliminated()) {
                    std::cout << "Player " << player->GetName() << " has been eliminated.\n";
                    map.SetCellValue(player->GetPosition().first, player->GetPosition().second, MapGenerator::FreeSpace);
                }
                else {
                    player->ResetPosition();
                    std::cout << "Player " << player->GetName() << " was hit and reset.\n";
                }

                it = bullets.erase(it);
                bulletHandled = true;
                break;
            }
        }
        if (bulletHandled) continue;

        
        if (cellValue == MapGenerator::DestructibleWall) {
            map.SetCellValue(x, y, MapGenerator::FreeSpace);
            std::cout << "Bullet destroyed a destructible wall at: (" << x << ", " << y << ")\n";
            updatedCells.emplace_back(x, y);
            it = bullets.erase(it);
            continue;
        }
        else if (cellValue == MapGenerator::NonDestructibleWall) {
            std::cout << "Bullet hit an indestructible wall at: (" << x << ", " << y << ")\n";
            it = bullets.erase(it);
            continue;
        }
        else if (cellValue == MapGenerator::DestructibleWallWithBomb) {
            std::cout << "Bullet hit a bomb at: (" << x << ", " << y << ")\n";
            map.SetCellValue(x, y, MapGenerator::FreeSpace);
            updatedCells.emplace_back(x, y);

            
            int explosionRadius = 10;
            for (int dx = -explosionRadius; dx <= explosionRadius; ++dx) {
                for (int dy = -explosionRadius; dy <= explosionRadius; ++dy) {
                    int newX = x + dx;
                    int newY = y + dy;

                    if (newX >= 0 && newX < mapHeight && newY >= 0 && newY < mapWidth) {
                        int affectedCellValue = map.GetCellValue(newX, newY);
                        if (affectedCellValue == MapGenerator::DestructibleWall) {
                            map.SetCellValue(newX, newY, MapGenerator::FreeSpace);
                            updatedCells.emplace_back(newX, newY);
                            std::cout << "Explosion destroyed a destructible wall at: (" << newX << ", " << newY << ")\n";
                        }
                    }
                }
            }
            it = bullets.erase(it);
            continue;
        }

        for (auto other = bullets.begin(); other != bullets.end(); ++other) {
            if (it != other && other->GetPosition() == std::make_pair(x, y)) {
                std::cout << "Bullet collided with another bullet at: (" << x << ", " << y << ")\n";
                it = bullets.erase(it);  
                bullets.erase(other);   
                bulletHandled = true;
                break;
            }
        }
        if (bulletHandled) continue;

        ++it;  
    }
}
