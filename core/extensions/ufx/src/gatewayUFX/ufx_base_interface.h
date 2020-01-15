#pragma once

#ifndef _UFX_BASE_INTERFACE_H
#define _UFX_BASE_INTERFACE_H

#include "t2sdk_interface.h"
#include <string>
using namespace std;

const int RET_OK = 0;
const int RET_ERR = -1;

const int UFX_FUNCNO_USER_LOGON = 10001;
const int UFX_FUNCNO_USER_LOGOFF = 10002;

const int UFX_OPT_ENTRUST = 91005;
const int UFX_ORD_ENTRUST = 91001;
const int UFX_CANCEL_ENTRUST = 91101;

const int UFX_ETF_ENTRUST = 91008;		// ETF申购/赎回
const int UFX_BASKET_ENTRUST = 91090;	// 篮子交易

const int UFX_QRY_FUNDLIST = 30001;
const int UFX_QRY_ASSETLIST = 30002;
const int UFX_QRY_FUNDASSET = 35003;
const int UFX_QRY_UNITASSET = 35011;
const int UFX_QRY_STOCK_TRADES = 33001;	// 证券成交查询
const int UFX_QRY_ETF_BASE_MSG = 35020;	// ETF基础数据
const int UFX_QRY_ETF_LIST = 35014;		// ETF申赎清单

const int UFX_QRY_STOCKPOS = 31001;
const int UFX_QRY_FUTUREPOS = 31003;

const int PACKER_VIRSION = 0x20;
const int DEFAULT_TIME_OUT = 10000;

const int MAX_OPERATOR_LEN = 18;
const int MAX_PASSWORD_LEN = 10;

const int UFX_FUNCNO_ENTRUST_QRY = 32001;	// 证券委托查询


class CUFXBaseInterfaceParam
{
public:
    int m_out_ErrorCode;
    std::string m_out_ErrorMsg;
    std::string m_out_MsgDetail;
    int m_out_DataCount;
    
    CUFXBaseInterfaceParam(){ClearInfo();};
    virtual ~CUFXBaseInterfaceParam(){};
    
    virtual int ClearInfo();
	virtual int CheckParamValid() { return true; };
    virtual int PackParam(IF2Packer *lpPacker)=0;
	virtual int UpackResult(IF2UnPacker *lpUnpacker) { return 0; };
    virtual int UpackHeader(IF2UnPacker *lpUnpacker); //解压头部的错误信息
	virtual void LogParam() {};
    virtual int GetFuncNo()=0;
	virtual int GetTimeOut() { return DEFAULT_TIME_OUT; };
};

class MyException
{
public:
	MyException(int errCode,const char *errMsg){m_ErrCode=errCode;m_ErrMsg=errMsg;}
	string m_ErrMsg;
	int m_ErrCode;
};

//void ShowPacket(IF2UnPacker *lpUnPacker);
std::string charToStr(const char *str);
#endif