#include "UFXGateway.h"
#include "UFXUnionConnection.h"

#pragma comment(lib,"t2sdk.lib")

#include <algorithm>
#include <chrono>
#include <ctime>
#include <string>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <direct.h>
#include <stdio.h>
using namespace std::chrono;
using namespace ufx_data;

#define FORMAT3(x) round(x * 1000)
#define UN_FORMAT3(x) x/1000.0

template<typename T> string numberToString(const T& t)
{
	ostringstream oss;  	//创建一个格式化输出流
	oss << t;				//把值传递到流中
	return oss.str();
}

string getTimeString(string format)
{
	system_clock::time_point now = system_clock::now();
	std::time_t time = system_clock::to_time_t(now);

	std::stringstream ss;   //把时间数据格式转换为字符串
	ss << std::put_time(std::localtime(&time), format.c_str());

	return ss.str();
}

UFXGateway::UFXGateway()
{
	//2017/12/12 yc
	ufxConfig = NewConfig();
	ufxConfig->AddRef();
	char buffer[1024];
	_getcwd(buffer, 1024);
	//printf("The current directory is: %s ", buffer);
	ufxConfig->Load("UFXUser.ini");
	//LOG(INFO) << "UFXGateway, 加载恒生配置文件，UFXUser.ini, " << "ServerAddress, "
	//	<< ufxConfig->GetString("t2sdk", "servers", "") << std::endl;


	subConfig = NewConfig();
	subConfig->AddRef();
	subConfig->Load("UFXsubscriber.ini");
	//LOG(INFO) << "UFXGateway, 加载恒生配置文件，UFXsubScriber.ini, " << "ServerAddress, "
	//	<< subConfig->GetString("t2sdk", "servers", "") << std::endl;
	_loadConfig(ufxConfig);

	/*ufxConfig = NewConfig();
	ufxConfig->AddRef();
	ufxConfig->Load("UFXsubscriber.ini");
	LOG(INFO) << "UFXGateway, 加载恒生配置文件，UFXsubscriber.ini, " << "ServerAddress, "
		<< ufxConfig->GetString("t2sdk", "servers", "") << std::endl;

	ufxConnection = NewConnection(ufxConfig);
	ufxConnection->AddRef();

	_loadConfig(_UFXUserIni);*/
	// 初始化 UFX和DF内部交易所的映射关系
	exchangeDF2UFX[DF_EXCH_SSE] = "1";
	exchangeDF2UFX[DF_EXCH_SZE] = "2";
	exchangeDF2UFX[DF_EXCH_SHFE] = "3";
	exchangeDF2UFX[DF_EXCH_CZCE] = "4";
	exchangeDF2UFX[DF_EXCH_CFFEX] = "7";
	exchangeDF2UFX[DF_EXCH_DCE] = "9";
	exchangeDF2UFX[DF_EXCH_HK_SSE] = "35";
	exchangeDF2UFX[DF_EXCH_HK_SZE] = "o";

	for (auto iter = exchangeDF2UFX.begin(); iter != exchangeDF2UFX.end(); iter++)
	{
		exchangeUFX2DF[iter->second] = iter->first;
	}
/*
	directDF2UFX[DF_ORDER_BUY] = "1";
	directDF2UFX[DF_ORDER_SELL] = "2";
	directDF2UFX[DF_ORDER_ETFCREATE] = "26";
	directDF2UFX[DF_ORDER_ETFREDEEM] = "27";
	for (auto iter = directDF2UFX.begin(); iter != directDF2UFX.end(); iter++)
	{
		directUFX2DF[iter->second] = iter->first;
	}*/
	
	orderStatusUFX2DF["1"] = DF_OS_NEW;		// 未报
	orderStatusUFX2DF["2"] = DF_OS_NEW;		// 待报
	orderStatusUFX2DF["3"] = DF_OS_NEW;		// 正报
	orderStatusUFX2DF["4"] = DF_OS_NEW;		// 已报
	orderStatusUFX2DF["5"] = DF_OS_ERROR;	// 废单
	orderStatusUFX2DF["6"] = DF_OS_PART_TRADE;	// 部成
	orderStatusUFX2DF["7"] = DF_OS_ALLTRD;		// 已成
	orderStatusUFX2DF["8"] = DF_OS_CANCEL;		// 部撤
	orderStatusUFX2DF["9"] = DF_OS_CANCEL;		// 已撤
	orderStatusUFX2DF["a"] = DF_OS_CANCEL;		// 待撤

	// 将系统启动时间记录到报单编号
	
	_reqID = atoi(getTimeString("%H%M").c_str()) * 10000;

	// 定时查询资金
	//this->eventEngine->addEvent(EVENT_TIMER, this, &UFXGateway::timerQuey);
}

UFXGateway::~UFXGateway()
{
	if (ufxConfig)
	{
		ufxConfig->Release();
		ufxConfig = NULL;
	}
	if (ufxConnection)
	{
		ufxConnection->Release();
		ufxConnection = NULL;
	}

	if (subConfig)
	{
		subConfig->Release();
		subConfig = NULL;
	}
	if (unionCon)
	{
		delete unionCon;
		unionCon = NULL;
	}
}

UFX_Callback* UFXGateway::tdCallback = NULL;

bool UFXGateway::connect(const string &user, const string &password, string &errorMsg)
{
	// 未创建ufxconnection,则创建连接
	if (this->isConnected == false)
	{
		try {
			unionCon = new CUFXUnionConnection(ufxConfig, subConfig, this);
		}
		catch (MyException exception)
		{
			_strLastErrMsg = exception.m_ErrMsg;
			//LOG(ERROR) << "UFXGateway::connect, UFX连接失败, 错误消息" << _strLastErrMsg;
			if(unionCon!=NULL)
				delete unionCon;
			return false;
		}
		this->isConnected = true;
		/*int rtn = ufxConnection->Create2BizMsg(this);
		if (0 == rtn)
		{
			if (rtn = ufxConnection->Connect(DEFAULT_TIME_OUT)) // 1000ms timeout
			{
				LOG(ERROR) << "UFXGateway::connect, UFX连接失败， 错误代码, " 
					<< rtn << ", 错误消息" <<  ufxConnection->GetErrorMsg(rtn);
			}
			else
			{
				this->isConnected = true;
			}
		}
		else
		{
			LOG(ERROR) << "UFXGateway::connect, UFX连接失败， 错误代码, " 
				<< rtn << ", 错误消息" << ufxConnection->GetErrorMsg(rtn);
		}
		while (_registered == false)
		_sleep(1);*/
	}
	
	// 需要等到onRegister之后才能Login
	this->isLogin = _login(user, password, errorMsg);
	return this->isLogin;
}

bool UFXGateway::disconnect()
{
	if (isLogin)
	{
		CUFXUserLogoff logOff = CUFXUserLogoff();
		logOff.m_in_user_token = _userToken;
		return unionCon->SyncCallUFX(&logOff) == RET_OK;
	}
	return true;
}

bool UFXGateway::queryAccountList()
{
	CUFXQryAccountList accQry = CUFXQryAccountList();

	accQry.m_in_user_token = _userToken;

	int iRetCallUFX = unionCon->ASyncCallUFX(&accQry);
	if (iRetCallUFX <= 0)
	{
		//LOG(ERROR) << "UFXGateway::queryAccountList，查询出错！" << std::endl;
		return false;
	}
	return true;
}

bool UFXGateway::queryAssetList()
{
	CUFXQryAssetUnitList assetQry = CUFXQryAssetUnitList();

	assetQry.m_in_user_token = _userToken;

	int iRetCallUFX = unionCon->ASyncCallUFX(&assetQry);
	if (iRetCallUFX <= 0)
	{
		//LOG(ERROR) << "UFXGateway::queryAssetList，查询出错！" << std::endl;
		return false;
	}
	return true;
}

bool UFXGateway::queryFuturePosition()
{
	CUFXQryFuturePos qryFuturePos;
	qryFuturePos.m_in_user_token = _userToken;
	qryFuturePos.m_in_account_code = _acctID;
	qryFuturePos.m_in_asset_no = _futureUnitD;
	int iRetCallUFX = unionCon->ASyncCallUFX(&qryFuturePos);
	return iRetCallUFX > 0;
}

bool UFXGateway::queryPosition()
{
	return queryStockPosition() && queryFuturePosition();
}

bool UFXGateway::queryTrades()
{
	CUFXQryStockTrades qryStockTrades;
	qryStockTrades.m_in_user_token = _userToken;
	qryStockTrades.m_in_account_code = _acctID;
	qryStockTrades.m_in_asset_no = _stockUnitID;
	int iRetCallUFX = unionCon->ASyncCallUFX(&qryStockTrades);
	return iRetCallUFX > 0;
}

bool UFXGateway::subscribe(string symbol, string exchange)
{
	//LOG(WARNING) << "UFXGateway::subscribe, 不支持订阅行情"
	//	<< std::endl;
	return false;
}

string UFXGateway::getUser()
{
	return _optId;
}

