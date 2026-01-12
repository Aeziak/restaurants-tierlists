#pragma once

#include <drogon/HttpController.h>

namespace controllers {

/**
 * @brief User Profile Controller
 * 
 * Protected endpoint that requires JWT authentication.
 * Returns current user's profile information from JWT token.
 */
class UserController : public drogon::HttpController<UserController>
{
  public:
    METHOD_LIST_BEGIN
    // GET /api/v1/user/profile - Protected endpoint
    ADD_METHOD_TO(UserController::getProfile, "/api/v1/user/profile", drogon::Get, "middleware::AuthMiddleware");
    METHOD_LIST_END

    /**
     * @brief Get current user profile
     * 
     * Requires valid JWT token in Authorization header.
     * Returns user ID and email extracted from token.
     */
    void getProfile(const drogon::HttpRequestPtr &req,
                    std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};

} // namespace controllers
