#include "ufx_base_interface.h"
//
//void ShowPacket(IF2UnPacker *lpUnPacker)
//{
//    int i = 0, t = 0, j = 0, k = 0;
//
//    for (i = 0; i < lpUnPacker->GetDatasetCount(); ++i)
//    {
//        // 设置当前结果集
//        lpUnPacker->SetCurrentDatasetByIndex(i);
//
//        printf("----------------[%d]-----------------\n",i);
//
//        // 打印所有记录
//        for (j = 0; j < (int)lpUnPacker->GetRowCount(); ++j)
//        {
//            // 打印每条记录
//            for (k = 0; k < lpUnPacker->GetColCount(); ++k)
//            {
//                printf("%20s = ", lpUnPacker->GetColName(k));
//                switch (lpUnPacker->GetColType(k))
//                {
//                case 'I':
//                    printf("%20d\n", lpUnPacker->GetIntByIndex(k));
//                    break;
//
//                case 'C':
//                    printf("%20c\n", lpUnPacker->GetCharByIndex(k));
//                    break;
//
//                case 'S':
//                    printf("%20s\n", lpUnPacker->GetStrByIndex(k));
//                    break;
//
//                case 'F':
//                    printf("%20f\n", lpUnPacker->GetDoubleByIndex(k));
//                    break;
//
//                case 'R':
//                    {
//                        int nLength = 0;
//                        void *lpData = lpUnPacker->GetRawByIndex(k, &nLength);
//
//                        // 对2进制数据进行处理
//                        break;
//                    }
//                default:
//                    // 未知数据类型
//                    printf("未知数据类型。\n");
//                    break;
//                }
//            }
//            lpUnPacker->Next();
//        }
//    }
//}

std::string charToStr(const char *str)
{
    if (NULL == str) return "";

    return str;
}

int CUFXBaseInterfaceParam::ClearInfo()
{
  m_out_ErrorCode = 0;
  m_out_ErrorMsg = "";
  m_out_MsgDetail = "";
  m_out_DataCount = 0;

  return 0;
}

int CUFXBaseInterfaceParam::UpackHeader( IF2UnPacker *lpUnpacker )
{
	lpUnpacker->SetCurrentDatasetByIndex(0);
	m_out_ErrorCode = lpUnpacker->GetInt("ErrorCode");
	m_out_ErrorMsg = charToStr(lpUnpacker->GetStr("ErrorMsg"));
	m_out_MsgDetail = charToStr(lpUnpacker->GetStr("MsgDetail"));
	m_out_DataCount = lpUnpacker->GetInt("DataCount");  

	return m_out_ErrorCode;
}


