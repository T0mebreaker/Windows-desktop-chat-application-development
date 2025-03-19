
// LANClientDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "LANClient.h"
#include "LANClientDlg.h"
#include "afxdialogex.h"
#include<locale>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLANClientDlg 对话框

HWND CLANClientDlg::hwnd = NULL;

CLANClientDlg::CLANClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LANCLIENT_DIALOG, pParent)
	, m_name(_T(""))
	, isCon(false)
	, m_et_Msg(_T(""))
	, isSet(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLANClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IP_ADDR, m_ip);
	DDX_Text(pDX, IDC_ET_NAME, m_name);
	DDX_Control(pDX, IDC_LS_MEMBER, m_Member);
	DDX_Text(pDX, IDC_ET_SHOWMSG, m_et_Msg);
	DDX_Control(pDX, IDC_ET_SHOWMSG, m_showMSg);
}

std::string CLANClientDlg::WtoA(CString& wstr)
{
	int cLen = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, 0, NULL);
	std::string str;
	str.resize(cLen + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, (LPSTR)str.data(), cLen, 0, NULL);
	return str;
}

unsigned __stdcall CLANClientDlg::RecvMsg(void* param)
{
	SOCKET* cli = (SOCKET*)param;
	while (1) {
		char* buf = new char[0xFF]{};
		int len = recv(*cli, buf, 0xFF, 0);
		if (len <= 0) {
			::PostMessageW(hwnd, UM_MODIUSER, 0, (LPARAM)buf); //接收消息错误，发出退出消息
			break;
		}
		::PostMessageW(hwnd, UM_MODIUSER, 1, (LPARAM)buf); //成功接收消息
	}
	return 0;
}

BEGIN_MESSAGE_MAP(CLANClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CNT, &CLANClientDlg::OnBnClickedBtnCnt)
	ON_BN_CLICKED(IDC_BTN_SEND, &CLANClientDlg::OnBnClickedBtnSend)
	ON_MESSAGE(UM_MODIUSER, &CLANClientDlg::OnUmSelSevMsg)
	ON_BN_CLICKED(IDC_BTN_SET, &CLANClientDlg::OnBnClickedBtnSet)
END_MESSAGE_MAP()


// CLANClientDlg 消息处理程序

BOOL CLANClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MINIMIZE);
	WSADATA data;
	int ret = WSAStartup(MAKEWORD(2, 2), &data);
	if (ret) {
		AfxMessageBox(L"初始化网络环境失败");
	}
	m_client = socket(AF_INET, SOCK_STREAM, 0);
	if (m_client == -1) {
		AfxMessageBox(L"创建套接字失败！");
	}

	// TODO: 在此添加额外的初始化代码 82.157.131.178
	m_ip.SetAddress(127, 0, 0, 1);//设置初始地址
	//m_ip.SetAddress(82, 157, 131, 178);
	hwnd = m_hWnd;
	SetDlgItemInt(IDC_ET_PORT,9999);
	setlocale(LC_ALL,"");
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLANClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLANClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CLANClientDlg::OnBnClickedBtnCnt()
{
	if (isCon) {
		closesocket(m_client);
		m_client = -1;
		isCon = false;
		AfxMessageBox(L"成功断开连接！");
		SetDlgItemText(IDC_BTN_CNT, _T("连接"));
		m_Member.DeleteAllItems();
		return;
	}

	if (m_client == -1) {
		m_client = socket(AF_INET, SOCK_STREAM, 0);
	}
	UpdateData();
	if (m_name.IsEmpty()) {
		AfxMessageBox(L"请输入昵称！");
		return;
	}
	SOCKADDR_IN addrSev;
	addrSev.sin_family = AF_INET;
	addrSev.sin_port = htons(GetDlgItemInt(IDC_ET_PORT));
	DWORD ip;
	m_ip.GetAddress(ip);
	addrSev.sin_addr.S_un.S_addr = htonl(ip);

	int res = connect(m_client, (sockaddr*)&addrSev, sizeof(addrSev));
	if (res == -1) {
		AfxMessageBox(L"连接服务器失败！");
		return;
	}
	m_Member.InsertItem(0, m_name);
	SetDlgItemText(IDC_BTN_CNT, _T("连接成功！"));
	_beginthreadex(0, 0, RecvMsg, &m_client, 0, 0);
	Sleep(500);
	SetDlgItemText(IDC_BTN_CNT, _T("断开连接！"));
	isCon = true;
	std::string na = WtoA(m_name);
	send(m_client, na.data(), na.size(), 0);
}


