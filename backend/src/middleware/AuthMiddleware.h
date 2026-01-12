#pragma once

#include <drogon/HttpFilter.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>

namespace middleware {

/**
 * @brief JWT Authentication Filter
 * 
 * Validates JWT tokens from NextAuth.js in the Authorization header.
 * Extracts user information and adds it to the request attributes.
 * 
 * Usage: Add this filter to protected endpoints via @filter annotation
 */
class AuthMiddleware : public drogon::HttpFilter<AuthMiddleware>
{
  public:
    AuthMiddleware() = default;
    ~AuthMiddleware() override = default;

    /**
     * @brief Filter method that validates JWT token
     * 
     * Extracts JWT from Authorization header, validates signature with NEXTAUTH_SECRET,
     * and adds userId to request attributes if valid.
     * 
     * @param req The HTTP request
     * @param fcb Callback to send response (401 if invalid)
     * @param ccb Callback to continue to controller (if valid)
     */
    void doFilter(const drogon::HttpRequestPtr &req,
                  drogon::FilterCallback &&fcb,
                  drogon::FilterChainCallback &&ccb) override;
};

} // namespace middleware
