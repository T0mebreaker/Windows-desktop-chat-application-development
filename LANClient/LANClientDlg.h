
// LANClientDlg.h: 头文件
//

#pragma once
//用户消息
#define UM_MODIUSER WM_USER+1 

// CLANClientDlg 对话框
class CLANClientDlg : public CDialogEx
{
// 构造
public:
	CLANClientDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LANCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
private:
	//宽字节转窄字符
	std::string  WtoA(CString &wstr);
	//接收信息的线程
	static unsigned __stdcall RecvMsg(void *param);
	static HWND hwnd;
private: 
	SOCKET m_client;
	bool isCon;//当前是否连接
	bool isSet;
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnCnt();
	// IP
	CIPAddressCtrl m_ip;
private:
	// 昵称
	CString m_name;
	// 当前在线成员
	CListCtrl m_Member;
public:
	afx_msg void OnBnClickedBtnSend();
	// 显示消息的内容
	CString m_et_Msg;
protected:
	//处理来自服务器的消息
	afx_msg LRESULT OnUmSelSevMsg(WPARAM wParam, LPARAM lParam);
private:
	// 显示消息
	CEdit m_showMSg;
public:
	afx_msg void OnBnClickedBtnSet();
};
