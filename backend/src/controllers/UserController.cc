#include "UserController.h"

namespace controllers {

void UserController::getProfile(const drogon::HttpRequestPtr &req,
                                std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    // Extract user info from request attributes (set by AuthMiddleware)
    auto userId = req->attributes()->get<std::string>("userId");
    auto email = req->attributes()->get<std::string>("email");

    // Return user profile
    Json::Value response;
    response["user"]["id"] = userId;
    response["user"]["email"] = email;
    response["message"] = "User profile retrieved successfully";

    auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
    resp->setStatusCode(drogon::k200OK);
    callback(resp);
}

} // namespace controllers
