// TDFGateway.cpp : 
//
#include "stdafx.h"
#include "TDFBase.h"
#pragma comment(lib,"TDFAPI30.lib")

#include <iostream>
#include <time.h>
#include <sys/timeb.h>

#include <sstream>
#include <direct.h>
#include <fmt/format.h>
//#include <kungfu/wingchun/gateway/macro.h>
//#include "../glog/GLogWrapper.h"
//using namespace std;



TDFBase::TDFBase()
{
	_loadConfig("TDF.ini");

	//while (connect() == false) {
	//	Sleep(10000);
	//}
	
}

TDFBase::~TDFBase()
{
	disconnect();
}

MdCallback* TDFBase::mdCallback = NULL;


void TDFBase::RecvData(THANDLE hTdf, TDF_MSG * pMsgHead)
{
	//if (mdCallback) mdCallback->msg("in RecvData");

	if (pMsgHead == NULL || pMsgHead->pData == NULL)
	{
		return;
	}
	unsigned int nItemCount = pMsgHead->pAppHead->nItemCount;
	if (!nItemCount)
	{
		return;
	}

	switch (pMsgHead->nDataType)
	{
		//if (mdCallback) mdCallback->msg("in dataType case");
		
		// 行情
		case MSG_DATA_MARKET:
		{
			//if (mdCallback) mdCallback->msg(to_string(nItemCount));
			for (int i = 0; i < nItemCount; i++)
			{
				const TDF_MARKET_DATA& tdfMktData = ((TDF_MARKET_DATA*)pMsgHead->pData)[i];
				
				if (mdCallback) {
					//mdCallback->onTick(tdfMktData.nTradingDay, tdfMktData.nTime, tdfMktData.szWindCode, tdfMktData.nOpen / 10000.0,
					//	tdfMktData.nHigh / 10000.0, tdfMktData.nLow / 10000.0, tdfMktData.nMatch / 10000.0);
					//mdCallback->msg(to_string(tdfMktData.iVolume));
					mdCallback->OnDepthMarketData(&tdfMktData);
				}
				//std::cout << "ID: " << tdfMktData.szWindCode << " TotalVolume: " << tdfMktData.iVolume << std::endl;

				//QUOTE_TRACE(fmt::format(" TDF Quote(Symbol){}, (Volume){},", tdfMktData.szWindCode, tdfMktData.iVolume));
				//mdCallback->msg(tdfMktData.szWindCode);
			}
		}
		break;
		case MSG_DATA_FUTURE:
		{
		}
		break;

		case MSG_DATA_INDEX:
		{
		}
		break;

		case MSG_DATA_TRANSACTION:
		{
			for (int i = 0; i < nItemCount; i++)
			{
				const TDF_TRANSACTION& tdfTransction = ((TDF_TRANSACTION*)pMsgHead->pData)[i];
				//if (mdCallback) {
					//mdCallback->OnTransByTick(&tdfTransction);
				//}
			}
		}
		break;

		case MSG_DATA_ORDERQUEUE:
		{
		}
		break;
		// 逐笔委托
		case MSG_DATA_ORDER:
		{
			for (int i = 0; i < nItemCount; i++)
			{
				const TDF_ORDER& tdfOrder = ((TDF_ORDER*)pMsgHead->pData)[i];
				//if (mdCallback) {
					//mdCallback->OnOrderByTick(&tdfOrder);
				//}
			}

		}
		break;

		default:
		{

		}
		break;
	}
}

void TDFBase::RecvSys(THANDLE hTdf, TDF_MSG * pSysMsg)
{
	return;
	if (hTdf == NULL) return;
	if (pSysMsg == NULL) return;
	size_t const count = pSysMsg->pAppHead->nItemCount;
	size_t const size = pSysMsg->pAppHead->nItemSize;
	switch (pSysMsg->nDataType) {
	case MSG_SYS_DISCONNECT_NETWORK:
		//cout << "TDFGateway::RecvSys, MSG_SYS_DISCONNECT_NETWORK" << std::endl;
		break;
	case MSG_SYS_CONNECT_RESULT:
		//cout << "TDFGateway::RecvSys, MSG_SYS_CONNECT_RESULT" << std::endl;
		break;
	case MSG_SYS_LOGIN_RESULT:
		//cout << "TDFGateway::RecvSys, MSG_SYS_LOGIN_RESULT" << std::endl;
		break;
	case MSG_SYS_CODETABLE_RESULT:
		//cout << "TDFGateway::RecvSys, MSG_SYS_CODETABLE_RESULT" << std::endl;
		break;
	case MSG_SYS_QUOTATIONDATE_CHANGE:
		//cout << "TDFGateway::RecvSys, MSG_SYS_QUOTATIONDATE_CHANGE" << std::endl;
		break;
	case MSG_SYS_MARKET_CLOSE:
		//cout << "TDFGateway::RecvSys, MSG_SYS_MARKET_CLOSE" << std::endl;
		break;
	case MSG_SYS_HEART_BEAT:
		//cout << "TDFGateway::RecvSys, MSG_SYS_HEART_BEAT" << std::endl;
		break;
	default:
		//cout << "TDFGateway::RecvSys, UNKONWN MSG TYPE" << pSysMsg->nDataType << std::endl;
		break;
	}
}

