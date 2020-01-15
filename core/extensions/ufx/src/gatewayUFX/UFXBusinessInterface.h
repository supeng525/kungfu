#pragma once
#include <vector>
#include <memory>
#include "ufx_base_interface.h"

//心跳
class CUFXBeat :public CUFXBaseInterfaceParam
{
public:
	CUFXBeat(){};
	~CUFXBeat() {};

	string m_in_user_token = { "" };

	int CheckParamValid()
	{
		if (m_in_user_token=="") return RET_ERR;
		return RET_OK;
	};

	int PackParam(IF2Packer *lpPacker)
	{
		if (NULL == lpPacker) return RET_ERR;
		lpPacker->BeginPack();
		lpPacker->AddField("user_token");		
		lpPacker->AddStr(m_in_user_token.c_str());
		lpPacker->EndPack();
		return RET_OK;
	};

	int UpackResult(IF2UnPacker *lpUnpacker)
	{	
		return RET_OK;
	};

	int GetFuncNo() {
		return 10000;
	};

};

//登出
class CUFXUserLogoff :public CUFXBaseInterfaceParam
{
public:
	CUFXUserLogoff(){};
	~CUFXUserLogoff() {};
	string m_in_user_token = { "" };

	int CheckParamValid()
	{
		if (m_in_user_token == "") return RET_ERR;
		return RET_OK;
	};

	int PackParam(IF2Packer *lpPacker)
	{
		if (NULL == lpPacker) return RET_ERR;
		lpPacker->BeginPack();
		lpPacker->AddField("user_token");
		lpPacker->AddStr(m_in_user_token.c_str());
		lpPacker->EndPack();
		return RET_OK;
	};

	int UpackResult(IF2UnPacker *lpUnpacker)
	{
		return RET_OK;
	};

	int GetFuncNo() {
		return 10002;
	};

};

// 账户名称
struct CFundName
{
	string account_code;
	string account_name;
	string account_type;
};

//资金账号列表查询
class CUFXQryAccountList :
	public CUFXBaseInterfaceParam
{
public:
	CUFXQryAccountList() {}
	~CUFXQryAccountList()
	{
		CFundName* pTemp;
		vector<CFundName*>::iterator iter;
		for (iter = m_out_fund_list.begin(); iter != m_out_fund_list.end(); iter++)
		{
			pTemp = *iter;
			if (pTemp != NULL)
			{
				delete pTemp;
				pTemp = NULL;
			}
		}
		m_out_fund_list.clear();
	}

	string m_in_user_token = { "" };

	string m_out_ErrorMsg = { "" };
	vector<CFundName*>m_out_fund_list;

	int CheckParamValid()
	{
		if (m_in_user_token == "") return RET_ERR;
		return RET_OK;
	}

	int PackParam(IF2Packer *lpPacker)
	{
		if (NULL == lpPacker) return RET_ERR;
		lpPacker->BeginPack();
		lpPacker->AddField("user_token");

		lpPacker->AddStr(m_in_user_token.c_str());

		lpPacker->EndPack();
		return RET_OK;
	};

	int UpackResult(IF2UnPacker *lpUnpacker)
	{
		if (NULL == lpUnpacker) return RET_ERR;
		int iErrorCode = UpackHeader(lpUnpacker);
		if (iErrorCode != 0)
		{
			m_out_ErrorMsg = m_out_ErrorMsg + ":" + m_out_MsgDetail;
			return iErrorCode;
		}
		if (lpUnpacker->GetDatasetCount() > 1)
		{
			lpUnpacker->SetCurrentDatasetByIndex(1);
			lpUnpacker->First();
			for (int i = 0; i < m_out_DataCount; i++)
			{
				lpUnpacker->SetCurrentDatasetByIndex(i + 1);
				CFundName *pFundInfo = new CFundName();
				pFundInfo->account_code = lpUnpacker->GetStr("account_code");
				pFundInfo->account_name = lpUnpacker->GetStr("account_name");
				pFundInfo->account_type = lpUnpacker->GetStr("account_type");
				m_out_fund_list.push_back(pFundInfo);
				lpUnpacker->Next();
			}
		}
		return RET_OK;
	}
	int GetFuncNo() { return UFX_QRY_FUNDLIST; };
};


//资产单元列表查询
class CUFXQryAssetUnitList :
	public CUFXBaseInterfaceParam
{
public:
	CUFXQryAssetUnitList() {}
	~CUFXQryAssetUnitList()
	{
		CFundName* pTemp;
		vector<CFundName*>::iterator iter;
		for (iter = m_out_fund_list.begin(); iter != m_out_fund_list.end(); iter++)
		{
			pTemp = *iter;
			if (pTemp != NULL)
			{
				delete pTemp;
				pTemp = NULL;
			}
		}
		m_out_fund_list.clear();
	}

	string m_in_user_token = { "" };

	string m_out_ErrorMsg = { "" };
	vector<CFundName*>m_out_fund_list;

	int CheckParamValid()
	{
		if (m_in_user_token == "") return RET_ERR;
		return RET_OK;
	}

	int PackParam(IF2Packer *lpPacker)
	{
		if (NULL == lpPacker) return RET_ERR;
		lpPacker->BeginPack();
		lpPacker->AddField("user_token");

		lpPacker->AddStr(m_in_user_token.c_str());

		lpPacker->EndPack();
		return RET_OK;
	};

	int UpackResult(IF2UnPacker *lpUnpacker)
	{
		if (NULL == lpUnpacker) return RET_ERR;
		int iErrorCode = UpackHeader(lpUnpacker);
		if (iErrorCode != 0)
		{
			m_out_ErrorMsg = m_out_ErrorMsg + ":" + m_out_MsgDetail;
			return iErrorCode;
		}
		if (lpUnpacker->GetDatasetCount() > 1)
		{
			lpUnpacker->SetCurrentDatasetByIndex(1);
			lpUnpacker->First();
			for (int i = 0; i < m_out_DataCount; i++)
			{
				lpUnpacker->SetCurrentDatasetByIndex(i + 1);
				CFundName *pFundInfo = new CFundName();
				pFundInfo->account_code = lpUnpacker->GetStr("account_code");
				pFundInfo->account_name = lpUnpacker->GetStr("account_name");
				pFundInfo->account_type = lpUnpacker->GetStr("account_type");
				m_out_fund_list.push_back(pFundInfo);
				lpUnpacker->Next();
			}
		}
		return RET_OK;
	}

	int GetFuncNo() { return UFX_QRY_ASSETLIST; };
};



//登录
class CUFXUserLogin :public CUFXBaseInterfaceParam
{
public:
	CUFXUserLogin() 
	{
		m_in_operator_no = "";
		m_in_password = "";
		m_in_mac_address = "";
		m_in_op_station = "";
		m_in_ip_address = "";
		m_in_authorization_id = "";
		m_out_user_token = "";
	};
	~CUFXUserLogin() {};