//void UFXGateway::timerQuey(Event ev)
//{
//	if (!isLogin)
//		return;
///*  
//    // 测试代码,自动生成订单
//	std::shared_ptr<OrderInfo> order = std::make_shared<OrderInfo>();
//	order->exchange = "SSE";
//	order->symbol = "510310";
//	order->atsSymbol = "510310.SSE";
//	order->direction = DF_DIRECTION_BUY;
//	order->offset = DF_OFFSET_OPEN;
//	order->price = 3.2;
//	order->status = DF_OS_NEW;
//	order->orderReqID = QString::number( _generateReqID());
//	order->gOrderID = order->orderReqID + ".UFX";
//	this->onOrder(order);
//*/
//
//	// 查询资金&持仓，持仓只查一次
//	if (!_tradeQueried)
//		_tradeQueried = queryTrades();
//
//	if (!_orderQueried)
//		_orderQueried = queryOrders();
//
//	/*
//	if (!_etfInfoQueried)
//	{
//		_etfInfoQueried = true;
//		for (auto itor = _etfInfoList.begin(); itor != _etfInfoList.end(); itor++)
//		{
//			if (itor->second.baseInfoInited != true)
//			{
//				bool qryResult = queryETFBaseMsg(itor->first, DF_EXCH_SSE);
//				if (qryResult == false)
//				{
//					postLog(LogLevel::WARNING, QString::fromLocal8Bit("查询%1基础信息失败, %2").
//						arg(itor->first, QString::fromStdString(unionCon->GetLastError())),
//						"UFXGateway::timerQuey");
//				}
//			}
//			else if (itor->second.constituentInited != true)
//			{
//				CUFXQryETFList qryETFList;
//				qryETFList.m_in_user_token = _userToken;
//				qryETFList.m_in_market_no = exchangeDF2UFX[DF_EXCH_SSE];
//				qryETFList.m_in_etf_code = itor->first.toStdString();
//				int iRetCallUFX = unionCon->SyncCallUFX(&qryETFList); // 同步操作
//				if (iRetCallUFX != RET_OK)
//				{
//					postLog(LogLevel::WARNING, QString::fromLocal8Bit("%1申赎清单查询出错, %2").
//						arg(itor->first, QString::fromStdString(qryETFList.m_out_ErrorMsg)),
//						"UFXGateway::timerQuey");
//				}
//				for (auto itor2 = qryETFList.m_out_etf_stock_list.begin();
//					itor2 != qryETFList.m_out_etf_stock_list.end();
//					itor2++)
//				{
//					QString symbol = QString::fromStdString((*itor2)->stock_code);
//					QString exchange = QString::fromStdString(exchangeUFX2DF[(*itor2)->market_no]);
//					QString atsSymbol = symbol + "." + exchange;
//					itor->second.constituentStocks[atsSymbol] = std::make_shared<ConstituentStock>();
//					itor->second.constituentStocks[atsSymbol]->symbol = symbol;
//					itor->second.constituentStocks[atsSymbol]->exchange = exchange;
//					itor->second.constituentStocks[atsSymbol]->atsSymbol = atsSymbol;
//					//itor->second.constituentStocks[atsSymbol]->name = name;
//					itor->second.constituentStocks[atsSymbol]->volume = (*itor2)->stock_amount;
//					CashReplaceFlag replaceFlag = static_cast<CashReplaceFlag>(QString::fromStdString((*itor2)->replace_flag).trimmed().toInt());
//					itor->second.constituentStocks[atsSymbol]->cashReplaceFlag = replaceFlag;
//					itor->second.constituentStocks[atsSymbol]->cashPremium = (*itor2)->replace_ratio;
//					itor->second.constituentStocks[atsSymbol]->replaceAmount = (*itor2)->replace_balance;
//				}
//				if (itor->second.constituentStocks.size() == itor->second.Recordnum)
//				{
//					postLog(LogLevel::INFO, QString::fromLocal8Bit("完成%1申赎清单查询!").arg(itor->first), "UFXGateway::timerQuey");
//					itor->second.constituentInited = true;
//				}
//				else
//				{
//					postLog(LogLevel::WARNING, QString::fromLocal8Bit("%1申赎清单查询出错,预期%2条记录，实际%3条记录").
//						arg(itor->first).arg(itor->second.Recordnum).arg(itor->second.constituentStocks.size()),
//						"UFXGateway::timerQuey");
//					itor->second.constituentInited = false;
//				}
//			}
//			_etfInfoQueried = _etfInfoQueried && itor->second.baseInfoInited && itor->second.constituentInited;
//		}
//	}
//	*/
//
//	if(_timerCounter % 3 == 0) // 3秒查一次
//		queryAccount();
//
//	if (_timerCounter % 5 == 1)
//		queryPosition();
//
//	if (_forceQuery && _timerCounter % _forceQueryPeriod == 1)
//		queryTrades();
//
//	if (_forceQuery && _timerCounter % _forceQueryPeriod == 2)
//		queryOrders();
//
//	_timerCounter++;
//}

bool UFXGateway::queryStockOrders()
{
	CUFXQryOrders qryOrders;
	qryOrders.m_in_user_token = _userToken;
	qryOrders.m_in_account_code = _acctID;
	qryOrders.m_in_asset_no = _stockUnitID;
	if(_forceQuery)
		qryOrders.m_in_state_list = "1,2,3,4,5,6,7,8,9,a";
	else
		qryOrders.m_in_state_list = "1,2,3,4,6";
	int iRetCallUFX = unionCon->ASyncCallUFX(&qryOrders);
	return iRetCallUFX > 0;
}

bool UFXGateway::queryFutureOrders()
{
	//CUFXQryOrders qryOrders;
	//qryOrders.m_in_user_token = _userToken;
	//qryOrders.m_in_account_code = _acctID;
	//qryOrders.m_in_asset_no = _futureUnitD;
	////qryOrders.m_in_state_list = "1,2,3,4,6";
	//int iRetCallUFX = unionCon->ASyncCallUFX(&qryOrders);
	//return iRetCallUFX > 0;
	return true;
}


bool UFXGateway::queryETFBaseMsg(string symbol, string exchange)
{
	CUFXQryETFBaseMsg qryETFBaseMsg;
	qryETFBaseMsg.m_in_user_token = _userToken;
	qryETFBaseMsg.m_in_market_no = exchangeDF2UFX[exchange];
	qryETFBaseMsg.m_in_etf_code = symbol;
	int iRetCallUFX = unionCon->ASyncCallUFX(&qryETFBaseMsg);
	return iRetCallUFX > 0;
}

bool UFXGateway::queryETFList(string symbol, string exchange)
{
	CUFXQryETFList qryETFList;
	qryETFList.m_in_user_token = _userToken;
	qryETFList.m_in_market_no = exchangeDF2UFX[exchange];
	qryETFList.m_in_etf_code = symbol;
	int iRetCallUFX = unionCon->ASyncCallUFX(&qryETFList);
	return iRetCallUFX > 0;
}

bool UFXGateway::sendETFEntrust(std::shared_ptr<OrderInfo> order)
{
	// 给订单编号
	int orderReqID = _generateReqID();
	order->gatewayName = gatewayName;
	order->orderReqID = numberToString(orderReqID);
	order->gOrderID = order->gatewayName + "." + order->orderReqID;
	order->status = DF_OS_NEW;


	CUFXETFEntrust etfEntrust;
	etfEntrust.m_in_user_token = _userToken;
	etfEntrust.m_in_account_code = _acctID;
	etfEntrust.m_in_asset_no = _stockUnitID;
	etfEntrust.m_in_market_no = exchangeDF2UFX[order->exchange];
	etfEntrust.m_in_stock_code = order->symbol;
	etfEntrust.m_in_entrust_direction = _getDirectionFromDF(order->symbol, order->exchange, order->direction, order->offset);

	// 申赎数量单位为篮子数，最小单位
	etfEntrust.m_in_entrust_amount = order->totalVolume;
	
	etfEntrust.m_in_extsystem_id = orderReqID;
	etfEntrust.m_in_mac_address = _MacAddress;
	etfEntrust.m_in_ip_address = _IPAddress;

	int iRetCallUFX = unionCon->ASyncCallUFX(&etfEntrust, orderReqID);
	_orderCache[orderReqID] = order;
	//LOG(INFO) << string("UFXGateway::sendETFEntrust, gOrderID, %1, orderDeskID, %2, atsSymbol, %3, direction, %4, totalVolume, %5, iRetCallUFX, %6")
	//	<<order->gOrderID << order->orderDeskID << order->atsSymbol << order->direction
	//	<< order->totalVolume << iRetCallUFX << std::endl;
	if (iRetCallUFX > 0)
	{
		return true;
	}
	else
	{
		//LOG(INFO) << string("ETF下单失败, %1,%2,%3")
		//	<<order->direction <<order->atsSymbol <<order->totalVolume
		//	<<"UFXGateway::sendETFEntrust" << std::endl;
		return false;
	}

	return false;
}

// UFXCallbacks
void UFXGateway::OnConnect(CConnectionInterface *lpConnection)
{
	//LOG(INFO) << "UFXGateway::OnConnect" << std::endl;
	this->isConnected = true;
}
void UFXGateway::OnSafeConnect(CConnectionInterface *lpConnection)
{
	//LOG(INFO) << "UFXGateway::OnSafeConnect" << std::endl;
	
}
void UFXGateway::OnRegister(CConnectionInterface *lpConnection)
{
	//std::cout << "UFXGateway::OnRegister" << std::endl;
	// 只有此回掉函数执行之后才可以登陆
	this->_registered = true;
}
void UFXGateway::OnClose(CConnectionInterface *lpConnection)
{
	//std::cout << "UFXGateway::OnClose" << std::endl;
	//if(tdCallback) tdCallback->_onDisconnected();
}
void UFXGateway::OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData)
{
	//std::cout << "UFXGateway::OnSent" << std::endl;
	//if (tdCallback)tdCallback->_onOrderEvent();
}
void UFXGateway::Reserved1(void *a, void *b, void *c, void *d)
{
	//std::cout << "UFXGateway::Reserved1" << std::endl;
}
void UFXGateway::Reserved2(void *a, void *b, void *c, void *d)
{
	//std::cout << "UFXGateway::Reserved2" << std::endl;
}
int  UFXGateway::Reserved3()
{
	return 0;
}
void UFXGateway::Reserved4()
{

}
void UFXGateway::Reserved5()
{

}
void UFXGateway::Reserved6()
{

}
void UFXGateway::Reserved7()
{

}
void UFXGateway::OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *lpUnPackerOrStr, int nResult)
{

}
void UFXGateway::OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult)
{

}

