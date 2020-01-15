#pragma once
#ifndef UFX_DATA_STRUCT_H
#define UFX_DATA_STRUCT_H

#include <iostream>
#include <string>
using namespace std;

namespace ufx_data
{
// ί�е���Ϣ
struct OrderInfo
{
	// atsSymbol����ATS
	string atsSymbol;
	string symbol;
	string exchange;
	string name;   // ��Ʊ����

	string orderSysID;		// �������ı������
	string orderDeskID;	// ��̨�ı������
	string orderReqID;		// gateway�ı������
	string gOrderID;		// gatewayName.orderReqID��ȫ��Ψһ
							//int strategyID;     //���Եı��
	string strategyName;     //��������

							  // �������
	string direction;		// ����/����
	string offset;		// ����/ƽ��
	double price = { 0 };
	int totalVolume = { 0 };	// ����������
	int tradeVolume = { 0 };	// �ѳɽ�����
	double tradePrice = { 0 };	// �ɽ�����
	string status;		// ����״̬
	string statusMsg;	// ״̬��Ϣ
	string orderTime;
	string cancelTime;

	int frontID = { 0 };
	int sessionID = { 0 };
	bool external = { false };
	string gatewayName;
};

// �ɽ���Ϣ
struct TradeInfo
{
	// ���������
	string symbol;
	string atsSymbol;
	string exchange;

	string tradeID;
	string gTradeID;
	string gOrderID;
	int strategyID;     //���Եı��
	string strategyName;     //���Եı��

							  // �ɽ����
	string direction;
	string offset;
	double price;
	int volume;
	string tradeTime;
	string gatewayName;
	bool external = { false };
};

// �ֲ���Ϣ
// TODO: �����ڻ�������Ҫ�Ż�
struct PositionInfo
{
	string symbol;
	string name;			// ��Լ����
	string exchange;
	string atsSymbol;		// symbol.exchange
	string atsPositionName;	// ��λ���ƣ�atsSymbol.direction
	string accountId;      // �ʽ��˻�ID

	string direction;		// �ֲַ���
	string hedgeFlag;		// Ͷ�����ױ���������־
	int position{ 0 };		// �ֲܳ��������������ռ�ã�
	double price{ 0 };      // �ֲ־���
	int comboPosition{ 0 };	// ���ռ�óֲ�
	int available{ 0 };		// ���óֲ���
	int frozen{ 0 };		// �ܶ��ᣬ��������ͨ+��������(ƽ��)+�쳣����
	int openFrozen{ 0 };	// ���ֶ���

	int ydPosition{ 0 };	// ��ֲ���
	int ydFrozen{ 0 };
	int tdPosition{ 0 };	// ��ֲ�
	int tdFrozen{ 0 };

	double marketValue{ 0 };    // ֤ȯ��ֵ
	string time;				// ʱ��
	string gatewayName;
};

enum AccountType
{
	UNION = 0,	// �ۺ��˻�
	STOCK,		// ��Ʊ�˻�
	FUTURE,		// �ڻ��˻�
	ETF_OPTION,	// ��Ʊ��Ȩ�˻�
	CREDIT		// ������ȯ�����˻�
};

// �û��ʺ��ʽ��˻���Ϣ
struct AccountInfo
{
	string accountId;	// ����Ƕ���˻��ģ���: ���˻�.���˻�
	string gAccountId;	// gatewayName.accountID
	string gatewayName;
	string currency;
	AccountType accType;	// �˻�����
	double preBalance;	// �����˻����㾻ֵ
	double balance;		// �˻����ʽ�
	double available;	// �����ʽ�
	double commission;	// ����������
	double margin;		// ��֤��ռ��
	double closeProfit;	// ƽ��ӯ��
	double positionProfit;	// �ֲ�ӯ��
	double totalAsset;	// ���ʲ�
	double netAsset;	// ���ʲ�
	string time;		// ����ʱ��
};

	const string DF_OS_NEW = "DF_OS_NEW";		// �ѱ�
	const string DF_OS_ERROR = "DF_OS_ERROR";	// �ϵ�
	const string DF_OS_PART_TRADE = "DF_OS_PART_TRADE";	// ����
	const string DF_OS_ALLTRD = "DF_OS_ALLTRD";		// �ѳ�
	const string DF_OS_CANCEL = "DF_OS_CANCEL";		// �ѳ�
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
	const string DF_DIRECTION_ETFCREATE = "DF_DIRECTION_ETFCREATE";	// ETF�깺
	const string DF_DIRECTION_ETFREDEEM = "DF_DIRECTION_ETFREDEEM";	// ETF���

	const string DF_OFFSET_OPEN = "DF_OFFSET_OPEN";
	const string DF_OFFSET_CLOSE = "DF_OFFSET_CLOSE";

	const string DF_POSITION_LONG = "DF_POSITION_LONG";
	const string DF_POSITION_SHORT = "DF_POSITION_SHORT";
};



#endif // UFX_DATA_STRUCT_H