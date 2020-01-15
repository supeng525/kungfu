//
// Created by qlu on 2019/1/10.
//

#ifndef KUNGFU_XTP_EXT_TYPE_CONVERT_H
#define KUNGFU_XTP_EXT_TYPE_CONVERT_H

#include <cstdio>
#include <cstring>
#include <ctime>

#include "xtp_api_struct.h"
#include "winAPI/TDFAPIStruct.h"
#include "gatewayUFX/ufx_data_struct.h"

#include <kungfu/yijinjing/time.h>
#include <kungfu/wingchun/msg.h>

namespace kungfu
{
    namespace wingchun
    {
        namespace xtp
        {
            using namespace kungfu::wingchun::msg::data;
            inline XTP_PROTOCOL_TYPE get_xtp_protocol_type(const std::string &p)
            {
                if (p == "udp")
                {
                    return XTP_PROTOCOL_UDP;
                }
                else
                {
                    return XTP_PROTOCOL_TCP;
                }
            }

            inline int64_t nsec_from_xtp_timestamp(int64_t xtp_time)
            {
                std::tm result = {};
                result.tm_year =  xtp_time / (int64_t)1e13 - 1900;
                result.tm_mon = xtp_time % (int64_t)1e13 / (int64_t)1e11 -1;
                result.tm_mday = xtp_time % (int64_t)1e11 / (int64_t)1e9;
                result.tm_hour = xtp_time % (int64_t)1e9 / (int64_t)1e7;
                result.tm_min =  xtp_time % (int)1e7 / (int)1e5;
                result.tm_sec = xtp_time % (int)1e5 /(int)1e3;
                int milli_sec = xtp_time %(int)1e3;
                std::time_t parsed_time = std::mktime(&result);
                int64_t nano = parsed_time * kungfu::yijinjing::time_unit::NANOSECONDS_PER_SECOND;
                return parsed_time * kungfu::yijinjing::time_unit::NANOSECONDS_PER_SECOND + milli_sec * kungfu::yijinjing::time_unit::NANOSECONDS_PER_MILLISECOND;
            }

			inline int64_t nsec_from_xtp_timestamp(int tradingDay, int time) 
			{
				// tradingDay:20190926, nTime:131632000(HHMMSSmmm)
				std::tm result = {};
				result.tm_year = tradingDay / (int)1e4 - 1900;
				result.tm_mon = tradingDay % (int)1e4 / (int)1e2 -1;
				result.tm_mday = tradingDay % (int)1e2;
				//SPDLOG_INFO("(year){}, (mon){}, (day){}", result.tm_year, result.tm_mon, result.tm_mday);

				result.tm_hour = time / (int)1e7;
				result.tm_min = time % (int)1e7 / (int)1e5;
				result.tm_sec = time % (int)1e5 / (int)1e3;
				int milli_sec = time % (int)1e3;
				//SPDLOG_INFO("(hour){}, (min){}, (sec){}, (msec){}", result.tm_hour, result.tm_min, result.tm_sec, milli_sec);

				std::time_t parsed_time = std::mktime(&result);
				//SPDLOG_INFO("(parsed_time){}", parsed_time);
				int64_t nano = parsed_time * kungfu::yijinjing::time_unit::NANOSECONDS_PER_SECOND;
				//SPDLOG_INFO("(nano){}", nano);
				return parsed_time * kungfu::yijinjing::time_unit::NANOSECONDS_PER_SECOND + milli_sec * kungfu::yijinjing::time_unit::NANOSECONDS_PER_MILLISECOND;
			}

