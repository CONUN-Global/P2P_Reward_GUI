
// P2P_Reward_GUIDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "P2P_Reward_GUI.h"
#include "P2P_Reward_GUIDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CP2PRewardGUIDlg 대화 상자



CP2PRewardGUIDlg::CP2PRewardGUIDlg(CWnd* pParent /*=nullptr*/) : CDialog(IDD_P2P_REWARD_GUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);	
	m_pThread = NULL;
	m_bStatus = TRUE;
	nFileSize = 0;
}

void CP2PRewardGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_WALLET_ADDR, m_Wallet_Address_List);
	DDX_Control(pDX, IDC_CB_WALLET_PRIVATEKEY, m_Wallet_Privatekey_List);
	DDX_Control(pDX, IDC_LogView, m_LogView);
	DDX_Control(pDX, IDC_REWARD_LIST, m_Reward_List);
	DDX_Control(pDX, IDC_BTN_WALLET_REMOVE, m_Btn_Wallet_Remove);
	DDX_Control(pDX, IDC_BTN_REWARD_INFO, m_Btn_Reward_Info);
	DDX_Control(pDX, IDC_NETWORK_NAME, m_NetName);
	DDX_Control(pDX, IDC_EDT_SMARTCONTRACT, m_SamrtContract);
	DDX_Control(pDX, IDC_BTN_WALLET_ADD, m_Btn_Wallet_Add);
	DDX_Control(pDX, IDC_BTN_REWARD, m_Btn_Reward);
}

BEGIN_MESSAGE_MAP(CP2PRewardGUIDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_WALLET_ADD, &CP2PRewardGUIDlg::OnBnClickedBtnWalletAdd)
	ON_BN_CLICKED(IDC_BTN_WALLET_REMOVE, &CP2PRewardGUIDlg::OnBnClickedBtnWalletRemove)
	ON_BN_CLICKED(IDC_BTN_REWARD, &CP2PRewardGUIDlg::OnBnClickedBtnReward)

	ON_BN_CLICKED(IDC_BTN_REWARD_INFO, &CP2PRewardGUIDlg::OnBnClickedBtnRewardInfo)
	ON_BN_CLICKED(IDC_BTN_ADD_NET, &CP2PRewardGUIDlg::OnBnClickedBtnAddNet)
	ON_BN_CLICKED(IDC_BTN_REMOVE_NET, &CP2PRewardGUIDlg::OnBnClickedBtnRemoveNet)

	ON_MESSAGE(THREAD_UPDATE, OnThreadUpdate)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_REWARD_LIST, &CP2PRewardGUIDlg::OnItemchanged)

	ON_CBN_SELCHANGE(IDC_CB_WALLET_ADDR, &CP2PRewardGUIDlg::OnCbnSelchangeCbWalletAddr)
	ON_CBN_SELCHANGE(IDC_CB_WALLET_PRIVATEKEY, &CP2PRewardGUIDlg::OnCbnSelchangeCbWalletPrivatekey)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_NCDESTROY()	
END_MESSAGE_MAP()


// CP2PRewardGUIDlg 메시지 처리기

BOOL CP2PRewardGUIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
		
	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.	
	m_Reward_List.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

	CRect rect;
	m_Reward_List.GetClientRect(&rect);
	m_Reward_List.InsertColumn(0, "FileName", LVCFMT_CENTER, 825);
	//m_Reward_List.InsertColumn(1, "FileName", LVCFMT_LEFT, 1055);
	
	if (false == init_Database())
	{
		::SendMessage(GetSafeHwnd(), WM_CLOSE, NULL, NULL);
	}
	else
	{
		bUnCheckState = false;

		ldPrice = 0.0f;
		llTime = 0;

		nFileSize = 0;		
	}

	m_strURL.Format("https://api.cryptoapis.io/v1/bc/eth/");

	CString Msg("Reward Program Start!!\n");
	LogDisplay(Msg);

	m_Btn_Reward.EnableWindow(FALSE);
		
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.	
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CP2PRewardGUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CP2PRewardGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CP2PRewardGUIDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN) // ENTER키 눌릴 시
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE) // ESC키 눌릴 시
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CP2PRewardGUIDlg::OnBnClickedBtnWalletRemove()
{
	if (1 == m_Wallet_Address_List.GetCount())
	{
		//데이터가 1개 일 경우 삭제를 못하도록 한다.
		MessageBox("데이터가 1개일 경우에는 삭제할 수 없습니다.", "Warnning", MB_OK);
		return;
	}
	else
	{
		CString Wallet_Address, Wallet_Privatekey;
		int nIndex = m_Wallet_Address_List.GetCurSel();

		m_Wallet_Address_List.GetLBText(m_Wallet_Address_List.GetCurSel(), Wallet_Address);
		m_Wallet_Privatekey_List.GetLBText(m_Wallet_Privatekey_List.GetCurSel(), Wallet_Privatekey);


		CString szQuery;
		szQuery.Format("Delete From Wallet Where Wallet_Address = '%s' and Wallet_Privatekey = '%s';", Wallet_Address.GetString(), Wallet_Privatekey.GetString());

		try
		{
			m_DB[eDB_Type::DB_WALLET].execDML(szQuery.GetString());
			LogDisplay(szQuery.GetString());

			m_Wallet_Address_List.DeleteString(nIndex);
			m_Wallet_Privatekey_List.DeleteString(nIndex);

			m_Wallet_Address_List.SetCurSel(0);
			m_Wallet_Privatekey_List.SetCurSel(0);

			if (0 == m_Wallet_Address_List.GetCount())
			{
				m_Wallet_Address_List.EnableWindow(FALSE);
				m_Wallet_Privatekey_List.EnableWindow(FALSE);
				m_Btn_Reward.EnableWindow(FALSE);
				m_Btn_Reward_Info.EnableWindow(FALSE);
				m_Btn_Wallet_Remove.EnableWindow(FALSE);
			}
		}
		catch (CppSQLite3Exception& e)
		{
			CString szErrorMsg;
			szErrorMsg.Format("%s : %s", e.errorCodeAsString(e.errorCode()), e.errorMessage());
			LogDisplay(szErrorMsg.GetString());
		}
	}

}


void CP2PRewardGUIDlg::OnBnClickedBtnRewardInfo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	str = "JSON(*.json)|*.json|";	// 선택할 파일 종류

	CFileDialog m_FileDlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, str, this);

	const int c_cMaxFiles = 100 /*선택할 파일 숫자*/;	// 메모리 부족현상으로 확장 안해주면 몇개 못씀

	const int c_cbBuffSize = (c_cMaxFiles * (MAX_PATH + 1)) + 1;

	m_FileDlg.GetOFN().lpstrFile = strFileList.GetBuffer(c_cbBuffSize);

	m_FileDlg.GetOFN().nMaxFile = c_cbBuffSize;

	if (m_FileDlg.DoModal() == IDOK)
	{
		int i = 0;		

		for (POSITION pos = m_FileDlg.GetStartPosition(); pos != NULL;)
		{
			// 전체삭제는 ResetContent

			File = m_FileDlg.GetNextPathName(pos);

			UpdateData(TRUE);

			szItem = m_Reward_List.GetItemText(i, 1);
			
			if (0 != szItem.Compare(File))
			{
				m_Reward_List.InsertItem(i, szItem);
				
				m_Reward_List.SetItem(i, 0, LVIF_TEXT, File, NULL, NULL, NULL, NULL);
			}
			i += 1;
			UpdateData(FALSE);			
		}

	}
}

void CP2PRewardGUIDlg::LogDisplay(CString _Msg)
{
	CString strTemp;
	CTime cTime = CTime::GetCurrentTime();
	strTemp.Format("[%04d/%02d/%02d %02d:%02d:%02d] : %s\r\n", cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond(), _Msg.GetString());
	m_LogView.SetSel(-1, -1);
	m_LogView.ReplaceSel(strTemp);
	//UpdateData(FALSE);

}

void CP2PRewardGUIDlg::OnBnClickedBtnReward()
{
	THREADPARAM* pThreadParam = new THREADPARAM;
	pThreadParam->pWnd = this;
	pThreadParam->pStatus = &m_bStatus;

	for (int nLoop = 0; nLoop < eDB_Type::DB_END; nLoop++)
		pThreadParam->m_DB_Path[nLoop] = m_DB_Path[nLoop];

	m_pThread = AfxBeginThread(RewardThreadFunc, pThreadParam);
}

void CP2PRewardGUIDlg::OnBnClickedBtnAddNet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	MessageBox("This feature is currently being prepared.", "Alram", MB_OK);

	m_pNetDlg.DoModal();
}


void CP2PRewardGUIDlg::OnBnClickedBtnRemoveNet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	MessageBox("This feature is currently being prepared.", "Alram", MB_OK);
}