void ShowPacket(IF2UnPacker *lpUnPacker)
{
	lpUnPacker->First();
	int i = 0, t = 0, j = 0, k = 0;
	for (i = 0; i < lpUnPacker->GetDatasetCount(); ++i)
	{
		// 设置当前结果集
		lpUnPacker->SetCurrentDatasetByIndex(i);
		lpUnPacker->First();
		LOG(INFO) << "----------------<<"<<i<<"-----------------\n" << std::endl;

		// 打印所有记录
		for (j = 0; j < (int)lpUnPacker->GetRowCount(); ++j)
		{
			// 打印每条记录
			for (k = 0; k < lpUnPacker->GetColCount(); ++k)
			{
				LOG(INFO) << lpUnPacker->GetColName(k)<<"=";
				switch (lpUnPacker->GetColType(k))
				{
				case 'I':
					LOG(INFO) << lpUnPacker->GetIntByIndex(k) << std::endl;
					break;

				case 'C':
					LOG(INFO) << lpUnPacker->GetCharByIndex(k) << std::endl;
					break;

				case 'S':
					LOG(INFO) << lpUnPacker->GetStrByIndex(k) << std::endl;
					break;

				case 'F':
					LOG(INFO) << lpUnPacker->GetDoubleByIndex(k) << std::endl;
					break;

				case 'R':
				{
					int nLength = 0;
					void *lpData = lpUnPacker->GetRawByIndex(k, &nLength);

					// 对2进制数据进行处理
					break;
				}
				default:
					// 未知数据类型
					LOG(INFO) << "未知数据类型\n";
					break;
				}
			}
			LOG(INFO) << "--------------------------------\n" << std::endl;
			lpUnPacker->Next();
		}
	}
	lpUnPacker->First();
}


string UFXGateway::_getDirectionFromDF(string symbol, string exchange, string direction, string offset)
{
	// TODO: 由于恒生的股票、债券、期货、期权用的是不同的direction代码，所以有新业务的时候，待补充此函数
	if (direction == DF_DIRECTION_BUY)
		return "1";
	else if (direction == DF_DIRECTION_SELL)
		return "2";
	else if (direction == DF_DIRECTION_ETFCREATE)	// ETF申购
		return "26";
	else if (direction == DF_DIRECTION_ETFREDEEM)	// ETF赎回
		return "27";
	else
	{
		//LOG(ERROR)<< "UFXGateway::_getDirectionFromDF, 未能识别的交易方向，"
		//	<<" symbol, " << symbol
		//	<<", exchange, " << exchange
		//	<< ", direction, " << direction
		//	<< ", offset, " << offset << std::endl;
		return " ";
	}
}

bool UFXGateway::_getDirectOffetFromUFX(string ufx_entrust_direction, string & direction, string & offset)
{
	if (ufx_entrust_direction == "1") // 买入
	{
		direction = DF_DIRECTION_BUY;
		offset = DF_OFFSET_OPEN;
	}
	else if (ufx_entrust_direction == "2")
	{
		direction = DF_DIRECTION_SELL;
		offset = DF_OFFSET_CLOSE;
	}
	else if (ufx_entrust_direction == "26")
	{
		direction = DF_DIRECTION_ETFCREATE;
	}
	else if (ufx_entrust_direction == "27")
	{
		direction = DF_DIRECTION_ETFREDEEM;
	}

	else
	{
		//LOG(ERROR) << "UFXGateway::_getDirectionFromDF, 未能识别的交易方向，"
		//	<< " ufx_entrust_direction, " << ufx_entrust_direction
		//	<< std::endl;
		return false;
	}

	return true;
}


// 解析UFX的包头数据
int UFXGateway::_parseHeader(IF2UnPacker * lpUnPacker, int & dataSetCount)
{
	lpUnPacker->SetCurrentDatasetByIndex(0);
	int errorCode = lpUnPacker->GetInt("ErrorCode");
	dataSetCount = lpUnPacker->GetDatasetCount();
	if (errorCode != 0)
	{
		_strLastErrMsg = _2Str(lpUnPacker->GetStr("ErrorMsg"));
		std::string msgDetail = _2Str(lpUnPacker->GetStr("MsgDetail"));
		//LOG(WARNING) << "UFXGateway::_parseHeader, "
		//	<< "errorCode," << errorCode
		//	<< " errorMsgs, " << _strLastErrMsg
		//	<< ", msgDetail, " << msgDetail << std::endl;
	}
	return errorCode;
}



// 解析插入订单的相应， UFX_FUNC_ENTRUST_INSERT
void UFXGateway::_praseRspOrder(IF2UnPacker * lpUnPacker, int packID)
{
	int dataCount = 0;
	int errorCode = _parseHeader(lpUnPacker, dataCount);

	if (errorCode != 0 && dataCount == 1)
	{
		if (_orderCache.find(packID) == _orderCache.end())
		{
			return;
		}
		if (_orderCache[packID]->status != DF_OS_ERROR && _orderCache[packID]->status != DF_OS_CANCEL)
		{
			_orderCache[packID]->status = DF_OS_ERROR;
			_orderCache[packID]->statusMsg = _strLastErrMsg;
			onOrder(_orderCache[packID]);
		}
	}

	lpUnPacker->SetCurrentDatasetByIndex(1);
	for (int row = 0; row < (int)lpUnPacker->GetRowCount(); ++row)
	{
		int orderDeskID = lpUnPacker->GetInt("entrust_no");
		int orderReqID = lpUnPacker->GetInt("extsystem_id");
		int entrust_fail_code = lpUnPacker->GetInt("entrust_fail_code");
		int risk_serial_no = lpUnPacker->GetInt("risk_serial_no");
		string fail_cause = _2Str(lpUnPacker->GetStr("fail_cause"));

		//LOG(INFO) << "UFXGateway::_praseRspOrder, " << orderReqID  << ", " <<orderDeskID << std::endl;
		if (_orderCache.find(orderReqID) == _orderCache.end())
		{
			//LOG(WARNING) << "UFXGateway::_praseRspOrder, 收到插入订单相应，但未找到对应的发送订单, "
			//	<< "orderReqID, " << orderReqID
			//	<< ", orderDeskID, " << orderDeskID
			//	<< ", entrust_fail_code, " << entrust_fail_code
			//	<< ", risk_serial_no, " << risk_serial_no
			//	<< ", fail_cause, " << fail_cause << std::endl;
		}
		else
		{
			std::shared_ptr<OrderInfo> order = _orderCache[orderReqID];
			order->orderDeskID = numberToString(orderDeskID);
			_entrustReqMap[orderDeskID] = atoi(order->orderReqID.c_str());
			if (entrust_fail_code == 0)
			{
				order->status = DF_OS_NEW;
			}
			else
			{
				order->status = DF_OS_ERROR; // 错单
				order->statusMsg = fail_cause.c_str();
				auto content = string("收到错误订单")
					+ "orderReqID, " + numberToString(orderReqID)
					+ ", orderDeskID, " + numberToString(orderDeskID)
					+ ", entrust_fail_code, " + numberToString(entrust_fail_code)
					+ ", risk_serial_no, " + numberToString(risk_serial_no)
					+ ", fail_cause, " + fail_cause.c_str();
				//LOG(ERROR) << "UFXGateway::_praseRspOrder ERROR_, content, " << content << std::endl;
			}
			order->orderTime = getTimeString("%Y/%m/%d %H:%M:%S");
			this->onOrder(order);
			if (tdCallback)tdCallback->_onOrderEvent(order);
		}
		
		lpUnPacker->Next();
	}

	return;
}

