//
// Created by qlu on 2019/2/11.
//

#include <utility>
#include <fmt/format.h>
#include <kungfu/wingchun/macro.h>

#include <kungfu/yijinjing/log/setup.h>

#include "marketdata_ufx.h"
#include "serialize_ufx.h"
#include "type_convert_xtp.h"

namespace kungfu
{
    namespace wingchun
    {
        namespace ufx
        {
            MarketDataUFX::MarketDataUFX(bool low_latency, yijinjing::data::locator_ptr locator, const std::string &json_config) :
                    MarketData(low_latency, std::move(locator), SOURCE_UFX)
            {
                yijinjing::log::copy_log_settings(get_io_device()->get_home(), SOURCE_UFX);
                config_ = nlohmann::json::parse(json_config);
				//tdfApi = new TDFBase();
				//tdfApi->setMdCallback(this);
            }

            MarketDataUFX::~MarketDataUFX()
            {
                //if (api_ != nullptr)
                //{
                //    api_->Release();
                //}
				if (tdfApi != nullptr)
				{
					tdfApi->disconnect();
				}
            }

            std::string MarketDataUFX::get_runtime_folder() const
            {
                auto home = get_io_device()->get_home();
                return home->locator->layout_dir(home, yijinjing::data::layout::LOG);
            }

            void MarketDataUFX::on_start()
            {
                MarketData::on_start();
                std::string runtime_folder = get_runtime_folder();
                // SPDLOG_INFO("Connecting TDF for {} at {}://{}:{}", config_.user_id, config_.protocol, config_.md_ip, config_.md_port);
				SPDLOG_INFO("Connecting TDF for {} at {}://{}:{}", config_.TDF_login_id, config_.protocol, config_.TDF_ip, config_.TDF_port);
				tdfApi = new TDFBase();
				tdfApi->setMdCallback(this);
				Sleep(100);
				bool con = tdfApi->connect();
				Sleep(100);
				if (con) {
					publish_state(BrokerState::LoggedIn);
					LOGIN_INFO(fmt::format("login success! (user_id) {},(connect){}", config_.TDF_login_id, con));
					Sleep(100);
					bool sub1 = tdfApi->subscribe("000001", "sz");
					bool sub2 = tdfApi->subscribe("600001", "sh");
					SPDLOG_INFO("sub result: {},{}",sub1, sub2);
				}
				else {
					publish_state(BrokerState::LoggedInFailed);
					LOGIN_ERROR("(ErrorMsg)TDF login failed");
				}
				//while (1) {
				//	Sleep(1000);
				//}
				
				//api_ = XTP::API::QuoteApi::CreateQuoteApi(config_.client_id, runtime_folder.c_str());
    //            if (config_.protocol == "udp")
    //            {
    //                api_->SetUDPBufferSize(config_.buffer_size);
    //            }
    //            api_->RegisterSpi(this);
    //            int res = api_->Login(config_.md_ip.c_str(), config_.md_port, config_.user_id.c_str(), config_.password.c_str(), get_xtp_protocol_type(config_.protocol));
    //            if (res == 0)
    //            {
    //                publish_state(BrokerState::LoggedIn);
    //                LOGIN_INFO(fmt::format("login success! (user_id) {}", config_.user_id));
    //            } else
    //            {
    //                publish_state(BrokerState::LoggedInFailed);
    //                XTPRI *error_info = api_->GetApiLastError();
    //                LOGIN_ERROR(fmt::format("(ErrorId) {}, (ErrorMsg){}", error_info->error_id, error_info->error_msg));
    //            }
            }

            bool MarketDataUFX::subscribe(const std::vector<Instrument> &instruments)
            {
                SUBSCRIBE_TRACE(fmt::format("(size) {}", instruments.size()));

                std::vector<std::string> sse_tickers;
                std::vector<std::string> sze_tickers;
                for (const auto &inst: instruments)
                {
                    std::string ticker = inst.instrument_id;
                    if (strcmp(inst.exchange_id, EXCHANGE_SSE) == 0)
                    {
                        sse_tickers.push_back(ticker);
                    } else if (strcmp(inst.exchange_id, EXCHANGE_SZE) == 0)
                    {
                        sze_tickers.push_back(ticker);
                    }
                }
                bool res = true;
                if (!sse_tickers.empty())
                {
                    res = res && subscribe(sse_tickers, EXCHANGE_SSE);
                }
                if (!sze_tickers.empty())
                {
                    res = res && subscribe(sze_tickers, EXCHANGE_SZE);
                }
                return res;
            }

            bool MarketDataUFX::subscribe(const std::vector<std::string> &instruments, const std::string &exchange_id)
            {
                int size = instruments.size();
                std::vector<char *> insts;
                insts.reserve(size);
                for (auto &s: instruments)
                {
                    insts.push_back((char *) &s[0]);
                }

				//转换交易所代码
				string exchange;
				if (strcmp((char *)exchange_id.c_str(), "SSE") == 0)
				{
					exchange = "sh";
				}
				else if (strcmp((char *)exchange_id.c_str(), "SZE") == 0)
				{
					exchange = "sz";
				}
				else
				{
					return false;
				}
				//订阅
				bool sub = true;
				for (int i = 0; i < size; i++)
				{
					sub = sub && tdfApi->subscribe(instruments[i].c_str(), exchange);
					//SPDLOG_INFO("SUB(id){}", instruments[i]);
				}
				//订阅是否成功
				if (sub)
				{
					SUBSCRIBE_INFO(fmt::format("subscribe request success, (size) {} (exchange_id) {}", size, exchange_id));
					//cout << "subscribe request success, (size) " << size << " (exchange_id) " << exchange_id << endl;
				}
				else
				{
					SUBSCRIBE_ERROR("failed to subscribe");
					//cout << "failed to subscribe" << endl;
				}

				return sub;



            }