			inline int64_t nsec_from_xtp_timestamp(string ufx_time)
			{
				// 2019/09/30 14:28:41
				std::tm result = {};
				// 2019/09/30
				string date = ufx_time.substr(0, 10);
				// 14:28:41
				string time = ufx_time.substr(11);
				int year = atoi(date.substr(0, 4).c_str());
				int mon = atoi(date.substr(5, 2).c_str());
				int day = atoi(date.substr(8, 2).c_str());
				int hr = atoi(time.substr(0, 2).c_str());
				int min = atoi(time.substr(3, 2).c_str());
				int sec = atoi(time.substr(6, 2).c_str());
				result.tm_year = year - 1900;
				result.tm_mon = mon - 1;
				result.tm_mday = day;
				result.tm_hour = hr;
				result.tm_min = min;
				result.tm_sec = sec;
				int milli_sec = 0;
				std::time_t parsed_time = std::mktime(&result);
				int64_t nano = parsed_time * kungfu::yijinjing::time_unit::NANOSECONDS_PER_SECOND;
				return parsed_time * kungfu::yijinjing::time_unit::NANOSECONDS_PER_SECOND + milli_sec * kungfu::yijinjing::time_unit::NANOSECONDS_PER_MILLISECOND;


			}

            inline void from_xtp(const XTP_MARKET_TYPE &xtp_market_type, char *exchange_id)
            {
                if (xtp_market_type == XTP_MKT_SH_A)
                {
                    strcpy(exchange_id, "SSE");
                } else if (xtp_market_type == XTP_MKT_SZ_A)
                {
                    strcpy(exchange_id, "SZE");
                }
            }

			inline void from_xtp(const string &ufx_market_type, char *exchange_id)
			{
				if(ufx_market_type == ufx_data::DF_EXCH_SSE){
					//SPDLOG_INFO("IN DF SSE");
					strcpy(exchange_id, "SSE");
				}
				else if (ufx_market_type == ufx_data::DF_EXCH_HK_SZE) {
					//SPDLOG_INFO("IN DF SZE");
					strcpy(exchange_id, "SZE");
				}
			}

            inline void to_xtp(XTP_MARKET_TYPE &xtp_market_type, const char *exchange_id)
            {
                if (!strcmp(exchange_id, "SSE"))
                {
                    xtp_market_type = XTP_MKT_SH_A;
                } else if (!strcmp(exchange_id, "SZE"))
                {
                    xtp_market_type = XTP_MKT_SZ_A;
                } else
                {
                    xtp_market_type = XTP_MKT_UNKNOWN;
                }
            }

            inline std::string exchange_id_from_xtp(const XTP_EXCHANGE_TYPE ex)
            {
                if (ex == XTP_EXCHANGE_SH)
                {
                    return EXCHANGE_SSE;
                }
                else if(ex == XTP_EXCHANGE_SZ)
                {
                    return EXCHANGE_SZE;
                }
                else
                {
                    return "Unknown";
                }
            }

            inline void from_xtp(const XTP_EXCHANGE_TYPE &xtp_exchange_type, char *exchange_id)
            {
                if (xtp_exchange_type == XTP_EXCHANGE_SH)
                {
                    strcpy(exchange_id, "SSE");
                } else if (xtp_exchange_type == XTP_EXCHANGE_SZ)
                {
                    strcpy(exchange_id, "SZE");
                }
            }

			inline void from_xtp(const TDF_CODE_INFO &tdf_code, char *exchange_id)
			{
				if (tdf_code.nMarketID == ID_MARKET_SZ) {
					strcpy(exchange_id, "SZE");
				}
				else if (tdf_code.nMarketID == ID_MARKET_SH) {
					strcpy(exchange_id, "SSE");
				}
				//SPDLOG_INFO("(exchange_id){}", exchange_id);
			}


            inline void to_xtp(XTP_EXCHANGE_TYPE &xtp_exchange_type, char *exchange_id)
            {
                if (strcmp(exchange_id, "SSE") == 0)
                {
                    xtp_exchange_type = XTP_EXCHANGE_SH;
                } else if (strcmp(exchange_id, "SZE") == 0)
                {
                    xtp_exchange_type = XTP_EXCHANGE_SZ;
                } else
                {
                    xtp_exchange_type = XTP_EXCHANGE_UNKNOWN;
                }
            }

