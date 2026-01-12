#include "AuthMiddleware.h"
#include <drogon/drogon.h>
#include <jwt-cpp/traits/jsoncpp/traits.h>  // ← Include spécifique pour les traits JsonCpp
#include <jwt-cpp/jwt.h>
#include <json/json.h>

#include <cstdlib>
#include <string>
#include <iostream>

namespace middleware {

void AuthMiddleware::doFilter(const drogon::HttpRequestPtr &req,
                              drogon::FilterCallback &&fcb,
                              drogon::FilterChainCallback &&ccb)
{
    // 1. Récupération des variables d'environnement
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

    // 2. Extraction du Header Authorization
    const std::string& authHeader = req->getHeader("Authorization");

    // 3. Validation du format "Bearer <token>"
    if (authHeader.empty() || authHeader.size() < 7 || authHeader.substr(0, 7) != "Bearer ")
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
        // Utilisez jwt::traits::jsoncpp avec l'include spécifique
        auto decoded = jwt::decode<jwt::traits::jsoncpp>(token);
        
        // Create verifier
        auto verifier = jwt::verify<jwt::default_clock, jwt::traits::jsoncpp>()
            .allow_algorithm(jwt::algorithm::hs256{secret})
            .with_issuer("restaurant-tier-list");
        
        // Verify the token
        verifier.verify(decoded);
        
        // Extract user_id from payload
        auto payload = decoded.get_payload();
        if (!payload.isMember("user_id")) {
            throw std::runtime_error("Missing user_id in token payload");
        }
        std::string userId = payload["user_id"].asString();
        
        // Store in request attributes
        req->attributes()->insert("user_id", userId);
        
        // Continue the chain
        ccb();
        
    } catch (const std::exception& e)
    {
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