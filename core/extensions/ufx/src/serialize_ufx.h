//
// Created by qlu on 2019/2/26.
//

#ifndef KUNGFU_SERIALIZE_H
#define KUNGFU_SERIALIZE_H

#include <nlohmann/json.hpp>
#include "xtp_api_struct.h"
#include "winAPI/TDFAPIStruct.h"
#include "gatewayUFX/ufx_data_struct.h"

namespace kungfu
{
    namespace wingchun
    {
        namespace ufx
        {
            /*inline void to_json(nlohmann::json &j, const XTPOrderInfo &order_info)
            {
                j["order_xtp_id"] = order_info.order_xtp_id;
                j["order_client_id"] = order_info.order_client_id;
                j["order_cancel_client_id"] = order_info.order_cancel_client_id;
                j["order_cancel_xtp_id"] = order_info.order_cancel_xtp_id;
                j["ticker"] = std::string(order_info.ticker);
                j["market"] = order_info.market;
                j["price"] = order_info.price;
                j["quantity"] = order_info.quantity;
                j["price_type"] = order_info.price_type;
                j["side"] = order_info.side;
                j["business_type"] = order_info.business_type;
                j["qty_traded"] = order_info.qty_traded;
                j["qty_left"] = order_info.qty_left;
                j["insert_time"] = order_info.insert_time;
                j["update_time"] = order_info.update_time;
                j["cancel_time"] = order_info.cancel_time;
                j["trade_amount"] = order_info.trade_amount;
                j["order_local_id"] = std::string(order_info.order_local_id);
                j["order_status"] = order_info.order_status;
                j["order_submit_status"] = order_info.order_submit_status;
                j["order_type"] = std::string(1, order_info.order_type);
            }*/
			//XTPOrderInfo &order_info;
			// ufx报单响应
			inline void to_json(nlohmann::json &j, const ufx_data::OrderInfo &order_info)
			{
				j["order_ufx_id"] = order_info.gOrderID; // gatewayName.orderReqID，全局唯一
				j["atsSymbol"] = order_info.;
				j["atsSymbol"] = order_info.exchange;
				j["atsSymbol"] = order_info.name;   // 股票名称
				j["atsSymbol"] = order_info.orderSysID;		// 交易所的报单编号
				j["atsSymbol"] = order_info.orderDeskID;	// 柜台的报单编号
				j["atsSymbol"] = order_info.orderReqID;		// gateway的报单编号
				j["atsSymbol"] = order_info.gOrderID;
										//int strategyID;     //策略的编号
				j["atsSymbol"] = order_info.strategyName;     //策略名称

								  // 报单相关
				j["atsSymbol"] = order_info.direction;		// 买入/卖出
				j["atsSymbol"] = order_info.offset;		// 开仓/平仓
				j["atsSymbol"] = order_info.price = { 0 };
				j["atsSymbol"] = order_info.totalVolume = { 0 };	// 报单总数量
				j["atsSymbol"] = order_info.tradeVolume = { 0 };	// 已成交数量
				j["atsSymbol"] = order_info.tradePrice = { 0 };	// 成交均价
				j["atsSymbol"] = order_info.status;		// 订单状态
				j["atsSymbol"] = order_info.statusMsg;	// 状态信息
				j["atsSymbol"] = order_info.orderTime;
				j["atsSymbol"] = order_info.cancelTime;
				j["atsSymbol"] = order_info.frontID = { 0 };
				j["atsSymbol"] = order_info.sessionID = { 0 };
				//bool external = { false };
				j["atsSymbol"] = order_info.gatewayName;
			}


			//ufx 成交信息
            inline void to_json(nlohmann::json &j, const ufx_data::TradeInfo &trade_info)
            {
				/*XTPTradeReport
                j["order_xtp_id"] = trade_info.order_xtp_id;
                j["order_client_id"] = trade_info.order_client_id;
                j["ticker"] = trade_info.ticker;
                j["market"] = trade_info.market;
                j["local_order_id"] = trade_info.local_order_id;
                j["exec_id"] = std::string(trade_info.exec_id);
                j["price"] = trade_info.price;
                j["quantity"] = trade_info.quantity;
                j["trade_time"] = trade_info.trade_time;
                j["trade_amount"] = trade_info.trade_amount;
                j["report_index"] = trade_info.report_index;
                j["order_exch_id"] = std::string(trade_info.order_exch_id);
                j["trade_type"] = std::string(1, trade_info.trade_type);
                j["business_type"] = trade_info.business_type;
                j["side"] = trade_info.side;
                j["position_effect"] = trade_info.position_effect;*/
				
				j["order_ufx_id"] = trade_info.gOrderID;
				// 代码编号相关
				j["symbol"] = trade_info.symbol;
				j["atsSymbol"] = trade_info.atsSymbol;
				j["exchange"] = trade_info.exchange;

				j["tradeID"] = trade_info.tradeID;
				j["gTradeID;"] = trade_info.gTradeID;
				j["strategyID"] = trade_info.strategyID;     //策略的编号
				j["strategyName"] = trade_info.strategyName;     //策略的编号
				// 成交相关
				j["direction"] = trade_info.direction;
				j["offset"] = trade_info.offset;
				j["price"] = trade_info.price;
				j["volume"] = trade_info.volume;
				j["tradeTime"] = trade_info.tradeTime;
				j["gatewayName"] = trade_info.gatewayName;
				//bool external = { false };



            }

