#pragma once


// NETWORK_ADD 대화 상자

class NETWORK_ADD : public CDialog
{
	DECLARE_DYNAMIC(NETWORK_ADD)

public:
	NETWORK_ADD(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~NETWORK_ADD();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NETWORK_ADD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedNetReg();
	CEdit m_Net_URL;
	CEdit m_Net_Port;
	CEdit m_Net_Name;
	CEdit m_Net_SmartContract;
};
