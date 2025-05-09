#include <iostream>
#include <conio.h>
#include <thread>
#include <vector> 
#include <memory>
#include <chrono>

#include "DatabaseManager.h"
#include "Routing.h"
#include "../MapGenerator/MapGenerator/MapGenerator.h"

int main() {

    DatabaseManager dbManager;
    http::Routing routing;
    routing.Run(dbManager);
    return 0;
}