			void MarketDataUFX::onDisconnect(int reason)
			{
				DISCONNECTED_ERROR(fmt::format("(disconnected) {}", reason));
				publish_state(BrokerState::DisConnected);
			}

			void MarketDataUFX::onTick(int day, int time, const char* code, float open, float high, float low, float close)
			{

			}

			// 行情
			void MarketDataUFX::OnDepthMarketData(const TDF_MARKET_DATA *tdfMarketData)
			{
				//SPDLOG_INFO("in DepthMarketData:(code){}, (volume){}, (time){}", tdfMarketData->szWindCode, tdfMarketData->iVolume, tdfMarketData->nTime);
				//QUOTE_TRACE("(Code){}, (Volume){}", tdfMarketData.szWindCode, tdfMarketData.iVolume);
				//QUOTE_TRACE(to_string(*tdfMarketData));
				msg::data::Quote &quote = get_writer(0)->open_data<msg::data::Quote>(0, msg::type::Quote);
				from_xtp(*tdfMarketData, quote);
				get_writer(0)->close_data();
			}

			//逐笔委托
			void MarketDataUFX::OnOrderByTick(const TDF_ORDER *tdfOrder)
			{

				ENTRUST_TRACE(to_string(*tdfOrder));
				msg::data::Entrust &entrust = get_writer(0)->open_data<msg::data::Entrust>(0, msg::type::Entrust);
				from_xtp(*tdfOrder, entrust);
				get_writer(0)->close_data();
			}

			// 逐笔成交
			void MarketDataUFX::OnTransByTick(const TDF_TRANSACTION *tdfTrans)
			{
				TRANSACTION_TRACE(to_string(*tdfTrans));
				msg::data::Transaction &transaction = get_writer(0)->open_data<msg::data::Transaction>(0, msg::type::Transaction);
				// add ufxInfo 待确定
				from_ufx(*tdfTrans, transaction);
				get_writer(0)->close_data();
			}

			// 用于log
			void MarketDataUFX::msg(string m)
			{
				SPDLOG_INFO(m);
			}

            //void MarketDataXTP::OnDisconnected(int reason)
            //{
            //    DISCONNECTED_ERROR(fmt::format("(reason) {}", reason));
            //    publish_state(BrokerState::DisConnected);
            //}

            //void MarketDataXTP::OnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last)
            //{
            //    if (error_info != nullptr && error_info->error_id != 0)
            //    {
            //        SUBSCRIBE_ERROR(fmt::format("(error_id) {} {error_msg} {} (is_last) {}", error_info->error_id, error_info->error_msg, is_last));
            //    } else
            //    {
            //        SUBSCRIBE_INFO(fmt::format("(ticker) {} (is_last) {}", ticker->ticker, is_last));
            //    }
            //}

            //void MarketDataXTP::OnSubTickByTick(XTPST *ticker, XTPRI *error_info, bool is_last)
            //{
            //    if (error_info != nullptr && error_info->error_id != 0)
            //    {
            //        SUBSCRIBE_ERROR(fmt::format("(error_id) {} {error_msg} {} (is_last) {}", error_info->error_id, error_info->error_msg, is_last));
            //    } else
            //    {
            //        SUBSCRIBE_INFO(fmt::format("(ticker) {} (is_last) {}", ticker->ticker, is_last));
            //    }
            //}

            //void MarketDataXTP::OnSubscribeAllTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info)
            //{
            //    if (error_info != nullptr && error_info->error_id != 0)
            //    {
            //        SUBSCRIBE_ERROR(fmt::format("(error_id) {} {error_msg} {}", error_info->error_id, error_info->error_msg));
            //    } else
            //    {
            //        SUBSCRIBE_INFO(fmt::format("subscribe all tick by tick success, exchange: {}", exchange_id_from_xtp(exchange_id)));
            //    }
            //}

            //void MarketDataXTP::OnDepthMarketData(XTPMD *market_data, int64_t *bid1_qty, int32_t bid1_count,
            //                                      int32_t max_bid1_count, int64_t *ask1_qty, int32_t ask1_count,
            //                                      int32_t max_ask1_count)
            //{
            //    QUOTE_TRACE(to_string(*market_data));
            //    msg::data::Quote &quote = get_writer(0)->open_data<msg::data::Quote>(0, msg::type::Quote);
            //    from_xtp(*market_data, quote);
            //    get_writer(0)->close_data();
            //}

            //void MarketDataXTP::OnTickByTick(XTPTBT *tbt_data)
            //{
            //    if (tbt_data->type == XTP_TBT_ENTRUST)
            //    {
            //        ENTRUST_TRACE(to_string(*tbt_data));

            //        msg::data::Entrust &entrust = get_writer(0)->open_data<msg::data::Entrust>(0, msg::type::Entrust);
            //        from_xtp(*tbt_data, entrust);
            //        get_writer(0)->close_data();
            //    } else if (tbt_data->type == XTP_TBT_TRADE)
            //    {
            //        TRANSACTION_TRACE(to_string(*tbt_data));

            //        msg::data::Transaction &transaction = get_writer(0)->open_data<msg::data::Transaction>(0, msg::type::Transaction);
            //        from_xtp(*tbt_data, transaction);
            //        get_writer(0)->close_data();
            //    }
            //}
        }
    }
}