            inline void to_json(nlohmann::json &j, const XTPOrderInsertInfo &order_info)
            {
                j["order_xtp_id"] = order_info.order_xtp_id;
                j["order_client_id"] = order_info.order_client_id;
                j["ticker"] = order_info.ticker;
                j["market"] = order_info.market;
                j["price"] = order_info.price;
                j["stop_price"] = order_info.stop_price;
                j["quantity"] = order_info.quantity;
                j["price_type"] = order_info.price_type;
                j["side"] = order_info.side;
                j["position_effect"] = order_info.position_effect;
            }

            inline void to_json(nlohmann::json &j, const XTPQueryAssetRsp &asset)
            {
                j["total_asset"] = asset.total_asset;
                j["buying_power"] = asset.buying_power;
                j["security_asset"] = asset.security_asset;
                j["fund_buy_amount"] = asset.fund_buy_amount;
                j["fund_buy_fee"] = asset.fund_buy_fee;
                j["fund_sell_amount"] = asset.fund_buy_amount;
                j["fund_sell_fee"] = asset.fund_sell_fee;
                j["withholding_amount"] = asset.withholding_amount;
                j["account_type"] = asset.account_type;

                j["frozen_margin"] = asset.frozen_margin;
                j["frozen_exec_cash"] = asset.frozen_exec_cash;
                j["frozen_exec_fee"] = asset.frozen_exec_fee;
                j["pay_later"] = asset.pay_later;
                j["preadva_pay"] = asset.preadva_pay;
                j["orig_banlance"] = asset.orig_banlance;
                j["banlance"] = asset.banlance;
                j["deposit_withdraw"] = asset.deposit_withdraw;
                j["trade_netting"] = asset.trade_netting;
                j["captial_asset"] = asset.captial_asset;
                j["force_freeze_amount"] = asset.force_freeze_amount;
                j["preferred_amount"] = asset.preferred_amount;
            }

            inline void to_json(nlohmann::json &j, const XTPQueryStkPositionRsp &pos)
            {
                j["ticker"] = std::string(pos.ticker);
                j["ticker_name"] = std::string(pos.ticker_name);
                j["market"] = pos.market;
                j["total_qty"] = pos.total_qty;
                j["sellable_qty"] = pos.sellable_qty;
                j["avg_price"] = pos.avg_price;
                j["unrealized_pnl"] = pos.unrealized_pnl;
                j["yesterday_position"] = pos.yesterday_position;
                j["purchase_redeemable_qty"] = pos.purchase_redeemable_qty;
                j["position_direction"] = pos.position_direction;

            }

            inline void to_json(nlohmann::json &j, const XTPMD &md)
            {
                j["exchange_id"] = md.exchange_id;
                j["ticker"] = std::string(md.ticker);
                j["last_price"] = md.last_price;
                j["pre_close_price"] = md.pre_close_price;
                j["open_price"] = md.open_price;
                j["high_price"] = md.high_price;
                j["low_price"] = md.low_price;
                j["close_price"] = md.close_price;

                j["pre_total_long_positon"] = md.pre_total_long_positon;
                j["total_long_positon"] = md.total_long_positon;
                j["pre_settl_price"] = md.pre_settl_price;
                j["settl_price"] = md.settl_price;

                j["upper_limit_price"] = md.upper_limit_price;
                j["lower_limit_price"] = md.lower_limit_price;

                j["pre_delta"] = md.pre_delta;
                j["curr_delta"] = md.curr_delta;

                j["data_time"] = md.data_time;

                j["qty"] = md.qty;
                j["turnover"] = md.turnover;
                j["avg_price"] = md.avg_price;

                j["bid"] = std::vector<double>(md.bid, std::end(md.bid));
                j["ask"] = std::vector<double>(md.ask, std::end(md.ask));

                j["bid_qty"] = std::vector<double>(md.bid_qty, std::end(md.bid_qty));
                j["ask_qty"] = std::vector<double>(md.ask_qty, std::end(md.ask_qty));

                j["trades_count"] = md.trades_count;
                j["ticker_status"] = std::string(md.ticker_status);
                j["data_type"] = md.data_type;
            }

            inline void to_json(nlohmann::json &j, const XTPOrderCancelInfo &cancel_info)
            {
                j["order_cancel_xtp_id"] = cancel_info.order_cancel_xtp_id;
                j["order_xtp_id"] = cancel_info.order_xtp_id;
            }

