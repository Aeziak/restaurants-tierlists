#include <drogon/drogon.h>
#include <iostream>
#include <cstdlib>

int main() {
    // Database configuration from environment variables
    std::string dbHost = std::getenv("DATABASE_HOST") ? std::getenv("DATABASE_HOST") : "localhost";
    std::string dbPort = std::getenv("DATABASE_PORT") ? std::getenv("DATABASE_PORT") : "5432";
    std::string dbName = std::getenv("DATABASE_NAME") ? std::getenv("DATABASE_NAME") : "restaurant_tierlist";
    std::string dbUser = std::getenv("DATABASE_USER") ? std::getenv("DATABASE_USER") : "tierlist_user";
    std::string dbPassword = std::getenv("DATABASE_PASSWORD") ? std::getenv("DATABASE_PASSWORD") : "tierlist_password";

    // Configure PostgreSQL connection
    std::string dbConnString = "host=" + dbHost + 
                                " port=" + dbPort + 
                                " dbname=" + dbName + 
                                " user=" + dbUser + 
                                " password=" + dbPassword;

    drogon::app()
        // Database client
        .createDbClient("postgresql", dbConnString, 1)
        // HTTP listener
        .addListener("0.0.0.0", 8080)
        .setLogLevel(trantor::Logger::kInfo)
        .setThreadNum(16);

    std::cout << "Server running on http://localhost:8080" << std::endl;
    std::cout << "Database: " << dbName << " @ " << dbHost << ":" << dbPort << std::endl;

    // Run HTTP framework
    drogon::app().run();

    return 0;
}