// 解析查询回的股票订单， 只在启动的时候调用一次，只返回有效订单
void UFXGateway::_parseRtnOrder(IF2UnPacker * lpUnPacker)
{
	//LOG(INFO) << "UFXGateway::_parseRtnOrder " << std::endl;
	int dataCount = 0;
	int errorCode = _parseHeader(lpUnPacker, dataCount);

	if (errorCode != 0 || dataCount < 2)
		return;

	lpUnPacker->SetCurrentDatasetByIndex(1);
	for (int j = 0; j < (int)lpUnPacker->GetRowCount(); ++j)
	{
		int orderReqID = lpUnPacker->GetInt("extsystem_id");
		if (orderReqID == 0)
		{
			lpUnPacker->Next();
			continue;	// 交易员通过O32报的单子
		}
		if (_orderCache.find(orderReqID) == _orderCache.end())
		{
			if (this->_acctID.compare(lpUnPacker->GetStr("account_code")) != 0)
			{
				//LOG(ERROR) << "UFXGateway::_parseRtnOrder, 收到订单的accountID与配置文件不一致,"
				//	<< " received, " << _2Str(lpUnPacker->GetStr("account_code"))
				//	<< ", _acctID, " << _acctID << std::endl;
				lpUnPacker->Next();
				continue;
			}
			if (this->_stockUnitID.compare(lpUnPacker->GetStr("asset_no")) != 0)
			{
				//LOG(ERROR) << "UFXGateway::_parseOrder, 收到订单的asset_no与配置文件不一致,"
				//	<< " received, " << lpUnPacker->GetStr("asset_no")
				//	<< ", _assetID, " << _stockUnitID << std::endl;
				lpUnPacker->Next();
				continue;
			}
			// 未在系统中找到订单，创建新订单
			std::shared_ptr<OrderInfo> order(new OrderInfo());
			order->gatewayName = gatewayName;
			order->symbol = lpUnPacker->GetStr("stock_code");
			order->exchange = exchangeUFX2DF[lpUnPacker->GetStr("market_no")];
			order->atsSymbol = order->symbol + "." + order->exchange;
			// order->name;   // 股票名称

			order->orderSysID = _2Str(lpUnPacker->GetStr("confirm_no")); // 交易所的报单编号，推送接口中没有此字段
			order->orderDeskID = _2Str(lpUnPacker->GetStr("entrust_no"));
			order->orderReqID = _2Str(lpUnPacker->GetStr("extsystem_id"));
			order->gOrderID = order->gatewayName + "." + order->orderReqID;
			_getDirectOffetFromUFX(lpUnPacker->GetStr("entrust_direction"), order->direction, order->offset);
			order->price = lpUnPacker->GetDouble("entrust_price");
			order->totalVolume = lpUnPacker->GetInt("entrust_amount");	// 报单总数量
			order->tradeVolume = lpUnPacker->GetInt("deal_amount");	// 已成交数量
			order->tradePrice = lpUnPacker->GetDouble("deal_price");	// 成交均价
			order->status = orderStatusUFX2DF[lpUnPacker->GetStr("entrust_state")];
			order->external = true;
			if (order->status == "")
			{
				//LOG(WARNING) << "UFXGateway::_parseOrder, 收到订单状态未知的order,"
				//	<< lpUnPacker->GetStr("entrust_state") << std::endl;
			}
			//order->statusMsg;	// 状态信息
			order->orderTime = _2Str(lpUnPacker->GetStr("entrust_time"));
			_orderCache[orderReqID] = order; 
			_entrustReqMap[atoi(order->orderDeskID.c_str())] = atoi(order->orderReqID.c_str());
			if(order->status == DF_OS_NEW || order->status == DF_OS_PART_TRADE)
				this->onOrder(order);
		}
		else
		{
			// 只更新部分字段
			auto order = _orderCache[orderReqID];
			_entrustReqMap[atoi(order->orderDeskID.c_str())] = atoi(order->orderReqID.c_str());
			if (order == nullptr)
			{
				//LOG(ERROR) << "UFXGateway::_parseRtnOrder, _orderCache中不应该出现空指针！" << orderReqID << std::endl;
				lpUnPacker->Next();
				continue;
			}
			order->orderSysID = _2Str(lpUnPacker->GetStr("confirm_no"));
			order->tradePrice = lpUnPacker->GetDouble("deal_price");	// 成交均价

			string status = orderStatusUFX2DF[lpUnPacker->GetStr("entrust_state")];
			if (status == "")
			{
				//LOG(WARNING) << "UFXGateway::_parseOrder, 收到订单状态未知的order,"
				//	<< lpUnPacker->GetStr("entrust_state") << std::endl;
			}
			int tradeVolume = lpUnPacker->GetInt("deal_amount");	// 已成交数量
			if (status != order->status || order->tradeVolume != tradeVolume)
			{
				order->status = status;
				order->tradeVolume = tradeVolume;
				this->onOrder(order);
			}
		}
		lpUnPacker->Next();
	}

	return;
}

void UFXGateway::_parseTrade(IF2UnPacker * lpUnPacker)
{
	LOG(INFO) << "UFXGateway::_parseTrade " << std::endl;
	int dataSetCount = 0;
	int errorCode = _parseHeader(lpUnPacker, dataSetCount);

	if (errorCode != 0 || dataSetCount < 2)
		return;

	lpUnPacker->SetCurrentDatasetByIndex(1);
	lpUnPacker->First();
	for (int row = 0; row < (int)lpUnPacker->GetRowCount(); ++row)
	{
		string tradeID = lpUnPacker->GetStr("deal_no");
		int orderReqID = lpUnPacker->GetInt("extsystem_id");
		if (_tradeCache.find(tradeID+"."+ numberToString(orderReqID)) != _tradeCache.end()) // 已处理过
		{
			lpUnPacker->Next();
			continue;
		}

		std::shared_ptr<TradeInfo> trade(new TradeInfo());
		trade->symbol = lpUnPacker->GetStr("stock_code");
		trade->exchange = exchangeUFX2DF[lpUnPacker->GetStr("market_no")];
		trade->atsSymbol = trade->symbol + "." + trade->exchange;

		trade->tradeID = tradeID;
		trade->gTradeID = trade->gatewayName + "." + trade->tradeID;
		_getDirectOffetFromUFX(lpUnPacker->GetStr("entrust_direction"), trade->direction, trade->offset);
		trade->price = lpUnPacker->GetDouble("deal_price");
		trade->volume = lpUnPacker->GetInt("deal_amount");
		trade->tradeTime = lpUnPacker->GetStr("deal_time");
		trade->gatewayName = gatewayName;
		
		bool externalTrade = false; // O32的订单，或者上次交易的订单
		if (_orderCache.find(orderReqID) == _orderCache.end()) // 未找到发出去的订单记录，则为外部订单
		{
			externalTrade = true;
		}
		else
		{
			auto order = _orderCache[orderReqID];
			externalTrade = order->external;
		}

		if(externalTrade)
		{
			trade->external = true;
			//wei Event tradeEvent(EVENT_HIS_TRADE);
			//wei tradeEvent.data_ = std::static_pointer_cast<void>(trade);
			//wei eventEngine->putEvent(tradeEvent);
		}
		else {
			trade->external = false;
			//wei Event tradeEvent(EVENT_TRADE);
			//wei tradeEvent.data_ = std::static_pointer_cast<void>(trade);
			//wei eventEngine->putEvent(tradeEvent);
		}
		_tradeCache[tradeID+"."+ numberToString(orderReqID)] = trade;
		lpUnPacker->Next();
	}
}

// 解析股票仓位数据 UFX_FUNC_STOCK_POS_QRY  31001
void UFXGateway::_parseStockPosition(IF2UnPacker * lpUnPacker)
{
	LOG(INFO) << "UFXGateway::_parseStockPosition " << std::endl;
	int dataSetCount = 0;
	int errorCode = _parseHeader(lpUnPacker, dataSetCount);

	//LOG(INFO) << "[errorCode]: " << errorCode << " [dataSetCount]: " << dataSetCount << std::endl;
	if (errorCode != 0 || dataSetCount < 2)
		return;

	lpUnPacker->SetCurrentDatasetByIndex(1);
	lpUnPacker->First();  // 如果lpUnpacker被其他函数使用过了，可能指针就不在第一个位置了
	int rowCount = lpUnPacker->GetRowCount();
	//if (tdCallback)tdCallback->msg(to_string(rowCount));
	for (int row = 0; row < (int)lpUnPacker->GetRowCount(); ++row)
	{
		std::shared_ptr<PositionInfo> pos(new PositionInfo());
		pos->accountId = _2Str(lpUnPacker->GetStr("account_code"));
		pos->gatewayName = this->gatewayName;
		pos->exchange = exchangeUFX2DF[lpUnPacker->GetStr("market_no")];
		pos->symbol = _2Str(lpUnPacker->GetStr("stock_code"));

		pos->name = _2Str(lpUnPacker->GetStr("stock_name"));
		pos->position = lpUnPacker->GetInt("current_amount");
		
		int beginPos = lpUnPacker->GetInt("begin_amount"); // 期初数量
		int todaySellPos = lpUnPacker->GetInt("today_sell_amount"); // 今日卖出
		pos->ydPosition = std::max(0, beginPos - todaySellPos); // 昨仓
		pos->tdPosition = pos->position - pos->ydPosition; // 今仓
		pos->openFrozen = lpUnPacker->GetInt("pre_buy_amount"); // 挂买单数量
		pos->frozen = lpUnPacker->GetInt("pre_sell_amount"); // 挂卖单数量
		pos->ydFrozen = std::min(pos->ydPosition, pos->frozen); // 平昨冻结
		pos->tdFrozen = pos->frozen - pos->ydFrozen; // 今冻结
		//pos->available = lpUnPacker->GetInt("enable_amount");
		pos->available = pos->position - pos->frozen;

		double current_cost = lpUnPacker->GetDouble("current_cost"); // 总成本，需要除以份额
		if (pos->position != 0)
			pos->price = current_cost / pos->position;
		pos->symbol = _2Str(lpUnPacker->GetStr("stock_code"));

		pos->direction = DF_POSITION_LONG;
		pos->atsSymbol = pos->symbol + "." + pos->exchange;
		pos->atsPositionName = pos->atsSymbol + "." + pos->direction;
		pos->time = getTimeString("%Y/%m/%d %H:%M:%S");
		if (tdCallback) {
			//LOG(INFO) << "[IN CALLBACK]" << std::endl;

			tdCallback->_onQueryPosition(pos, row == (rowCount-1));
			//tdCallback->msg(to_string(row));

		}
		//else {
		//	LOG(INFO) << "[NOT !!!]" << std::endl;
		//}
		
		//this->onPosition(pos);
		lpUnPacker->Next();
	}

	return;
}