            inline void from_xtp(const XTP_PRICE_TYPE &xtp_price_type, const  XTP_MARKET_TYPE &xtp_exchange_type, PriceType &price_type)
            {
                if (xtp_price_type == XTP_PRICE_LIMIT)
                    price_type = PriceType::Limit;
                else if (xtp_price_type == XTP_PRICE_BEST5_OR_CANCEL)
                    price_type = PriceType::FakBest5;
                else if (xtp_exchange_type == XTP_MKT_SH_A)
                {
                    if (xtp_price_type == XTP_PRICE_BEST5_OR_LIMIT)
                        price_type = PriceType::ReverseBest;
                }
                else if(xtp_exchange_type == XTP_MKT_SZ_A)
                {
                    if (xtp_price_type == XTP_PRICE_BEST_OR_CANCEL)
                        price_type = PriceType::Fak;
                    else if (xtp_price_type == XTP_PRICE_FORWARD_BEST)
                        price_type = PriceType::ForwardBest;
                    else if (xtp_price_type == XTP_PRICE_REVERSE_BEST_LIMIT)
                        price_type = PriceType::ReverseBest;
                }
                else
                    price_type = PriceType::UnKnown;
            }

            inline void to_xtp(XTP_PRICE_TYPE &xtp_price_type, const PriceType &price_type, const char* exchange)
            {
                if (price_type == PriceType::Limit)
                    xtp_price_type = XTP_PRICE_LIMIT;
                else if ((price_type == PriceType::Any) || (price_type == PriceType::FakBest5))
                    xtp_price_type = XTP_PRICE_BEST5_OR_CANCEL;
                else if (strcmp(exchange, EXCHANGE_SSE) == 0)
                {
                    if (price_type == PriceType::ReverseBest)
                        xtp_price_type = XTP_PRICE_BEST5_OR_LIMIT;
                }
                else if (strcmp(exchange, EXCHANGE_SZE) == 0)
                {
                    if (price_type == PriceType::Fak)
                        xtp_price_type = XTP_PRICE_BEST_OR_CANCEL;
                    else if (price_type == PriceType::ForwardBest)
                        xtp_price_type = XTP_PRICE_FORWARD_BEST;
                    else if (price_type == PriceType::ReverseBest)
                        xtp_price_type = XTP_PRICE_REVERSE_BEST_LIMIT;
                }
                else
                    xtp_price_type = XTP_PRICE_TYPE_UNKNOWN;
            }

            inline void from_xtp(const XTP_ORDER_STATUS_TYPE &xtp_order_status, OrderStatus &status)
            {
                if (xtp_order_status == XTP_ORDER_STATUS_INIT || xtp_order_status == XTP_ORDER_STATUS_NOTRADEQUEUEING)
                {
                    status = OrderStatus::Pending;
                } else if (xtp_order_status == XTP_ORDER_STATUS_ALLTRADED)
                {
                    status = OrderStatus::Filled;
                } else if (xtp_order_status == XTP_ORDER_STATUS_CANCELED)
                {
                    status = OrderStatus::Cancelled;
                } else if (xtp_order_status == XTP_ORDER_STATUS_PARTTRADEDQUEUEING)
                {
                    status = OrderStatus::PartialFilledActive;
                } else if (xtp_order_status == XTP_ORDER_STATUS_PARTTRADEDNOTQUEUEING)
                {
                    status = OrderStatus::PartialFilledNotActive;
                } else if (xtp_order_status == XTP_ORDER_STATUS_REJECTED)
                {
                    status = OrderStatus::Error;
                } else
                {
                    status = OrderStatus::Unknown;
                }
            }

			inline void from_xtp(const string &ufx_order_status, OrderStatus &status)
			{
				if (ufx_order_status == ufx_data::DF_OS_NEW)
				{
					status = OrderStatus::Pending;
				}
				else if (ufx_order_status == ufx_data::DF_OS_ALLTRD)
				{
					status = OrderStatus::Filled;
				}
				else if (ufx_order_status == ufx_data::DF_OS_CANCEL)
				{
					status = OrderStatus::Cancelled;
				}
				else if (ufx_order_status == ufx_data::DF_OS_PART_TRADE)
				{
					status = OrderStatus::PartialFilledActive;
				}
				//else if (xtp_order_status == XTP_ORDER_STATUS_PARTTRADEDNOTQUEUEING)
				//{
				//	status = OrderStatus::PartialFilledNotActive;
				//}
				else if (ufx_order_status == ufx_data::DF_OS_ERROR)
				{
					status = OrderStatus::Error;
				}
				else
				{
					status = OrderStatus::Unknown;
				}
			}

