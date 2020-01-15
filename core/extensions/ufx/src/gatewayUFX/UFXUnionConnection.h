#pragma once
#include "UFXBusinessInterface.h"
#include "UFXGateway.h"

class CUFXUnionConnection
{
private:
	//报单连接
	CConnectionInterface *con1;
	//回调连接
	CConnectionInterface *con2;
	//订约接口
	CSubscribeInterface *sub;

	//CLock m_lock; 
	//int ParseCallUFXRet();	
	//业务名字
	//char m_BizName[32];
	string m_errMsg;
	int _nVersion;
public:
	~CUFXUnionConnection();
	CUFXUnionConnection(CConfigInterface *lpConfig1, CConfigInterface *lpConfig2, UFXGateway *pGateWay);

	//int InitCallParam(CUFXBaseInterfaceParam *ufxParam);
	
	string GetLastError();

	//std::string m_out_ErrorMsg;
	void show_packet(IF2UnPacker *pUnpacker);

	//创建订阅参数
	//CSubscribeParamInterface* CreateSubParam(string strUserID);

	//订阅登录
	int SubUFXLogin(CUFXUserLogin *pUserLogin, string topicName);

	//异步消息，返回发送句柄，正确则为正数，否则为错误
	int ASyncCallUFX(CUFXBaseInterfaceParam *pUfxParam, int nPacketId=1);
	//同步消息，正确返回0，否则为错误
	int SyncCallUFX(CUFXBaseInterfaceParam *ufxParam);
};