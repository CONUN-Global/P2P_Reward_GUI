
// P2P_Reward_GUIDlg.h: 헤더 파일
//

#pragma once
#include "Common.h"
#include "WALLET_ADD.h"
#include "NETWORK_ADD.h"

#define THREAD_UPDATE ( WM_USER +1 )

using namespace rapidjson;

typedef struct ThreadParam
{
	CWnd* pWnd;
	BOOL* pStatus;
	CString JsonFileName;
	CString	m_DB_Path[5];

	ThreadParam()
	{
		pWnd = NULL;
		pStatus = FALSE;
		JsonFileName.Empty();
		
		for (int i = 0; i < eDB_Type::DB_END; i++)
			m_DB_Path[i].Empty();
	};
}THREADPARAM;

static CMutex g_mutex(FALSE, NULL);
static UINT JsonToDBThreadFunc(LPVOID pThreadParam);
static UINT JsonDBRemoveThreadFunc(LPVOID pThreadParam);
static UINT RewardThreadFunc(LPVOID pThreadParam);

static size_t WriteMemoryCallback(char* contents, size_t size, size_t nmemb, std::string* out)
{
	std::size_t total_size = size * nmemb;
	if (total_size)
	{
		out->append(contents, total_size);
		return total_size;
	}
	return 0;
}

// CP2PRewardGUIDlg 대화 상자
class CP2PRewardGUIDlg : public CDialog
{
// 생성입니다.
public:
	CP2PRewardGUIDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	virtual BOOL PreTranslateMessage(MSG* pMsg);
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_P2P_REWARD_GUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBtnWalletAdd();
	afx_msg void OnBnClickedBtnWalletRemove();
	afx_msg void OnBnClickedBtnRewardInfo();
	afx_msg void OnBnClickedBtnReward();
	afx_msg void OnBnClickedBtnAddNet();
	afx_msg void OnBnClickedBtnRemoveNet();
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnCbnSelchangeCbWalletAddr();
	afx_msg void OnCbnSelchangeCbWalletPrivatekey();

	afx_msg void OnClose();

public:
	//bool bRead_Reward_ResultInfo(CString FileName);
	void Insert_Reward_Info(CString FileName);
	void Remove_Reward_Info(CString _FileName);
	void LogDisplay(CString _Msg);

public:
	WALLET_ADD		m_pWalletDlg;
	NETWORK_ADD		m_pNetDlg;

	CRichEditCtrl	m_LogView;	

	CListCtrl		m_Reward_List;

	CComboBox		m_Wallet_Address_List;
	CComboBox		m_Wallet_Privatekey_List;
	CComboBox		m_NetName;
		
	CButton			m_Btn_Reward_Info;		
	CButton			m_Btn_UseAllCheck;
	CButton			m_Btn_Wallet_Add;
	CButton			m_Btn_Wallet_Remove;
	CButton			m_Btn_Reward;

	CEdit			m_SamrtContract;
	
	CString			m_DB_Path[eDB_Type::DB_END];

	Document		m_DocWallet, m_DocData, m_DocStringData;

	CppSQLite3DB	m_DB[eDB_Type::DB_END];
	CppSQLite3Query	m_Query[eDB_Type::DB_END];

	FILE* m_fp[eFile_Type::FILE_TYPE_END];

	long double m_nTotalRewardCount;
	__int64 nFileSize;	

	CString FileName, Reward_DB_Path, Full_Path, szQuery, szProjectID, szSubTaskID, szWalletAddr, szTimeUnit, szFileName, szRetMsg, str, File, strFileList, szItem,
		    szMasterWalletAddr, szMasterWalletPrivateKey;
	

	std::string stFileName;

	long double ldPrice;
	long long   llTime;	

	bool bUnCheckState;

	CString m_strContentType;
	CString m_strContentHeader;
	CString m_strURL;

	CString m_strParam, m_strParam2, m_strParam3, m_strParam4, m_strParam5, m_strParam6, m_strParam7, m_strParam8, m_strParam9, m_strParam10;

	CString SmartContract = "0x6b2e8703f7072461619ea15b5fd0bc85bdbf82af";
	CString APIKey = "4c2c015c79d06fadd3670ba8aa17f3187b5604ff";
	CString Network_Type = "ropsten";

	long double m_Balance = 0.0f;
	__int64 m_Gas_Price, m_Gas_Limit;
	bool init_Database();

private:
	BOOL m_bStatus;
	CWinThread* m_pThread;
	CURL* m_curl;
	CURLcode m_res;
	struct curl_slist* m_headerlist;
	CString m_strTargetURL;
	CString m_strResourceJSON;
	CString m_str_buffer;
	std::string m_Temp_Str_Buffer;

	afx_msg LRESULT OnThreadUpdate(WPARAM w, LPARAM l);

public:
	void myUpdateData(BOOL enable);	
	
	bool SetContentHeader(CString _APIKey);
	void SetUrl(CString _Url);

	long double TokenBalance(const CString _Param, const CString _Param2, const CString _Param3);
	__int64 GetGasFee(const CString _Param);
	__int64 GetGasLimit(const CString _Param, const CString _Param2, const CString _Param3);
	bool Token_Transfer(const CString Param /*Network Type*/, const CString Param2 /*From Address*/, const CString Param3 /*To Address*/,
		const CString Param4 /*contract address*/, const CString Param5 /*privateKey*/, const CString Param6 /*gas Price*/, const CString Param7 /*gas Limit*/, const long double Param8 /*token count*/,
		CString & _Out);
	inline void SetCurlHeaderList(const CString _Content_Type)
	{
		m_headerlist = curl_slist_append(m_headerlist, _Content_Type.GetString());
	};

	inline void AppendUrl(const CString _Url)
	{
		m_strTargetURL.Append(_Url.GetString());
	}
};