	string m_in_operator_no = { "" };
	string m_in_mac_address = { "" };
	string m_in_op_station = { "" };
	string m_in_ip_address = { "" };
	string m_in_password = { "" };
	string m_in_authorization_id = { "" };
	string m_out_user_token = { "" };

	int CheckParamValid() 
	{
		if (m_in_operator_no.length() > MAX_OPERATOR_LEN) return RET_ERR;
		if (m_in_password.length() > MAX_PASSWORD_LEN) return RET_ERR;
		return RET_OK;
	};

	int PackParam(IF2Packer *lpPacker)
	{
		if (NULL == lpPacker) return RET_ERR;
		lpPacker->BeginPack();
		lpPacker->AddField("operator_no");
		lpPacker->AddField("password");
		lpPacker->AddField("mac_address");
		lpPacker->AddField("op_station");
		lpPacker->AddField("ip_address");
		lpPacker->AddField("authorization_id");
		lpPacker->AddStr(m_in_operator_no.c_str());
		lpPacker->AddStr(m_in_password.c_str());
		lpPacker->AddStr(m_in_mac_address.c_str());
		lpPacker->AddStr(m_in_op_station.c_str());
		lpPacker->AddStr(m_in_ip_address.c_str());
		lpPacker->AddStr(m_in_authorization_id.c_str());
		lpPacker->EndPack();
		return RET_OK;
	};

	int UpackResult(IF2UnPacker *lpUnpacker)
	{
		if (NULL == lpUnpacker) return RET_ERR;
		lpUnpacker->SetCurrentDatasetByIndex(0);
		int iErrorCode = UpackHeader(lpUnpacker);
		if (iErrorCode != 0)
		{
			m_out_ErrorMsg = m_out_ErrorMsg + ":" + m_out_MsgDetail;
			return iErrorCode;
		}
		if (lpUnpacker->GetDatasetCount() > 1)
			lpUnpacker->SetCurrentDatasetByIndex(1);
		m_out_user_token = charToStr(lpUnpacker->GetStr("user_token"));
		return RET_OK;
	};
	
	int GetFuncNo() { return UFX_FUNCNO_USER_LOGON; };

};


struct CFundInfo
{
public:
	string	account_code;
	string	currency_code;
	string	total_asset;
	string	nav;
	string	yesterday_nav;
	string	current_balance;
	string	begin_balance;
	string	futu_deposit_balance;
	string	occupy_deposit_balance;
	string	futu_asset;
	string	stock_asset;
	string	bond_asset;
	string	fund_asset;
	string	repo_asset;
	string	other_asset;
	string	fund_share;
	string	fund_net_asset;
	string	payable_balance;
	string	receivable_balance;
};

//资金查询
class CUFXQryAccount :
	public CUFXBaseInterfaceParam
{
public:
	CUFXQryAccount() {}
	~CUFXQryAccount() 
	{
		CFundInfo* pTemp;
		vector<CFundInfo*>::iterator iter;
		for (iter = m_out_fund_list.begin(); iter != m_out_fund_list.end(); iter++)
		{
			pTemp = *iter;
			if (pTemp != NULL)
			{
				delete pTemp;
				pTemp = NULL;
			}
		}
		m_out_fund_list.clear();
	}

	string m_in_user_token = { "" };
	string m_in_account_code = { "" };
	
	vector<CFundInfo*>m_out_fund_list;

	int CheckParamValid()
	{
		if(m_in_user_token=="") return RET_ERR;
		if(m_in_account_code == "") return RET_ERR;
		return RET_OK;
	}
	int PackParam(IF2Packer *lpPacker)
	{
		if (NULL == lpPacker) return RET_ERR;
		lpPacker->BeginPack();
		lpPacker->AddField("user_token");
		lpPacker->AddField("account_code");
		lpPacker->AddStr(m_in_user_token.c_str());
		lpPacker->AddStr(m_in_account_code.c_str());
		lpPacker->EndPack();
		return RET_OK;
	};

	int UpackResult(IF2UnPacker *lpUnpacker)
	{
		if (NULL == lpUnpacker) return RET_ERR;

		int iErrorCode = UpackHeader(lpUnpacker);
		if (iErrorCode != 0)
		{	
			m_out_ErrorMsg = m_out_ErrorMsg + ":" + m_out_MsgDetail;
			return iErrorCode;
		}
		
		if (lpUnpacker->GetDatasetCount() > 1)
		{
			lpUnpacker->SetCurrentDatasetByIndex(1);
			lpUnpacker->First();
			for (int i = 0; i < m_out_DataCount; i++)
			{
				CFundInfo *pFundInfo = new CFundInfo();
				pFundInfo->account_code = lpUnpacker->GetStr("account_code");
				pFundInfo->currency_code = lpUnpacker->GetStr("currency_code");
				pFundInfo->total_asset = lpUnpacker->GetStr("total_asset");
				pFundInfo->nav = lpUnpacker->GetStr("nav");
				pFundInfo->yesterday_nav = lpUnpacker->GetStr("yesterday_nav");
				pFundInfo->current_balance = lpUnpacker->GetStr("current_balance");
				pFundInfo->begin_balance = lpUnpacker->GetStr("begin_balance");
				pFundInfo->futu_deposit_balance = lpUnpacker->GetStr("futu_deposit_balance");
				pFundInfo->occupy_deposit_balance = lpUnpacker->GetStr("occupy_deposit_balance");
				pFundInfo->futu_asset = lpUnpacker->GetStr("futu_asset");
				pFundInfo->stock_asset = lpUnpacker->GetStr("stock_asset");
				pFundInfo->bond_asset = lpUnpacker->GetStr("bond_asset");
				pFundInfo->fund_asset = lpUnpacker->GetStr("fund_asset");
				pFundInfo->repo_asset = lpUnpacker->GetStr("repo_asset");
				pFundInfo->other_asset = lpUnpacker->GetStr("other_asset");
				pFundInfo->fund_share = lpUnpacker->GetStr("fund_share");
				pFundInfo->fund_net_asset = lpUnpacker->GetStr("fund_net_asset");
				pFundInfo->payable_balance = lpUnpacker->GetStr("payable_balance");
				pFundInfo->receivable_balance = lpUnpacker->GetStr("receivable_balance");
				m_out_fund_list.push_back(pFundInfo);
				lpUnpacker->Next();
			}
		}
		return RET_OK;
	}
	int GetFuncNo() { return UFX_QRY_FUNDASSET; };
};


