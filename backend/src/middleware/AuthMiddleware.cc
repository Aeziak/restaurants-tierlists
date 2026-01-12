#include "AuthMiddleware.h"
#include <jwt-cpp/jwt.h>
#include <cstdlib>
#include <string>
#include <iostream>

namespace middleware {

void AuthMiddleware::doFilter(const drogon::HttpRequestPtr &req,
                               drogon::FilterCallback &&fcb,
                               drogon::FilterChainCallback &&ccb)
{
    // Extract Authorization header
    auto authHeader = req->getHeader("Authorization");
    
    if (authHeader.empty())
    {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            Json::Value{
                {"error", Json::Value{
                    {"code", "UNAUTHORIZED"},
                    {"message", "Missing Authorization header"}
                }}
            }
        );
        resp->setStatusCode(drogon::k401Unauthorized);
        fcb(resp);
        return;
    }

    // Check for "Bearer " prefix
    if (authHeader.substr(0, 7) != "Bearer ")
    {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            Json::Value{
                {"error", Json::Value{
                    {"code", "UNAUTHORIZED"},
                    {"message", "Invalid Authorization header format. Expected: Bearer <token>"}
                }}
            }
        );
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
            auto resp = drogon::HttpResponse::newHttpJsonResponse(
                Json::Value{
                    {"error", Json::Value{
                        {"code", "SERVER_ERROR"},
                        {"message", "Authentication not configured"}
                    }}
                }
            );
            resp->setStatusCode(drogon::k500InternalServerError);
            fcb(resp);
            return;
        }

        std::string secret(secretEnv);

        // Decode and verify JWT
        auto decoded = jwt::decode(token);
        
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
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            Json::Value{
                {"error", Json::Value{
                    {"code", "UNAUTHORIZED"},
                    {"message", "Invalid or expired token"}
                }}
            }
        );
        resp->setStatusCode(drogon::k401Unauthorized);
        fcb(resp);
    }
    catch (const std::exception& e)
    {
        std::cerr << "JWT processing error: " << e.what() << std::endl;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            Json::Value{
                {"error", Json::Value{
                    {"code", "UNAUTHORIZED"},
                    {"message", "Token validation failed"}
                }}
            }
        );
        resp->setStatusCode(drogon::k401Unauthorized);
        fcb(resp);
    }
}

} // namespace middleware