void CP2PRewardGUIDlg::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if(LVIF_STATE == pNMLV->uChanged )
	{
		if ( (pNMLV->uOldState & 0x1000) && (pNMLV->uNewState & 0x2000) )
		{
			m_Btn_Reward.EnableWindow(FALSE);
			//체크된 아이템
			Insert_Reward_Info(m_Reward_List.GetItemText(pNMLV->iItem, 0));
		}
		else if ((pNMLV->uOldState & 0x2000) && (pNMLV->uNewState & 0x1000))
		{
			m_Btn_Reward.EnableWindow(FALSE);
			//언체크된 아이템
			Remove_Reward_Info(m_Reward_List.GetItemText(pNMLV->iItem, 0));			
		}
	}
	
}

//bool CP2PRewardGUIDlg::bRead_Reward_ResultInfo(CString _RewardResultInfo_File_Name)
//{
//	// TODO: 여기에 구현 코드 추가.
//	bool bRet = true;
//	return bRet;
//}



void CP2PRewardGUIDlg::Insert_Reward_Info(CString _FileName)
{
	THREADPARAM* pThreadParam = new THREADPARAM;
	pThreadParam->pWnd = this;
	pThreadParam->pStatus = &m_bStatus;
	pThreadParam->JsonFileName = _FileName;
	
	for(int nLoop = 0; nLoop < eDB_Type::DB_END; nLoop++)
		pThreadParam->m_DB_Path[nLoop] = m_DB_Path[nLoop];

	m_pThread = AfxBeginThread(JsonToDBThreadFunc, pThreadParam);
}

void CP2PRewardGUIDlg::Remove_Reward_Info(CString _FileName)
{
	THREADPARAM* pThreadParam = new THREADPARAM;
	pThreadParam->pWnd = this;
	pThreadParam->pStatus = &m_bStatus;
	pThreadParam->JsonFileName = _FileName;

	for (int nLoop = 0; nLoop < eDB_Type::DB_END; nLoop++)
		pThreadParam->m_DB_Path[nLoop] = m_DB_Path[nLoop];

	m_pThread = AfxBeginThread(JsonDBRemoveThreadFunc, pThreadParam);
}


void CP2PRewardGUIDlg::OnBnClickedBtnWalletAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	m_pWalletDlg.DoModal();

}


void CP2PRewardGUIDlg::OnCbnSelchangeCbWalletAddr()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nAddressSel = m_Wallet_Address_List.GetCurSel();
	m_Wallet_Privatekey_List.SetCurSel(nAddressSel);
}


void CP2PRewardGUIDlg::OnCbnSelchangeCbWalletPrivatekey()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nPrivatekeySel = m_Wallet_Privatekey_List.GetCurSel();
	m_Wallet_Address_List.SetCurSel(nPrivatekeySel);
}

void CP2PRewardGUIDlg::OnClose()
{
	CDialog::OnClose();

	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.	
	m_bStatus = FALSE;

	if (NULL != m_pThread)
	{
		WaitForSingleObject(m_pThread->m_hThread, 5000);

		m_pThread = NULL;

		for (int nLoop = 0; nLoop < eDB_Type::DB_END; nLoop++)
		{
			try
			{
				if (NULL != &m_DB[nLoop])
					m_DB[nLoop].close();
				else
					continue;
			}
			catch (CppSQLite3Exception& e)
			{
				m_LogView.ReplaceSel(e.errorMessage());
			}
		}
	}
}

