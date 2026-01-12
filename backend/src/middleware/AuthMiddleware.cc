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
        // Decode the token
        auto decoded = jwt::decode(token);
        
        // Create verifier with the correct secret variable
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{secret})  // ← FIX 1: utilisez 'secret' pas 'jwtSecret'
            .with_issuer("restaurant-tier-list");
        
        // Verify the token
        verifier.verify(decoded);
        
        // Extract user_id from payload
        auto claims = decoded.get_payload_json();
        std::string userId = claims["user_id"].asString();
        
        // Store in request attributes
        req->attributes()->insert("user_id", userId);
        
        // Continue the chain
        ccb();  // ← FIX 2: utilisez 'ccb()' pas 'fcb()' pour continuer la chaîne
        
    } catch (const std::exception& e)
    {
        LOG_ERROR << "JWT Processing error: " << e.what();
        Json::Value error;
        error["error"]["code"] = "UNAUTHORIZED";  // ← FIX 3: UNAUTHORIZED au lieu de BAD_REQUEST
        error["error"]["message"] = "Invalid or expired token";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k401Unauthorized);  // ← FIX 4: 401 au lieu de 400
        fcb(resp);
    }
}

} // namespace middleware