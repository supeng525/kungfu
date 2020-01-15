#pragma once
/*
Author: li.dp@outlook.com
Date: 2017.11.21
Comment: 恒生网关的功能的实现
*/
#ifndef UFXGATEWAY_H
#define UFXGATEWAY_H

#include <iostream>
#include <string>
#include <map>
//#include <qmutex.h>
//#include "glog/GLogWrapper.h"
//#include "gatewayBase/BaseTask.h"
//#include "gatewayBase/BaseGateway.h"
#include "t2sdk_interface.h"  // 恒生TCP通讯接口
#include "UFXBusinessInterface.h"
#include "ufx_data_struct.h"
//#include "gatewayBase/ETFComposition.h"

using std::string;
using std::map;
//using std::cout;

#define LOG(x) std::cout

/// 定义恒生系统的功能号
#define UFX_FUNC_HEART_BEAT	10000
#define UFX_FUNC_LOGIN		10001

class UFX_Callback {
	public:
		//virtual void onTick(int day, int time, const char* code, float open, float high, float low, float close) = 0;
		virtual void _onDisconnected() = 0;
		virtual void _onOrderEvent(std::shared_ptr<ufx_data::OrderInfo> &orderEvent) = 0;
		virtual void _onTradeEvent(std::shared_ptr<ufx_data::TradeInfo> &tradeEvent) = 0;
		virtual void _onCancelError() = 0;
		virtual void _onQueryPosition(std::shared_ptr<ufx_data::PositionInfo> &pos, bool is_last) = 0;
		virtual void _onQueryAsset(std::shared_ptr<ufx_data::AccountInfo> &acc) = 0;
		virtual void msg(string m) = 0;
};


class CUFXUnionConnection;
void ShowPacket(IF2UnPacker *lpUnPacker);

class UFXGateway : public CCallbackInterface, public CSubCallbackInterface
{
public:
	UFXGateway();
	
	~UFXGateway();


public:
	/// Gateway 接口
	virtual bool connect(const string &user, const string &password, string &errorMsg) final;

	virtual bool disconnect() final;

	virtual bool sendOrder(std::shared_ptr<ufx_data::OrderInfo> order) final;

	virtual bool cancelOrder(std::shared_ptr<ufx_data::OrderInfo> order) final;

	virtual bool cancelAllOrders() final;

	// 获取ETF申赎清单
	//virtual bool getETFComposition(string symbol, string exchange, ETFComposition & out_etf) final;

	// 查询资金
	virtual bool queryAccount() final;

	virtual bool queryPosition() final;

	// 查询所有订单
	virtual bool queryOrders() final;

	virtual bool queryTrades() final;

	virtual bool subscribe(string symbol, string exchange) final;

	virtual string getUser() final;

	// 定时查询资金
	//void timerQuey(Event ev);

	string sendOrder(string symbol, string exchange, string direction, string offset, double price, int volume);
	bool cancelOrder(string symbol, string direction, double price);
	bool cancelOrder(string order_id);
	static void setMdCallback(UFX_Callback* callback) { tdCallback = callback; };

public:
	/// UFXGateway特有的接口
	bool queryAccountList();
	bool queryAssetList();
	bool queryStockPosition();
	bool queryFuturePosition();
	bool queryStockOrders();
	bool queryFutureOrders();

	bool queryETFBaseMsg(string symbol, string exchange);
	bool queryETFList(string symbol, string exchange);

	bool sendETFEntrust(std::shared_ptr<ufx_data::OrderInfo> order);

public:
	/// CCallbackInterface
	void FUNCTION_CALL_MODE OnConnect(CConnectionInterface *lpConnection);
	void FUNCTION_CALL_MODE OnSafeConnect(CConnectionInterface *lpConnection);
	void FUNCTION_CALL_MODE OnRegister(CConnectionInterface *lpConnection);
	void FUNCTION_CALL_MODE OnClose(CConnectionInterface *lpConnection);
	void FUNCTION_CALL_MODE OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData);
	void FUNCTION_CALL_MODE Reserved1(void *a, void *b, void *c, void *d);
	void FUNCTION_CALL_MODE Reserved2(void *a, void *b, void *c, void *d);
	int  FUNCTION_CALL_MODE Reserved3();
	void FUNCTION_CALL_MODE Reserved4();
	void FUNCTION_CALL_MODE Reserved5();
	void FUNCTION_CALL_MODE Reserved6();
	void FUNCTION_CALL_MODE Reserved7();
	void FUNCTION_CALL_MODE OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *lpUnPackerOrStr, int nResult);
	void FUNCTION_CALL_MODE OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult);
	void FUNCTION_CALL_MODE OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage* lpMsg);

public:
	/// CSubCallbackInterface
	unsigned long  FUNCTION_CALL_MODE QueryInterface(const char *iid, IKnown **ppv)
	{
		return 0;
	}
	unsigned long  FUNCTION_CALL_MODE AddRef()
	{
		return 0;
	}
	unsigned long  FUNCTION_CALL_MODE Release()
	{
		return 0;
	}

	void FUNCTION_CALL_MODE OnReceived(CSubscribeInterface *lpSub, int subscribeIndex, const void *lpData, int nLength, LPSUBSCRIBE_RECVDATA lpRecvData);
	void FUNCTION_CALL_MODE OnRecvTickMsg(CSubscribeInterface *lpSub, int subscribeIndex, const char* TickMsgInfo);