            inline void to_json(nlohmann::json &j, const XTPTickByTickStruct &tick)
            {
                j["exchange_id"] = tick.exchange_id;
                j["ticker"] = std::string(tick.ticker);
                j["seq"] = tick.seq;
                j["data_time"] = tick.data_time;
                j["type"] = tick.type;
                if (tick.type == XTP_TBT_ENTRUST)
                {
                    j["channel_no"] = tick.entrust.channel_no;
                    j["seq"] = tick.entrust.seq;
                    j["price"] = tick.entrust.price;
                    j["qty"] = tick.entrust.qty;
                    j["side"] = std::string(1, tick.entrust.side);
                    j["ord_type"] = std::string(1, tick.entrust.ord_type);
                } else if (tick.type == XTP_TBT_TRADE)
                {
                    j["channel_no"] = tick.trade.channel_no;
                    j["seq"] = tick.trade.seq;
                    j["price"] = tick.trade.price;
                    j["qty"] = tick.trade.qty;
                    j["money"] = tick.trade.money;
                    j["bid_no"] = tick.trade.bid_no;
                    j["ask_no"] = tick.trade.ask_no;
                    j["trade_flag"] = std::string(1, tick.trade.trade_flag);
                }
            }

			//inline void to_json(nlohmann::json &j, const TDF_MARKET_DATA &tdfMktData)
			//{
			//	j["szWinCode"] = std::string(tdfMktData.szWindCode);
			//	j["total_volume"] = tdfMktData.iVolume;
			//	j["time"] = tdfMktData.nTime;
			//	j["status"] = tdfMktData.nStatus;
			//	j["pre_close"] = tdfMktData.nPreClose;
			//	j["open"] = tdfMktData.nOpen;
			//	j["high"] = tdfMktData.nHigh;
			//	j["low"] = tdfMktData.nLow;
			//	j["match"] = tdfMktData.nMatch;
			//	j["bid"] = std::vector<__int64>(tdfMktData.nBidPrice, std::end(tdfMktData.nBidPrice));
			//	j["ask"] = std::vector<__int64>(tdfMktData.nAskPrice, std::end(tdfMktData.nAskPrice));
			//	j["bid_qty"] = std::vector<__int64>(tdfMktData.nBidVol, std::end(tdfMktData.nBidVol));
			//	j["ask_qty"] = std::vector<__int64>(tdfMktData.nAskVol, std::end(tdfMktData.nAskVol));
			//	
			//}

			inline void to_json(nlohmann::json &j, const TDF_ORDER &tdfOrder)
			{
				j["order_szWindCode"] = std::string(tdfOrder.szWindCode);
				j["order_szCode"] = std::string(tdfOrder.szCode);
				j["business_day"] = tdfOrder.nActionDay;
				j["insert_time"] = tdfOrder.nTime;
				j["order_no"] = tdfOrder.nOrder;
				j["price"] = tdfOrder.nPrice;
				j["quantity"] = tdfOrder.nVolume;
				j["order_kind"] = std::string(1, tdfOrder.chOrderKind);
				j["function_code"] = std::string(1, tdfOrder.chFunctionCode);
				j["broker"] = tdfOrder.nBroker;
				j["order_status"] = std::string(1, tdfOrder.chStatus);
				j["flag"] = std::string(1, tdfOrder.chFlag);
				j["market_ID"] = tdfOrder.pCodeInfo->nMarketID;
				j["data_level"] = tdfOrder.pCodeInfo->nDataLevel;
				j["data_source"] = tdfOrder.pCodeInfo->nFlag;
				j["type"] = tdfOrder.pCodeInfo->nType;
			}

			inline void to_json(nlohmann::json &j, const TDF_TRANSACTION &tdfTrans)
			{
				j["order_szWindCode"] = std::string(tdfTrans.szWindCode);
				j["order_szCode"] = std::string(tdfTrans.szCode);
				j["business_day"] = tdfTrans.nActionDay;
				j["insert_time"] = tdfTrans.nTime;
				j["trans_no"] = tdfTrans.nIndex;
				j["price"] = tdfTrans.nPrice;
				j["quantity"] = tdfTrans.nVolume;
				j["turnover"] = tdfTrans.nTurnover;
				j["direction"] = tdfTrans.nBSFlag;
				j["order_kind"] = std::string(1, tdfTrans.chOrderKind);
				j["function_code"] = std::string(1, tdfTrans.chFunctionCode);
				j["ask_order_no"] = tdfTrans.nAskOrder;
				j["bid_order_no"] = tdfTrans.nBidOrder;
				j["market_ID"] = tdfTrans.pCodeInfo->nMarketID;
				j["data_level"] = tdfTrans.pCodeInfo->nDataLevel;
				j["data_source"] = tdfTrans.pCodeInfo->nFlag;
				j["type"] = tdfTrans.pCodeInfo->nType;
			}

            template<typename T>
            std::string to_string(const T &ori)
            {
                nlohmann::json j;
                to_json(j, ori);
                return j.dump();
            }
        }
    }
}


#endif //KUNGFU_SERIALIZE_H