bool CP2PRewardGUIDlg::init_Database()
{
	bool bRet = true;

	// TODO: 여기에 구현 코드 추가.
		// Wallet DB
	TCHAR PathBuffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, PathBuffer);
	m_DB_Path[eDB_Type::DB_WALLET].Append(PathBuffer);
	m_DB_Path[eDB_Type::DB_WALLET].Append("\\Wallet.db");

	m_DB_Path[eDB_Type::DB_NETWORK].Append(PathBuffer);
	m_DB_Path[eDB_Type::DB_NETWORK].Append("\\Network.db");

	m_DB_Path[eDB_Type::DB_PROJECT].Append(PathBuffer);
	m_DB_Path[eDB_Type::DB_PROJECT].Append("\\Project.db");

	m_DB_Path[eDB_Type::DB_REWARD].Append(PathBuffer);
	m_DB_Path[eDB_Type::DB_REWARD].Append("\\Reward.db");

	m_DB_Path[eDB_Type::DB_RESULT].Append(PathBuffer);
	m_DB_Path[eDB_Type::DB_RESULT].Append("\\Result.db");

	for (int nLoop = 0; nLoop < eFile_Type::FILE_TYPE_END; nLoop++)
	{
		m_fp[nLoop] = NULL;
	}
		
	//Wallet.db 파일 존재 유무 체크
	m_DB[eDB_Type::DB_WALLET].open(m_DB_Path[eDB_Type::DB_WALLET].GetString());	
	if (false == m_DB[eDB_Type::DB_WALLET].tableExists("Wallet"))
	{
		//DB는 존재하지만 테이블이 없다.
		szQuery.Empty();
		szQuery.Append("CREATE TABLE Wallet(Wallet_Address TEXT NOT NULL CHECK(length(Wallet_Address) > 0 AND length(Wallet_Address) < 255) UNIQUE, \n\
						Wallet_Privatekey TEXT NOT NULL CHECK(length(Wallet_Privatekey) > 0 AND length(Wallet_Privatekey) < 4096), \n\
						PRIMARY KEY(Wallet_Address), \n\
						CONSTRAINT Wallet_Address CHECK(Wallet_Address like '0x%'));");

		try
		{
			m_DB[eDB_Type::DB_WALLET].execDML(szQuery.GetString());
			LogDisplay(szQuery);

			if (IDCANCEL == m_pWalletDlg.DoModal())
			{
				bRet = false;
			}
		}
		catch (CppSQLite3Exception& e)
		{
			CString szErrorMsg;
			szErrorMsg.Format("%s : %s", e.errorCodeAsString(e.errorCode()), e.errorMessage());
			LogDisplay(szErrorMsg.GetString());
		}
	}

	if (bRet == true)
	{
		try
		{
			szQuery.Empty();
			szQuery.Format("select Wallet_Address, Wallet_Privatekey from Wallet");

			m_Query[eDB_Type::DB_WALLET] = m_DB[eDB_Type::DB_WALLET].execQuery(szQuery);
			LogDisplay(szQuery);

			CString szAddress, szPrivatekey;
			szAddress = m_Query[eDB_Type::DB_WALLET].fieldValue(0);
			szPrivatekey = m_Query[eDB_Type::DB_WALLET].fieldValue(1);

			if (((false == m_Query[eDB_Type::DB_WALLET].fieldIsNull(0)) && false == szAddress.IsEmpty()) || ((false == m_Query[eDB_Type::DB_WALLET].fieldIsNull(0)) && false == szPrivatekey.IsEmpty()))
			{
				do
				{
					try
					{
						szAddress = m_Query[eDB_Type::DB_WALLET].fieldValue(0);
						szPrivatekey = m_Query[eDB_Type::DB_WALLET].fieldValue(1);

						m_Wallet_Address_List.AddString(szAddress);
						m_Wallet_Privatekey_List.AddString(szPrivatekey);

						m_Query[eDB_Type::DB_WALLET].nextRow();
					}
					catch (CppSQLite3Exception& e)
					{
						CString szErrorMsg;
						szErrorMsg.Format("%s : %s", e.errorCodeAsString(e.errorCode()), e.errorMessage());
						LogDisplay(szErrorMsg.GetString());
					}
				} while (!m_Query[eDB_Type::DB_WALLET].eof());

				m_Wallet_Address_List.SetCurSel(0);
				m_Wallet_Privatekey_List.SetCurSel(0);
			}
			else
			{
				m_Wallet_Address_List.SetCurSel(0);
				m_Wallet_Privatekey_List.SetCurSel(0);

				m_Wallet_Address_List.EnableWindow(FALSE);
				m_Wallet_Privatekey_List.EnableWindow(FALSE);

				m_Btn_Reward_Info.EnableWindow(FALSE);
				m_Btn_Wallet_Remove.EnableWindow(FALSE);
				m_Btn_Reward.EnableWindow(FALSE);
			}
		}
		catch (CppSQLite3Exception& e)
		{
			CString szErrorMsg;
			szErrorMsg.Format("%s : %s", e.errorCodeAsString(e.errorCode()), e.errorMessage());
			LogDisplay(szErrorMsg.GetString());
		}

		m_Query[eDB_Type::DB_WALLET].finalize();

		// Network DB 
		m_DB[eDB_Type::DB_NETWORK].open(m_DB_Path[eDB_Type::DB_NETWORK].GetString());
		if (false == m_DB[eDB_Type::DB_NETWORK].tableExists("Network"))
		{
			//DB는 존재하지만 테이블이 없다.
			szQuery.Empty();
			szQuery.Append("CREATE TABLE Network (Network_Name TEXT NOT NULL DEFAULT 'None',  \n\
						Network_URL	TEXT NOT NULL DEFAULT 'None', \n\
						Network_Port	INTEGER NOT NULL DEFAULT 0, \n\
						PRIMARY KEY(Network_URL));");

			try
			{
				m_DB[eDB_Type::DB_NETWORK].execDML(szQuery.GetString());
				LogDisplay(szQuery);

				szQuery.Empty();
				szQuery.Format("CREATE INDEX IX_Network_Name ON Network (Network_Name);");
				m_DB[eDB_Type::DB_NETWORK].execDML(szQuery.GetString());
				LogDisplay(szQuery);

				szQuery.Empty();
				szQuery.Format("insert or replace into Network(Network_Name, Network_URL, Network_Port) values ('%s', '%s', %d);", "Cryptoapis", "https://api.cryptoapis.io/v1/bc/eth/", 80);
				m_DB[eDB_Type::DB_NETWORK].execDML(szQuery.GetString());
				LogDisplay(szQuery);
			}
			catch (CppSQLite3Exception& e)
			{
				CString szErrorMsg;
				szErrorMsg.Format("%s : %s", e.errorCodeAsString(e.errorCode()), e.errorMessage());
				LogDisplay(szErrorMsg.GetString());
			}
		}

		// Project DB 
		m_DB[eDB_Type::DB_PROJECT].open(m_DB_Path[eDB_Type::DB_PROJECT].GetString());
		if (false == m_DB[eDB_Type::DB_PROJECT].tableExists("Project"))
		{
			//DB는 존재하지만 테이블이 없다.
			szQuery.Empty();
			szQuery.Append("CREATE TABLE Project ( ProjectID TEXT NOT NULL DEFAULT 'None', \n\
						Credit	REAL NOT NULL DEFAULT 0.0, \n\
						Task_Count	INTEGER NOT NULL, \n\
						Start_DateTime	TEXT NOT NULL, \n\
						End_DateTime	TEXT NOT NULL, \n\
						Spend_Time	INTEGER NOT NULL, \n\
						PRIMARY KEY(ProjectID));");

			try
			{
				int nRet = m_DB[eDB_Type::DB_PROJECT].execDML(szQuery.GetString());

				LogDisplay(szQuery);

				szQuery.Empty();
				szQuery.Format("CREATE INDEX IX_ProjectID ON Project (ProjectID);");
				LogDisplay(szQuery);

				if (nRet == 0)
				{
					m_DB[eDB_Type::DB_PROJECT].execDML(szQuery.GetString());
					LogDisplay(szQuery);
				}
			}
			catch (CppSQLite3Exception& e)
			{
				CString szErrorMsg;
				szErrorMsg.Format("%s : %s", e.errorCodeAsString(e.errorCode()), e.errorMessage());
				LogDisplay(szErrorMsg.GetString());
			}

		}

		// Reward DB 
		m_DB[eDB_Type::DB_REWARD].open(m_DB_Path[eDB_Type::DB_REWARD].GetString());

		if (false == m_DB[eDB_Type::DB_REWARD].tableExists("Reward"))
		{
			//DB는 존재하지만 테이블이 없다.
			try
			{
				szQuery.Empty();
				szQuery.Append("CREATE TABLE Reward (No INTEGER NOT NULL DEFAULT 0, \n\
								Project_ID	TEXT NOT NULL DEFAULT 'None', \n\
								SubTask_ID	TEXT NOT NULL DEFAULT 'None', \n\
								Result_FileName 	TEXT NOT NULL DEFAULT 'None', \n\
								Price	REAL NOT NULL DEFAULT 0.0, \n\
								Wallet_Address	TEXT NOT NULL CHECK(length(Wallet_Address) > 0 AND length(Wallet_Address) < 255), \n\
								Work_Time	REAL NOT NULL DEFAULT 0, \n\
								Time_Unit	TEXT NOT NULL CHECK(Time_Unit = 's' or Time_Unit = 'm' or Time_Unit = 'h'), \n\
								PRIMARY KEY(Project_ID, SubTask_ID, Wallet_Address), CONSTRAINT Wallet_Address CHECK(Wallet_Address like '0x%'));");

				m_DB[eDB_Type::DB_REWARD].execDML(szQuery.GetString());
				LogDisplay(szQuery);

				szQuery.Empty();
				szQuery.Format("CREATE INDEX IX_PROJECT_ID ON Reward (Project_ID);");
				m_DB[eDB_Type::DB_REWARD].execDML(szQuery);
				LogDisplay(szQuery);

				szQuery.Empty();
				szQuery.Format("CREATE INDEX IX_PROJECT_WITH_SUBTASK ON Reward (Project_ID, SubTask_ID);");
				m_DB[eDB_Type::DB_REWARD].execDML(szQuery);
				LogDisplay(szQuery);

				szQuery.Empty();
				szQuery.Format("CREATE INDEX IX_PROJECT_WITH_WALLET_ADDR ON Reward (Project_ID, Wallet_Address);");
				m_DB[eDB_Type::DB_REWARD].execDML(szQuery);
				LogDisplay(szQuery);
			}
			catch (CppSQLite3Exception& e)
			{
				CString szErrorMsg;
				szErrorMsg.Format("%s : %s", e.errorCodeAsString(e.errorCode()), e.errorMessage());
				LogDisplay(szErrorMsg.GetString());
			}
		}

		// Rank_Reward Table Exists
		if (false == m_DB[eDB_Type::DB_REWARD].tableExists("Rank_Reward"))
		{
			//DB는 존재하지만 테이블이 없다.
			// Rank_reward 테이블 생성으로 들어간다.
			try
			{
				m_DB[eDB_Type::DB_REWARD].execDML("CREATE TABLE Rank_Reward (Level INTEGER NOT NULL DEFAULT 0, \n\
				Project_ID	TEXT NOT NULL DEFAULT 'None', \n\
				SubTask_ID	TEXT NOT NULL DEFAULT 'None', \n\
				Wallet_Address	TEXT NOT NULL CHECK(length(Wallet_Address) > 0 AND length(Wallet_Address) < 255), \n\
				Work_Time	REAL NOT NULL DEFAULT 0, \n\
				Price	REAL NOT NULL DEFAULT 0.0, \n\
				PRIMARY KEY(Project_ID, SubTask_ID), CONSTRAINT Wallet_Address CHECK(Wallet_Address like '0x%'));");
			}
			catch (CppSQLite3Exception& e)
			{
				CString szErrorMsg;
				szErrorMsg.Format("%s : %s", e.errorCodeAsString(e.errorCode()), e.errorMessage());
				LogDisplay(szErrorMsg.GetString());
			}
		}

		// Result DB 
		m_DB[eDB_Type::DB_RESULT].open(m_DB_Path[eDB_Type::DB_RESULT].GetString());

		if (false == m_DB[eDB_Type::DB_RESULT].tableExists("Reward_Result"))
		{
			//DB는 존재하지만 테이블이 없다.
			szQuery.Empty();
			szQuery.Append("CREATE TABLE Reward_Result ( Project_ID	TEXT NOT NULL DEFAULT 'None', \n\
					To_Wallet_Addr	TEXT NOT NULL CHECK(length(To_Wallet_Addr) > 0 AND length(To_Wallet_Addr) < 255), \n\
					From_Wallet_Addr	TEXT NOT NULL CHECK(length(From_Wallet_Addr) > 0 AND length(From_Wallet_Addr) < 255), \n\
					Price	REAL NOT NULL DEFAULT 0.0, \n\
					Reward_DateTime	DATETIME NOT NULL DEFAULT '', \n\
					PRIMARY KEY(Project_ID, To_Wallet_Addr, From_Wallet_Addr), \n\
					CONSTRAINT From_Wallet_Addr CHECK(From_Wallet_Addr LIKE '0x%'), \n\
					CONSTRAINT To_Wallet_Addr CHECK(To_Wallet_Addr LIKE '0x%'));");

			try
			{
				m_DB[eDB_Type::DB_RESULT].execDML(szQuery.GetString());
				LogDisplay(szQuery);

				szQuery.Empty();
				szQuery.Format("CREATE INDEX IX_PROJECT_ID ON Result (Project_ID);");
				m_DB[eDB_Type::DB_RESULT].execDML(szQuery);
				LogDisplay(szQuery);

				szQuery.Empty();
				szQuery.Format("CREATE INDEX IX_PROJECT_WITH_TO_WALLET_ADDR ON Result (Project_ID, To_Wallet_Addr);");
				m_DB[eDB_Type::DB_RESULT].execDML(szQuery);
				LogDisplay(szQuery);

				szQuery.Empty();
				szQuery.Format("CREATE INDEX IX_PROJECT_WITH_FROM_WALLET_ADDR ON Result (Project_ID, From_Wallet_Addr);");
				m_DB[eDB_Type::DB_RESULT].execDML(szQuery);
				LogDisplay(szQuery);
			}
			catch (CppSQLite3Exception& e)
			{
				CString szErrorMsg;
				szErrorMsg.Format("%s : %s", e.errorCodeAsString(e.errorCode()), e.errorMessage());
				LogDisplay(szErrorMsg.GetString());
			}
		}
	}
	return bRet;
}

