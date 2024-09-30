#include "drogon_ex.h"
#include <string>
#include <cxxopts.hpp>

using namespace drogon;

drogon::Task<Json::Value> health_check(const drogon::HttpRequestPtr &req) {
	co_return jO({{"status", "Ok"}});
}

Task<Json::Value> getUserProfile(const HttpRequestPtr &req)
{
	auto params = req->parameters();
	auto r_params = req->getRoutingParameters();
	auto username = r_params[0];

	if (username.empty()) {
		throw RESTException("Username parameter is missing", HttpStatusCode::k400BadRequest);
	}

	auto client = app().getFastDbClient("default");
//	auto result = co_await client->execSqlCoro("SELECT * FROM users WHERE username = $1", username);
	auto result = co_await client->execSqlCoro("SELECT * FROM airports LIMIT 100");

	Json::Value resultJson;
	for (const auto &row : result) {
		Json::Value jsonRow;
		for (const auto &field : row) {
			jsonRow[field.name()] = field.as<std::string>();
		}
		resultJson.append(jsonRow);
	}
//	if (result.empty()) {
//	    throw RESTException("User not found: " + username, HttpStatusCode::k404NotFound);
//	}
//
//	Json::Value resultJson;
//	for (const auto& row : result) {
//		Json::Value rowJson;
//		for (const auto& field : row) {
//			rowJson[field.name()] = field.as<std::string>();
//		}
//		resultJson.append(rowJson);
//	}
//
	co_return resultJson;
}



int main(int argc, char* argv[]) {
	cxxopts::Options o_parser("MyApp", "Description of MyApp");
	o_parser.add_options()
		("p,port", "Port number", cxxopts::value<int>()->default_value("8000"));
	auto options = o_parser.parse(argc, argv);

	registerRoute("/healthcheck", Get, health_check);
	registerRoute("/user/{username}/profile", Get, getUserProfile);

	drogon::app().loadConfigFile("./config.json");

	orm::PostgresConfig db_config;
	const char* db_url = std::getenv("DB_URL");
	if (db_url == nullptr) {
		throw std::runtime_error("DB_URL environment variable is not set");
	}
	parseConnectionString(db_url, db_config);

	db_config.name = "default";
	db_config.autoBatch = false;
	db_config.characterSet = "utf8";
	db_config.isFast = true;
	db_config.connectionNumber = 10;
	db_config.timeout = 10;
	db_config.connectOptions = {
								   {"statement_timeout", "3s"},
								   {"lock_timeout", "0.5s"},
							   } ;

	app().addDbClient(db_config);

	app().setLogLevel(trantor::Logger::kTrace)
		.addListener("0.0.0.0", options["port"].as<int>())
		.setThreadNum(0) // If 0, the number of IO threads will be the number of all hardware cores.
		.run();
}

//   "db_clients": [
//  {
//    "name": "default",          // Name for this DB client (used in app().getDbClient("postgres_client"))
//    "rdbms": "postgresql",              // Database type: postgresql, mysql, sqlite3, etc.
//    "host": "localhost",                // Hostname for the database server
//    "port": 5432,                       // Port for PostgreSQL
//    "dbname": "testdb",                 // Database name
//    "user": "testuser",                 // Username
//    "passwd": "testpassword",           // Password for the user
//    "is_fast": true,                    // Whether to use getFastDbClient for performance (single-threaded contexts)
//    "connection_number": 10,            // Size of the connection pool
//    "characterSet": "utf8",              // Character set (e.g., utf8)
//    "connectOptions": {
//      "statement_timeout": "3s",
//      "lock_timeout": "0.5s"
//    }
//  }
//],