#include "AuthMiddleware.h"
#include <jwt-cpp/jwt.h>
#include <cstdlib>
#include <string>
#include <iostream>

// Use picojson traits (default in jwt-cpp)
using jwt_traits = jwt::traits::kazuho_picojson;

namespace middleware {

void AuthMiddleware::doFilter(const drogon::HttpRequestPtr &req,
                               drogon::FilterCallback &&fcb,
                               drogon::FilterChainCallback &&ccb)
{
    // Extract Authorization header
    auto authHeader = req->getHeader("Authorization");
    
    if (authHeader.empty())
    {
        Json::Value error;
        error["error"]["code"] = "UNAUTHORIZED";
        error["error"]["message"] = "Missing Authorization header";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k401Unauthorized);
        fcb(resp);
        return;
    }

    // Check for "Bearer " prefix
    if (authHeader.substr(0, 7) != "Bearer ")
    {
        Json::Value error;
        error["error"]["code"] = "UNAUTHORIZED";
        error["error"]["message"] = "Invalid Authorization header format. Expected: Bearer <token>";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k401Unauthorized);
        fcb(resp);
        return;
    }

    // Extract token (remove "Bearer " prefix)
    std::string token = authHeader.substr(7);

    try
    {
        // Get shared secret from environment
        const char* secretEnv = std::getenv("NEXTAUTH_SECRET");
        if (!secretEnv)
        {
            std::cerr << "ERROR: NEXTAUTH_SECRET environment variable not set!" << std::endl;
            Json::Value error;
            error["error"]["code"] = "SERVER_ERROR";
            error["error"]["message"] = "Authentication not configured";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(drogon::k500InternalServerError);
            fcb(resp);
            return;
        }
 with picojson traits
        auto decoded = jwt::decode<jwt_traits>(token);
        
        // Verify JWT signature and claims with picojson traits
        auto verifier = jwt::verify<jwt::default_clock, jwt_traits>token);
        
        // Verify JWT signature and claims
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{secret})
            .with_issuer("http://localhost:3000"); // NextAuth.js default issuer

        verifier.verify(decoded);

        // Extract user claims
        std::string userId = decoded.get_payload_claim("sub").as_string(); // Google ID
        std::string email = decoded.get_payload_claim("email").as_string();
        
        // Attach user info to request attributes for use in controllers
        req->attributes()->insert("userId", userId);
        req->attributes()->insert("email", email);

        // Log successful authentication (debug only)
        std::cout << "Authenticated user: " << email << " (ID: " << userId << ")" << std::endl;

        // Token is valid, continue to controller
        ccb();
    }
    catch (const jwt::error::token_verification_exception& e)
    {
        std::cerr << "JWT verification failed: " << e.what() << std::endl;
        Json::Value error;
        error["error"]["code"] = "UNAUTHORIZED";
        error["error"]["message"] = "Invalid or expired token";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k401Unauthorized);
        fcb(resp);
    }
    catch (const std::exception& e)
    {
        std::cerr << "JWT processing error: " << e.what() << std::endl;
        Json::Value error;
        error["error"]["code"] = "UNAUTHORIZED";
        error["error"]["message"] = "Token validation failed";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k401Unauthorized);
        fcb(resp);
    }
}

} // namespace middleware