UINT JsonToDBThreadFunc(LPVOID pThreadParam)
{
	g_mutex.Lock();

	THREADPARAM* pParam = (THREADPARAM*)pThreadParam;

	CP2PRewardGUIDlg* pDlg = (CP2PRewardGUIDlg*)pParam->pWnd;
	BOOL* pStatus = pParam->pStatus;
	
	CString szDBFileName[eDB_Type::DB_END];

	for (int nCount = 0; nCount < 5; nCount++)
		szDBFileName[nCount] = pParam->m_DB_Path[nCount];

	CString szJsonFileName = pParam->JsonFileName;

	delete pParam;

	StReward_Info _reward_Info;
	StReward	  m_Reward;

	// TODO: 여기에 구현 코드 추가.
	std::multimap<CString, struct StReward_Info> m_Reward_Info;
	std::multimap<CString, struct StReward_Info>::iterator it;
	std::multimap<CString, struct StReward_Info>::iterator it2;

	std::map<CString, std::multimap<CString, struct StReward_Info>> ResultInfo_Map;
	std::map<CString, std::multimap<CString, struct StReward_Info>>::iterator TempIT;
	
	DWORD dwFileSize = 0;

	CFileFind	pFind;
	CFileException pFileException;

	int nLoop = 0;
	__int64 Work_Time = 0;
	int TempTaskCount = 0;
	double TempdCredit = 0.0f;

	if (TRUE == pFind.FindFile(szJsonFileName))
	{
		char* Temp = nullptr;

		errno_t err = fopen_s(&pDlg->m_fp[eFile_Type::FILE_TYPE_REWARD], szJsonFileName.GetString(), "r");

		err = _fseeki64(pDlg->m_fp[eFile_Type::FILE_TYPE_REWARD], 0, SEEK_END);

		dwFileSize = ftell(pDlg->m_fp[eFile_Type::FILE_TYPE_REWARD]);

		err = _fseeki64(pDlg->m_fp[eFile_Type::FILE_TYPE_REWARD], 0, SEEK_SET);

		if (0 != dwFileSize)
		{
			try
			{
				Temp = new char [static_cast<size_t>(dwFileSize)];

				if (NULL != Temp)
				{
					uint64_t	tot_num = 0;
					uint64_t	cur_num = 0;

					while ((cur_num = fread(&Temp[tot_num], sizeof(char), static_cast<size_t>(dwFileSize - tot_num), pDlg->m_fp[eFile_Type::FILE_TYPE_REWARD])) > 0)
					{
						
						tot_num += cur_num;
					}

					if (tot_num != dwFileSize)
					{
						pDlg->LogDisplay("not Read JSON File All Contents");
					}
					else
					{
						err = fclose(pDlg->m_fp[eFile_Type::FILE_TYPE_REWARD]);
					}

				}
			}
			catch (std::bad_alloc)
			{
				pDlg->LogDisplay("Memory Allocation Fail");
			}

			pDlg->m_DocData.Parse((const char*)Temp, static_cast<size_t>(dwFileSize));
			std::string Project_State;
			Project_State.assign(pDlg->m_DocData["proj_flag"].GetString());

			if ("DONE" == Project_State)
			{
				m_Reward.Project_Flag.Format("%s", pDlg->m_DocData["proj_flag"].GetString());

				try
				{
					const Value& Task_Info = pDlg->m_DocData["task_info"];
					const Value& NameList = Task_Info["tasknamelist"];

					if (NameList.IsArray())
					{
						_reward_Info.Project_ID.Format("%s", pDlg->m_DocData["proj_ID"].GetString());

						for (SizeType i = 0; i < NameList.Size(); i++)
						{
							//if (true == NameList[i]["value"].IsString())
							//{
							//	// 해당 금액이 스트링이라면
							//	std::string TempPrice = NameList[i]["value"].GetString();
							//	_reward_Info.Price = static_cast<float>(atof(TempPrice.c_str()));
							//}
							//else if (true == NameList[i]["value"].IsFloat() || true == NameList[i]["value"].IsLosslessFloat())
							//{
							//	_reward_Info.Price = NameList[i]["value"].GetFloat();
							//}
							//else if (true == NameList[i]["value"].IsDouble() || true == NameList[i]["value"].IsLosslessDouble())
							//{
							//	_reward_Info.Price = NameList[i]["value"].GetDouble();
							//}
							//else if (true == NameList[i]["value"].IsInt64() || true == NameList[i]["value"].IsUint64())
							//{
							//	_reward_Info.Price = static_cast<float>(NameList[i]["value"].GetInt64());
							//}
							//else if (true == NameList[i]["value"].IsInt() || true == NameList[i]["value"].IsUint())
							//{
							//	_reward_Info.Price = static_cast<float>(NameList[i]["value"].GetInt());
							//}

							_reward_Info.Work_Time = NameList[i]["work_time"].GetInt64();

							if (_reward_Info.Work_Time != 0)
							{
								TempTaskCount += 1;

								Work_Time += NameList[i]["work_time"].GetInt64();

								_reward_Info.Time_Unit.Format("%s", NameList[i]["time_unit"].GetString());

								//m_nTotalRewardCount += _nTotalRewardCount += _reward_Info.Price;

								CString TempWallet = NameList[i]["wallet_addr"].GetString();
								if (-1 == TempWallet.Find("0x", 0))
								{
									_reward_Info.Wallet_addr.Format("%s%s", "0x", NameList[i]["wallet_addr"].GetString());
								}
								else
								{
									_reward_Info.Wallet_addr.Format("%s", NameList[i]["wallet_addr"].GetString());
								}

								_reward_Info.SubTask_ID.Format("%s", NameList[i]["sub_task_id"].GetString());

								m_Reward.Reward_List.insert(std::pair<CString, struct StReward_Info>(_reward_Info.Wallet_addr, _reward_Info));
							}
						}
					}
				}
				catch (CppSQLite3Exception& e)
				{
					CString szErrorMsg;
					szErrorMsg.Format("%s : %s", e.errorCodeAsString(e.errorCode()), e.errorMessage());
					pDlg->LogDisplay(szErrorMsg.GetString());
				}
			}
			else
			{
				pDlg->LogDisplay("resultInfo.json File is Not Found DONE State");
			}

			TCHAR FName[_MAX_FNAME];
			TCHAR FExt[_MAX_EXT];

			_splitpath_s(szJsonFileName, NULL, NULL, NULL, NULL, FName, _MAX_FNAME, FExt, _MAX_EXT);
			pDlg->szFileName.Format("%s%s", FName, FExt);
			ResultInfo_Map.insert(std::pair<CString, std::multimap<CString, struct StReward_Info>>(pDlg->szFileName.GetString(), m_Reward.Reward_List));

			pDlg->szQuery.Empty();

			CString TempProjID = pDlg->m_DocData["proj_ID"].GetString();
			TempdCredit = pDlg->m_DocData["credit"].GetDouble();
			//int TempTaskCount = pDlg->m_DocData["total_task_num"].GetInt();
			CString TempCreStDate = pDlg->m_DocData["task_info"]["task_cre_date"].GetString();
			CString TempCreEndDate;
			CString TempTotalWorkTime;
			TempTotalWorkTime.Format("%I64d", Work_Time);
			
			pDlg->szQuery.Empty();
			pDlg->szQuery.Format("select '%s', (datetime('%s', '+%s seconds'));", TempCreStDate.GetString(), TempCreStDate.GetString(), TempTotalWorkTime.GetString());
			pDlg->m_Query[eDB_Type::DB_PROJECT] = pDlg->m_DB[eDB_Type::DB_PROJECT].execQuery(pDlg->szQuery);

			pDlg->LogDisplay(pDlg->szQuery.GetString());

			TempCreStDate = pDlg->m_Query[eDB_Type::DB_PROJECT].fieldValue(0);
			TempCreEndDate = pDlg->m_Query[eDB_Type::DB_PROJECT].fieldValue(1);
			
			pDlg->m_Query[eDB_Type::DB_PROJECT].finalize();

			CString Spend_Time = "0";

			pDlg->szQuery.Format("select strftime(%s,'%s') - strftime(%s,'%s')", "'%s'", TempCreEndDate.GetString(), "'%s'", TempCreStDate.GetString());
			pDlg->m_Query[eDB_Type::DB_PROJECT] = pDlg->m_DB[eDB_Type::DB_PROJECT].execQuery(pDlg->szQuery);

			pDlg->LogDisplay(pDlg->szQuery.GetString());

			Spend_Time = pDlg->m_Query[eDB_Type::DB_PROJECT].fieldValue(0);

			pDlg->m_Query[eDB_Type::DB_PROJECT].finalize();

			pDlg->szQuery.Empty();
			pDlg->szQuery.Format("insert or Replace into Project(ProjectID, Credit, Task_Count, Start_DateTime, End_DateTime, Spend_Time) values ('%s', %lf, %d, '%s', '%s', %d);\n\n",
				TempProjID.GetString(), TempdCredit, TempTaskCount, TempCreStDate.GetString(), TempCreEndDate.GetString(), atoi(Spend_Time.GetString()));

			pDlg->m_DB[eDB_Type::DB_PROJECT].execDML(pDlg->szQuery);
			
			pDlg->LogDisplay(pDlg->szQuery.GetString());

			m_Reward.Reward_List.clear();
		}

		if (NULL != Temp)
		{
			delete [] Temp;
			Temp = nullptr;
		}
	}	
	dwFileSize = 0;

	//파일이 없다.
	if (false == pDlg->m_DB[eDB_Type::DB_REWARD].tableExists("Reward"))
	{
		// Wallet.db 파일 생성으로 들어간다.
		try
		{
			pDlg->m_DB[eDB_Type::DB_REWARD].execDML("CREATE TABLE Reward (No INTEGER NOT NULL DEFAULT 0, \n\
					Project_ID	TEXT NOT NULL DEFAULT 'None', \n\
					SubTask_ID	TEXT NOT NULL DEFAULT 'None', \n\
					Result_FileName 	TEXT NOT NULL DEFAULT 'None', \n\
					Price	REAL NOT NULL DEFAULT 0.0, \n\
					Wallet_Address	TEXT NOT NULL CHECK(length(Wallet_Address) > 0 AND length(Wallet_Address) < 255), \n\
					Work_Time	REAL NOT NULL DEFAULT 0, \n\
					Time_Unit	TEXT NOT NULL CHECK(Time_Unit = 's' or Time_Unit = 'm' or Time_Unit = 'h'), \n\
					PRIMARY KEY(Project_ID, SubTask_ID, Wallet_Address), CONSTRAINT Wallet_Address CHECK(Wallet_Address like '0x%'));");

			pDlg->m_DB[eDB_Type::DB_REWARD].execDML("CREATE INDEX IX_PROJECT_ID ON Reward (Project_ID);");
			pDlg->m_DB[eDB_Type::DB_REWARD].execDML("CREATE INDEX IX_PROJECT_WITH_SUBTASK ON Reward (Project_ID, SubTask_ID);");
			pDlg->m_DB[eDB_Type::DB_REWARD].execDML("CREATE INDEX IX_PROJECT_WITH_WALLET_ADDR ON Reward (Project_ID, Wallet_Address);");
		}
		catch (CppSQLite3Exception& e)
		{
			CString szErrorMsg;
			szErrorMsg.Format("%s : %s", e.errorCodeAsString(e.errorCode()), e.errorMessage());
			pDlg->LogDisplay(szErrorMsg.GetString());
		}
	}

	if (false == pDlg->m_DB[eDB_Type::DB_REWARD].tableExists("Rank_Reward"))
	{
		// Wallet.db 파일 생성으로 들어간다.
		try
		{
			pDlg->m_DB[eDB_Type::DB_REWARD].execDML("CREATE TABLE Rank_Reward (Level INTEGER NOT NULL DEFAULT 0, \n\
					Project_ID	TEXT NOT NULL DEFAULT 'None', \n\
					SubTask_ID	TEXT NOT NULL DEFAULT 'None', \n\
					Wallet_Address	TEXT NOT NULL CHECK(length(Wallet_Address) > 0 AND length(Wallet_Address) < 255), \n\
					Work_Time	REAL NOT NULL DEFAULT 0, \n\
					Price	NUMERIC NOT NULL DEFAULT 0.0, \n\
					PRIMARY KEY(Project_ID, SubTask_ID), CONSTRAINT Wallet_Address CHECK(Wallet_Address like '0x%'));");
		}
		catch (CppSQLite3Exception& e)
		{
			CString szErrorMsg;
			szErrorMsg.Format("%s : %s", e.errorCodeAsString(e.errorCode()), e.errorMessage());
			pDlg->LogDisplay(szErrorMsg.GetString());
		}
	}

	TCHAR FName[_MAX_FNAME];
	TCHAR FExt[_MAX_EXT];

	_splitpath_s(szJsonFileName, NULL, NULL, NULL, NULL, FName, _MAX_FNAME, FExt, _MAX_EXT);

	//szFileName.Format("%s%s", FName, FExt);

	for (TempIT = ResultInfo_Map.begin();  TempIT != ResultInfo_Map.end(); ++TempIT)
	{
		if (TempIT->first == pDlg->szFileName.GetString())
		{
			it = TempIT->second.begin();			

			CString szProject_ID, szSubTask_ID;

			for (it = TempIT->second.begin(); it != TempIT->second.end(); ++it)
			{
				try
				{
					szProject_ID = it->second.Project_ID.GetString();
					szSubTask_ID = it->second.SubTask_ID.GetString();

					pDlg->szQuery.Empty();
					pDlg->szQuery.Format("insert or replace into Reward(No, Project_ID, SubTask_ID, Result_FileName, Price, Wallet_Address, Work_Time, Time_Unit) values (%d, '%s', '%s', '%s%s', %lf, '%s', %I64d, '%s')", //%lf, '%s', %I64d, '%s');",
						nLoop + 1, it->second.Project_ID.GetString(), it->second.SubTask_ID.GetString(), FName, FExt, it->second.Price, it->second.Wallet_addr.GetString(), it->second.Work_Time, it->second.Time_Unit.GetString());
					pDlg->m_DB[eDB_Type::DB_REWARD].execDML(pDlg->szQuery.GetString());
					pDlg->LogDisplay(pDlg->szQuery.GetString());

				}
				catch (CppSQLite3Exception& e)
				{
					CString szErrorMsg;
					szErrorMsg.Format("%s : %s", e.errorCodeAsString(e.errorCode()), e.errorMessage());
					pDlg->LogDisplay(szErrorMsg.GetString());
				}

				nLoop += 1;
			}

			pDlg->m_Query[eDB_Type::DB_REWARD].finalize();
			pDlg->szQuery.Empty();
			pDlg->szQuery.Format("delete From Rank_Reward Where Project_ID = '%s';", szProject_ID.GetString());

			pDlg->m_DB[eDB_Type::DB_REWARD].execDML(pDlg->szQuery);
			pDlg->LogDisplay(pDlg->szQuery.GetString());

			pDlg->m_Query[eDB_Type::DB_REWARD].finalize();
			pDlg->szQuery.Empty();
			pDlg->szQuery.Format("insert or replace into Rank_Reward(Level, Project_ID, SubTask_ID, Wallet_Address, Work_Time, Price) SELECT per_rank*100 AS Level, Project_ID, SubTask_ID, Wallet_Address, Work_Time, 0 AS Price FROM (SELECT PERCENT_RANK() OVER (ORDER BY Work_Time ASC) as per_rank, Project_ID, SubTask_ID, Wallet_Address, Work_Time FROM Reward Where Project_ID = '%s') a WHERE Project_ID = '%s';",
				szProject_ID.GetString(), szProject_ID.GetString());

			pDlg->m_DB[eDB_Type::DB_REWARD].execDML(pDlg->szQuery);
			pDlg->LogDisplay(pDlg->szQuery.GetString());

			// Rank_Reward 테이블에서 프로젝트 ID값으로 지급을 받을 총 유저수를 구한다.
			pDlg->m_Query[eDB_Type::DB_REWARD].finalize();
			pDlg->szQuery.Empty();
			pDlg->szQuery.Format("select count(SubTask_ID) From Rank_Reward Where Project_ID = %s", szProject_ID.GetString());

			pDlg->m_Query[eDB_Type::DB_REWARD] = pDlg->m_DB[eDB_Type::DB_REWARD].execQuery(pDlg->szQuery);
			pDlg->LogDisplay(pDlg->szQuery.GetString());

			int nUserCount = 0;

			while (!pDlg->m_Query[eDB_Type::DB_REWARD].eof())
			{
				nUserCount = atoi(pDlg->m_Query[eDB_Type::DB_REWARD].fieldValue(0));

				pDlg->m_Query[eDB_Type::DB_REWARD].nextRow();
			}

			// Rank_Reward 테이블에서 프로젝트 ID값으로 상위 10%안에 들어간 유저들의 수를 구한다.
			pDlg->m_Query[eDB_Type::DB_REWARD].finalize();
			pDlg->szQuery.Empty();
			pDlg->szQuery.Format("select count(*), * From Rank_Reward Where Project_ID = %s and Level <= 10", szProject_ID.GetString());

			pDlg->m_Query[eDB_Type::DB_REWARD] = pDlg->m_DB[eDB_Type::DB_REWARD].execQuery(pDlg->szQuery);
			pDlg->LogDisplay(pDlg->szQuery.GetString());

			int nTopUserCount = 0;

			while (!pDlg->m_Query[eDB_Type::DB_REWARD].eof())
			{
				nTopUserCount = atoi(pDlg->m_Query[eDB_Type::DB_REWARD].fieldValue(0));

				pDlg->m_Query[eDB_Type::DB_REWARD].nextRow();
			}

			// 상위 
			//크래팃을 유저수로 나눠본다.
			double DivCredit = TempdCredit / nUserCount;			
			double Top10_DivCredit =DivCredit + ((DivCredit + 125)/100);
			int nRemain_User = nUserCount - nTopUserCount;
			double OtherDivCredit = (TempdCredit - (Top10_DivCredit * nTopUserCount)) / nRemain_User;
			
			pDlg->szQuery.Empty();
			pDlg->szQuery.Format("Total User = [%d], Total_Credit = [%lf], Top 10 User = [%d], Top10 UserReward Credit = [%lf], Etc User = [%d], Remain User Reward Credit = [%lf] ", 
				nUserCount, TempdCredit, nTopUserCount, Top10_DivCredit, nRemain_User,  OtherDivCredit);
			pDlg->LogDisplay(pDlg->szQuery.GetString());

			// Price 값을 업데이트 시키자
			// 우선 top 10%의 유저들에게 지급할 코인양 업데이트
			pDlg->szQuery.Empty();
			pDlg->szQuery.Format("update Rank_Reward Set Price = %lf Where Project_ID = %s and Level <= 10", Top10_DivCredit, szProject_ID.GetString());
			pDlg->m_DB[eDB_Type::DB_REWARD].execDML(pDlg->szQuery);
			pDlg->LogDisplay(pDlg->szQuery.GetString());

			// 나머지 유저들에게 지급할 코인양 업데이트
			pDlg->szQuery.Empty();
			pDlg->szQuery.Format("update Rank_Reward Set Price = %lf Where Project_ID = %s and Level > 10", OtherDivCredit, szProject_ID.GetString());
			pDlg->m_DB[eDB_Type::DB_REWARD].execDML(pDlg->szQuery);
			pDlg->LogDisplay(pDlg->szQuery.GetString());
		}
	}

	pDlg->m_Btn_Reward.EnableWindow(TRUE);

	pDlg->LogDisplay("Json Info Save DB Success");

	g_mutex.Unlock();
	return 0;
}

