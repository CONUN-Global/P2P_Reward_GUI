// NETWORK_ADD.cpp: 구현 파일
//

#include "pch.h"
#include "P2P_Reward_GUI.h"
#include "P2P_Reward_GUIDlg.h"
#include "NETWORK_ADD.h"
#include "afxdialogex.h"


// NETWORK_ADD 대화 상자

IMPLEMENT_DYNAMIC(NETWORK_ADD, CDialog)

NETWORK_ADD::NETWORK_ADD(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_NETWORK_ADD, pParent)
{

}

NETWORK_ADD::~NETWORK_ADD()
{
}

void NETWORK_ADD::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDT_NET_URL, m_Net_URL);
	DDX_Control(pDX, IDC_EDT_NET_PORT, m_Net_Port);
	DDX_Control(pDX, IDC_EDT_NET_NAME, m_Net_Name);
	DDX_Control(pDX, IDC_EDT_NET_SMARTCONTRACT, m_Net_SmartContract);
}


BEGIN_MESSAGE_MAP(NETWORK_ADD, CDialog)
	ON_BN_CLICKED(ID_NET_REG, &NETWORK_ADD::OnBnClickedNetReg)
END_MESSAGE_MAP()


// NETWORK_ADD 메시지 처리기


void NETWORK_ADD::OnBnClickedNetReg()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szNetName;
	CString szNetUrl;
	CString	szNetPort;
	CString szNetSmartContract;

	GetDlgItemText(IDC_EDT_NET_NAME, szNetName);
	GetDlgItemText(IDC_EDT_NET_URL, szNetUrl);
	GetDlgItemText(IDC_EDT_NET_PORT, szNetPort);
	GetDlgItemText(IDC_EDT_NET_SMARTCONTRACT, szNetSmartContract);

	CString szQuery, Msg;
	szQuery.Format("insert or replace into Network(Network_Name, Network_URL, Network_Port) values ('%s', '%s', %d);", szNetName.GetString(), szNetUrl.GetString(), atoi(szNetPort.GetString()));

	((CP2PRewardGUIDlg*)GetParent())->UpdateData(TRUE);

	((CP2PRewardGUIDlg*)GetParent())->m_DB[eDB_Type::DB_NETWORK].execDML(szQuery.GetString());

	szQuery.Empty();
	
	((CP2PRewardGUIDlg*)GetParent())->m_NetName.GetCurSel();
	((CP2PRewardGUIDlg*)GetParent())->SetDlgItemText(IDC_EDT_SMARTCONTRACT, szNetSmartContract);
}
