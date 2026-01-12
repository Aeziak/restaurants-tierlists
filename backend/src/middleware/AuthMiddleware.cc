#include "AuthMiddleware.h"
#include <drogon/drogon.h>
#include <jwt-cpp/jwt.h>

#include <cstdlib>
#include <string>
#include <iostream>

namespace middleware {

void AuthMiddleware::doFilter(const drogon::HttpRequestPtr &req,
                              drogon::FilterCallback &&fcb,
                              drogon::FilterChainCallback &&ccb)
{
    static const char* secretEnv = std::getenv("NEXTAUTH_SECRET");
    static const std::string secret = secretEnv ? secretEnv : "";

    if (secret.empty())
    {
        LOG_ERROR << "CRITICAL: NEXTAUTH_SECRET environment variable not set!";
        Json::Value error;
        error["error"]["code"] = "SERVER_ERROR";
        error["error"]["message"] = "Authentication configuration error";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k500InternalServerError);
        fcb(resp);
        return;
    }

    const std::string& authHeader = req->getHeader("Authorization");

    if (authHeader.size() < 7 || authHeader.substr(0, 7) != "Bearer ")
    {
        Json::Value error;
        error["error"]["code"] = "UNAUTHORIZED";
        error["error"]["message"] = "Missing or invalid Authorization header (Expected: 'Bearer <token>')";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k401Unauthorized);
        fcb(resp);
        return;
    }

    std::string token = authHeader.substr(7);

    try {
        // ✅ Utiliser picojson_traits explicitement
        using traits = jwt::picojson_traits;

        // Decode JWT
        auto decoded = jwt::decode<traits>(token);

        // Verify JWT
        jwt::verify<traits>(jwt::default_clock{})
            .with_issuer("restaurant-tier-list")
            .allow_algorithm(jwt::algorithm::hs256{secret})
            .verify(decoded);

        // Extract user_id
        std::string userId;
        for (auto& claim : decoded.get_payload_claims()) {
            if (claim.first == "user_id") {
                userId = claim.second.as_string();
                break;
            }
        }

        if (userId.empty()) {
            throw std::runtime_error("Missing user_id in token payload");
        }

        req->attributes()->insert("user_id", userId);
        ccb();

    } catch (const std::exception& e) {
        LOG_ERROR << "JWT Processing error: " << e.what();
        Json::Value error;
        error["error"]["code"] = "UNAUTHORIZED";
        error["error"]["message"] = "Invalid or expired token";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k401Unauthorized);
        fcb(resp);
    }
}

} // namespace middleware