            inline void from_xtp(const XTP_SIDE_TYPE &xtp_side, Side &side)
            {
                if (xtp_side == XTP_SIDE_BUY)
                {
                    side = Side::Buy;
                } else if (xtp_side == XTP_SIDE_SELL)
                {
                    side = Side::Sell;
                }
            }

			inline void from_xtp(const string &ufx_side, Side &side)
			{
				if (ufx_side == ufx_data::DF_DIRECTION_BUY) {
					side = Side::Buy;
				}
				else if (ufx_side == ufx_data::DF_DIRECTION_SELL) {
					side = Side::Sell;
				}
			}

            inline void to_xtp(XTP_SIDE_TYPE &xtp_side, const Side &side)
            {
                if (side == Side::Buy)
                {
                    xtp_side = XTP_SIDE_BUY;
                } else if (side == Side::Sell)
                {
                    xtp_side = XTP_SIDE_SELL;
                }
            }

			inline void to_xtp(string &ufx_side, const Side &side)
			{
				if (side == Side::Buy) {
					ufx_side = ufx_data::DF_DIRECTION_BUY;
				}
				else if (side == Side::Sell) {
					ufx_side = ufx_data::DF_DIRECTION_SELL;
				}

			}

            inline void to_xtp(XTPMarketDataStruct &des, const Quote &ori)
            {
                //TODO
            }

            inline void from_xtp(const XTPMarketDataStruct &ori, Quote &des)
            {
                strcpy(des.source_id, SOURCE_XTP);
                des.data_time = nsec_from_xtp_timestamp(ori.data_time);
                strcpy(des.trading_day, yijinjing::time::strftime(des.data_time, "%Y%m%d").c_str());
                strcpy(des.instrument_id, ori.ticker);
                from_xtp(ori.exchange_id, des.exchange_id);

                if (ori.data_type == XTP_MARKETDATA_OPTION)
                {
                    des.instrument_type = InstrumentType::StockOption;
                } else
                {
                    // 目前除逆回购之外的债券均当做股票
                    if (is_reverse_repurchase(des.instrument_id, des.exchange_id))
                    {
                        des.instrument_type = InstrumentType::Bond;
                    } else
                    {
                        des.instrument_type = InstrumentType::Stock;
                    }
                }

                des.last_price = ori.last_price;
                des.pre_settlement_price = ori.pre_settl_price;
                des.pre_close_price = ori.pre_close_price;
                des.open_price = ori.open_price;
                des.high_price = ori.high_price;
                des.low_price = ori.low_price;
                des.volume = ori.qty;
                des.turnover = ori.turnover;
                des.close_price = ori.close_price;
                des.settlement_price = ori.settl_price;
                des.upper_limit_price = ori.upper_limit_price;
                des.lower_limit_price = ori.lower_limit_price;

                for (int i = 0; i < 10; i++)
                {
                    des.ask_price[i] = ori.ask[i];
                    des.ask_volume[i] = ori.ask_qty[i];
                    des.bid_price[i] = ori.bid[i];
                    des.bid_volume[i] = ori.bid_qty[i];
                }
            }