// 解析期货仓位数据 UFX_FUNC_FUTURE_POS_QRY  31003
void UFXGateway::_parseFuturePosition(IF2UnPacker * lpUnPacker)
{
	LOG(INFO) << "UFXGateway::_parseFuturePosition " << std::endl;
	int dataSetCount = 0;
	int errorCode = _parseHeader(lpUnPacker, dataSetCount);

	if (errorCode != 0 || dataSetCount < 2)
		return;

	lpUnPacker->SetCurrentDatasetByIndex(1);
	lpUnPacker->First();
	int rowCount = lpUnPacker->GetRowCount();
	//if (tdCallback)tdCallback->msg(to_string(rowCount));
	for (int row = 0; row < (int)lpUnPacker->GetRowCount(); ++row)
	{
		std::shared_ptr<PositionInfo> pos(new PositionInfo());
		pos->accountId = _2Str(lpUnPacker->GetStr("account_code"));
		pos->gatewayName = this->gatewayName;
		pos->exchange = exchangeUFX2DF[lpUnPacker->GetStr("market_no")];
		pos->symbol = _2Str(lpUnPacker->GetStr("stock_code"));
		pos->atsSymbol = pos->symbol + "." + pos->exchange;
		pos->name = _2Str(lpUnPacker->GetStr("stock_name"));
		pos->position = lpUnPacker->GetInt("current_amount");
		pos->available = lpUnPacker->GetInt("enable_amount");
		pos->tdPosition = lpUnPacker->GetInt("today_amount");
		pos->tdFrozen = pos->tdPosition - lpUnPacker->GetInt("today_enable_amount");
		pos->ydPosition = lpUnPacker->GetInt("lastday_amount");
		pos->ydFrozen = pos->ydPosition - lpUnPacker->GetInt("lastday_enable_amount");
		pos->hedgeFlag = lpUnPacker->GetStr("invest_type");
		double price = lpUnPacker->GetDouble("current_cost"); // 总成本，需要除以份额
		if(pos->position != 0)
			pos->price = price / pos->position;

		string ufxDirect = _2Str(lpUnPacker->GetStr("position_flag"));
		if (ufxDirect == "1") // 多头持仓
			pos->direction = DF_POSITION_LONG;
		else if (ufxDirect == "2")
			pos->direction = DF_POSITION_SHORT;

		pos->atsSymbol = pos->symbol + "." + pos->exchange;
		pos->atsPositionName = pos->atsSymbol + "." + pos->direction;
		pos->time = getTimeString("%Y/%m/%d %H:%M:%S");
		//LOG(INFO) << "UFXGateway::_parseFuturePosition, " << pos->atsSymbol
			//<< pos->direction  << pos->position << pos->hedgeFlag << std::endl;
		if (tdCallback) {
			tdCallback->_onQueryPosition(pos, row == (rowCount-1));
			//tdCallback->msg(to_string(row));
		}
		//this->onPosition(pos);
		lpUnPacker->Next();
	}
	return;
}

// 解析账户资金，UFX_FUNC_FUNDASSET_QRY 35003
// 由于期货、现货采用不同的资产单元，所以此函数废弃 -by lidp 20180116
void UFXGateway::_parseAccount(IF2UnPacker * lpUnPacker)
{
	int dataCount = 0;
	int errorCode = _parseHeader(lpUnPacker, dataCount);

	if (errorCode != 0 || dataCount < 2)
		return;

	lpUnPacker->SetCurrentDatasetByIndex(1);
	lpUnPacker->First();
	for (int row = 0; row < (int)lpUnPacker->GetRowCount(); ++row)
	{
		std::shared_ptr<AccountInfo> acc(new AccountInfo());
		acc->currency = _2Str(lpUnPacker->GetStr("currency_code"));
		if (acc->currency != "CNY")  // TODO: 暂时只处理人民币
		{
			//LOG(WARNING) << "UFXGateway::_parseAccount, 账户中存在非CNY资产, "
			//	<< acc->currency << std::endl;
			lpUnPacker->Next();
			continue;
		}
		acc->accountId = _2Str(lpUnPacker->GetStr("account_code"));
		acc->gatewayName = this->gatewayName;
		acc->gAccountId = acc->gatewayName + "." + acc->accountId;
		acc->totalAsset = lpUnPacker->GetDouble("total_asset");
		acc->netAsset = acc->totalAsset;
		acc->balance = lpUnPacker->GetDouble("current_balance");
		acc->available = acc->balance; // TODO: 冻结资金?
		acc->preBalance = lpUnPacker->GetDouble("begin_balance");
		acc->margin = lpUnPacker->GetDouble("occupy_deposit_balance");
		acc->time = getTimeString("%Y/%m/%d %H:%M:%S");
		//if (tdCallback) {
		//	tdCallback->_onQueryAsset(acc);
		//}
		//this->onAccount(acc);
		lpUnPacker->Next();
	}
	return;
}

void UFXGateway::_parseRspCancel(IF2UnPacker * lpUnPacker)
{
	/*int dataCount = 0;
	int errorCode = _parseHeader(lpUnPacker, dataCount);

	if (errorCode != 0 || dataCount == 0)
		return;

	lpUnPacker->SetCurrentDatasetByIndex(1);
	lpUnPacker->First();
	for (int row = 0; row < (int)lpUnPacker->GetRowCount(); ++row)
	{
		int entrustID = lpUnPacker->GetInt("entrust_no");
		if (_entrustReqMap.find(entrustID) == _entrustReqMap.end())
		{
			LOG(WARNING) << "UFXGateway::_parseRspCancel, 未找到撤单相应对应的订单, entrust_no,"
				<< entrustID << std::endl;
			return;
		}
		char success_flag = lpUnPacker->GetChar("success_flag");
		if (success_flag == '1')
		{
			_orderCache[_entrustReqMap[entrustID]]->status = DF_OS_CANCEL;
			this->onOrder(_orderCache[_entrustReqMap[entrustID]]);
		}
		else
		{
			QString fail_cause = _2Str(lpUnPacker->GetStr("success_flag"));
			LOG(WARNING) << QString("UFXGateway::_parseRspCancel, 撤单错误, entrust_no, %1, fail_cause, %2").
				arg(entrustID).arg(fail_cause).toStdString() << std::endl;
			return;
		}
		lpUnPacker->Next();
	}*/
	return;
}

// 解析资产单元查询 35011
void UFXGateway::_parseRspUnitAsset(IF2UnPacker * lpUnPacker)
{
	//LOG(INFO) << "(_parseRspUnitAsset)" << std::endl;
	int dataCount = 0;
	int errorCode = _parseHeader(lpUnPacker, dataCount);
	//LOG(INFO) << "[ErrorCode]: " << errorCode << std::endl;
	if (errorCode != 0 || dataCount < 2)
		return;

	lpUnPacker->SetCurrentDatasetByIndex(1);
	lpUnPacker->First();
	for (int row = 0; row < (int)lpUnPacker->GetRowCount(); ++row)
	{
		std::shared_ptr<AccountInfo> acc(new AccountInfo());
		acc->currency = _2Str(lpUnPacker->GetStr("currency_code"));
		if (acc->currency != "CNY")  // TODO: 暂时只处理人民币
		{
			//LOG(WARNING) << "UFXGateway::_parseAccount, 账户中存在非CNY资产, "
			//	<< acc->currency << std::endl;
			lpUnPacker->Next();
			continue;
		}
		string account_code = _2Str(lpUnPacker->GetStr("account_code"));
		string asset_no = _2Str(lpUnPacker->GetStr("asset_no"));
		acc->accountId = account_code + "." + asset_no;
		acc->gatewayName = this->gatewayName;
		acc->gAccountId = acc->gatewayName + "." + acc->accountId;
		if (asset_no == _futureUnitD)
		{
			acc->accType = AccountType::FUTURE;
		}
		else
		{
			acc->accType = AccountType::UNION;
		}
		double futu_asset = lpUnPacker->GetDouble("futu_asset");
		double stock_asset = lpUnPacker->GetDouble("stock_asset");
		double bond_asset = lpUnPacker->GetDouble("bond_asset");
		double fund_asset = lpUnPacker->GetDouble("fund_asset");
		double repo_asset = lpUnPacker->GetDouble("repo_asset");
		double option_asset = lpUnPacker->GetDouble("option_asset");
		double other_asset = lpUnPacker->GetDouble("other_asset");

		acc->balance = lpUnPacker->GetDouble("current_balance");
		acc->totalAsset = futu_asset + stock_asset + bond_asset + fund_asset + repo_asset + option_asset + other_asset + acc->balance;
		acc->netAsset = acc->totalAsset;
		acc->available = acc->balance; // TODO: 冻结资金?
		acc->preBalance = lpUnPacker->GetDouble("begin_balance");
		acc->time = getTimeString("%Y/%m/%d %H:%M:%S");
		//LOG(INFO) << "[IN_FOR_lpUnPacker]" << std::endl;
		if (tdCallback) {
			//LOG(INFO) << "[IN_CALLBACK]" << std::endl;
			tdCallback->_onQueryAsset(acc);
		}
		//else {
		//	LOG(INFO) << "[NOT!!! CALLBACK]" << std::endl;
		//}
		//this->onAccount(acc);
		lpUnPacker->Next();
	}
	return;
}