UINT JsonDBRemoveThreadFunc(LPVOID pThreadParam)
{
	g_mutex.Lock();

	THREADPARAM* pParam = (THREADPARAM*)pThreadParam;

	CP2PRewardGUIDlg* pDlg = (CP2PRewardGUIDlg*)pParam->pWnd;
	BOOL* pStatus = pParam->pStatus;
	CString szJsonFileName = pParam->JsonFileName;

	delete pParam;
		
	// TODO: 여기에 구현 코드 추가.
	GetCurrentDirectory(MAX_PATH, pDlg->Reward_DB_Path.GetBuffer());
	pDlg->Full_Path.AppendFormat("%s%s%s", pDlg->Reward_DB_Path.GetString(), "\\", Reward_DB_Name);

	TCHAR FName[_MAX_FNAME];
	TCHAR FExt[_MAX_EXT];

	_splitpath_s(szJsonFileName, NULL, NULL, NULL, NULL, FName, _MAX_FNAME, FExt, _MAX_EXT);
	szJsonFileName.Format("%s%s", FName, FExt);

	CString Project_ID;
	Project_ID.Empty();

	try
	{
		//파일이 없다.
		if (true == pDlg->m_DB[eDB_Type::DB_REWARD].tableExists("Reward"))
		{
			try
			{
				pDlg->m_DB[eDB_Type::DB_REWARD].setBusyTimeout(1000);
				pDlg->szQuery.Empty();
				pDlg->szQuery.Format("select Project_ID From Reward where Result_FileName = '%s' LIMIT 1;\n\n", szJsonFileName.GetString());
				pDlg->m_Query[eDB_Type::DB_REWARD] = pDlg->m_DB[eDB_Type::DB_REWARD].execQuery(pDlg->szQuery.GetString());
				Project_ID = pDlg->m_Query[eDB_Type::DB_REWARD].fieldValue(0);
				pDlg->m_Query[eDB_Type::DB_REWARD].finalize();
				pDlg->LogDisplay(pDlg->szQuery.GetString());

				pDlg->szQuery.Empty();
				pDlg->szQuery.Format("delete From Reward Where Result_FileName = '%s';\n\n", Project_ID.GetString());
				pDlg->m_DB[eDB_Type::DB_REWARD].execDML(pDlg->szQuery.GetString());
				pDlg->LogDisplay(pDlg->szQuery.GetString());
			}
			catch (CppSQLite3Exception& e)
			{
				CString szErrorMsg;
				szErrorMsg.Format("%s : %s", e.errorCodeAsString(e.errorCode()), e.errorMessage());
				pDlg->LogDisplay(szErrorMsg.GetString());
			}
		}

		if (true == pDlg->m_DB[eDB_Type::DB_PROJECT].tableExists("Project"))
		{
			try
			{
				pDlg->m_DB[eDB_Type::DB_PROJECT].setBusyTimeout(1000);
				pDlg->szQuery.Empty();
				pDlg->szQuery.Format("delete From Project Where ProjectID = '%s';\n\n", Project_ID.GetString());

				pDlg->m_DB[eDB_Type::DB_PROJECT].execDML(pDlg->szQuery.GetString());

				pDlg->LogDisplay(pDlg->szQuery.GetString());
			}
			catch (CppSQLite3Exception& e)
			{
				CString szErrorMsg;
				szErrorMsg.Format("%s : %s", e.errorCodeAsString(e.errorCode()), e.errorMessage());
				pDlg->LogDisplay(szErrorMsg.GetString());
			}
		}
	}
	catch (CppSQLite3Exception& e)
	{
		CString szErrorMsg;
		szErrorMsg.Format("%s : %s", e.errorCodeAsString(e.errorCode()), e.errorMessage());
		pDlg->LogDisplay(szErrorMsg.GetString());
	}

	int nLoop = 0;
	for (int i = 0; i < pDlg->m_Reward_List.GetItemCount(); i++)
	{
		UINT state = pDlg->m_Reward_List.GetItemState(i, LVIS_STATEIMAGEMASK);
		BOOL bChecked = (BOOL)(((state & LVIS_STATEIMAGEMASK) >> 12) - 1);
		bChecked = pDlg->m_Reward_List.GetCheck(i);

		if (bChecked == TRUE)
		{
			nLoop += 1;
		}
	}

	if (nLoop < pDlg->m_Reward_List.GetItemCount())
	{
		pDlg->m_Btn_Reward.EnableWindow(TRUE);
	}
	else
	{
		pDlg->m_Btn_Reward.EnableWindow(FALSE);
	}	

	g_mutex.Unlock();

	return 0;
}