void CLANClientDlg::OnBnClickedBtnSend()
{
	if (m_client == -1) {
		SetDlgItemText(IDC_BTN_SEND, _T("网络错误！"));
		Sleep(500);
		SetDlgItemText(IDC_BTN_SEND, _T("发送"));
		return;
	}
	CString msg;
	GetDlgItemText(IDC_ET_MSG, msg);
	if (msg.IsEmpty()) {
		SetDlgItemText(IDC_BTN_SEND, _T("消息为空！"));
		Sleep(500);
		SetDlgItemText(IDC_BTN_SEND, _T("发送"));
		return;
	}
	UpdateData();
	std::string str = WtoA(msg);
	int len = send(m_client, str.data(), str.size(), 0);
	if (len == str.size()) {
		msg = _T("@你:") + msg + _T("\r\n");
		m_et_Msg.Append(msg);
		SetDlgItemText(IDC_ET_MSG, _T(""));
		UpdateData(false);
	}
	
	m_showMSg.LineScroll(m_showMSg.GetLineCount() - 10);
}


afx_msg LRESULT CLANClientDlg::OnUmSelSevMsg(WPARAM wParam, LPARAM lParam)
{
	if (!wParam) {
		char* msg = (char*)lParam;
		delete[] msg;
		UpdateData();
		m_et_Msg.Append(_T("你已经断线！\r\n"));
		UpdateData(false);
		m_Member.DeleteAllItems();
		return -1;
	}

	char* msg = (char*)lParam;
	if (msg[0] == '1' && msg[1] == ':') {
		USES_CONVERSION;
		CString s = A2W(&msg[2]);
		UpdateData();
		m_et_Msg.Append(s + L"\r\n");
		UpdateData(false);
		int index = s.Find(L':');
		s.GetBuffer()[index] = L'\0';
		m_Member.InsertItem(0,s);
	}
	else if (msg[0] == '2' && msg[1] == ':') {
		USES_CONVERSION;
		CString s = A2W(&msg[2]);
		UpdateData();
		m_et_Msg.Append(s + L"\r\n");
		UpdateData(false);
		int index = s.Find(L':');
		s.GetBuffer()[index] = L'\0';
		for (int i = 0; i < m_Member.GetItemCount(); i++) {
			if (m_Member.GetItemText(i, 0)==s) {
				m_Member.DeleteItem(i);
				break;
			}
		}
	}
	else if (msg[0] == '3' && msg[1] == ':') {
		USES_CONVERSION;
		CString s = A2W(&msg[2]);
		UpdateData();
		m_et_Msg.Append(s + L"\r\n");
		UpdateData(false);
	}
	else if (msg[0] == '4' && msg[1] == ':') {
		USES_CONVERSION;
		CString s = A2W(&msg[2]);
		int index = 0;
		for (int i = 0; i < s.GetLength(); i++) {
			if (s[i] == L':') {
				s.GetBuffer()[i] = L'\0';
				m_Member.InsertItem(0, &s.GetBuffer()[index]);
				index = i + 1;
			}
		}
		
	}
	m_showMSg.LineScroll(m_showMSg.GetLineCount() - 10);
	delete[] msg;
	return 0;
}


void CLANClientDlg::OnBnClickedBtnSet()
{
	if (isSet)
	{
		RECT rect;
		GetWindowRect(&rect);
		rect.right += 360;
		MoveWindow(&rect);
	}
	else 
	{
		RECT rect;
		GetWindowRect(&rect);
		rect.right -= 360;
		MoveWindow(&rect);
	}
	isSet = !isSet;
}