// 解析ETF基础信息
void UFXGateway::_parseRspETFBase(IF2UnPacker * lpUnPacker)
{
	/*int dataCount = 0;
	int errorCode = _parseHeader(lpUnPacker, dataCount);

	if (errorCode != 0 || dataCount < 2)
		return;

	lpUnPacker->SetCurrentDatasetByIndex(1);
	lpUnPacker->First();
	for (int row = 0; row < (int)lpUnPacker->GetRowCount(); ++row)
	{
		auto etf_code = _2Str(lpUnPacker->GetStr("etf_code"));
		if (_etfInfoList.find(etf_code) == _etfInfoList.end())
		{
			LOG(WARNING) << "UFXGateway::_parseRspETFBase, 收到未预期的ETF基础信息！" << std::endl;
			continue;
		}
		auto etfInfo = &_etfInfoList[etf_code];
		if (etfInfo->baseInfoInited)
		{
			LOG(INFO) << "UFXGateway::_parseRspETFBase, 已经初始化ETF基础信息！"
				<< etf_code << std::endl;
		}

		etfInfo->TradingDay = lpUnPacker->GetInt("business_date");
		etfInfo->EstimateCashComponent = lpUnPacker->GetDouble("estimate_cash");
		etfInfo->CashComponent = lpUnPacker->GetDouble("yesterday_cash");
		etfInfo->CreationRedemption = lpUnPacker->GetDouble("creation_redeem_type");
		etfInfo->CreationRedemptionUnit = lpUnPacker->GetInt("report_unit");
		etfInfo->MaxCashRatio = lpUnPacker->GetDouble("max_cash_ratio");
		etfInfo->Recordnum = lpUnPacker->GetInt("stock_num");
		etfInfo->NAV = lpUnPacker->GetDouble("yesterday_nav");

		etfInfo->baseInfoInited = true;
		lpUnPacker->Next();
	}*/
	return;
}

void UFXGateway::_parseRspETFList(IF2UnPacker * lpUnPacker)
{
	int dataCount = 0;
	int errorCode = _parseHeader(lpUnPacker, dataCount);

	if (errorCode != 0 || dataCount < 2)
		return;

	lpUnPacker->SetCurrentDatasetByIndex(1);
	lpUnPacker->First();
	for (int row = 0; row < (int)lpUnPacker->GetRowCount(); ++row)
	{
		/*auto etf_code = QString::fromStdString(_2Str(lpUnPacker->GetStr("etf_code")));
		if (_etfInfoList.find(etf_code) == _etfInfoList.end())
		{
			LOG(WARNING) << "UFXGateway::_parseRspETFBase, 收到未预期的ETF基础信息！" << std::endl;
			continue;
		}
		auto etfInfo = &_etfInfoList[etf_code];
		if (etfInfo->baseInfoInited)
		{
			LOG(INFO) << "UFXGateway::_parseRspETFBase, 已经初始化ETF基础信息！"
				<< etf_code.toStdString() << std::endl;
		}

		etfInfo->TradingDay = lpUnPacker->GetDouble("business_date");
		etfInfo->EstimateCashComponent = lpUnPacker->GetDouble("estimate_cash");
		etfInfo->CashComponent = lpUnPacker->GetDouble("yesterday_cash");
		etfInfo->CreationRedemption = lpUnPacker->GetDouble("creation_redeem_type");
		etfInfo->CreationRedemptionUnit = lpUnPacker->GetInt("report_unit");
		etfInfo->MaxCashRatio = lpUnPacker->GetDouble("max_cash_ratio");
		etfInfo->Recordnum = lpUnPacker->GetInt("stock_num");
		etfInfo->NAV = lpUnPacker->GetDouble("yesterday_nav");

		etfInfo->baseInfoInited = true;
		lpUnPacker->Next();*/
	}
	return;
}

//检查自成交，检查零股
bool UFXGateway::_riskCheck(std::shared_ptr<OrderInfo> newOrder)
{
	// 检查自成交
	for (auto itor = _orderCache.begin(); itor != _orderCache.end(); itor++)
	{
		if (itor->second->atsSymbol != newOrder->atsSymbol || itor->second->direction == newOrder->direction)
			continue;

		if (itor->second->status != DF_OS_NEW
			&& itor->second->status != DF_OS_PART_TRADE
			&& itor->second->status != DF_OS_OTHER)
			continue;

		// 方向相反，且未完全成交
		if (newOrder->direction == DF_DIRECTION_BUY)
		{
			if (FORMAT3(newOrder->price) >= FORMAT3(itor->second->price))
				return false;
		}
		else if (newOrder->direction == DF_DIRECTION_SELL)
		{
			if (newOrder->price <= itor->second->price)
				return false;
		}
		
	}
	if (newOrder->totalVolume % 100 != 0)
	{
		return false;
	}
	
	return true;
}

// 异步调用的回掉函数
void UFXGateway::OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage* lpMsg)
{
	//LOG(INFO) << "UFXGateway::OnReceivedBizMsg" << std::endl;
	if (lpMsg != NULL)
	{
		//成功,应用程序不能释放lpBizMessageRecv消息
		//LOG(INFO) << "ERRORNO(): " << lpMsg->GetErrorNo() << std::endl;
		
		if (lpMsg->GetErrorNo() == 0)
		{
			int iLen = 0;
			const void * lpBuffer = lpMsg->GetContent(iLen);
			IF2UnPacker * lpUnPacker = NewUnPacker((void *)lpBuffer, iLen);
			//ShowPacket(lpUnPacker);
			int func_id = lpMsg->GetFunction();
			//LOG(INFO) << std::endl;
			LOG(INFO) << "FUNC_ID: " << func_id << std::endl;
			switch (func_id)
			{
			case UFX_FUNCNO_ENTRUST_QRY:
				_parseRtnOrder(lpUnPacker);
				break;
			// 下单委托
			case UFX_ORD_ENTRUST:
			{
				int orderReqID = lpMsg->GetPacketId();
				_praseRspOrder(lpUnPacker, orderReqID);
				break;
			}
			// func_id = 31001将调用上层_onQueryPosition()
			case UFX_QRY_STOCKPOS:
				_parseStockPosition(lpUnPacker);
				break;
			// func_id = 31003 将调用上层_onQueryPosition()
			case UFX_QRY_FUTUREPOS:
				_parseFuturePosition(lpUnPacker);
				break;
			case UFX_QRY_STOCK_TRADES:
				_parseTrade(lpUnPacker);
				break;

			// func_id = 35003 将会调用上层_onQueryAsset()
			case UFX_QRY_FUNDASSET:
				_parseAccount(lpUnPacker);
				break;
			case UFX_CANCEL_ENTRUST:
				_parseRspCancel(lpUnPacker);
				break;
			// func_id = 35011 将会调用上层_onQueryAsset()
			case UFX_QRY_UNITASSET:
				_parseRspUnitAsset(lpUnPacker);
				break;
			case UFX_QRY_FUNDLIST:
				//LOG(INFO) << "UFX_QRY_FUNDLIST" << std::endl;
				break;
			case UFX_QRY_ASSETLIST:
				//LOG(INFO) << "UFX_QRY_ASSETLIST" << std::endl;
				break;
			case UFX_QRY_ETF_BASE_MSG:
				_parseRspETFBase(lpUnPacker);
				break;
			case UFX_QRY_ETF_LIST:
				_parseRspETFList(lpUnPacker);
				break;
			default:
				//LOG(INFO) << "UNKONWN, " << func_id << std::endl;
				break;
			}
		}
		else
		{
			//LOG(ERROR) << lpMsg->GetErrorInfo() << std::endl;
		}
	}
}