UINT RewardThreadFunc(LPVOID pThreadParam)
{
	g_mutex.Lock();

	bool bRet = false;

	THREADPARAM* pParam = (THREADPARAM*)pThreadParam;
	CP2PRewardGUIDlg* pDlg = (CP2PRewardGUIDlg*)pParam->pWnd;

	delete pParam;

	// 보상 지급을 시작한다.
// 1. MasterWallet address 와 MasterWallet PrivateKey를 가져온다.         
	pDlg->m_Wallet_Address_List.GetLBText(pDlg->m_Wallet_Address_List.GetCurSel(), pDlg->szMasterWalletAddr);
	pDlg->m_Wallet_Privatekey_List.GetLBText(pDlg->m_Wallet_Privatekey_List.GetCurSel(), pDlg->szMasterWalletPrivateKey);


	// 2. 현재 마스터의 잔액을 불러온다.
	pDlg->m_Balance = pDlg->TokenBalance(pDlg->Network_Type, pDlg->szMasterWalletAddr, pDlg->SmartContract);

	// 1. 우선 체크된 리스트컬럼의 FileName.json 리스트를 가져온다.
	std::map<int, CString>	RewardMap;
	std::map<int, CString>::iterator	RewardMap_IT;

	for (int i = 0; i < pDlg->m_Reward_List.GetItemCount(); i++)
	{
		UINT state = pDlg->m_Reward_List.GetItemState(i, LVIS_STATEIMAGEMASK);
		BOOL bChecked = (BOOL)(((state & LVIS_STATEIMAGEMASK) >> 12) - 1);
		bChecked = pDlg->m_Reward_List.GetCheck(i);

		if (bChecked == TRUE)
		{
			CString FilePath = pDlg->m_Reward_List.GetItemText(i, 0);

			TCHAR FName[_MAX_FNAME];
			TCHAR FExt[_MAX_EXT];

			_splitpath_s(FilePath, NULL, NULL, NULL, NULL, FName, _MAX_FNAME, FExt, _MAX_EXT);
			FilePath.Empty();
			FilePath.Format("%s%s", FName, FExt);

			RewardMap.insert(std::pair<int, CString>(i, FilePath.GetString()));
			pDlg->LogDisplay(FilePath);
		}
	}

	// 보상 계산 하기
	for (RewardMap_IT = RewardMap.begin(); RewardMap_IT != RewardMap.end(); RewardMap_IT++)
	{
		CString Msg;
		Msg.Format("%d : %s", RewardMap_IT->first, RewardMap_IT->second.GetString());
		pDlg->LogDisplay(Msg);

		// Project DB에서 Project_ID값을 추출한다.
		pDlg->szQuery.Empty();
		pDlg->szQuery.Format("select Project_ID From Reward Where Result_FileName = '%s' LIMIT 1;", RewardMap_IT->second.GetString());

		pDlg->m_Query[eDB_Type::DB_REWARD] = pDlg->m_DB[eDB_Type::DB_REWARD].execQuery(pDlg->szQuery);
		pDlg->LogDisplay(pDlg->szQuery);

		CString szProject_ID;

		while (!pDlg->m_Query[eDB_Type::DB_REWARD].eof())
		{
			szProject_ID = pDlg->m_Query[eDB_Type::DB_REWARD].fieldValue(0);

			pDlg->m_Query[eDB_Type::DB_REWARD].nextRow();
		}

		std::map<CString, struct stReward_Merge>	TempMerge_Reward;
		std::map<CString, struct stReward_Merge>::iterator	it_TempMerge_Reward;
		std::pair<std::map<CString, struct stReward_Merge>::iterator, bool> ret;

		// DB에서 해당 파일명에 맞는 유저들의 수를 가져온다.
		pDlg->szQuery.Empty();
		pDlg->szQuery.Format("select Wallet_Address, Price From Rank_Reward Where Project_ID = %s", szProject_ID.GetString());

		pDlg->m_Query[eDB_Type::DB_REWARD] = pDlg->m_DB[eDB_Type::DB_REWARD].execQuery(pDlg->szQuery);
		pDlg->LogDisplay(pDlg->szQuery);

		CString szWallet_Address = "";
		double dPrice = 0.0f;
		CString TokenPrice = "";
		CString GasPrice = "";
		CString GasLimit = "";

		// 3. 현재 Gas Fee를 구한다.
		pDlg->m_Gas_Price = pDlg->GetGasFee(pDlg->Network_Type);

		while (!pDlg->m_Query[eDB_Type::DB_REWARD].eof())
		{
			szWallet_Address = pDlg->m_Query[eDB_Type::DB_REWARD].fieldValue(0);
			dPrice = atof(pDlg->m_Query[eDB_Type::DB_REWARD].fieldValue(1));		

			// 4. 현재 Gas Limit을 구한다.
			pDlg->m_Gas_Limit = pDlg->GetGasLimit(pDlg->Network_Type, pDlg->szMasterWalletAddr, szWallet_Address);

			GasPrice.Format("%I64d", pDlg->m_Gas_Price);
			GasLimit.Format("%I64d", pDlg->m_Gas_Limit);

			stReward_Merge	Merge;

			TokenPrice.Format("%lf", dPrice);

			Merge.szWallet_Address.Format("%s", szWallet_Address.GetString());
			Merge.szTotal_Token = dPrice;
			Merge.szGas_Price.Format("%I64d", pDlg->m_Gas_Price);
			Merge.szGas_Limit.Format("%I64d", pDlg->m_Gas_Limit);

			ret = TempMerge_Reward.insert(std::pair<CString, struct stReward_Merge>(szWallet_Address.GetString(), Merge));

			if (ret.second == false)
			{
				// 이미 존재하기 때문에 기존 존재하는 element값을 업데이트 시킨다.
				TempMerge_Reward.find(szWallet_Address)->second.szTotal_Token += dPrice;
			}
			pDlg->m_Query[eDB_Type::DB_REWARD].nextRow();
		}

		for (it_TempMerge_Reward = TempMerge_Reward.begin(); it_TempMerge_Reward != TempMerge_Reward.end(); it_TempMerge_Reward++)
		{
			CString TxHash = "";

			// iterator 돌리면서 아래 함수 호출하기
			CString GasLimite, GasPrice;
			GasLimit.Format("%I64d", pDlg->GetGasLimit(pDlg->Network_Type, pDlg->szMasterWalletAddr, it_TempMerge_Reward->first.GetString()));
			GasPrice.Format("%I64d", pDlg->GetGasFee(pDlg->Network_Type));
			
			bRet = pDlg->Token_Transfer(pDlg->Network_Type, pDlg->szMasterWalletAddr, it_TempMerge_Reward->first, pDlg->SmartContract, pDlg->szMasterWalletPrivateKey, GasPrice, GasLimit, it_TempMerge_Reward->second.szTotal_Token, TxHash);
			//bRet = pDlg->Token_Transfer(pDlg->Network_Type, pDlg->szMasterWalletAddr, it_TempMerge_Reward->first, pDlg->SmartContract, pDlg->szMasterWalletPrivateKey, it_TempMerge_Reward->second.szGas_Price, it_TempMerge_Reward->second.szGas_Limit, it_TempMerge_Reward->second.szTotal_Token, TxHash);

			if (false == TxHash.IsEmpty() || 0 != TxHash.Find("None"))
			{
				// DB에 기록시키기
				pDlg->m_Query[eDB_Type::DB_RESULT].finalize();
				pDlg->szQuery.Empty();
				pDlg->szQuery.Format("insert or replace into Reward_Result(Project_ID, From_Wallet_Addr, To_Wallet_Addr, Price, TxHash, Reward_DateTime) values ('%s', '%s', '%s', %lf, '%s', (SELECT datetime('now', 'localtime')));", szProject_ID.GetString(), pDlg->szMasterWalletAddr.GetString(), it_TempMerge_Reward->first.GetString(), it_TempMerge_Reward->second.szTotal_Token, TxHash.GetString());

				pDlg->m_DB[eDB_Type::DB_RESULT].execDML(pDlg->szQuery);
				pDlg->LogDisplay(pDlg->szQuery.GetString());
			}

			if(it_TempMerge_Reward != TempMerge_Reward.end())
				Sleep(5000);
		}

	}
	g_mutex.Unlock();

	return 0;
}

