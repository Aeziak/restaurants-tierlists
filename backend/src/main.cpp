#include <drogon/drogon.h>
#include <iostream>
#include <cstdlib>
#include <string>

int main() {
    // 1. Récupération des variables d'environnement (avec noms exacts de ton fichier)
    const std::string dbHost = std::getenv("DATABASE_HOST") ? std::getenv("DATABASE_HOST") : "localhost";
    const std::string dbPort = std::getenv("DATABASE_PORT") ? std::getenv("DATABASE_PORT") : "5432";
    const std::string dbName = std::getenv("DATABASE_NAME") ? std::getenv("DATABASE_NAME") : "restaurant_tierlist";
    const std::string dbUser = std::getenv("DATABASE_USER") ? std::getenv("DATABASE_USER") : "tierlist_user";
    const std::string dbPassword = std::getenv("DATABASE_PASSWORD") ? std::getenv("DATABASE_PASSWORD") : "tierlist_password";

    // 2. Configuration moderne du client DB
    drogon::orm::DbConfig config;
    config.setDbType(drogon::orm::ClientType::PostgreSQL);
    config.setHost(dbHost);
    config.setPort(static_cast<unsigned short>(std::stoi(dbPort)));
    config.setDbName(dbName);
    config.setUsername(dbUser);
    config.setPassword(dbPassword);
    config.setConnectionNumber(1);
    config.setName("default"); // Identifiant utilisé par app().getDbClient("default")
    config.setCharacterSet("utf8");

    // 3. Configuration de l'application
    drogon::app()
        .addDbClient(config) // Utilise addDbClient(config) à la place de createDbClient
        .addListener("0.0.0.0", 8080)
        .setLogLevel(trantor::Logger::kInfo)
        .setThreadNum(16);

    // Logs de démarrage (Plus propre avec LOG_INFO)
    LOG_INFO << "Server running on http://0.0.0.0:8080";
    LOG_INFO << "Database: " << dbName << " @ " << dbHost << ":" << dbPort;

    // 4. Run framework
    drogon::app().run();

    return 0;
}