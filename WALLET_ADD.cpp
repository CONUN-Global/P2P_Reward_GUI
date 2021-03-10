// WALLET_ADD.cpp: 구현 파일
//

#include "pch.h"
#include "P2P_Reward_GUI.h"
#include "P2P_Reward_GUIDlg.h"
#include "WALLET_ADD.h"
#include "afxdialogex.h"


// WALLET_ADD 대화 상자

IMPLEMENT_DYNAMIC(WALLET_ADD, CDialog)

WALLET_ADD::WALLET_ADD(CWnd* pParent /*=nullptr*/)	: CDialog(IDD_WALLET_ADD, pParent)
{

}

WALLET_ADD::~WALLET_ADD()
{
}

void WALLET_ADD::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDT_WALLET_ADDR_ADD, m_Edt_Wallet_Addr_Add);
	DDX_Control(pDX, IDC_EDT_WALLET_PRIVATE_ADD, m_Edt_Wallet_Private_Add);
}


BEGIN_MESSAGE_MAP(WALLET_ADD, CDialog)
	ON_BN_CLICKED(ID_REG, &WALLET_ADD::OnBnClickedReg)
	ON_BN_CLICKED(ID_REG_CANCEL, &WALLET_ADD::OnBnClickedRegCancel)
END_MESSAGE_MAP()


// WALLET_ADD 메시지 처리기


void WALLET_ADD::OnBnClickedReg()
{
	CString szAddress, szPrivateKey;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItemText(IDC_EDT_WALLET_ADDR_ADD, szAddress);
	GetDlgItemText(IDC_EDT_WALLET_PRIVATE_ADD, szPrivateKey);
	
	CString szQuery, Msg;
	szQuery.AppendFormat("insert or replace into Wallet(Wallet_Address, Wallet_Privatekey) values ('%s', '%s');", szAddress.GetString(), szPrivateKey.GetString());

	((CP2PRewardGUIDlg*)GetParent())->UpdateData(TRUE);
	// Wallet 테이블이 존재하는 체크
	if (false == ((CP2PRewardGUIDlg*)GetParent())->m_DB[eDB_Type::DB_WALLET].tableExists("Wallet"))
	{
		try
		{
			// Wallet.db 파일은 있으나, 테이블이 없다. 생성으로 들어간다.
			((CP2PRewardGUIDlg*)GetParent())->m_DB[eDB_Type::DB_WALLET].execDML("CREATE TABLE Wallet (Wallet_Address TEXT NOT NULL CHECK(length(Wallet_Address) > 0 AND length(Wallet_Address) < 255) UNIQUE, \n\
											Wallet_Privatekey TEXT NOT NULL CHECK(length(Wallet_Privatekey) > 0 AND length(Wallet_Privatekey) < 4096), \n\
											PRIMARY KEY(Wallet_Address), \n\
											CONSTRAINT Wallet_Address CHECK(Wallet_Address like '0x%'));\n");
		}
		catch (CppSQLite3Exception& Exception)
		{
			((CP2PRewardGUIDlg*)GetParent())->LogDisplay(Exception.errorMessage());
		}
	}

	try
	{
		((CP2PRewardGUIDlg*)GetParent())->m_DB[eDB_Type::DB_WALLET].execDML(szQuery.GetString());

		Msg.Format("Wallet Address = [%s], Wallet PrivateKey = [%s] Add Success\n", szAddress.GetString(), szPrivateKey.GetString());
		((CP2PRewardGUIDlg*)GetParent())->LogDisplay(Msg.GetString());

		szQuery.Format("select Wallet_Address, Wallet_Privatekey from Wallet;");
		((CP2PRewardGUIDlg*)GetParent())->m_Query[eDB_Type::DB_WALLET] = ((CP2PRewardGUIDlg*)GetParent())->m_DB[eDB_Type::DB_WALLET].execQuery(szQuery.GetString());


		((CP2PRewardGUIDlg*)GetParent())->m_Wallet_Address_List.ResetContent();
		((CP2PRewardGUIDlg*)GetParent())->m_Wallet_Privatekey_List.ResetContent();

		while (!((CP2PRewardGUIDlg*)GetParent())->m_Query[eDB_Type::DB_WALLET].eof())
		{
			((CP2PRewardGUIDlg*)GetParent())->m_Wallet_Address_List.AddString(((CP2PRewardGUIDlg*)GetParent())->m_Query[eDB_Type::DB_WALLET].fieldValue(0));
			((CP2PRewardGUIDlg*)GetParent())->m_Wallet_Privatekey_List.AddString(((CP2PRewardGUIDlg*)GetParent())->m_Query[eDB_Type::DB_WALLET].fieldValue(1));

			if (false == ((CP2PRewardGUIDlg*)GetParent())->m_Query[eDB_Type::DB_WALLET].eof())
				((CP2PRewardGUIDlg*)GetParent())->m_Query[eDB_Type::DB_WALLET].nextRow();
		}

		if (0 < ((CP2PRewardGUIDlg*)GetParent())->m_Wallet_Address_List.GetCount())
		{
			((CP2PRewardGUIDlg*)GetParent())->m_Wallet_Address_List.SetCurSel(0);
			((CP2PRewardGUIDlg*)GetParent())->m_Wallet_Privatekey_List.SetCurSel(0);

			((CP2PRewardGUIDlg*)GetParent())->m_Btn_Reward.EnableWindow(TRUE);
			((CP2PRewardGUIDlg*)GetParent())->m_Btn_Reward_Info.EnableWindow(TRUE);
			((CP2PRewardGUIDlg*)GetParent())->m_Btn_Wallet_Remove.EnableWindow(TRUE);

			((CP2PRewardGUIDlg*)GetParent())->m_Wallet_Address_List.EnableWindow(TRUE);
			((CP2PRewardGUIDlg*)GetParent())->m_Wallet_Privatekey_List.EnableWindow(TRUE);
		}
		
		((CP2PRewardGUIDlg*)GetParent())->m_Query[eDB_Type::DB_WALLET].finalize();
		((CP2PRewardGUIDlg*)GetParent())->UpdateData(FALSE);
	}
	catch (CppSQLite3Exception& Exception)
	{
		if (0 > szAddress.Find("0x"))
		{
			Msg.AppendFormat("%s", "Wallet Address Not Found '0x'");
			((CP2PRewardGUIDlg*)GetParent())->LogDisplay(Msg);
			((CP2PRewardGUIDlg*)GetParent())->LogDisplay(Exception.errorMessage());

			MessageBox(Msg, "Error", MB_OK);
		}
	}

	OnOK();
}


void WALLET_ADD::OnBnClickedRegCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnCancel();
}
