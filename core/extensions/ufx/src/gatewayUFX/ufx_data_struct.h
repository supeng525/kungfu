#pragma once
#ifndef UFX_DATA_STRUCT_H
#define UFX_DATA_STRUCT_H

#include <iostream>
#include <string>
using namespace std;

namespace ufx_data
{
// 委托单信息
struct OrderInfo
{
	// atsSymbol用于ATS
	string atsSymbol;
	string symbol;
	string exchange;
	string name;   // 股票名称

	string orderSysID;		// 交易所的报单编号
	string orderDeskID;	// 柜台的报单编号
	string orderReqID;		// gateway的报单编号
	string gOrderID;		// gatewayName.orderReqID，全局唯一
							//int strategyID;     //策略的编号
	string strategyName;     //策略名称

							  // 报单相关
	string direction;		// 买入/卖出
	string offset;		// 开仓/平仓
	double price = { 0 };
	int totalVolume = { 0 };	// 报单总数量
	int tradeVolume = { 0 };	// 已成交数量
	double tradePrice = { 0 };	// 成交均价
	string status;		// 订单状态
	string statusMsg;	// 状态信息
	string orderTime;
	string cancelTime;

	int frontID = { 0 };
	int sessionID = { 0 };
	bool external = { false };
	string gatewayName;
};

// 成交信息
struct TradeInfo
{
	// 代码编号相关
	string symbol;
	string atsSymbol;
	string exchange;

	string tradeID;
	string gTradeID;
	string gOrderID;
	int strategyID;     //策略的编号
	string strategyName;     //策略的编号

							  // 成交相关
	string direction;
	string offset;
	double price;
	int volume;
	string tradeTime;
	string gatewayName;
	bool external = { false };
};

// 持仓信息
// TODO: 对于期货，还需要优化
struct PositionInfo
{
	string symbol;
	string name;			// 合约名称
	string exchange;
	string atsSymbol;		// symbol.exchange
	string atsPositionName;	// 仓位名称，atsSymbol.direction
	string accountId;      // 资金账户ID

	string direction;		// 持仓方向
	string hedgeFlag;		// 投机、套保、套利标志
	int position{ 0 };		// 总持仓量（不包含组合占用）
	double price{ 0 };      // 持仓均价
	int comboPosition{ 0 };	// 组合占用持仓
	int available{ 0 };		// 可用持仓量
	int frozen{ 0 };		// 总冻结，包括非流通+报单冻结(平仓)+异常冻结
	int openFrozen{ 0 };	// 开仓冻结

	int ydPosition{ 0 };	// 昨持仓量
	int ydFrozen{ 0 };
	int tdPosition{ 0 };	// 今持仓
	int tdFrozen{ 0 };

	double marketValue{ 0 };    // 证券市值
	string time;				// 时间
	string gatewayName;
};

enum AccountType
{
	UNION = 0,	// 综合账户
	STOCK,		// 股票账户
	FUTURE,		// 期货账户
	ETF_OPTION,	// 股票期权账户
	CREDIT		// 融资融券信用账户
};

// 用户帐号资金账户信息
struct AccountInfo
{
	string accountId;	// 如果是多层账户的，则: 父账户.子账户
	string gAccountId;	// gatewayName.accountID
	string gatewayName;
	string currency;
	AccountType accType;	// 账户类型
	double preBalance;	// 昨日账户结算净值
	double balance;		// 账户总资金
	double available;	// 可用资金
	double commission;	// 今日手续费
	double margin;		// 保证金占用
	double closeProfit;	// 平仓盈亏
	double positionProfit;	// 持仓盈亏
	double totalAsset;	// 总资产
	double netAsset;	// 净资产
	string time;		// 更新时间
};

	const string DF_OS_NEW = "DF_OS_NEW";		// 已报
	const string DF_OS_ERROR = "DF_OS_ERROR";	// 废单
	const string DF_OS_PART_TRADE = "DF_OS_PART_TRADE";	// 部成
	const string DF_OS_ALLTRD = "DF_OS_ALLTRD";		// 已成
	const string DF_OS_CANCEL = "DF_OS_CANCEL";		// 已撤
	const string DF_OS_OTHER = "DF_OS_OTHER";


	const string DF_EXCH_SSE = "DF_EXCH_SSE";
	const string DF_EXCH_SZE = "DF_EXCH_SZE";
	const string DF_EXCH_SHFE = "DF_EXCH_SHFE";
	const string DF_EXCH_CZCE = "DF_EXCH_CZCE";
	const string DF_EXCH_CFFEX = "DF_EXCH_CFFEX";
	const string DF_EXCH_DCE = "DF_EXCH_DCE";
	const string DF_EXCH_HK_SSE = "DF_EXCH_HK_SSE";
	const string DF_EXCH_HK_SZE = "DF_EXCH_HK_SZE";

	const string DF_DIRECTION_BUY = "DF_DIRECTION_BUY";
	const string DF_DIRECTION_SELL = "DF_DIRECTION_SELL";
	const string DF_DIRECTION_ETFCREATE = "DF_DIRECTION_ETFCREATE";	// ETF申购
	const string DF_DIRECTION_ETFREDEEM = "DF_DIRECTION_ETFREDEEM";	// ETF赎回

	const string DF_OFFSET_OPEN = "DF_OFFSET_OPEN";
	const string DF_OFFSET_CLOSE = "DF_OFFSET_CLOSE";

	const string DF_POSITION_LONG = "DF_POSITION_LONG";
	const string DF_POSITION_SHORT = "DF_POSITION_SHORT";
};



#endif // UFX_DATA_STRUCT_H