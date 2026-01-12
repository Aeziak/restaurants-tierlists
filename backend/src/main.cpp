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

    // 2. Création de la configuration spécifique Postgres
    drogon::orm::PostgresConfig pgConfig;
    pgConfig.host_ = dbHost;
    pgConfig.port_ = static_cast<unsigned short>(std::stoi(dbPort));
    pgConfig.dbname_ = dbName;
    pgConfig.user_ = dbUser;
    pgConfig.password_ = dbPassword;
    pgConfig.isFast_ = false;
    pgConfig.connectionNumber_ = 1;
    pgConfig.name_ = "default";
    pgConfig.characterSet_ = "utf8";

    // 3. Encapsulation dans le variant DbConfig
    drogon::orm::DbConfig config(pgConfig);

    // 4. Configuration et lancement
    drogon::app()
        .addDbClient(config) 
        .addListener("0.0.0.0", 8080)
        .setLogLevel(trantor::Logger::kInfo)
        .setThreadNum(16);

    LOG_INFO << "Server running on http://0.0.0.0:8080";
    LOG_INFO << "Database (Postgres) configured: " << dbName;

    drogon::app().run();

    return 0;
}