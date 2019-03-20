
// ProcessManagerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ProcessManager.h"
#include "ProcessManagerDlg.h"
#include "afxdialogex.h"
#include "Resource.h"
#include "Common.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCommon Common;

int dpix;
int dpiy;

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CProcessManagerDlg �Ի���




CProcessManagerDlg::CProcessManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProcessManagerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcessManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_TabMain);
}

BEGIN_MESSAGE_MAP(CProcessManagerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(UM_ICONNOTIFY, (LRESULT (__thiscall CWnd::*)(WPARAM,LPARAM))OnIconNotify)  
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CProcessManagerDlg::OnSelchangeTabMain)
END_MESSAGE_MAP()


// CProcessManagerDlg ��Ϣ�������

BOOL CProcessManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	int dpix;
	int dpiy;


	//��ʼ��Tab��
	m_TabMain.InsertItem(0, _T("������Ϣ"));           
	m_TabMain.InsertItem(1, _T("���̼��"));   



	//�������ԶԻ������
	EnumProcess.Create(IDD_DIALOG_ENUMPROCESS,GetDlgItem(IDC_TAB1));
	Monitor.Create(IDD_DIALOG_MONITOR,GetDlgItem(IDC_TAB1));

	CRect tabRect;
	GetWindowRect(&tabRect);
	CPaintDC dc(this);
	dpix = GetDeviceCaps(dc.m_hDC,LOGPIXELSX);
	dpiy = GetDeviceCaps(dc.m_hDC,LOGPIXELSY);
	tabRect.bottom += (LONG)(1+21*(dpiy/96.0));
	MoveWindow(&tabRect);
	m_TabMain.GetClientRect(&tabRect);    // ��ȡ��ǩ�ؼ��ͻ���Rect   

	// ����tabRect��ʹ�串�Ƿ�Χ�ʺϷ��ñ�ǩҳ   
	tabRect.left += 1;                  
	tabRect.right -= 1;   
	tabRect.top += 25;   
	tabRect.bottom -= 1;   

	// ���ݵ����õ�tabRect����m_jzmDlg�ӶԻ��򣬲�����Ϊ��ʾ   
	EnumProcess.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
	Monitor.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);

	Common.DeviceHandle = OpenDevice(L"\\\\.\\ProcessManagerLinkName");//ProcessManagerLinkName
	if(Common.DeviceHandle ==(HANDLE)-1)
	{
		MessageBox(L"���豸ʧ��");
		return FALSE;
	}

	VOID ContructNotifyConData();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CProcessManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CProcessManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CProcessManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CProcessManagerDlg::OnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ULONG        m_SelectTab = 0;
	m_SelectTab = m_TabMain.GetCurSel();
	CRect tabRect;   
	m_TabMain.GetClientRect(&tabRect);    // ��ȡ��ǩ�ؼ��ͻ���Rect   
	// ����tabRect��ʹ�串�Ƿ�Χ�ʺϷ��ñ�ǩҳ   
	tabRect.left += 1;                  
	tabRect.right -= 1;   
	tabRect.top += 25;   
	tabRect.bottom -= 1;   
	switch(m_SelectTab)
	{
	case 0:
		{
			EnumProcess.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
			Monitor.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
			if(::MessageBox(NULL,L"ȷ���뿪�����",L"Noti",1)==MB_OK)
			{
				Monitor.m_bOk = FALSE;
			}
			break;
		}
	case 1:
		{
			EnumProcess.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
			Monitor.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
			break;
		}
	}
	*pResult = 0;
}



void CProcessManagerDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	Shell_NotifyIcon(NIM_DELETE, &m_nid);
	CloseHandle(Common.DeviceHandle);
	CDialogEx::OnCancel();
}


VOID CProcessManagerDlg::ContructNotifyConData()
{
	m_nid.cbSize = sizeof(NOTIFYICONDATA);    
	m_nid.hWnd = m_hWnd;          
	m_nid.uID = IDR_MAINFRAME;     
	m_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;   
	m_nid.uCallbackMessage = UM_ICONNOTIFY;            
	m_nid.hIcon = m_hIcon;                            
	CString strTemp=L"������Ҫ��ǿ��������㱻Ű.........";       
	lstrcpyn(m_nid.szTip,strTemp, sizeof(m_nid.szTip) / sizeof(m_nid.szTip[0]));
	Shell_NotifyIcon(NIM_ADD, &m_nid);   //��ʾ����

}


void CProcessManagerDlg::OnIconNotify(WPARAM wParam, LPARAM lParam)
{
/*	switch ((UINT)lParam)
	{
	case WM_RBUTTONDOWN: 
		CMenu menu;
		menu.LoadMenu(IDR_MENU_NOTIFY);
		CPoint point;
		GetCursorPos(&point);
		SetForegroundWindow();   //���õ�ǰ����
		menu.GetSubMenu(0)->TrackPopupMenu(
			TPM_LEFTBUTTON|TPM_RIGHTBUTTON, 
			point.x, point.y, this, NULL); 
		PostMessage(WM_USER, 0, 0);
		break;
	}
*/
}