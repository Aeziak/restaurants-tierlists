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
        // Utilisez le trait par défaut (picojson)
        auto decoded = jwt::decode(token);
        
        // Create verifier avec le trait par défaut
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{secret})
            .with_issuer("restaurant-tier-list");
        
        // Verify the token
        verifier.verify(decoded);
        
        // Extract user_id from payload - avec picojson
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