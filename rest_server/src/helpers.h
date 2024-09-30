#include <regex>
#include <stdexcept>
#include <string>
#include <drogon/drogon.h>
#include <initializer_list>

using namespace drogon;


class RESTException : public std::runtime_error {
	HttpStatusCode status_code_m;
public:
	HttpStatusCode status_code() const { return status_code_m; }
	explicit RESTException(const std::string &message,
						   HttpStatusCode status_code = drogon::HttpStatusCode::k500InternalServerError):
		status_code_m(status_code), std::runtime_error(message) {}
};

/**
 * @example
 * Json::Value json = jO({
 *     {"name", "John Doe"},
 *     {"age", 30}
 * });
 * // Results in: {"name": "John Doe", "age": 30}
 */
Json::Value jO(std::initializer_list<std::pair<std::string, Json::Value>> list) {
	Json::Value json;
	for (const auto& [key, value] : list) {
		json[key] = value;
	}
	return json;
}

/**
 * @example
 * orm::PostgresConfig config;
 * parseConnectionString("postgres://user:pass@localhost:5432/mydb?sslmode=disable", config);
 * // Results in:
 * // config.username = "user"
 * // config.password = "pass"
 * // config.host = "localhost"
 * // config.port = 5432
 * // config.databaseName = "mydb"
 * // (sslmode parameter is not handled in this implementation)
 */
void parseConnectionString(const std::string& connectionString, orm::PostgresConfig& config) {
	// 'postgres://node:node@swarm.next:15001/aip_data?sslmode=disable'
	std::regex pattern("(?:postgresql|postgres)://(?:([^:@]+)(?::([^@]+))?@)?([^:/]+)(?::([^/]+))?/([^?]+)(?:\\?(.*))?");
	std::smatch match;

	if (std::regex_match(connectionString, match, pattern)) {
		if (match[1].matched) config.username = match[1].str();
		if (match[2].matched) config.password = match[2].str();
		if (match[3].matched) config.host = match[3].str();
		if (match[4].matched) config.port = std::stoi(match[4].str());
		else config.port = 5432; // Default port if not defined
		if (match[5].matched) config.databaseName = match[5].str();

		if (match[6].matched) {
			std::string params = match[6].str();
			std::regex param_pattern("([^=&]+)=([^&]+)");
			std::sregex_iterator it(params.begin(), params.end(), param_pattern);
			std::sregex_iterator end;

			while (it != end) {
				std::smatch param_match = *it;
				std::string key = param_match[1].str();
				std::string value = param_match[2].str();
				// Handle additional parameters if needed
				// For example: if (key == "sslmode") config.sslmode = value;
				++it;
			}
		}
	}
}
