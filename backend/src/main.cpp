#include <drogon/drogon.h>
#include <iostream>
#include <cstdlib>
#include <string>

int main() {
    // 1. Récupération des variables d'environnement
    const std::string dbHost = std::getenv("DATABASE_HOST") ? std::getenv("DATABASE_HOST") : "localhost";
    const std::string dbPort = std::getenv("DATABASE_PORT") ? std::getenv("DATABASE_PORT") : "5432";
    const std::string dbName = std::getenv("DATABASE_NAME") ? std::getenv("DATABASE_NAME") : "restaurant_tierlist";
    const std::string dbUser = std::getenv("DATABASE_USER") ? std::getenv("DATABASE_USER") : "tierlist_user";
    const std::string dbPassword = std::getenv("DATABASE_PASSWORD") ? std::getenv("DATABASE_PASSWORD") : "tierlist_password";

    // 2. Création de la configuration au format JSON (Standard Drogon)
    Json::Value dbConfig;
    dbConfig["rdbms"] = "postgresql";
    dbConfig["host"] = dbHost;
    dbConfig["port"] = std::stoi(dbPort);
    dbConfig["dbname"] = dbName;
    dbConfig["user"] = dbUser;
    dbConfig["passwd"] = dbPassword;
    dbConfig["is_fast"] = false;
    dbConfig["connection_number"] = 1;
    dbConfig["name"] = "default";

    // 3. Configuration et lancement
    drogon::app()
        .addDbClient(dbConfig) 
        .addListener("0.0.0.0", 8080)
        .setLogLevel(trantor::Logger::kInfo)
        .setThreadNum(16);

    LOG_INFO << "Server running on http://0.0.0.0:8080";
    LOG_INFO << "Database connection configured for: " << dbName;

    drogon::app().run();

    return 0;
}