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
    // 1. Récupération de la clé secrète
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
        error["error"]["message"] = "Missing or invalid Authorization header";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k401Unauthorized);
        fcb(resp);
        return;
    }

    std::string token = authHeader.substr(7);

    try {
        // 4. Décodage et vérification avec les traits par défaut
        // jwt-cpp gère automatiquement le parsing JSON interne ici
        auto decoded = jwt::decode(token);
        
        jwt::default_clock clock;
        auto verifier = jwt::verify(clock)
            .with_issuer("restaurant-tier-list")
            .allow_algorithm(jwt::algorithm::hs256{secret});
        
        verifier.verify(decoded);
        
        // 5. Extraction sécurisée du user_id
        if (decoded.has_payload_claim("user_id")) 
        {
            std::string userId = decoded.get_payload_claim("user_id").as_string();
            
            // Stockage dans la requête pour les contrôleurs
            req->attributes()->insert("user_id", userId);
            
            // Succès : passage au filtre suivant ou au contrôleur
            ccb();
        } 
        else 
        {
            throw std::runtime_error("Payload missing 'user_id' claim");
        }
        
    } catch (const std::exception& e)
    {
        LOG_ERROR << "JWT Auth Error: " << e.what();
        Json::Value error;
        error["error"]["code"] = "UNAUTHORIZED";
        error["error"]["message"] = "Invalid or expired token";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k401Unauthorized);
        fcb(resp);
    }
}

} // namespace middleware