void FUNCTION_CALL_MODE UFXGateway::OnReceived(CSubscribeInterface * lpSub, int subscribeIndex, const void * lpData, int nLength, LPSUBSCRIBE_RECVDATA lpRecvData)
{
	//LOG(INFO) << "UFXGateway::OnReceived" 
	//	<< ",lpSub," << int(lpSub)
	//	<< ",subscribeIndex," << subscribeIndex
	//	<< ",lpData," << int(lpData)
	//	<< ",nLength," << nLength
	//	<< ",lpRecvData," << int(lpRecvData)
	//	<<", lpRecvData->iFilterDataLen," << lpRecvData->iFilterDataLen
	//	<< std::endl;
	if (lpData == NULL || nLength == 0)
	{
		//LOG(WARNING) << "UFXGateway::OnReceived，推送数据指针为空, 或者长度为0" << endl;
		return;
	}

	IF2UnPacker* lpUnPackBody = NewUnPacker((void*)lpData, nLength);
	lpUnPackBody->AddRef();

	ShowPacket(lpUnPackBody);
	if (_forceQuery) // 如果定时查询，则不再处理推送消息
	{
		//LOG(INFO) << "已经定时查询，return." << std::endl;
		return;
	}
		
	int orderReqID = atoi(_2Str(lpUnPackBody->GetStr("extsystem_id")));
	if (_orderCache.find(orderReqID) == _orderCache.end())
	{
		//LOG(ERROR) << "UFXGateway::OnReceived, 收到订阅，但未找到对应的订单信息！" << orderReqID << std::endl;
		return;
	}
	auto order = _orderCache[orderReqID];

	string msgtype = lpUnPackBody->GetStr("msgtype");
	
	if(msgtype == "a")
	{
		string status = orderStatusUFX2DF[lpUnPackBody->GetStr("entrust_state")];
		if (status == "")
		{
			//LOG(WARNING) << "UFXGateway::OnReceived, 收到订单状态未知的order,"
			//	<< lpUnPackBody->GetStr("entrust_state") << std::endl;
		}
		if (status != order->status)
		{
			order->status = status;
			this->onOrder(order);
			// 调用上层
			if (tdCallback)tdCallback->_onOrderEvent(order);
		}
	}
	else if (msgtype == "b") // 委托确认
	{
		order->orderSysID = _2Str(lpUnPackBody->GetStr("confirm_no"));
		order->status = orderStatusUFX2DF[lpUnPackBody->GetStr("entrust_state")];
		if (order->status == "")
		{
			//LOG(WARNING) << "UFXGateway::OnReceived, 收到订单状态未知的order,"
			//	<< lpUnPackBody->GetStr("entrust_state") << std::endl;
		}
		this->onOrder(order);
		// 调用上层
		if (tdCallback)tdCallback->_onOrderEvent(order);
	}
	else if (msgtype == "c") // 委托废单
	{
		order->statusMsg = lpUnPackBody->GetStr("revoke_cause");	// 废单原因
		string status = orderStatusUFX2DF[lpUnPackBody->GetStr("entrust_state")];
		if (status == "")
		{
			//LOG(WARNING) << "UFXGateway::OnReceived, 收到订单状态未知的order,"
			//	<< lpUnPackBody->GetStr("entrust_state") << std::endl;
		}
		if (status != order->status)
		{
			order->status = status;
			this->onOrder(order);
			// 调用上层
			if (tdCallback)tdCallback->_onOrderEvent(order);
		}
	}
	else if (msgtype == "d") // 委托撤单的响应
	{
		// 无需处理撤单响应
	}
	else if (msgtype == "e") // 委托撤单成功
	{
		order->statusMsg = "";
		const char* revo = lpUnPackBody->GetStr("revoke_cause");	// 撤单原因
		if (revo != NULL) order->statusMsg = revo;
		string status = "";
		const char* stat = lpUnPackBody->GetStr("entrust_state");
		if (stat != NULL) status = stat;
		//string status = orderStatusUFX2DF[lpUnPackBody->GetStr("entrust_state")];
		order->cancelTime = getTimeString("%Y/%m/%d %H:%M:%S");
		if (order->status == "")
		{
			//LOG(WARNING) << "UFXGateway::OnReceived, 收到订单状态未知的order,"
			//	<< lpUnPackBody->GetStr("entrust_state") << std::endl;
		}
		if (status != order->status)
		{
			order->status = status;
			this->onOrder(order);
			//调用上层
			if (tdCallback)tdCallback->_onOrderEvent(order);
		}
	}
	else if (msgtype == "f") // 撤单失败
	{
		//LOG(WARNING) << "UFXGateway::OnReceived, 撤单失败"
		//	<< ", cancelReqID, " << lpUnPackBody->GetInt("entrust_no")
		//	<< ", orderDeskID, " << lpUnPackBody->GetInt("cancel_entrust_no")
		//	<< ", status, " << lpUnPackBody->GetInt("entrust_state")
		//	<< ", orderReqID, " << order->orderReqID
		//	<< ", revokeCause, " << lpUnPackBody->GetStr("revoke_cause")
		//	<< std::endl;

	}
	else if (msgtype == "g") // 委托成交
	{
		string tradeID = lpUnPackBody->GetStr("deal_no");
		if (_tradeCache.find(tradeID+"."+order->orderReqID) != _tradeCache.end()) // 已处理过
		{
			return;
		}

		std::shared_ptr<TradeInfo> trade(new TradeInfo());
		trade->symbol = lpUnPackBody->GetStr("stock_code");
		trade->exchange = exchangeUFX2DF[lpUnPackBody->GetStr("market_no")];
		trade->atsSymbol = trade->symbol + "." + trade->exchange;
		trade->tradeID = tradeID;
		trade->gTradeID = trade->gatewayName + "." + trade->tradeID;
		trade->gOrderID = order->gOrderID;
		_getDirectOffetFromUFX(lpUnPackBody->GetStr("entrust_direction"), trade->direction, trade->offset);
		trade->price = lpUnPackBody->GetDouble("deal_price");
		trade->volume = lpUnPackBody->GetInt("deal_amount");

		//trade->tradeTime = lpUnPackBody->GetStr("deal_time");
		string date = lpUnPackBody->GetStr("deal_date");
		string time = lpUnPackBody->GetStr("deal_time");
		// tradeTime 20191008 131034
		trade->tradeTime = date + " " + time;

		trade->gatewayName = gatewayName;
		trade->external = order->external;
		this->onTrade(trade);
		// 调用上层_onTradeEvent
		if (tdCallback) tdCallback->_onTradeEvent(trade);
		
		_tradeCache[tradeID + "." + order->orderReqID] = trade;
		int tradeVolume = lpUnPackBody->GetInt("total_deal_amount");
		string status = orderStatusUFX2DF[lpUnPackBody->GetStr("entrust_state")];
		if (status != order->status || tradeVolume != order->tradeVolume)
		{
			order->status = status;
			order->tradeVolume = tradeVolume;
			if (order->tradeVolume != 0)
				order->tradePrice = lpUnPackBody->GetDouble("total_deal_balance") / order->tradeVolume;

			this->onOrder(order);
			if (tdCallback)tdCallback->_onOrderEvent(order);
		}
	}
	else
	{
		//LOG(WARNING) << "UFXGateway::OnReceived, 收到未处理类型的消息, msgtype," << msgtype<< std::endl;
	}

	lpUnPackBody->Release();
}

void FUNCTION_CALL_MODE UFXGateway::OnRecvTickMsg(CSubscribeInterface * lpSub, int subscribeIndex, const char * TickMsgInfo)
{
	//LOG(WARNING) << "***********OnRecvTickMsg****************" << std::endl;
}

//2017/12/12 yc
bool UFXGateway::_loadConfig(CConfigInterface *lpConfig)
{
	// 是否强制定时查询
	if (lpConfig->GetInt("Query", "ForceQuery", 0) != 0)
	{
		_forceQuery = true;
	}
	else
	{
		_forceQuery = false;
	}
	_forceQueryPeriod = lpConfig->GetInt("Query", "ForceQueryPeriod", 0);
	// 加载操作员
	_optId = lpConfig->GetString("OPERATOR", "OperatorId", "");
	_optPwd = lpConfig->GetString("OPERATOR", "OperatorPassword", "");
	_IPAddress = lpConfig->GetString("OPERATOR", "IPAddress", "");
	_MacAddress = lpConfig->GetString("OPERATOR", "MacAddress", "");
	_station = lpConfig->GetString("OPERATOR", "Station", "");
	_authorizationID = lpConfig->GetString("OPERATOR", "AuthorizationID", "");

	// 加载账户
	_acctID = lpConfig->GetString("Account", "AccountId", "");
	_stockUnitID = lpConfig->GetString("Account", "StockUnitID", "");
	_futureUnitD = lpConfig->GetString("Account", "FutureUnitID", "");

	LOG(INFO) << "UFXGateway, 加载用户信息完成, "
		<< "_optId, " << _optId
		<< ", _optPwd, " << _optPwd
		<< ", _MacAddress, " << _MacAddress
		<< ", _IPAddress, " << _IPAddress
		<< ", _acctID, " << _acctID
		<< ", _stockUnitID, " << _stockUnitID
		<< ", _futureUnitD, " << _futureUnitD
		<<", _forceQuery, " << _forceQuery
		<<", _forceQueryPeriod, " << _forceQueryPeriod
		<< std::endl;

	/*string etfs = lpConfig->GetString("Query", "ETFList", "");
	QStringList etf = etfs.split(',');
	for (auto itor = etf.begin(); itor != etf.end(); itor++)
	{
		_etfInfoList[*itor] = ETFComposition();
	}*/
	return true;
}

bool UFXGateway::_login(string opterID, string password, string &errorMsg)
{
	if (opterID != _optId)
	{
		//LOG(ERROR) << "UFXGateway::_login, 登陆用户与配置文件不一致！"
		//	<< "配置文件用户, " << _optId
		//	<< ", 传入用户," << opterID << std::endl;
		return false;
	}
	CUFXUserLogin userLogin ;
	userLogin.m_in_operator_no = opterID;
	userLogin.m_in_password = password;
	userLogin.m_in_mac_address = _MacAddress;
	userLogin.m_in_op_station = _station;
	userLogin.m_in_ip_address = _IPAddress;
	userLogin.m_in_authorization_id = _authorizationID;

	_userToken = "";
	int iRetCallUFX = unionCon->SyncCallUFX(&userLogin);
	//call_service(m_pConnect,pUserLogin);
	if (iRetCallUFX == RET_OK)
	{
		if (userLogin.m_out_ErrorCode != 0)
		{
			errorMsg = userLogin.m_out_ErrorMsg.c_str();
			iRetCallUFX = userLogin.m_out_ErrorCode;
			//LOG(ERROR) << "UFXGateway::_login, UFX登陆失败, 错误原因，" << userLogin.m_out_ErrorMsg << std::endl;
		}
		else
		{
			//LOG(ERROR) << string("UFX Login succeed") << "UFXGateway::_login" << std::endl;
			string strTopicName;
			strTopicName = subConfig->GetString("subcribe", "topic_name", "ufx_topic");
			//推送接口登录
			int subResult = unionCon->SubUFXLogin(&userLogin, strTopicName);
			if (subResult != 0)
			{
				//LOG(ERROR) << string("UFX subscribe failed, error msg:") << unionCon->GetLastError().c_str() << "UFXGateway::_login" << std::endl;
			}
			else
			{
				//LOG(ERROR) << string("UFX subscribe succed") << "UFXGateway::_login" << std::endl;
			}
			_userToken = userLogin.m_out_user_token;
		}
	}
	else
	{
		//LOG(ERROR) << "UFXGateway::_login, UFX connect failed, error msg: " << unionCon->GetLastError() << std::endl;
	}
	
	if (iRetCallUFX!= RET_OK)
		return false;
	else
		return true;	
}

