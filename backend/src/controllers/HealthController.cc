#include "HealthController.h"
#include <drogon/drogon.h>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace api::v1;

std::string getCurrentTimestampISO8601() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    gmtime_s(&tm, &time_t);
#else
    gmtime_r(&time_t, &tm);
#endif
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S") << "Z";
    return oss.str();
}

void HealthController::getHealth(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) 
{
    Json::Value response;
    bool allHealthy = true;
    
    // Timestamp
    response["timestamp"] = getCurrentTimestampISO8601();
    
    // Check PostgreSQL
    auto dbClient = drogon::app().getDbClient();
    std::string postgresStatus = "down";
    
    if (dbClient) {
        try {
            // Simple query to test connection
            auto result = dbClient->execSqlSync("SELECT 1");
            if (result.size() > 0) {
                postgresStatus = "up";
            } else {
                allHealthy = false;
            }
        } catch (const std::exception &e) {
            allHealthy = false;
            LOG_WARN << "PostgreSQL health check failed: " << e.what();
        }
    } else {
        allHealthy = false;
        LOG_WARN << "PostgreSQL client not initialized";
    }
    
    response["services"]["postgres"] = postgresStatus;
    
    // Check Redis (placeholder - requires redis-plus-plus integration)
    // For now, assume Redis is up if we reach this point
    // TODO: Add actual Redis PING check in future story
    std::string redisStatus = "up";
    response["services"]["redis"] = redisStatus;
    
    // Overall status
    response["status"] = allHealthy ? "healthy" : "degraded";
    
    // HTTP status code
    auto httpResponse = HttpResponse::newHttpJsonResponse(response);
    httpResponse->setStatusCode(allHealthy ? k200OK : k503ServiceUnavailable);
    
    callback(httpResponse);
}
