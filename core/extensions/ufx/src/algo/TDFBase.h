#pragma once
// TDFAPI ะ่าช
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>   
#include "../winAPI/TDFAPI.h"
#endif

#include "../basictool/InitFile.h"
#include <iostream>
#include<string>
#include<vector>
#include<map>
using namespace std;

class MdCallback {
public:
	virtual void onDisconnect(int reason) = 0;
	virtual void onTick(int day, int time, const char* code, float open, float high, float low, float close) = 0;
	virtual void OnDepthMarketData(const TDF_MARKET_DATA *tdfMarketData) = 0;
	virtual void OnOrderByTick(const TDF_ORDER *tdfOrder) = 0;
	virtual void OnTransByTick(const TDF_TRANSACTION *tdfTrans) = 0;
	virtual void msg(string m) = 0;
};


class TDFBase
{
public:
	TDFBase();
	~TDFBase();

	static void RecvData(THANDLE hTdf, TDF_MSG * pMsgHead);
	static void RecvSys(THANDLE hTdf, TDF_MSG * pSysMsg);

	bool subscribe(string symbol);
	bool subscribe(string symbol, string exchange);
	void subscribeFile(string filename);
	static void setMdCallback(MdCallback* callback) { mdCallback = callback; };
	std::map<std::string, std::string> iniInfo();

public:
	bool connect( /*string &errorMsg*/);
	bool disconnect();

protected:
	bool _loadConfig(string iniFile);

	THANDLE tdfhandle = NULL;
	bool isConnected = false;

	string _serverIP;
	string _serverPort;
	string _loginUser;
	string _password;
	string _marketCodes;
	string _outputMarket;
	int _subMode;
	int _subscribeTypeFlag;

	//std::vector<std::string> *subList;

	static MdCallback* mdCallback;

};