void CP2PRewardGUIDlg::myUpdateData(BOOL enable)
{
	SendMessage(THREAD_UPDATE, enable, 0);
}
LRESULT CP2PRewardGUIDlg::OnThreadUpdate(WPARAM w, LPARAM l)
{
	UpdateData(w);  //이 부분에서 다이얼로그를 업데이트한다.
	return 0;
}

bool CP2PRewardGUIDlg::SetContentHeader(CString _APIKey)
{
	bool bRet = false;

	// Content Type 헤더 설정	
	m_strContentType = "Content-Type: application/json";
	m_strContentHeader = "X-API-Key: ";
	m_strContentHeader.Append(_APIKey.GetString());
	//m_strContentHeader.Format();

	if (false == m_strContentHeader.IsEmpty() && false == m_strContentType.IsEmpty())
	{
		bRet = true;
	}

	return bRet;
}

void CP2PRewardGUIDlg::SetUrl(CString _Url)
{
	m_strTargetURL.Format("%s", _Url.GetString());
}

long double CP2PRewardGUIDlg::TokenBalance(const CString _Param, const CString _Param2, const CString _Param3)
{
	long double nTokenBalance = 0.0f;

	m_headerlist = nullptr;
	curl_global_init(CURL_GLOBAL_ALL);
	m_curl = curl_easy_init();

	SetContentHeader(APIKey.GetString());
	SetCurlHeaderList(m_strContentType);
	SetCurlHeaderList(m_strContentHeader);

	SetUrl(m_strURL);

	if (m_curl)
	{
		AppendUrl(_Param.GetString());
		AppendUrl("/tokens/");
		AppendUrl(_Param2.GetString());	//wallet_address
		AppendUrl("/");
		AppendUrl(_Param3.GetString());	//contract address
		AppendUrl("/balance");	//method	


		curl_easy_setopt(m_curl, CURLOPT_URL, m_strTargetURL.GetString());
		curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headerlist);
		curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYHOST, false);

		// 결과 기록
		curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_Temp_Str_Buffer);

		m_res = curl_easy_perform(m_curl);

		if (m_res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(m_res));
			return false;
		}
		else
		{
			m_str_buffer.Format("%s", m_Temp_Str_Buffer.c_str());

			m_DocStringData.Parse(m_str_buffer.GetString());					

			if (false == m_DocStringData.HasMember("meta") || false == m_DocStringData.HasMember("error"))
			{
				m_Balance = atof(m_DocStringData["payload"]["token"].GetString());
			}

			if (0 == m_Balance)
			{
				nTokenBalance = 0;
			}
			else
			{
				nTokenBalance = m_Balance;
				CString Msg;
				Msg.Format("Balance = [%I64d]", static_cast<__int64>(nTokenBalance));
				LogDisplay(Msg.GetString());
			}
		}
	}
	m_DocStringData.RemoveAllMembers();// .Clear();
	m_str_buffer.Empty();
	m_Temp_Str_Buffer.clear();

	curl_easy_cleanup(m_curl);
	curl_slist_free_all(m_headerlist);
	curl_global_cleanup();

	return nTokenBalance;
}