// 资产单元的资金查询
class CUFXQryUnitAsset :
	public CUFXBaseInterfaceParam
{
public:
	CUFXQryUnitAsset() {}
	~CUFXQryUnitAsset()
	{
	}

	string m_in_user_token = { "" };
	string m_in_account_code = { "" };
	string m_in_asset_no = { "" };

	int CheckParamValid()
	{
		if (m_in_user_token == "") return RET_ERR;
		if (m_in_account_code == "") return RET_ERR;
		return RET_OK;
	}
	int PackParam(IF2Packer *lpPacker)
	{
		if (NULL == lpPacker) return RET_ERR;
		lpPacker->BeginPack();
		lpPacker->AddField("user_token");
		lpPacker->AddField("account_code");
		if (m_in_asset_no != "")
			lpPacker->AddField("asset_no");
		lpPacker->AddStr(m_in_user_token.c_str());
		lpPacker->AddStr(m_in_account_code.c_str());
		if (m_in_asset_no != "")
			lpPacker->AddStr(m_in_asset_no.c_str());
		lpPacker->EndPack();
		return RET_OK;
	};

	int UpackResult(IF2UnPacker *lpUnpacker)
	{
		// 不用解析
		return RET_OK;
	}
	int GetFuncNo() { return UFX_QRY_UNITASSET; };
};


//ETF基础信息查询
class CUFXQryETFBaseMsg :
	public CUFXBaseInterfaceParam
{
public:
	CUFXQryETFBaseMsg() {}
	~CUFXQryETFBaseMsg()
	{
	}

	string m_in_user_token = { "" };
	string m_in_market_no = { "" };
	string m_in_etf_type = { "" };
	string m_in_etf_code = { "" };	// ETF代码（二级市场）

	int CheckParamValid()
	{
		if (m_in_user_token == "") return RET_ERR;
		if (m_in_market_no == "") return RET_ERR;
		if (m_in_etf_code == "") return RET_ERR;
		return RET_OK;
	}
	int PackParam(IF2Packer *lpPacker)
	{
		if (NULL == lpPacker) return RET_ERR;

		lpPacker->BeginPack();
		lpPacker->AddField("user_token");
		if (m_in_market_no != "")
			lpPacker->AddField("market_no");
		if (m_in_etf_code != "")
			lpPacker->AddField("etf_code");
		if (m_in_etf_type != "")
			lpPacker->AddField("etf_type");
		
		lpPacker->AddStr(m_in_user_token.c_str());
		if (m_in_market_no != "")
			lpPacker->AddStr(m_in_market_no.c_str());
		if (m_in_etf_code != "")
			lpPacker->AddStr(m_in_etf_code.c_str());
		if (m_in_etf_type != "")
			lpPacker->AddStr(m_in_etf_type.c_str());
		lpPacker->EndPack();
		return RET_OK;
	};


	int UpackResult(IF2UnPacker *lpUnpacker)
	{
		return RET_OK;
	}

	int GetFuncNo() { return UFX_QRY_ETF_BASE_MSG; };
};


// ETF成分股信息
struct CETFStock
{
	int business_date = { 0 }; //	N8	业务日期
	string market_no = { "" }; //	C3	交易市场
	string stock_code = { "" }; //	C16	证券代码
	int stock_amount = { 0 }; // N12	证券数量
	string replace_flag = { "" }; // 	C1	现金替代标志
	double replace_ratio = { 0.0 }; // 	N9.8	溢价比率
	double replace_balance = { 0.0 }; // 	N16.2	替代金额
	double redeem_replace_balance = { 0.0 }; // N16.2	赎回替代金额
};

//ETF申赎列表
class CUFXQryETFList :
	public CUFXBaseInterfaceParam
{
public:
	CUFXQryETFList() {}
	~CUFXQryETFList()
	{
		CETFStock* pTemp;
		vector<CETFStock*>::iterator iter;
		for (iter = m_out_etf_stock_list.begin(); iter != m_out_etf_stock_list.end(); iter++)
		{
			pTemp = *iter;
			if (pTemp != NULL)
			{
				delete pTemp;
				pTemp = NULL;
			}
		}
		m_out_etf_stock_list.clear();
	}

	string m_in_user_token = { "" };
	string m_in_market_no = { "" };
	string m_in_etf_code = { "" };	// ETF代码（二级市场）

	vector<CETFStock *> m_out_etf_stock_list;
	int CheckParamValid()
	{
		if (m_in_user_token == "") return RET_ERR;
		if (m_in_market_no == "") return RET_ERR;
		if (m_in_etf_code == "") return RET_ERR;
		return RET_OK;
	}

	int PackParam(IF2Packer *lpPacker)
	{
		if (NULL == lpPacker) return RET_ERR;

		lpPacker->BeginPack();
		lpPacker->AddField("user_token");
		if (m_in_market_no != "")
			lpPacker->AddField("market_no");
		if (m_in_etf_code != "")
			lpPacker->AddField("etf_code");

		lpPacker->AddStr(m_in_user_token.c_str());
		if (m_in_market_no != "")
			lpPacker->AddStr(m_in_market_no.c_str());
		if (m_in_etf_code != "")
			lpPacker->AddStr(m_in_etf_code.c_str());
		lpPacker->EndPack();
		return RET_OK;
	};

	int UpackResult(IF2UnPacker *lpUnpacker)
	{
		if (NULL == lpUnpacker) return RET_ERR;

		int iErrorCode = UpackHeader(lpUnpacker);
		if (iErrorCode != 0)
		{
			m_out_ErrorMsg = m_out_ErrorMsg + ":" + m_out_MsgDetail;
			return iErrorCode;
		}

		if (lpUnpacker->GetDatasetCount() > 1)
		{
			lpUnpacker->SetCurrentDatasetByIndex(1);
			lpUnpacker->First();

			for (int i = 0; i < (int)lpUnpacker->GetRowCount(); i++)
			{
				CETFStock *pStockInfo = new CETFStock();
				pStockInfo->business_date = lpUnpacker->GetInt("business_date");
				pStockInfo->market_no = lpUnpacker->GetStr("market_no");
				pStockInfo->stock_code = lpUnpacker->GetStr("stock_code");
				pStockInfo->stock_amount = lpUnpacker->GetInt("stock_amount");
				pStockInfo->stock_code = lpUnpacker->GetStr("stock_code");
				pStockInfo->replace_flag = lpUnpacker->GetStr("replace_flag");
				pStockInfo->replace_ratio = lpUnpacker->GetDouble("replace_ratio");
				pStockInfo->replace_balance = lpUnpacker->GetDouble("replace_balance");
				pStockInfo->redeem_replace_balance = lpUnpacker->GetDouble("redeem_replace_balance");
				m_out_etf_stock_list.push_back(pStockInfo);
				lpUnpacker->Next();
			}
		}

		return RET_OK;
	}

	int GetFuncNo() { return UFX_QRY_ETF_LIST; };
};


