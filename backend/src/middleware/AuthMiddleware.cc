#include "AuthMiddleware.h"
#include <drogon/drogon.h>
#include <jwt-cpp/jwt.h>
// ✅ Include the specific traits header
#include <jwt-cpp/traits/kazuho-picojson/traits.h>

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
        // ✅ Define the traits type (correct)
        using traits = jwt::traits::kazuho_picojson;
        // ✅ Option A: Use `jwt::create<traits>()` and `jwt::verify<traits>()`
        // ✅ Option B: Import the entire `jwt::` namespace for this trait (often easier)
        using namespace jwt::defaults;

        // Decode JWT
        auto decoded = jwt::decode<traits>(token);

        // Verify JWT
        // Using the namespace import (Option B), it looks clean:
        verify()
            .allow_algorithm(jwt::algorithm::hs256{secret})
            .verify(decoded);

        // --- IMPORTANT: API CHANGE in jwt-cpp v0.7.0+ ---
        // `get_payload_claims()` was replaced by `get_payload_json()`[citation:1]
        auto payload = decoded.get_payload_json();

        // Extract user_id - the method to access claims has changed.
        std::string userId;
        if (payload.has("user_id")) {
            userId = payload["user_id"].as_string(); // Access via picojson
        } else if (payload.has("sub")) { // Fallback to standard 'subject' claim
            userId = payload["sub"].as_string();
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