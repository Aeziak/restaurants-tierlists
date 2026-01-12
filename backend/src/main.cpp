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

    // 2. Configuration Postgres (Noms conformes aux suggestions du compilateur)
    drogon::orm::PostgresConfig pgConfig;
    pgConfig.host = dbHost;
    pgConfig.port = static_cast<unsigned short>(std::stoi(dbPort));
    pgConfig.dbname = dbName;
    pgConfig.user = dbUser;
    pgConfig.password = dbPassword;
    pgConfig.isFast = false;
    pgConfig.connectionNumber = 1;
    pgConfig.name = "default";
    pgConfig.characterSet = "utf8";

    // 3. Encapsulation dans le variant DbConfig
    drogon::orm::DbConfig config(pgConfig);

    // 4. Configuration de l'application
    drogon::app()
        .addDbClient(config) 
        .addListener("0.0.0.0", 8080)
        .setLogLevel(trantor::Logger::kInfo)
        .setThreadNum(16);

    LOG_INFO << "Backend started on http://0.0.0.0:8080";
    
    drogon::app().run();

    return 0;
}