struct CStockInfo
{
public:
	string account_code;
	string asset_no;
	string combi_no;
	string market_no;
	string stock_code;
	string stockholder_id;
	string hold_seat;
	string invest_type;
	string current_amount;
	string enable_amount;
	string begin_cost;
	string current_cost;
	string pre_buy_amount;
	string pre_sell_amount;
	string pre_buy_balance;
	string pre_sell_balance;
	string today_buy_amount;
	string today_sell_amount;
	string today_buy_balance;
	string today_sell_balance;
	string today_buy_fee;
	string today_sell_fee;
};

//股份查询
class CUFXQryStockPos:
	public CUFXBaseInterfaceParam
{
public:
	CUFXQryStockPos() {}
	~CUFXQryStockPos()
	{
		CStockInfo* pTemp;
		vector<CStockInfo*>::iterator iter;
		for (iter = m_out_stock_list.begin(); iter != m_out_stock_list.end(); iter++)
		{
			pTemp = *iter;
			if (pTemp != NULL)
			{
				delete pTemp;
				pTemp = NULL;
			}
		}
		m_out_stock_list.clear();
	}

	string m_in_user_token = { "" };
	string m_in_account_code = { "" };
	string m_in_asset_no = { "" };
	string m_in_combi_no = { "" };
	string m_in_market_no = { "" };
	string m_in_stock_code = { "" };
	string m_in_holder_acc = { "" };
	string m_in_seat = { "" };

	vector<CStockInfo *> m_out_stock_list;
	int CheckParamValid()
	{
		if (m_in_user_token == "") return RET_ERR;
		if (m_in_account_code == "") return RET_ERR;
		if(m_in_asset_no=="") return RET_ERR;
		return RET_OK;
	}
	int PackParam(IF2Packer *lpPacker)
	{
		if (NULL == lpPacker) return RET_ERR;

		lpPacker->BeginPack();
		lpPacker->AddField("user_token");
		lpPacker->AddField("account_code");
		if (m_in_asset_no != "")
			lpPacker->AddField("asset_no");
		if (m_in_combi_no != "")
			lpPacker->AddField("combi_no");

		if (m_in_market_no != "")
			lpPacker->AddField("market_no_list");
		if (m_in_stock_code != "")
			lpPacker->AddField("stock_code");
		if (m_in_holder_acc != "")
			lpPacker->AddField("stockholder_id");
		if (m_in_seat != "")
			lpPacker->AddField("holde_seat");
		/*if (m_in_invest_type != 0)
			lpPacker->AddField("invest_type_list");
		if (m_in_position_flag != 0)
			lpPacker->AddField("position_flag");*/


		lpPacker->AddStr(m_in_user_token.c_str());
		lpPacker->AddStr(m_in_account_code.c_str());
		if (m_in_asset_no != "")
			lpPacker->AddStr(m_in_asset_no.c_str());
		if (m_in_combi_no != "")
			lpPacker->AddStr(m_in_combi_no.c_str());
		if (m_in_market_no != "")			
			lpPacker->AddStr(m_in_market_no.c_str());
		if (m_in_stock_code != "")
			lpPacker->AddStr(m_in_stock_code.c_str());
		if (m_in_holder_acc != "")
			lpPacker->AddStr(m_in_holder_acc.c_str());
		if (m_in_seat != "")
			lpPacker->AddStr(m_in_seat.c_str());
		/*if (m_in_invest_type != 0)
			lpPacker->AddChar(m_in_invest_type);
		if (m_in_position_flag != 0)
			lpPacker->AddChar(m_in_position_flag);*/
		lpPacker->EndPack();
		return RET_OK;
	};


	int UpackResult(IF2UnPacker *lpUnpacker)
	{
		if (NULL == lpUnpacker) return RET_ERR;

		int iErrorCode = UpackHeader(lpUnpacker);
		if (iErrorCode != 0)
		{
			m_out_ErrorMsg = m_out_ErrorMsg + ":" + m_out_MsgDetail;
			return iErrorCode;
		}

		if (lpUnpacker->GetDatasetCount() > 1)
		{
			lpUnpacker->SetCurrentDatasetByIndex(1);
			lpUnpacker->First();

			for (int i = 0; i < m_out_DataCount; i++)
			{
				CStockInfo *pStockInfo = new CStockInfo();
				pStockInfo->account_code = lpUnpacker->GetStr("account_code");
				pStockInfo->asset_no = lpUnpacker->GetStr("asset_no");
				pStockInfo->combi_no = lpUnpacker->GetStr("combi_no");
				pStockInfo->market_no = lpUnpacker->GetStr("market_no");
				pStockInfo->stock_code = lpUnpacker->GetStr("stock_code");
				pStockInfo->stockholder_id = lpUnpacker->GetStr("stockholder_id");
				pStockInfo->hold_seat = lpUnpacker->GetStr("hold_seat");
				pStockInfo->invest_type = lpUnpacker->GetStr("invest_type");
				pStockInfo->current_amount = lpUnpacker->GetStr("current_amount");
				pStockInfo->enable_amount = lpUnpacker->GetStr("enable_amount");
				pStockInfo->begin_cost = lpUnpacker->GetStr("begin_cost");
				pStockInfo->current_cost = lpUnpacker->GetStr("current_cost");
				pStockInfo->pre_buy_amount = lpUnpacker->GetStr("pre_buy_amount");
				pStockInfo->pre_sell_amount = lpUnpacker->GetStr("pre_sell_amount");
				pStockInfo->pre_buy_balance = lpUnpacker->GetStr("pre_buy_balance");
				pStockInfo->pre_sell_balance = lpUnpacker->GetStr("pre_sell_balance");
				pStockInfo->today_buy_amount = lpUnpacker->GetStr("today_buy_amount");
				pStockInfo->today_sell_amount = lpUnpacker->GetStr("today_sell_amount");
				pStockInfo->today_buy_balance = lpUnpacker->GetStr("today_buy_balance");
				pStockInfo->today_buy_fee = lpUnpacker->GetStr("today_buy_fee");
				pStockInfo->today_sell_fee = lpUnpacker->GetStr("today_sell_fee");
				m_out_stock_list.push_back(pStockInfo);
				lpUnpacker->Next();
			}
		}
		return RET_OK;
	}
	//void LogParam();
	int GetFuncNo() { return UFX_QRY_STOCKPOS; };
};


