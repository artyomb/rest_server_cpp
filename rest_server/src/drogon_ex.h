#include <drogon/drogon.h>
#include <json/json.h>
#include <drogon/utils/coroutine.h>
#include <stdexcept>
#include "helpers.h"

template <typename Func>
void registerRoute(const std::string &path, drogon::HttpMethod method, Func &&func)
{
	drogon::app().registerHandler(path,
		[func = std::forward<Func>(func)](drogon::HttpRequestPtr req) -> Task<HttpResponsePtr> {
			try {
			  auto resp = drogon::HttpResponse::newHttpJsonResponse( co_await func(req) );
			  co_return resp;
			} catch (const RESTException &e) {
			  auto resp = drogon::HttpResponse::newHttpJsonResponse(jO({{"error", e.what()}}));
			  resp->setStatusCode(e.status_code());
			  co_return resp;
			} catch (const std::exception &e) {
			  auto resp = drogon::HttpResponse::newHttpJsonResponse(jO({{"error", "Internal server error"},{"details", e.what()}}));
			  resp->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
			  co_return resp;
			}
	  }, {method}
	);
}