			inline void from_xtp(const TDF_MARKET_DATA &ori, Quote &des)
			{
				strcpy(des.source_id, SOURCE_XTP);
				//SPDLOG_INFO("(ActionDay){}, (tradingday){}, (nTime){}, (exchange){}",ori.nActionDay, ori.nTradingDay, ori.nTime, ori.pCodeInfo->nMarketID);
				// tradingDay:20190926, nTime:131632000 (+8时区)
				des.data_time = nsec_from_xtp_timestamp(ori.nTradingDay, ori.nTime);
				strcpy(des.trading_day, yijinjing::time::strftime(des.data_time, "%Y%m%d").c_str());
				strcpy(des.instrument_id, ori.szCode);

				

				from_xtp(*ori.pCodeInfo, des.exchange_id);

				////char exchange[3] = { 0 };
				////strncpy(exchange, ori.szWindCode + 7, 2);
				////if (strcmp(exchange, "SZ") == 0) {
				////	strcpy(des.exchange_id, "SZE");
				////}
				////else if (strcmp(exchange, "SH") == 0) {
				////	strcpy(des.exchange_id, "SSE");
				////}

				////TODO: 默认只有股票
				des.instrument_type = InstrumentType::Stock;

				des.last_price = ori.nMatch / 10000.0;
				des.pre_settlement_price = ori.nPreClose / 10000.0;
				des.pre_close_price = ori.nPreClose / 10000.0;
				des.open_price = ori.nOpen / 10000.0;
				des.high_price = ori.nHigh / 10000.0;
				des.low_price = ori.nLow / 10000.0;
				des.volume = ori.iVolume;
				des.turnover = ori.iTurnover;
				//TO DO: TDF_MARKET_DATA没有收盘价，暂设为0
				des.close_price = 0;
				//TO DO: TDF_MARKET_DATA没有结算价，暂设为0
				des.settlement_price = 0;
				des.upper_limit_price = ori.nHighLimited / 10000.0;
				des.lower_limit_price = ori.nLowLimited / 10000.0;

				for (int i = 0; i < 10; i++)
				{
					des.ask_price[i] = ori.nAskPrice[i] / 10000.0;
					des.ask_volume[i] = ori.nAskVol[i];
					des.bid_price[i] = ori.nBidPrice[i] / 10000.0;
					des.bid_volume[i] = ori.nBidVol[i];
				}

			}


            inline void to_xtp(XTPOrderInsertInfo &des, const OrderInput &ori)
            {
                strcpy(des.ticker, ori.instrument_id);
                to_xtp(des.market, ori.exchange_id);
                des.price = ori.limit_price;
                des.quantity = ori.volume;
                to_xtp(des.side, ori.side);
                to_xtp(des.price_type, ori.price_type, ori.exchange_id);
                des.business_type = XTP_BUSINESS_TYPE_CASH;
            }

			inline void to_xtp(std::shared_ptr<ufx_data::OrderInfo> &des, const OrderInput &ori)
			{
				if (!strcmp(ori.exchange_id, "SSE")) {
					des->exchange = ufx_data::DF_EXCH_SSE;
				}
				else if (!strcmp(ori.exchange_id, "SZE")) {
					des->exchange = ufx_data::DF_EXCH_SZE;
				}
				des->symbol = ori.instrument_id;
				des->price = ori.limit_price;
				des->totalVolume = ori.volume;
				to_xtp(des->direction, ori.side);

				if (ori.offset == Offset::Open) {
					des->offset = ufx_data::DF_OFFSET_OPEN;
				}
				else if (ori.offset == Offset::Close) {
					des->offset = ufx_data::DF_OFFSET_CLOSE;
				}

			}

            inline void from_xtp(const XTPOrderInsertInfo &ori, OrderInput &des)
            {
                //TODO
            }

            inline void from_xtp(const XTPOrderInfo &ori, Order &des)
            {
                strcpy(des.instrument_id, ori.ticker);
                from_xtp(ori.market, des.exchange_id);
                from_xtp(ori.price_type, ori.market, des.price_type);
                des.volume = ori.quantity;
                des.volume_traded = ori.qty_traded;
                des.volume_left = ori.qty_left;
                des.limit_price = ori.price;
                from_xtp(ori.order_status, des.status);
                from_xtp(ori.side, des.side);
                des.offset = Offset::Open;
                if (ori.business_type == XTP_BUSINESS_TYPE_CASH)
                {
                    des.instrument_type = InstrumentType::Stock;
                }
                if (ori.update_time > 0)
                {
                    des.update_time = nsec_from_xtp_timestamp(ori.update_time);
                }
            }

