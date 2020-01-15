//
// Created by Su on 2019-10-09
//

#ifndef KUNGFU_UFX_EXT_COMMON_H
#define KUNGFU_UFX_EXT_COMMON_H

#include <nlohmann/json.hpp>

namespace kungfu
{
    namespace wingchun
    {
        namespace ufx
        {
            struct Configuration
            {
                /*int client_id;
                std::string user_id;
                std::string password;
                std::string software_key;
                std::string md_ip;
                int md_port;
                std::string td_ip;
                int td_port;
                std::string protocol;
                int buffer_size;*/
				int client_id;
				std::string ufx_user_id;
				std::string ufx_password;
				std::string ufx_ip;
				int ufx_port;
				std::string TDF_ip;
				int TDF_port;
				std::string TDF_login_id;
				std::string TDF_password;
				int buffer_size;
				std::string protocol;
            };

            inline void from_json(const nlohmann::json& j, Configuration& c)
            {
                j.at("client_id").get_to(c.client_id);
				j.at("ufx_user_id").get_to(c.ufx_user_id);
                j.at("ufx_password").get_to(c.ufx_password);
                j.at("ufx_ip").get_to(c.ufx_ip);
				j.at("ufx_port").get_to(c.ufx_port);
				j.at("TDF_ip").get_to(c.TDF_ip);
				j.at("TDF_port").get_to(c.TDF_port);
				j.at("TDF_login_id").get_to(c.TDF_login_id);
				j.at("TDF_password").get_to(c.TDF_passwordt);
                c.protocol =  j.value("protocol", "tcp");
                if (c.protocol != "udp")
                {
                    c.protocol = "tcp";
                }
                c.buffer_size = j.value("buffer_size", 64);
            }
        }
    }
}
#endif //KUNGFU_UFX_EXT_COMMON_H
