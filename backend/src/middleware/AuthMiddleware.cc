#include "AuthMiddleware.h"
#include <drogon/drogon.h> // Nécessaire pour les types Drogon
#include <jwt-cpp/jwt.h>
#include <cstdlib>
#include <string>
#include <iostream>

namespace middleware {

void AuthMiddleware::doFilter(const drogon::HttpRequestPtr &req,
                              drogon::FilterCallback &&fcb,
                              drogon::FilterChainCallback &&ccb)
{
    // 1. Récupération des variables d'environnement (Optimisation: fait une seule fois grâce à 'static')
    static const char* secretEnv = std::getenv("NEXTAUTH_SECRET");
    static const std::string secret = secretEnv ? secretEnv : "";
    
    // Si la variable n'est pas définie, on bloque tout pour éviter une faille de sécurité
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
    // On vérifie d'abord la taille pour éviter une exception sur substr
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

    // Extraction du token
    std::string token = authHeader.substr(7);

    try
    {
        // 4. Décodage du token
        auto decoded = jwt::decode(token);

        // 5. Vérification (Signature + Issuer + Expiration)
        // Note: L'algorithme doit correspondre à celui utilisé par NextAuth (souvent HS256 par défaut pour JWS)
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{secret})
            // .with_issuer("http://localhost:3000") // DANGER: À décommenter uniquement si tu es sûr de l'issuer en prod
            .leeway(60); // Ajoute 60s de tolérance pour les problèmes d'horloge entre serveurs

        verifier.verify(decoded);

        // 6. Extraction sécurisée des claims (évite les crashs si le claim manque)
        std::string userId;
        std::string email;

        if (decoded.has_payload_claim("sub"))
            userId = decoded.get_payload_claim("sub").as_string();
        
        if (decoded.has_payload_claim("email"))
            email = decoded.get_payload_claim("email").as_string();

        // 7. Injection des infos dans la requête
        if (!userId.empty()) req->attributes()->insert("userId", userId);
        if (!email.empty())  req->attributes()->insert("email", email);

        // LOG_DEBUG est mieux que cout pour Drogon (thread-safe et niveau de log géré)
        LOG_DEBUG << "Authenticated User: " << email << " (ID: " << userId << ")";

        // Tout est bon, on passe au contrôleur
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
        error["error"]["code"] = "BAD_REQUEST"; // Souvent un format malformé
        error["error"]["message"] = "Token processing failed";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k400BadRequest);
        fcb(resp);
    }
}

} // namespace middleware