			inline void from_xtp(std::shared_ptr<ufx_data::OrderInfo> &ori, Order &des) 
			{
				strcpy(des.instrument_id, ori->symbol.c_str());
				from_xtp(ori->exchange, des.exchange_id);
				
				//from_xtp(ori.price_type, ori.market, des.price_type);
				// TODO: ufx orderinfo中没有price type，暂定为市价单
				des.price_type = PriceType::Any;

				des.volume = ori->totalVolume;
				des.volume_traded = ori->tradeVolume;
				des.volume_left = (ori->totalVolume - ori->tradeVolume);
				des.limit_price = ori->price;
				from_xtp(ori->status, des.status);
				from_xtp(ori->direction, des.side);
				des.offset = Offset::Open;
				//if (ori.business_type == XTP_BUSINESS_TYPE_CASH)
				//{
				//	des.instrument_type = InstrumentType::Stock;
				//}
				// TODO: 默认只有普通股票业务
				des.instrument_type = InstrumentType::Stock;

				//2019/09/30 14:28:41
				if (ori->orderTime != "")
				{
					des.update_time = nsec_from_xtp_timestamp(ori->orderTime);
				}
			}

            inline void from_xtp(const XTPTradeReport &ori, Trade &des)
            {
                strcpy(des.instrument_id, ori.ticker);
                des.volume = ori.quantity;
                des.price = ori.price;
                from_xtp(ori.market, des.exchange_id);
                from_xtp(ori.side, des.side);
                des.offset = Offset::Open;
                if (ori.business_type == XTP_BUSINESS_TYPE_CASH)
                {
                    des.instrument_type = InstrumentType::Stock;
                }
                des.trade_time = nsec_from_xtp_timestamp(ori.trade_time);
            }

			inline void from_xtp(const std::shared_ptr<ufx_data::TradeInfo> &ori, Trade &des)
			{
				strcpy(des.instrument_id, ori->symbol.c_str());
				des.volume = ori->volume;
				des.price = ori->price;
				from_xtp(ori->exchange, des.exchange_id);
				from_xtp(ori->direction, des.side);
				des.offset = Offset::Open;
				// TODO: 暂时都设为stock
				des.instrument_type = InstrumentType::Stock;
				// tradeTime 20191008 131034
				int d = atoi(ori->tradeTime.substr(0, 8).c_str());
				int t = atoi(ori->tradeTime.substr(9, 6).c_str());
				des.trade_time = nsec_from_xtp_timestamp(d,t*1000);
			}

            inline void from_xtp(const XTPQueryStkPositionRsp &ori, Position &des)
            {
                strcpy(des.instrument_id, ori.ticker);
                from_xtp(ori.market, des.exchange_id);
                des.volume = ori.total_qty;
                des.yesterday_volume = ori.sellable_qty;
                des.position_cost_price = ori.avg_price;
            }

			inline void from_xtp(std::shared_ptr<ufx_data::PositionInfo> &ori, Position &des)
			{
				//SPDLOG_INFO("in transfer position data");
				strcpy(des.instrument_id, ori->symbol.c_str());
				from_xtp(ori->exchange, des.exchange_id);
				des.volume = ori->position;
				des.yesterday_volume = ori->available;
				des.position_cost_price = ori->price;
				//SPDLOG_INFO("(ins_id){}(exchange){}(volume){}(yVolume){}(pCostPrice){}", 
				//	des.instrument_id, des.exchange_id, des.volume, des.yesterday_volume, des.position_cost_price);
			}

            inline void from_xtp(const XTPQueryAssetRsp &ori, Asset &des)
            {
                strcpy(des.source_id, SOURCE_XTP);
                des.avail = ori.buying_power;
            }

			inline void from_xtp(std::shared_ptr<ufx_data::AccountInfo> &ori, Asset &des)
			{
				//SPDLOG_INFO("in transfer data");
				strcpy(des.source_id, SOURCE_XTP);
				des.avail = ori->available;
			}

