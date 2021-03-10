#pragma once


// WALLET_ADD 대화 상자

class WALLET_ADD : public CDialog
{
	DECLARE_DYNAMIC(WALLET_ADD)

public:
	WALLET_ADD(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~WALLET_ADD();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WALLET_ADD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Edt_Wallet_Addr_Add;
	CEdit m_Edt_Wallet_Private_Add;
	afx_msg void OnBnClickedReg();
	afx_msg void OnBnClickedRegCancel();
};