//股份查询
class CUFXQryFuturePos :
	public CUFXBaseInterfaceParam
{
public:
	CUFXQryFuturePos() {}
	~CUFXQryFuturePos()
	{
		CStockInfo* pTemp;
		vector<CStockInfo*>::iterator iter;
		for (iter = m_out_stock_list.begin(); iter != m_out_stock_list.end(); iter++)
		{
			pTemp = *iter;
			if (pTemp != NULL)
			{
				delete pTemp;
				pTemp = NULL;
			}
		}
		m_out_stock_list.clear();
	}

	string m_in_user_token = { "" };
	string m_in_account_code = { "" };
	string m_in_asset_no = { "" };

	vector<CStockInfo *> m_out_stock_list;
	int CheckParamValid()
	{
		if (m_in_user_token == "") return RET_ERR;
		if (m_in_account_code == "") return RET_ERR;
		if (m_in_asset_no == "") return RET_ERR;
		return RET_OK;
	}
	int PackParam(IF2Packer *lpPacker)
	{
		if (NULL == lpPacker) return RET_ERR;

		lpPacker->BeginPack();
		lpPacker->AddField("user_token");
		lpPacker->AddField("account_code");
		lpPacker->AddField("asset_no");

		lpPacker->AddStr(m_in_user_token.c_str());
		lpPacker->AddStr(m_in_account_code.c_str());
		lpPacker->AddStr(m_in_asset_no.c_str());
		
		lpPacker->EndPack();
		return RET_OK;
	};

	int GetFuncNo() { return UFX_QRY_FUTUREPOS; };
};


struct COrderInfo
{
public:
	string entrust_date;
	string entrust_time;
	string operator_no;
	string batch_no;
	string entrust_no;
	string report_no;
	int extsystem_id;
	string third_reff;
	string account_code;
	string asset_no;
	string combi_no;
	string stockholder_id;
	string report_seat;
	string market_no;
	string stock_code;
	string entrust_direction;
	string price_type;
	string entrust_price;
	int entrust_amount;
	string pre_buy_frozen_balance;
	string pre_sell_balance;
	string confirm_no;
	string entrust_state;
	string first_deal_time;
	string deal_amount;
	string deal_balance;
	string deal_price;
	string deal_times;
	string withdraw_amount;
	string withdraw_cause;
	//string position_str;
	//string exchange_report_no;
};


//委托查询
class CUFXQryOrders :
	public CUFXBaseInterfaceParam
{
public:
	CUFXQryOrders() {}
	~CUFXQryOrders() 
	{
		COrderInfo* pTemp;
		vector<COrderInfo*>::iterator iter;
		for (iter = m_out_order_list.begin(); iter != m_out_order_list.end(); iter++)
		{
			pTemp = *iter;
			if (pTemp != NULL)
			{
				delete pTemp;
				pTemp = NULL;
			}
		}
		m_out_order_list.clear();
	}
	string m_in_user_token = { "" };
	string m_in_account_code = { "" };
	string m_in_asset_no = { "" };
	string m_in_combi_no = { "" };
	string m_in_market_no = { "" };
	string m_in_stock_code = { "" };
	string m_in_holder_acc = { "" };
		
	int m_in_batch_no = { 0 };
	int m_in_entrust_no = { 0 };
	int m_in_extsystem_id = { 0 };
	string m_in_entrust_direction = { "" };
	string m_in_state_list = { "" };
	string m_in_third_reff = { "" };
	
	vector<COrderInfo *> m_out_order_list;

	int CheckParamValid()
	{
		if (m_in_user_token == "") return RET_ERR;
		if (m_in_account_code == "") return RET_ERR;
		if (m_in_asset_no == "") return RET_ERR;
		return RET_OK;
	}
	int PackParam(IF2Packer *lpPacker)
	{
		if (NULL == lpPacker) return RET_ERR;

		lpPacker->BeginPack();
		lpPacker->AddField("user_token");
		lpPacker->AddField("account_code");
		if (m_in_asset_no != "")
			lpPacker->AddField("asset_no");
		if (m_in_combi_no != "")
			lpPacker->AddField("combi_no");
		if (m_in_market_no != "")
			lpPacker->AddField("market_no_list");
		if (m_in_stock_code != "")
			lpPacker->AddField("stock_code");
		if (m_in_holder_acc != "")
			lpPacker->AddField("stockholder_id");
				
		
		if (m_in_entrust_direction != "")
			lpPacker->AddField("entrust_direction");
		if (m_in_state_list != "")
			lpPacker->AddField("entrust_state_list");
		if (m_in_third_reff != "")
			lpPacker->AddField("third_reff");

		if (m_in_batch_no != 0)
			lpPacker->AddField("batch_no");
		if (m_in_entrust_no != 0)
			lpPacker->AddField("entrust_no");
		if (m_in_extsystem_id != 0)
			lpPacker->AddField("extsystem_id");

		lpPacker->AddStr(m_in_user_token.c_str());
		lpPacker->AddStr(m_in_account_code.c_str());
		if (m_in_asset_no != "")
			lpPacker->AddStr(m_in_asset_no.c_str());
		if (m_in_combi_no != "")
			lpPacker->AddStr(m_in_combi_no.c_str());
		if (m_in_market_no != "")
			lpPacker->AddStr(m_in_market_no.c_str());
		if (m_in_stock_code != "")
			lpPacker->AddStr(m_in_stock_code.c_str());
		if (m_in_holder_acc != "")
			lpPacker->AddStr(m_in_holder_acc.c_str());

		if (m_in_entrust_direction != "")
			lpPacker->AddStr(m_in_entrust_direction.c_str());
		if (m_in_state_list !=  "")
			lpPacker->AddStr(m_in_state_list.c_str());
		if (m_in_third_reff != "")
			lpPacker->AddStr(m_in_third_reff.c_str());

		if (m_in_batch_no != 0)
			lpPacker->AddInt(m_in_batch_no);
		if (m_in_entrust_no != 0)
			lpPacker->AddInt(m_in_entrust_no);
		if (m_in_extsystem_id != 0)
			lpPacker->AddInt(m_in_extsystem_id);

		lpPacker->EndPack();
		return RET_OK;
	};

	int UpackResult(IF2UnPacker *lpUnpacker)
	{
		if (NULL == lpUnpacker) return RET_ERR;

		int iErrorCode = UpackHeader(lpUnpacker);
		if (iErrorCode != 0)
		{
			m_out_ErrorMsg = m_out_ErrorMsg + ":" + m_out_MsgDetail;
			return iErrorCode;
		}

		if (lpUnpacker->GetDatasetCount() > 1)
		{
			lpUnpacker->SetCurrentDatasetByIndex(1);
			lpUnpacker->First();
			for (int i = 0; i < m_out_DataCount; i++)
			{
				lpUnpacker->SetCurrentDatasetByIndex(i + 1);
				COrderInfo *pOrderInfo = new COrderInfo();
				pOrderInfo->entrust_date = lpUnpacker->GetStr("entrust_date");
				pOrderInfo->entrust_time = lpUnpacker->GetStr("entrust_time");
				pOrderInfo->operator_no = lpUnpacker->GetStr("operator_no");
				pOrderInfo->batch_no = lpUnpacker->GetStr("batch_no");
				pOrderInfo->entrust_no = lpUnpacker->GetStr("entrust_no");
				pOrderInfo->report_no = lpUnpacker->GetStr("report_no");
				pOrderInfo->extsystem_id = lpUnpacker->GetInt("extsystem_id");
				pOrderInfo->third_reff = lpUnpacker->GetStr("third_reff");
				pOrderInfo->account_code = lpUnpacker->GetStr("account_code");
				pOrderInfo->asset_no = lpUnpacker->GetStr("asset_no");
				pOrderInfo->combi_no = lpUnpacker->GetStr("combi_no");
				pOrderInfo->stockholder_id = lpUnpacker->GetStr("stockholder_id");

				pOrderInfo->report_seat = lpUnpacker->GetStr(" report_seat");
				pOrderInfo->market_no = lpUnpacker->GetStr("market_no");
				pOrderInfo->entrust_direction = lpUnpacker->GetStr("entrust_direction");
				pOrderInfo->price_type = lpUnpacker->GetStr("price_type");
				pOrderInfo->entrust_price = lpUnpacker->GetStr("entrust_price");
				pOrderInfo->entrust_amount = lpUnpacker->GetInt("entrust_amount");
				pOrderInfo->pre_buy_frozen_balance = lpUnpacker->GetStr("pre_buy_frozen_balance");
				pOrderInfo->pre_sell_balance = lpUnpacker->GetStr("pre_sell_balance");
				pOrderInfo->confirm_no = lpUnpacker->GetStr("confirm_no");
				pOrderInfo->entrust_state = lpUnpacker->GetStr("entrust_state");
				pOrderInfo->first_deal_time = lpUnpacker->GetStr("first_deal_time");
				pOrderInfo->deal_amount = lpUnpacker->GetStr("deal_amount");
				pOrderInfo->deal_balance = lpUnpacker->GetStr("deal_balance");
				pOrderInfo->deal_price = lpUnpacker->GetStr("deal_price");
				pOrderInfo->deal_times = lpUnpacker->GetStr("deal_times");
				pOrderInfo->withdraw_amount = lpUnpacker->GetStr("withdraw_amount");
				pOrderInfo->withdraw_cause = lpUnpacker->GetStr("withdraw_cause");

				m_out_order_list.push_back(pOrderInfo);
				lpUnpacker->Next();
			}
		}
		return RET_OK;
	}
	//void LogParam();
	int GetFuncNo() { return UFX_FUNCNO_ENTRUST_QRY; };
};


