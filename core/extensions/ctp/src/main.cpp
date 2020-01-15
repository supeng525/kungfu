#include "marketdata_ctp.h"
#include "common.h"
#include <kungfu/yijinjing/journal/journal.h>
#include <kungfu/yijinjing/nanomsg/socket.h>
#include "cxxopts.hpp"

using namespace kungfu;
using namespace kungfu::yijinjing;



class PyLocatorDemo : public data::locator
{
	using data::locator::locator;

	bool has_env(const std::string &name) const override
	{
		return true;
	}

	const std::string get_env(const std::string &name) const override
	{
		return "./";
	}

	const std::string layout_dir(data::location_ptr location, data::layout l) const override
	{
		return "./";
	}

	const std::string layout_file(data::location_ptr location, data::layout l, const std::string &name) const override
	{
		return "./";
	}

	const std::string default_to_system_db(data::location_ptr location, const std::string &name) const override
	{
		return "./";
	}

	const std::vector<int> list_page_id(data::location_ptr location, uint32_t dest_id) const override
	{
		std::vector<int> demo;
		demo.push_back(1);
		return demo;
	}
};


int main(int argc, char* argv[])
{
	cxxopts::Options options("Xtp Md Service");
	options.add_options()
		("c, client-id", "client id", cxxopts::value<int>()->default_value("1"))
		("i, ip", "ip", cxxopts::value<std::string>()->default_value("120.27.164.69"))
		("p, port", "port", cxxopts::value<int>()->default_value("6001"))
		("k, software-key", "software key", cxxopts::value<std::string>()->default_value("b8aa7173bba3470e390d787219b2112e"))
		("u, user-id", "user id", cxxopts::value<std::string>()->default_value("xxxxxxx"))
		("password", "password", cxxopts::value<std::string>()->default_value("xxxxxxx"))
		("save-path", "save path", cxxopts::value<std::string>()->default_value("/shared/kungfu/runtime"))
		("log-level", "log level", cxxopts::value<int>()->default_value("2"));
	;

	auto result = options.parse(argc, argv);

	int client_id = result["client-id"].as<int>();
	std::string ip = result["ip"].as<std::string>();
	int port = result["port"].as<int>();
	std::string software_key = result["software-key"].as<std::string>();
	std::string user_id = result["user-id"].as<std::string>();
	std::string password = result["password"].as<std::string>();
	std::string save_path = result["save-path"].as<std::string>();
	int log_level = result["log-level"].as<int>();

	/*kungfu::xtp::TdGateway gateway(client_id, software_key, save_path, ip, port, user_id, password, log_level);
	gateway.init();
	gateway.start();*/
	//kungfu::yijinjing::data::location_ptr home;
	//yijinjing::data::locator *ptr ;
	PyLocatorDemo *ptr = new PyLocatorDemo;
	yijinjing::data::locator_ptr home(ptr);
	bool low_latency = false;
	//bool lazy = false;
	//kungfu::yijinjing::io_device test = kungfu::yijinjing::io_device(home, low_latency,lazy);
	const std::string json_config = "{ \"md_uri\":\"tcp://180.166.103.34:51218\",\"account_id\":\"66600810\" }";

	//std::string broker_id;
	//std::string password;
	//std::string auth_code;
	//std::string product_info;
	//std::string app_id;
	//MarketDataCTP(bool low_latency, yijinjing::data::locator_ptr locator, const std::string &json_config);
	wingchun::ctp::MarketDataCTP ctp(low_latency, home, json_config);
	return 0;
}