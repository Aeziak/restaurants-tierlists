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

    // ... (Environment variable and header checks remain the same as your existing code) ...

    std::string token = authHeader.substr(7);

    try {
        // Use nlohmann_json trait (better support)
        using traits = jwt::traits::nlohmann_json;

        // Decode JWT
        auto decoded = jwt::decode<traits>(token);

        // Verify JWT
        auto verifier = jwt::verify<jwt::default_clock, traits>()
            .allow_algorithm(jwt::algorithm::hs256{secret})
            .with_issuer("restaurant-tier-list");
        
        verifier.verify(decoded);

        // Extract user_id from payload
        auto payload = decoded.get_payload_json();
        std::string userId;
        
        if (payload.contains("user_id")) {
            userId = payload["user_id"].get<std::string>();
        } else if (payload.contains("sub")) {
            userId = payload["sub"].get<std::string>();
        }

        if (userId.empty()) {
            throw std::runtime_error("Missing user_id or sub in token payload");
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
}