//委托报单
class CUFXETFEntrust :public CUFXBaseInterfaceParam
{
public:
	CUFXETFEntrust() {}
	~CUFXETFEntrust() {}
	string m_in_user_token = { "" };
	string m_in_account_code = { "" };
	string m_in_asset_no = { "" };
	string m_in_market_no = { "" };
	string m_in_stock_code = { "" };
	string m_in_entrust_direction = { "" };

	// 申赎数量单位为篮子数。如最小申购单位是100万份，要申赎200万，该字段填2
	int m_in_entrust_amount = { 0 };
	int m_in_extsystem_id = { 0 };
	string m_in_third_reff = { "" };
	string m_in_mac_address = { "" };
	string m_in_ip_address = { "" };
	string m_in_hd_volserial = { "" };
	string m_in_op_station = { "" };
	string m_in_terminal_info = { "" };

	int m_out_entrust_no = { 0 };
	int m_out_batch_no = { 0 };
	int m_out_extsystem_id = { 0 };

	int CheckParamValid()
	{
		if (m_in_user_token == "") return RET_ERR;
		if (m_in_account_code == "") return RET_ERR;
		if (m_in_asset_no == "") return RET_ERR;
		if (m_in_market_no == "") return RET_ERR;
		if (m_in_stock_code == "") return RET_ERR;
		if (m_in_entrust_direction == "") return RET_ERR;
		if (m_in_entrust_amount == 0) return RET_ERR;
		return RET_OK;
	}

	int PackParam(IF2Packer *lpPacker)
	{
		if (NULL == lpPacker) return RET_ERR;
		lpPacker->BeginPack();
		lpPacker->AddField("user_token");
		lpPacker->AddField("account_code");
		lpPacker->AddField("asset_no");
		lpPacker->AddField("market_no");
		lpPacker->AddField("stock_code");
		lpPacker->AddField("entrust_direction");
		lpPacker->AddField("entrust_amount");

		if (m_in_extsystem_id != 0)
			lpPacker->AddField("extsystem_id");
		if (m_in_third_reff != "")
			lpPacker->AddField("third_reff");
		if (m_in_mac_address != "")
			lpPacker->AddField("mac_address");
		if (m_in_ip_address != "")
			lpPacker->AddField("ip_address");
		if (m_in_hd_volserial != "")
			lpPacker->AddField("hd_volserial");
		if (m_in_op_station != "")
			lpPacker->AddField("op_station");
		if (m_in_terminal_info != "")
			lpPacker->AddField("terminal_info");

		lpPacker->AddStr(m_in_user_token.c_str());
		lpPacker->AddStr(m_in_account_code.c_str());
		lpPacker->AddStr(m_in_asset_no.c_str());
		lpPacker->AddStr(m_in_market_no.c_str());
		lpPacker->AddStr(m_in_stock_code.c_str());
		lpPacker->AddStr(m_in_entrust_direction.c_str());
		lpPacker->AddInt(m_in_entrust_amount);

		if (m_in_extsystem_id != 0)
			lpPacker->AddInt(m_in_extsystem_id);
		if (m_in_third_reff != "")
			lpPacker->AddStr(m_in_third_reff.c_str());
		if (m_in_mac_address != "")
			lpPacker->AddStr(m_in_mac_address.c_str());
		if (m_in_ip_address != "")
			lpPacker->AddStr(m_in_ip_address.c_str());
		if (m_in_hd_volserial != "")
			lpPacker->AddStr(m_in_hd_volserial.c_str());
		if (m_in_op_station != "")
			lpPacker->AddStr(m_in_op_station.c_str());
		if (m_in_terminal_info != "")
			lpPacker->AddStr(m_in_terminal_info.c_str());
		lpPacker->EndPack();
		return RET_OK;
	};

	int UpackResult(IF2UnPacker *lpUnpacker)
	{
		return RET_OK;
	}

	//void LogParam();
	int GetFuncNo() { return UFX_ETF_ENTRUST; };
};




//委托报单
class CUFXBasketEntrust :public CUFXBaseInterfaceParam
{
public:
	CUFXBasketEntrust() {}
	~CUFXBasketEntrust() {}
	string m_in_user_token = { "" };
	string m_in_account_code = { "" };
	string m_in_asset_no = { "" };

	std::vector<COrderInfo> m_order_details = {};

	string m_in_market_no = { "" };
	string m_in_stock_code = { "" };
	string m_in_entrust_direction = { "" };