bool TDFBase::_loadConfig(string iniFile)
{
	//std::cout << "in load config" << std::endl;
	TIniFile tf;
	//char buf[80];
	//getcwd(buf, sizeof(buf));
	//printf("current working directory : %s\n", buf);
	//  D:\kungfu\core\extensions\xtp\src
	if (!tf.Open(iniFile.c_str()))
	{
		//cout << "打开TDF配置文件失败, " << iniFile << std::endl;
		return false;
	}
	char tmp[256] = { 0 };

	// 加载操作员
	memset(tmp, 0, sizeof(tmp));
	tf.ReadTString("SERVER", "ServerIP", "", tmp, sizeof(tmp) - 1);
	_serverIP = tmp;

	memset(tmp, 0, sizeof(tmp));
	tf.ReadTString("SERVER", "ServerPort", "", tmp, sizeof(tmp) - 1);
	_serverPort = tmp;

	memset(tmp, 0, sizeof(tmp));
	tf.ReadTString("SERVER", "LoginUser", "", tmp, sizeof(tmp) - 1);
	_loginUser = tmp;

	memset(tmp, 0, sizeof(tmp));
	tf.ReadTString("SERVER", "LoginPassword", "", tmp, sizeof(tmp) - 1);
	_password = tmp;

	int subMarketCount = 1;
	string marketCode = "MarketCode_" + std::to_string(subMarketCount);
	memset(tmp, 0, sizeof(tmp));
	while (strcmp(tf.ReadTString("SUBSCRIBE", marketCode.c_str(), "", tmp, sizeof(tmp) - 1), "") != 0)
	{
		_marketCodes += (string(tmp) + ";");
		subMarketCount++;
		marketCode = "MarketCode_" + std::to_string(subMarketCount);
		memset(tmp, 0, sizeof(tmp));
	}

	memset(tmp, 0, sizeof(tmp));
	_subMode = tf.ReadInt("SUBSCRIBE", "SubscribeMode", -1);
	if (_subMode == -1)
	{
		//cout << "TDFGateway::_loadConfig, 读取订阅模式失败" << endl;
		return false;
	}

	memset(tmp, 0, sizeof(tmp));
	_outputMarket = tf.ReadInt("SUBSCRIBE", "OutputMarket", 0);

	_subscribeTypeFlag = tf.ReadInt("SUBSCRIBE", "SubscribeTypeFlag", 0);
	

	//cout << "TDFGateway::_loadConfig"
	//	<< ", _serverIP, " << _serverIP
	//	<< ", _serverPort, " << _serverPort
	//	<< ", _password, " << _password
	//	<< ", _subMode, " << _subMode
	//	<< ", _outputMarket, " << _outputMarket
	//	<< ", _marketCodes" << _marketCodes
	//	<< std::endl;

	return true;
}

bool TDFBase::connect( /*string &errorMsg*/)
{
	TDF_OPEN_SETTING_EXT settings = { 0 };
	memset(&settings, 0, sizeof(settings));
	strncpy(settings.siServer[0].szIp, _serverIP.c_str(), sizeof(settings.siServer[0].szIp) - 1);
	strncpy(settings.siServer[0].szPort, _serverPort.c_str(), sizeof(settings.siServer[0].szPort) - 1);
	strncpy(settings.siServer[0].szUser, _loginUser.c_str(), sizeof(settings.siServer[0].szUser) - 1);
	strncpy(settings.siServer[0].szPwd, _password.c_str(), sizeof(settings.siServer[0].szPwd) - 1);
	settings.nServerNum = 1;
	settings.pfnMsgHandler = RecvData;
	settings.pfnSysMsgNotify = RecvSys;
	settings.szMarkets = _marketCodes.c_str(); // "" or _marketCodes.c_str();
	//settings.szSubScriptions = "000001.sz";
	settings.nTypeFlags = _subscribeTypeFlag; // DATA_TYPE_ORDER | DATA_TYPE_TRANSACTION; //| DATA_TYPE_ORDERQUEUE;

	settings.nTime = 0;
	cout << "TDFGateway::connect, subscribe market, " << std::endl;
	THANDLE hTDF = NULL;
	TDF_ERR nErr = TDF_ERR_SUCCESS;

	//return true;

	hTDF = TDF_OpenExt(&settings, &nErr);
	
	if (hTDF == NULL) {
		cout << "TDF_Open return error, error_code, " << nErr << std::endl;
		//errorMsg = string::fromLocal8Bit("TDF登陆错误，错误代码:%1").arg(nErr);
		return false;
	}
	else
	{
		isConnected = true;
		tdfhandle = hTDF;
	}
	return true;
}

bool TDFBase::disconnect()
{
	if (isConnected && tdfhandle != NULL)
	{
		tdfhandle = NULL;
		isConnected = false;
		bool res = (TDF_Close(tdfhandle) == 0);
		//if (mdCallback) { mdCallback->onDisconnect(res); }
		return res;
	}
	//if (mdCallback) { mdCallback->onDisconnect(0); }
	return true;
}

bool TDFBase::subscribe(string symbol, string exchange)
{
	// 由于Wind是按照整个市场订阅的，所以这里仅做记录，作为筛选

	string windCode = symbol + "." + exchange;
	
	
	int ret = TDF_SetSubscription(tdfhandle, windCode.c_str(), SUBSCRIPTION_ADD);
	//cout << "subscribe result: " << ret << endl;
	//subList->push_back(symbol);

	return ret == 0;
}

bool TDFBase::subscribe(string symbol)
{
	int ret = TDF_SetSubscription(tdfhandle, symbol.c_str(), SUBSCRIPTION_ADD);
	//subList->push_back(symbol);
	return true;
}


void TDFBase::subscribeFile(string filename)
{
	char line[1024];
	FILE* fp = fopen(filename.c_str(), "r");
	while (fgets(line, 1024, fp) != NULL) {
		if (line[0] == '#') continue;
		string stock = line;
		stock = stock.substr(0, 6);
		subscribe(stock, "sz");
	}
	fclose(fp);
}


std::map<std::string, std::string> TDFBase::iniInfo()
{
	std::map<std::string, std::string> res;
	res["user"] = _loginUser;
	res["ip"] = _serverIP;
	res["port"] = _serverPort;
	return res;
}