public:
	bool isConnected = { false };
	bool isLogin = { false };

	bool _login(string opterID, string password, string &errorMsg);

private:
	// UFX 配置对象，从ini文件读取操作员、账户信息
	//CConfigInterface * ufxConfig = { NULL };
	//string _UFXUserIni = "UFXUser.ini";
	//bool _loadConfig(std::string iniFile);

	// 根据order中的direction和offset，获得UFX的direction
	string _getDirectionFromDF(string symbol, string exchange, string direction, string offset);

	// 根据恒生的entrust_direct，获得direct和offset
	bool _getDirectOffetFromUFX(string ufx_entrust_direction, string &direction, string &offset);

	// 生成订单请求号
	int _generateReqID() { return ++_reqID; }
	
	// 解析包头
	int _parseHeader(IF2UnPacker * lpUnPacker, int &dataCount);

	// 解析下单之后，返回的订单回报
	void _praseRspOrder(IF2UnPacker * lpUnPacker, int orderReqID);

	// 解析查询返回的订单信息
	void _parseRtnOrder(IF2UnPacker * lpUnPacker);
	// 解析成交信息
	void _parseTrade(IF2UnPacker * lpUnPacker);
	// 解析股票持仓信息
	void _parseStockPosition(IF2UnPacker * lpUnPacker);
	// 解析期货持仓信息
	void _parseFuturePosition(IF2UnPacker * lpUnPacker);
	// 解析资金账户信息
	void _parseAccount(IF2UnPacker * lpUnPacker);
	// 解析撤单信息
	void _parseRspCancel(IF2UnPacker *lpUnPacker);
	// 解析资产单元的资金信息
	void _parseRspUnitAsset(IF2UnPacker *lpUnPacker);
	// 解析ETF基础数据
	void _parseRspETFBase(IF2UnPacker *lpUnPacker);
	// 解析ETF成分股信息
	void _parseRspETFList(IF2UnPacker *lpUnpacker);

	// 风控检查，主要检查自成交
	bool _riskCheck(std::shared_ptr<ufx_data::OrderInfo> newOrder);
private:
	// 是否已经查询过ETF申赎清单
	bool _etfInfoQueried = { false };
	// ETF申述清单, key: etf的sybol， 510300， TODO: 暂时只支持上交所
	//std::map<string, ETFComposition> _etfInfoList; 

private:
	string gatewayName = "ufx";

	void onOrder(std::shared_ptr<ufx_data::OrderInfo> newOrder) {};
	void onPosition(std::shared_ptr<ufx_data::PositionInfo> pos) {};
	void onAccount(std::shared_ptr<ufx_data::AccountInfo> acc) {};
	void onTrade(std::shared_ptr<ufx_data::TradeInfo>) {};

	const char * _2Str(const char *str) { return NULL == str ? "" : str; }

	CConnectionInterface *ufxConnection = { NULL };

	// key: reqID, value: Order
	std::map<int, std::shared_ptr<ufx_data::OrderInfo> > _orderCache;
	// key: tradeID, 保存所有的成交信息
	std::map<string, std::shared_ptr<ufx_data::TradeInfo> > _tradeCache;
	// key: entrust_no, value: reqID，主要用于撤单，因为撤单时不返回第三方系统ID
	std::map<int, int> _entrustReqMap;

	std::map<string, string> exchangeUFX2DF;
	std::map<string, string> exchangeDF2UFX;
	
	std::map<string, string> directUFX2DF;
	std::map<string, string> directDF2UFX;

	std::map<string, string> orderStatusUFX2DF;
	std::map<string, string> orderStatusDF2UFX;
	
	// 用户令牌，每次登陆后获取
	string _userToken = "";

	// onRegisted Callback中设置为True
	bool _registered = { false };

	// 是否已经查询过订单
	bool _orderQueried = { false };
	// 是否已经查询过成交
	bool _tradeQueried = { false };

	// 系统请求号
	int _reqID = { 0 };
	// 计时器计数器，onTime时递增
	int _timerCounter = { 0 };
	// 是否定时查询订单和成交
	bool _forceQuery = { false };
	// 定时查询订单和成交的周期
	int _forceQueryPeriod = { 0 };

	// 从配置文件获得如下信息
	string _optId = "";
	string _optPwd = "";
	string _MacAddress = "";
	string _IPAddress = "";
	string _station= "";
	string _authorizationID = "";

	string _acctID = "";	// 资金账户
	string _stockUnitID = "";	// 股票资产单元
	string _futureUnitD = "";	// 期货资产单元
							//2017/12/11 yc
							//错误信息
	string _strLastErrMsg = "";
	// UFX 配置对象，从ini文件读取操作员、账户信息
	CConfigInterface *ufxConfig = { NULL };
	CConfigInterface *subConfig = { NULL };
	CUFXUnionConnection *unionCon = { NULL };

	bool _loadConfig(CConfigInterface *ufxConfig);
	
	// 
	static UFX_Callback* tdCallback;

};


#endif