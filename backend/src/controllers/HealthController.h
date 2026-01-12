#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api::v1 {

class HealthController : public drogon::HttpController<HealthController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(HealthController::getHealth, "/api/v1/health", Get);
    METHOD_LIST_END

    void getHealth(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback);
};

} // namespace api::v1