__int64 CP2PRewardGUIDlg::GetGasFee(CString _Param)
{
	__int64 GasPrice = 0;

	m_headerlist = nullptr;
	curl_global_init(CURL_GLOBAL_ALL);
	m_curl = curl_easy_init();

	SetContentHeader(APIKey.GetString());
	SetCurlHeaderList(m_strContentType);
	SetCurlHeaderList(m_strContentHeader);

	SetUrl(m_strURL);

	if (m_curl)
	{
		AppendUrl(_Param.GetString());
		AppendUrl("/txs/fee");

		curl_easy_setopt(m_curl, CURLOPT_URL, m_strTargetURL.GetString());
		curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headerlist);
		curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYHOST, false);

		// 결과 기록
		curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_Temp_Str_Buffer);

		m_res = curl_easy_perform(m_curl);

		if (m_res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(m_res));
			return false;
		}
		else
		{
			m_str_buffer.Format("%s", m_Temp_Str_Buffer.c_str());

			m_DocStringData.Parse(m_str_buffer.GetString());

			if (false == m_DocStringData.HasMember("meta") || false == m_DocStringData.HasMember("error"))
			{
				GasPrice = static_cast<__int64>(strtoll(m_DocStringData["payload"]["average"].GetString(), NULL, 10) * 1000000000);
			}

			CString Msg;
			Msg.Format("Gas Price = [%I64d Wei]", static_cast<__int64>(GasPrice));
			LogDisplay(Msg.GetString());
		}
	}
	m_DocStringData.RemoveAllMembers();	
	m_str_buffer.Empty();
	m_Temp_Str_Buffer.clear();

	return GasPrice;
}

__int64 CP2PRewardGUIDlg::GetGasLimit(const CString _Param /*Network Type*/, const CString _Param2 /*Master Wallet Address*/, const CString _Param3 /*Receive Wallet Address*/)
{
	__int64 GasLimit = 0;

	m_headerlist = nullptr;
	curl_global_init(CURL_GLOBAL_ALL);
	m_curl = curl_easy_init();
	
	SetCurlHeaderList(m_strContentType);
	SetCurlHeaderList(m_strContentHeader);
	SetUrl(m_strURL);

	if (m_curl)
	{
		AppendUrl(_Param.GetString());
		AppendUrl("/txs/gas");

		m_strResourceJSON.Append("{ \"fromAddress\" : \"");
		m_strResourceJSON.Append(_Param2.GetString());
		m_strResourceJSON.Append("\", \"toAddress\" : \"");
		m_strResourceJSON.Append(_Param3.GetString());
		m_strResourceJSON.Append("\"}");

		curl_easy_setopt(m_curl, CURLOPT_URL, m_strTargetURL.GetString());
		curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headerlist);
		curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYHOST, false);
		curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
		curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, m_strResourceJSON.GetString());
		// 결과 기록
		curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_Temp_Str_Buffer);

		m_res = curl_easy_perform(m_curl);

		if (m_res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(m_res));
			return false;
		}
		else
		{
			m_str_buffer.Format("%s", m_Temp_Str_Buffer.c_str());

			m_DocStringData.Parse(m_str_buffer.GetString());

			if (false == m_DocStringData.HasMember("meta") || false == m_DocStringData.HasMember("error"))
			{
				GasLimit = static_cast<__int64>(strtoll(m_DocStringData["payload"]["gasLimit"].GetString(), NULL, 10) * 1.40);
			}
			CString Msg;
			Msg.Format("Gas Limit = [%I64d]", GasLimit);
			LogDisplay(Msg.GetString());
		}
	}

	m_DocStringData.RemoveAllMembers();
	m_strResourceJSON.Empty();
	m_str_buffer.Empty();
	m_Temp_Str_Buffer.clear();

	curl_easy_cleanup(m_curl);
	curl_slist_free_all(m_headerlist);
	curl_global_cleanup();

	return GasLimit;
}

bool CP2PRewardGUIDlg::Token_Transfer(const CString Param /*Network Type*/, const CString Param2 /*From Address*/, const CString Param3 /*To Address*/,
									  const CString Param4 /*contract address*/, const CString Param5 /*privateKey*/, const CString Param6 /*gas Price*/, const CString Param7 /*gas Limit*/, const long double Param8 /*token count*/,
									  CString &_Out)
{
	bool bRet = true;

	int nLoop = 0;

	m_headerlist = nullptr;
	curl_global_init(CURL_GLOBAL_ALL);
	m_curl = curl_easy_init();

	SetCurlHeaderList(m_strContentType);
	SetCurlHeaderList(m_strContentHeader);
	SetUrl(m_strURL);

	StErrorCode StCode;
	StReward_Result Result;

	if (m_curl)
	{
		AppendUrl(Param.GetString());
		AppendUrl("/tokens/transfer");

		//string to Hex
		m_strResourceJSON.Append("{ \"fromAddress\" : \"");
		m_strResourceJSON.Append(Param2.GetString());

		m_strResourceJSON.Append("\", \"toAddress\" : \"");
		m_strResourceJSON.Append(Param3.GetString());
		
		m_strResourceJSON.Append("\", \"contract\" : \"");
		m_strResourceJSON.Append(Param4.GetString());
		
		m_strResourceJSON.Append("\", \"privateKey\" : \"");
		m_strResourceJSON.Append(Param5.GetString());
		
		m_strResourceJSON.Append("\", \"gasPrice\" : \"");
		m_strResourceJSON.Append(Param6.GetString());
		
		m_strResourceJSON.Append("\", \"gasLimit\" : \"");		
		//CString TempParam7;
		//TempParam7.Format("%d", 100000);
		m_strResourceJSON.Append(Param7.GetString());
		
		m_strResourceJSON.Append("\", \"token\" : \"");
		CString TempParam8;
		TempParam8.Format("%lf", Param8);

		m_strResourceJSON.Append(TempParam8.GetString());
		m_strResourceJSON.Append("\"}");

		curl_easy_setopt(m_curl, CURLOPT_URL, m_strTargetURL.GetString());
		curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headerlist);
		curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYHOST, false);
		curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
		curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, m_strResourceJSON.GetString());
		// 결과 기록
		curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_Temp_Str_Buffer);

		m_res = curl_easy_perform(m_curl);

		if (m_res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(m_res));
			StCode.Msg.Append("None");
			StCode.Hex.Append("None");
			StCode.Code.Append(curl_easy_strerror(m_res));
		}
		else
		{
			m_str_buffer.Format("%s", m_Temp_Str_Buffer.c_str());

			m_DocStringData.Parse(m_str_buffer.GetString());
			if(true == m_DocStringData.HasMember("meta") || true == m_DocStringData.HasMember("error") || true == m_DocStringData.HasMember("message"))
			{
				StCode.Msg.Append(m_DocStringData["meta"]["error"]["message"].GetString());
				StCode.Code.Format("%d",(m_DocStringData["meta"]["error"]["code"].GetInt()));
				StCode.Hex.Append("None");
			}
			else
			{
				StCode.Hex.Append(m_DocStringData["payload"]["hex"].GetString());
				StCode.Code.Append("0");
				StCode.Msg.Append("None");
			}			

			CString Msg;

			switch (atoi(StCode.Code.GetString()))
			{
				case 3014:
				{
					Msg.Format("TX Fee  exceeds the configured cap(1.00 ether) Msg = [%s], Code = [%s]", StCode.Msg.GetString(), StCode.Code.GetString());
					LogDisplay(Msg.GetString());
				}break;

				case 3022:
				{
					Msg.Format("Not enough tokens Msg = [%s], Code = [%s]", StCode.Msg.GetString(), StCode.Code.GetString());
					LogDisplay(Msg.GetString());
				}break;

				case 3023:
				{
					Msg.Format("Balance is not enough Msg = [%s], Code = [%s]", StCode.Msg.GetString(), StCode.Code.GetString());
					LogDisplay(Msg.GetString());
				}break;

				case 3024:
				{
					Msg.Format("contract cannot be null or empty Msg = [%s], Code = [%s]", StCode.Msg.GetString(), StCode.Code.GetString());
					LogDisplay(Msg.GetString());
				}break;

				case 3025:
				{
					Msg.Format("Check input Data no reference fount to Privided contract and from Address Msg = [%s], Code = [%s]", StCode.Msg.GetString(), StCode.Code.GetString());
					LogDisplay(Msg.GetString());					
				}break;

				default:
				{
					Msg.Format("Token Send Transaction Hash = [%s], TokenCount = [%lf], Msg = [%s], Code = [%s]", StCode.Hex.GetString(), Param8, StCode.Msg.GetString(), StCode.Code.GetString());
					LogDisplay(Msg.GetString());					
				}
			}
		}

		_Out.Format("%s", StCode.Hex.GetString());
	}
	m_strResourceJSON.Empty();
	m_DocStringData.RemoveAllMembers();
	m_str_buffer.Empty();
	m_Temp_Str_Buffer.clear();

	curl_easy_cleanup(m_curl);
	curl_slist_free_all(m_headerlist);
	curl_global_cleanup();

	return bRet;
}