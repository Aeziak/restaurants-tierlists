#include "AuthMiddleware.h"
#include <drogon/drogon.h>
#include <jwt-cpp/jwt.h> 
// RETIRÉ: #include <jwt-cpp/traits/kazuho-picojson.h> (C'était la cause de l'erreur)

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

    try
    {
        // 4. Décodage avec spécification explicite des traits PICOJSON
        // On garde cette partie car le compilateur en a besoin pour savoir comment parser le JSON
        auto decoded = jwt::decode<jwt::traits::kazuho_picojson>(token);

        // 5. Vérification
        auto verifier = jwt::verify<jwt::traits::kazuho_picojson>()
            .allow_algorithm(jwt::algorithm::hs256{secret})
            .leeway(60);

        verifier.verify(decoded);

        // 6. Extraction des claims
        std::string userId;
        std::string email;

        if (decoded.has_payload_claim("sub"))
            userId = decoded.get_payload_claim("sub").as_string();
        
        if (decoded.has_payload_claim("email"))
            email = decoded.get_payload_claim("email").as_string();

        // 7. Injection dans la requête
        if (!userId.empty()) req->attributes()->insert("userId", userId);
        if (!email.empty())  req->attributes()->insert("email", email);

        LOG_DEBUG << "Authenticated User: " << email << " (ID: " << userId << ")";

        ccb();
    }
    catch (const jwt::error::token_verification_exception& e)
    {
        LOG_WARN << "JWT Verification failed: " << e.what();
        Json::Value error;
        error["error"]["code"] = "UNAUTHORIZED";
        error["error"]["message"] = "Invalid token signature or expired token";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k401Unauthorized);
        fcb(resp);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR << "JWT Processing error: " << e.what();
        Json::Value error;
        error["error"]["code"] = "BAD_REQUEST";
        error["error"]["message"] = "Token processing failed";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k400BadRequest);
        fcb(resp);
    }
}

} // namespace middleware