	// 申赎数量单位为篮子数。如最小申购单位是100万份，要申赎200万，该字段填2
	int m_in_entrust_amount = { 0 };
	int m_in_extsystem_id = { 0 };
	string m_in_third_reff = { "" };
	string m_in_mac_address = { "" };
	string m_in_ip_address = { "" };
	string m_in_hd_volserial = { "" };
	string m_in_op_station = { "" };
	string m_in_terminal_info = { "" };

	int m_out_entrust_no = { 0 };
	int m_out_batch_no = { 0 };
	int m_out_extsystem_id = { 0 };

	int CheckParamValid()
	{
		if (m_in_user_token == "") return RET_ERR;
		if (m_in_account_code == "") return RET_ERR;
		if (m_in_asset_no == "") return RET_ERR;
		if (m_order_details.size() == 0) return RET_ERR;
		return RET_OK;
	}

	int PackParam(IF2Packer *lpPacker)
	{
		if (NULL == lpPacker) return RET_ERR;
		lpPacker->BeginPack();
		lpPacker->AddField("user_token");
		lpPacker->AddField("account_code");
		lpPacker->AddField("asset_no");
		lpPacker->AddField("market_no");
		lpPacker->AddField("stock_code");
		lpPacker->AddField("entrust_direction");
		lpPacker->AddField("entrust_amount");
		lpPacker->AddField("extsystem_id");

		for (auto it = m_order_details.begin(); it != m_order_details.end(); it++)
		{
			lpPacker->AddStr(m_in_user_token.c_str());
			lpPacker->AddStr(m_in_account_code.c_str());
			lpPacker->AddStr(m_in_asset_no.c_str());
			lpPacker->AddStr(m_in_market_no.c_str());
			lpPacker->AddStr(it->stock_code.c_str());
			lpPacker->AddStr(it->entrust_direction.c_str());
			lpPacker->AddInt(it->entrust_amount);
			lpPacker->AddInt(it->extsystem_id);
		}
		lpPacker->EndPack();
		return RET_OK;
	};

	int UpackResult(IF2UnPacker *lpUnpacker)
	{
		return RET_OK;
	}

	//void LogParam();
	int GetFuncNo() { return UFX_BASKET_ENTRUST; };
};



//委托报单
class CUFXOrderInsert:public CUFXBaseInterfaceParam
{
public:
	CUFXOrderInsert() {}
	~CUFXOrderInsert() {}
	string m_in_user_token = { "" };
	string m_in_account_code = { "" };
	string m_in_asset_no = { "" };
	string m_in_combi_no = { "" };
	string m_in_market_no = { "" };
	string m_in_stock_code = { "" };
	string m_in_holder_acc = { "" };

	string m_in_seat = { "" };
	string m_in_direction = { "" };
	string m_in_price_type = { "" };
	double m_in_price = { 0.0 };
	int m_in_amount = { 0 };
	int m_in_batch_no = { 0 };
	int m_in_extsystem_id = { 0 };

	string m_in_third_reff = { "" };
	string m_in_mac_address = { "" };
	string m_in_ip_address = { "" };
	string m_in_hd_volserial = { "" };
	string m_in_op_station = { "" };
	string m_in_terminal_info = { "" };

	int m_out_entrust_no = { 0 };
	int m_out_batch_no = { 0 };
	int m_out_extsystem_id = { 0 };
	
	int CheckParamValid()
	{
		if (m_in_user_token == "") return RET_ERR;
		if (m_in_account_code == "") return RET_ERR;
		if (m_in_market_no == "") return RET_ERR;
		if (m_in_stock_code == "") return RET_ERR;
		if (m_in_direction == "") return RET_ERR;
		if (m_in_price_type == "") return RET_ERR;
		if (m_in_price == 0) return RET_ERR;
		if (m_in_amount == 0) return RET_ERR;
		return RET_OK;
	}
	int PackParam(IF2Packer *lpPacker)
	{
		if (NULL == lpPacker) return RET_ERR;
		lpPacker->BeginPack();
		lpPacker->AddField("user_token");
		lpPacker->AddField("account_code");		
		lpPacker->AddField("market_no");
		lpPacker->AddField("stock_code");
		lpPacker->AddField("entrust_direction");
		lpPacker->AddField("price_type");
		lpPacker->AddField("entrust_price");
		lpPacker->AddField("entrust_amount");

		if(m_in_asset_no!="")
			lpPacker->AddField("asset_no");
		if (m_in_batch_no != 0)
			lpPacker->AddField("batch_no");
		if(m_in_extsystem_id != 0)
			lpPacker->AddField("extsystem_id");
		if(m_in_combi_no!="")
			lpPacker->AddField("combi_no");
		if(m_in_holder_acc!="")		
			lpPacker->AddField("stockholder_id");
		if (m_in_seat != "")
		lpPacker->AddField("report_seat");
		if (m_in_third_reff != "")
			lpPacker->AddField("third_reff");		
		if (m_in_mac_address!="")
			lpPacker->AddField("mac_address");
		if (m_in_ip_address != "")
			lpPacker->AddField("ip_address");
		if (m_in_hd_volserial!="")
			lpPacker->AddField("hd_volserial");
		if (m_in_op_station != "")
			lpPacker->AddField("op_station");
		if (m_in_terminal_info != "")
			lpPacker->AddField("terminal_info");

		lpPacker->AddStr(m_in_user_token.c_str());
		lpPacker->AddStr(m_in_account_code.c_str());
		lpPacker->AddStr(m_in_market_no.c_str());
		lpPacker->AddStr(m_in_stock_code.c_str());
		lpPacker->AddStr(m_in_direction.c_str());
		lpPacker->AddStr(m_in_price_type.c_str());
		lpPacker->AddDouble(m_in_price);
		lpPacker->AddInt(m_in_amount);

		if (m_in_asset_no != "")
			lpPacker->AddStr(m_in_asset_no.c_str());
		if (m_in_batch_no != 0)
			lpPacker->AddInt(m_in_batch_no);
		if (m_in_extsystem_id != 0)
			lpPacker->AddInt(m_in_extsystem_id);
		if (m_in_combi_no != "")
			lpPacker->AddStr(m_in_combi_no.c_str());
		if (m_in_holder_acc != "")
			lpPacker->AddStr(m_in_holder_acc.c_str());
		if (m_in_seat != "")
			lpPacker->AddStr(m_in_seat.c_str());
		if (m_in_third_reff != "")
			lpPacker->AddStr(m_in_third_reff.c_str());
		if (m_in_mac_address != "")
			lpPacker->AddStr(m_in_mac_address.c_str());
		if (m_in_ip_address != "")
			lpPacker->AddStr(m_in_ip_address.c_str());
		if (m_in_hd_volserial != "")
			lpPacker->AddStr(m_in_hd_volserial.c_str());
		if (m_in_op_station != "")
			lpPacker->AddStr(m_in_op_station.c_str());
		if (m_in_terminal_info != "")
			lpPacker->AddStr(m_in_terminal_info.c_str());
		lpPacker->EndPack();
		return RET_OK;
	};