bool UFXGateway::queryAccount()
{
	CUFXQryUnitAsset qryStock = CUFXQryUnitAsset();
	qryStock.m_in_user_token = _userToken;
	qryStock.m_in_account_code = _acctID;
	qryStock.m_in_asset_no = _stockUnitID;
	
	int iRetCallUFX = unionCon->ASyncCallUFX(&qryStock);
	
	if (iRetCallUFX <= 0)
	{
		_strLastErrMsg = unionCon->GetLastError();
		//LOG(ERROR) << "UFXGateway::queryAccount, UFX查询股票资产单元资金失败, 错误原因，" << _strLastErrMsg << std::endl;
		return false;
	}

	CUFXQryUnitAsset qryFuture = CUFXQryUnitAsset();
	qryFuture.m_in_user_token = _userToken;
	qryFuture.m_in_account_code = _acctID;
	qryFuture.m_in_asset_no = _futureUnitD;

	iRetCallUFX = unionCon->ASyncCallUFX(&qryFuture);

	if (iRetCallUFX <= 0)
	{
		_strLastErrMsg = unionCon->GetLastError();
		LOG(ERROR) << "UFXGateway::queryAccount, UFX查询期货资产单元资金失败, 错误原因，" << _strLastErrMsg << std::endl;
		return false;
	}

	return true;
}

bool UFXGateway::queryStockPosition()
{
	CUFXQryStockPos qryStockPos;	
	qryStockPos.m_in_user_token = _userToken;
	qryStockPos.m_in_account_code = _acctID;
	qryStockPos.m_in_asset_no = _stockUnitID;
	int iRetCallUFX = unionCon->ASyncCallUFX(&qryStockPos);
	return iRetCallUFX > 0;
}

bool UFXGateway::queryOrders()
{
	return queryStockOrders() && queryFutureOrders();
}

bool UFXGateway::sendOrder(std::shared_ptr<OrderInfo> order)
{
	// 给订单编号
	int orderReqID = _generateReqID();
	order->gatewayName = gatewayName;
	order->orderReqID = numberToString(orderReqID);
	order->gOrderID = order->gatewayName + "." + order->orderReqID;
	order->status = DF_OS_NEW;

	/*if (_riskCheck(order) == false)
	{
		LOG(ERROR) << string("触发UFXGateway风控") << "UFXGateway::sendOrder" << std::endl;
		order->status = DF_OS_CANCEL;
		order->statusMsg = string("触发UFXGateway风控");
		onOrder(order);
		return false;
	}*/

	CUFXOrderInsert orderInsert;
	orderInsert.m_in_user_token = _userToken;
	orderInsert.m_in_account_code = _acctID;
	if (order->exchange == DF_EXCH_CFFEX || order->exchange == DF_EXCH_SHFE || order->exchange == DF_EXCH_DCE || order->exchange == DF_EXCH_CZCE)
	{
		orderInsert.m_in_asset_no = _futureUnitD;
	}
	else
	{
		orderInsert.m_in_asset_no = _stockUnitID;
	}
	orderInsert.m_in_market_no = exchangeDF2UFX[order->exchange];
	orderInsert.m_in_stock_code=order->symbol;

	orderInsert.m_in_direction = _getDirectionFromDF(order->symbol, order->exchange, order->direction, order->offset);
	orderInsert.m_in_price_type="0";  // price_type, 暂时只支持限价单
	orderInsert.m_in_price=order->price;
	orderInsert.m_in_amount=order->totalVolume;
	orderInsert.m_in_extsystem_id = orderReqID;
	orderInsert.m_in_mac_address= _MacAddress;	
	orderInsert.m_in_ip_address = _IPAddress;

	int iRetCallUFX = unionCon->ASyncCallUFX(&orderInsert, orderReqID);
	_orderCache[orderReqID] = order;
	//LOG(INFO) << "UFXGateway::sendOrder, gOrderID," << order->gOrderID <<", orderDeskID, "<< order->orderDeskID 
	//	<<", atsSymbol, "<< order->atsSymbol <<", direction, "<< order->direction <<", price, " << order->price <<", totalVolume, "<< order->totalVolume 
	//	<<", iRetCallUFX, " << iRetCallUFX
	//	<< std::endl;
	if (iRetCallUFX > 0)
	{
		return true;
	}
	else
	{
		//LOG(ERROR) << "UFXGateway::sendOrder 下单失败,"
		//	<< order->direction  <<", "<< order->atsSymbol << ", " << order->totalVolume << ", " << order->price
		//	<< std::endl;
		return false;
	}
}

bool UFXGateway::cancelOrder(std::shared_ptr<OrderInfo> order)
{
	if (order->status == DF_OS_ALLTRD || order->status == DF_OS_CANCEL || order->status == DF_OS_ERROR)  // 无需撤单
		return true;
	CUFXOrderCancel orderCancel;
	orderCancel.m_in_user_token = _userToken;
	orderCancel.m_in_account_code = _acctID;
	orderCancel.m_in_entrust_no = atoi(order->orderDeskID.c_str());

	orderCancel.m_in_mac_address = _MacAddress;
	orderCancel.m_in_ip_address = _IPAddress;

	int iRetCallUFX = unionCon->ASyncCallUFX(&orderCancel);

	//LOG(INFO) << "UFXGateway::cancelOrder: gOrderID, " << order->gOrderID <<", orderDeskID, "<< order->orderDeskID <<", atsSymbol, "<< order->atsSymbol
	//	<<", direction, "<< order->direction <<", price, " << order->price <<", iRetCallUFX, " << iRetCallUFX
	//	<< std::endl;

	return iRetCallUFX > 0;
}

bool UFXGateway::cancelAllOrders()
{
	//LOG(INFO) << "UFXGateway::cancelAllOrders" << std::endl;
	for (auto itor = _orderCache.begin(); itor != _orderCache.end(); itor++)
	{
		cancelOrder(itor->second);
	}
	return true;
}


/*bool UFXGateway::getETFComposition(string symbol, string exchange, ETFComposition & out_etf)
{
	auto itor = _etfInfoList.find(symbol);
	if (itor != _etfInfoList.end())
	{
		if (itor->second.baseInfoInited && itor->second.constituentInited)
		{
			out_etf.baseInfoInited = true;
			out_etf.CashComponent = itor->second.CashComponent;
			out_etf.constituentInited = true;
			out_etf.constituentStocks.clear();
			for (auto i = itor->second.constituentStocks.begin(); i != itor->second.constituentStocks.end(); i++)
			{
				out_etf.constituentStocks[i->first] = i->second;
			}
			out_etf.CreationRedemption = itor->second.CreationRedemption;
			out_etf.CreationRedemptionUnit = itor->second.CreationRedemptionUnit;
			out_etf.EstimateCashComponent = itor->second.EstimateCashComponent;
			out_etf.Fundid1 = itor->second.Fundid1;
			out_etf.MaxCashRatio = itor->second.MaxCashRatio;
			out_etf.Recordnum = itor->second.Recordnum;
			out_etf.TradingDay = itor->second.TradingDay;
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}*/

string UFXGateway::sendOrder(string symbol, string exchange, string direction, string offset, double price, int volume)
{

	string atsSymbol = symbol + "." + exchange;
	/*if (instGateway.find(atsSymbol) == instGateway.end())
	{
	LOG(ERROR) << "BaseStrategy::sendOrder, 未找到策略对合约配置的交易网关, strategy, "
	<< name.toStdString() << ", atsSymbol, " << atsSymbol.toStdString() << std::endl;
	}
	QString gatewayName = instGateway[atsSymbol].tdGateway;*/
	string gatewayName = "UFXGateway";
	std::shared_ptr<OrderInfo> newOrder(new OrderInfo());
	newOrder->symbol = symbol;
	newOrder->exchange = exchange;
	newOrder->atsSymbol = atsSymbol;
	newOrder->direction = direction;
	newOrder->offset = offset;
	newOrder->price = price;
	newOrder->totalVolume = volume;
	newOrder->gatewayName = gatewayName;

	bool sendStatus = sendOrder(newOrder);
	string gOrderID = newOrder->gOrderID;
	//LOG(INFO) << "UFXGateway::sendOrder: " << "," <<gOrderID << "," << symbol << "," << exchange << "," << offset << "," << direction << "," << price << "," << volume
	//	<< std::endl;
	//cout << "SEND ORDER, order status " << newOrder->status << " ,session id " << newOrder->sessionID << endl;
	if (sendStatus)
		return newOrder->gOrderID;
	else
		return "";
}

bool UFXGateway::cancelOrder(string symbol, string direction, double price)
{
	//LOG(INFO) << "UFXGateway::cancelOrder, symbol:" << symbol <<", direction:" << direction <<", price: " << price << std::endl;

	for (auto itor = _orderCache.begin(); itor != _orderCache.end(); itor++)
	{
		if (itor->second->symbol == symbol && itor->second->direction == direction /*&& itor->second->price == price*/) {
			cancelOrder(itor->second);
		}
	}
	return true;
}

bool UFXGateway::cancelOrder(string order_id)
{
	for (auto itor = _orderCache.begin(); itor != _orderCache.end(); itor++)
	{
		if (itor->second->gOrderID == order_id) {
			return cancelOrder(itor->second);
		}	
	}
	return false;
}