            inline void from_xtp(const XTPTickByTickStruct &ori, Entrust &des)
            {
                from_xtp(ori.exchange_id, des.exchange_id);
                strcpy(des.instrument_id, ori.ticker);
                des.data_time = nsec_from_xtp_timestamp(ori.data_time);

                des.price = ori.entrust.price;
                des.volume = ori.entrust.qty;
                des.main_seq = ori.entrust.channel_no;
                des.seq = ori.entrust.seq;

                if (ori.entrust.ord_type == '1')
                {
                    des.price_type = PriceType::Any;
                } else if (ori.entrust.ord_type == '2')
                {
                    des.price_type = PriceType::Limit;
                } else if (ori.entrust.ord_type == 'U')
                {
                    des.price_type = PriceType::ForwardBest;
                }
            }

			inline void from_xtp(const TDF_ORDER &ori, Entrust &des)
			{
				from_xtp(*ori.pCodeInfo, des.exchange_id);
				strcpy(des.instrument_id, ori.szCode);
				des.data_time = nsec_from_xtp_timestamp(ori.nTime);

				des.price = ori.nPrice;
				des.volume = ori.nVolume;
				// TO DO: TDF中没有主次序号
				des.main_seq = ori.chFunctionCode;
				
				des.seq = ori.nOrder;

				//TO DO:委托类别
				des.price_type = PriceType::Any;
				//if (ori.entrust.ord_type == '1')
				//{
				//	des.price_type = PriceType::Any;
				//}
				//else if (ori.entrust.ord_type == '2')
				//{
				//	des.price_type = PriceType::Limit;
				//}
				//else if (ori.entrust.ord_type == 'U')
				//{
				//	des.price_type = PriceType::ForwardBest;
				//}
			}


            inline void from_xtp(const XTPTickByTickStruct &ori, Transaction &des)
            {
                strcpy(des.source_id, SOURCE_XTP);

                from_xtp(ori.exchange_id, des.exchange_id);
                strcpy(des.instrument_id, ori.ticker);
                des.data_time = nsec_from_xtp_timestamp(ori.data_time);

                des.main_seq = ori.trade.channel_no;
                des.seq = ori.trade.seq;

                des.price = ori.trade.price;
                des.volume = ori.trade.qty;

                des.bid_no = ori.trade.bid_no;
                des.ask_no = ori.trade.ask_no;

                switch (ori.trade.trade_flag)
                {
                    case 'B':
                    {
                        des.bs_flag = BsFlag::Buy;
                        break;
                    }
                    case 'S':
                    {
                        des.bs_flag = BsFlag::Sell;
                        break;
                    }
                    case 'N':
                    {
                        des.bs_flag = BsFlag::Unknown;
                        break;
                    }
                    case '4':
                    {
                        des.exec_type = ExecType::Cancel;
                        break;
                    }
                    case 'F':
                    {
                        des.exec_type = ExecType::Trade;
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }

			inline void from_xtp(const TDF_TRANSACTION &ori, Transaction &des)
			{
				strcpy(des.source_id, SOURCE_XTP);

				from_xtp(*ori.pCodeInfo, des.exchange_id);
				strcpy(des.instrument_id, ori.szCode);
				des.data_time = nsec_from_xtp_timestamp(ori.nTime);

				//TO DO: TDF_TRANSACTION中没有主 子序号
				des.main_seq = ori.chFunctionCode;
				des.seq = ori.nIndex;

				des.price = ori.nPrice;
				des.volume = ori.nVolume;

				des.bid_no = ori.nBidOrder;
				des.ask_no = ori.nAskOrder;

				switch (ori.nBSFlag)
				{
				case 'B':
				{
					des.bs_flag = BsFlag::Buy;
					break;
				}
				case 'S':
				{
					des.bs_flag = BsFlag::Sell;
					break;
				}
				case ' ':
				{
					des.bs_flag = BsFlag::Unknown;
					break;
				}
				default:
				{
					break;
				}
				}
			}

        }
    }
}
#endif //KUNGFU_XTP_EXT_TYPE_CONVERT_H