	int UpackResult(IF2UnPacker *lpUnpacker)
	{	
		if (NULL == lpUnpacker) return RET_ERR;

		int iErrorCode = UpackHeader(lpUnpacker);
		if (iErrorCode != 0)
		{	
			m_out_ErrorMsg = m_out_ErrorMsg + ":" + m_out_MsgDetail;
			return iErrorCode;
		}
		
		if (lpUnpacker->GetDatasetCount() > 1)
		{
			lpUnpacker->SetCurrentDatasetByIndex(1);
			lpUnpacker->First();
			m_out_ErrorCode = lpUnpacker->GetInt("entrust_fail_code");
			m_out_ErrorMsg = charToStr(lpUnpacker->GetStr("fail_cause"));

			m_out_entrust_no = lpUnpacker->GetInt("entrust_no");
			m_out_batch_no = lpUnpacker->GetInt("batch_no");
			m_out_extsystem_id = lpUnpacker->GetInt("extsystem_id");
		}
		
		return RET_OK;
	}

	//void LogParam();
	int GetFuncNo() { return UFX_ORD_ENTRUST; };
};

//委托撤单
class CUFXOrderCancel :public CUFXBaseInterfaceParam
{
public:
	CUFXOrderCancel() {}
	~CUFXOrderCancel() {}

	string m_in_user_token = { "" };
	string m_in_account_code = { "" };
	string m_in_combi_no = { "" };
	int m_in_entrust_no = { -1 };

	string m_in_mac_address = { "" };
	string m_in_ip_address = { "" };
	string m_in_hd_volserial = { "" };
	string m_in_op_station = { "" };
	string m_in_terminal_info = { "" };

	int m_out_entrust_no = { 0 };
	string m_out_market_no = { "" };
	string m_out_stock_code = { "" };
	
	void ClearEntrustInfo()
	{
		m_out_entrust_no=-1;
		m_out_market_no = "";
		m_out_stock_code = "";
	}
	int CheckParamValid()
	{
		if (m_in_user_token == "") return RET_ERR;
		if (m_in_account_code == "") return RET_ERR;
		if (m_in_entrust_no < 0) return RET_ERR;		
		return RET_OK; 
	};
	int PackParam(IF2Packer *lpPacker)
	{
		if (NULL == lpPacker) return RET_ERR;
		lpPacker->BeginPack();
		lpPacker->AddField("user_token");
		lpPacker->AddField("account_code");
		lpPacker->AddField("entrust_no");
		if(m_in_combi_no!="")
			lpPacker->AddField("combi_no");
		if (m_in_mac_address != "")
			lpPacker->AddField("mac_address");
		if (m_in_ip_address != "")
			lpPacker->AddField("ip_address");
		if (m_in_hd_volserial != "")
			lpPacker->AddField("hd_volserial");
		if (m_in_op_station != "")
			lpPacker->AddField("op_station");
		if (m_in_terminal_info != "")
			lpPacker->AddField("terminal_info");

		lpPacker->AddStr(m_in_user_token.c_str());
		lpPacker->AddStr(m_in_account_code.c_str());
		lpPacker->AddInt(m_in_entrust_no);

		if (m_in_combi_no != "")
			lpPacker->AddStr(m_in_combi_no.c_str());
		if (m_in_mac_address != "")
			lpPacker->AddStr(m_in_mac_address.c_str());
		if (m_in_ip_address != "")
			lpPacker->AddStr(m_in_ip_address.c_str());
		if (m_in_hd_volserial != "")
			lpPacker->AddStr(m_in_hd_volserial.c_str());
		if (m_in_op_station != "")
			lpPacker->AddStr(m_in_op_station.c_str());
		if (m_in_terminal_info != "")
			lpPacker->AddStr(m_in_terminal_info.c_str());
		lpPacker->EndPack();
		return RET_OK;
	};
	int UpackResult(IF2UnPacker *lpUnpacker)
	{
		if (NULL == lpUnpacker)
			return RET_ERR;
		int iErrorCode = UpackHeader(lpUnpacker);
		if (iErrorCode != 0)
		{
			m_out_ErrorMsg = m_out_ErrorMsg + " " + m_out_MsgDetail;
			return iErrorCode;
		}

		if (lpUnpacker->GetDatasetCount() > 1)
		{
			lpUnpacker->SetCurrentDatasetByIndex(1);
			try
			{
				m_out_ErrorCode = atol(lpUnpacker->GetStr("success_flag"));
				m_out_ErrorMsg = charToStr(lpUnpacker->GetStr("fail_cause"));

				m_out_entrust_no = lpUnpacker->GetInt("entrust_no");
				m_out_market_no = charToStr(lpUnpacker->GetStr("market_no"));
				m_out_stock_code = charToStr(lpUnpacker->GetStr("stock_code"));
			}
			catch (exception e)
			{
				m_out_ErrorMsg = e.what();
				return -2;
			}
		}
		return RET_OK;
	};
	//void LogParam() {};
	int GetFuncNo() { return UFX_CANCEL_ENTRUST; };
	//int GetTimeOut() { return DEFAULT_TIME_OUT * 10; };
};



//委托查询
class CUFXQryStockTrades :
	public CUFXBaseInterfaceParam
{
public:
	CUFXQryStockTrades() {}
	~CUFXQryStockTrades()
	{
	}
	string m_in_user_token = { "" };
	string m_in_account_code = { "" };
	string m_in_asset_no = { "" };
	int m_in_request_num = { 10000 };	// 请求数，TODO：待处理成交超过10000的情况
	
	int CheckParamValid()
	{
		if (m_in_user_token == "") return RET_ERR;
		if (m_in_account_code == "") return RET_ERR;
		if (m_in_asset_no == "") return RET_ERR;
		return RET_OK;
	}
	int PackParam(IF2Packer *lpPacker)
	{
		if (NULL == lpPacker) return RET_ERR;

		lpPacker->BeginPack();
		lpPacker->AddField("user_token");
		lpPacker->AddField("account_code");
		lpPacker->AddField("asset_no");
		lpPacker->AddField("request_num");
		lpPacker->AddStr(m_in_user_token.c_str());
		lpPacker->AddStr(m_in_account_code.c_str());
		lpPacker->AddStr(m_in_asset_no.c_str());
		lpPacker->AddInt(m_in_request_num);

		lpPacker->EndPack();
		return RET_OK;
	};

	int UpackResult(IF2UnPacker *lpUnpacker)
	{
		return RET_OK;
	}

	int GetFuncNo() { return UFX_QRY_STOCK_TRADES; };
};


