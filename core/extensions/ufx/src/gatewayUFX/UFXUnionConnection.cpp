#include "t2sdk_interface.h"
#include "UFXUnionConnection.h"
#include "UFXGateway.h"

CUFXUnionConnection::CUFXUnionConnection(CConfigInterface * lpConfig1,CConfigInterface * lpConfig2, UFXGateway *pGateWay)
{
    con1  = NewConnection(lpConfig1);    
	con1 ->AddRef();
	int ret = con1 ->Create2BizMsg(pGateWay);
	if (ret != 0)
	{
		throw MyException(-1, con1 ->GetErrorMsg(ret));
	}
	ret =con1 ->Connect(DEFAULT_TIME_OUT);
    if(ret !=0)
    {
		throw MyException(-1,con1 ->GetErrorMsg(ret));
    }

	con2 = NewConnection(lpConfig2);
    ret = con2->Create(NULL);
    if(ret != 0)
	{
		throw MyException(-1,con2->GetErrorMsg(ret));
	}
	ret =con2->Connect(DEFAULT_TIME_OUT);
    if(ret !=0)
    {
		throw MyException(-1,con2->GetErrorMsg(ret));
    }  	
	
	string bizName = lpConfig2->GetString("subcribe", "biz_name", "");	
	sub=con2->NewSubscriber(pGateWay,(char*)bizName.c_str(),5000);
	if (!sub)
	{
		const char* errMsg = con2->GetMCLastError();
		if(errMsg)
			throw MyException(-1,con2->GetMCLastError());
		else
			throw MyException(-1, "");
	}
	sub->AddRef();
	_nVersion = 2;
}

CUFXUnionConnection::~CUFXUnionConnection()
{
    if (NULL != con1 ) con1 ->Release();
	if (NULL != con2) con2->Release();
    /*if (NULL != m_pPacker) 
    {
        m_pPacker->FreeMem(m_pPacker->GetPackBuf());
        m_pPacker->Release();
    }*/
}

int CUFXUnionConnection::SyncCallUFX(CUFXBaseInterfaceParam *pUfxParam)
{
    if (NULL == con1 )  return RET_ERR;
	//if (NULL == con2)  return RET_ERR;
    if (NULL == pUfxParam) return RET_ERR;
    //if (NULL == m_pPacker) return RET_ERR;
	
	//m_lock.Lock();
	IF2Packer *pPacker=NewPacker(_nVersion);
	if(pPacker==NULL)
	{
		m_errMsg="NewPacker Err";
		//m_lock.Unlock();
		return -1;
	}
	//m_lock.Unlock();
    pUfxParam->PackParam(pPacker);
    int hSend = con1 ->SendBiz(pUfxParam->GetFuncNo(),pPacker,0);
	if(hSend<0)
	{
		m_errMsg=con1 ->GetErrorMsg(hSend);		
        pPacker->FreeMem(pPacker->GetPackBuf());
        pPacker->Release();    
		return -1;
	}
	pPacker->FreeMem(pPacker->GetPackBuf());
    pPacker->Release();    
	
	void *pUnpacker=NULL;
    int iRcv = con1 ->RecvBiz(hSend,&pUnpacker,pUfxParam->GetTimeOut());
    if (iRcv != 0)
    {
		m_errMsg=con1 ->GetErrorMsg(iRcv);
		return -1;
    }
	//int iRcv = pUserLogon->SubscribeTopic(pSubParam, 5000, &lpBack, pSubPack);
	ShowPacket((IF2UnPacker *)pUnpacker);
	pUfxParam->UpackResult((IF2UnPacker *)pUnpacker);
	return RET_OK;

}

int CUFXUnionConnection::SubUFXLogin(CUFXUserLogin *pUserLogin,string topicName)
{	
	//CSubscribeParamInterface *pSubParam = CreateSubParam(pUserLogin->m_in_operator_no);
	CSubscribeParamInterface* lpSubscribeParam = NewSubscribeParam();
	lpSubscribeParam->AddRef();
	//设置主题名字
	lpSubscribeParam->SetTopicName((char*)topicName.c_str());
	lpSubscribeParam->SetFromNow(false);	
	lpSubscribeParam->SetReplace(false);
	//添加过滤字段
	lpSubscribeParam->SetFilter("operator_no", (char*)pUserLogin->m_in_operator_no.c_str());
	//lpSubscribeParam->SetFilter("msgtype","a");

	//添加发送频率
	lpSubscribeParam->SetSendInterval(0);
	
	//m_lock.Lock();
	IF2Packer* pSubPack = NewPacker(_nVersion);
	if (pSubPack == NULL)
	{
		m_errMsg = "NewPacker Err";
		//m_lock.Unlock();
		return -1;
	}
	//m_lock.Unlock();
	pSubPack->AddRef();
	//创建一个业务包
	pSubPack->BeginPack();
	pSubPack->AddField("login_operator_no");
	pSubPack->AddField("password");
	pSubPack->AddStr(pUserLogin->m_in_operator_no.c_str());
	pSubPack->AddStr(pUserLogin->m_in_password.c_str());
	
	pSubPack->EndPack();
	IF2UnPacker* lpBack = NULL;
	int iRcv = sub->SubscribeTopic(lpSubscribeParam, 5000, &lpBack, pSubPack);
	if (NULL != pSubPack)
	{
		pSubPack->FreeMem(pSubPack->GetPackBuf());
		pSubPack->Release();
	}
	if (iRcv <= 0)
	{
		m_errMsg = con2->GetErrorMsg(iRcv);
		return -1;
	}
	return RET_OK;
}

string CUFXUnionConnection::GetLastError()
{
	return m_errMsg;
}


void CUFXUnionConnection::show_packet(IF2UnPacker *pUnpacker)
{
	if (NULL == pUnpacker)
	{
		//printf("返回报文为空！！！\n");
		return;
	}

	ShowPacket(pUnpacker);
}

//以2进制形式和恒生UFX交互
int CUFXUnionConnection::ASyncCallUFX(CUFXBaseInterfaceParam *pUfxParam,int nPacketId)
{
	if (NULL == con1)  return RET_ERR;	
	if (NULL == pUfxParam) return RET_ERR;	
	IF2Packer *pPacker = NewPacker(_nVersion);
	if (pPacker == NULL)
	{
		m_errMsg = "NewPacker Err";		
		return -1;
	}
	pUfxParam->PackParam(pPacker);
	IBizMessage* lpBizMessage = NewBizMessage();
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction(pUfxParam->GetFuncNo());
	lpBizMessage->SetPacketType(REQUEST_PACKET);
	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	lpBizMessage->SetPacketId(nPacketId);
	//二进制的包和恒生接口进行交互
	int ret=con1->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	lpBizMessage->Release();
	return ret;
}

 