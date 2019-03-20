// MonitorProcess.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ProcessManager.h"
#include "MonitorProcess.h"
#include "afxdialogex.h"
#include "Monitor.h"

// MonitorProcess �Ի���

extern HANDLE g_hEvent[3];

IMPLEMENT_DYNAMIC(MonitorProcess, CDialogEx)

MonitorProcess::MonitorProcess(CWnd* pParent /*=NULL*/)
	: CDialogEx(MonitorProcess::IDD, pParent)
{

}

MonitorProcess::~MonitorProcess()
{
}

void MonitorProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT, m_EditControl);
}


BEGIN_MESSAGE_MAP(MonitorProcess, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_DENY, &MonitorProcess::OnBnClickedButtonDeny)
	ON_BN_CLICKED(IDC_BUTTON_ACCEPT, &MonitorProcess::OnBnClickedButtonAccept)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// MonitorProcess ��Ϣ�������


void MonitorProcess::OnBnClickedButtonDeny()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (g_hEvent[2]!=NULL)
	{
		SetEvent(g_hEvent[2]);

		ResetEvent(g_hEvent[2]);
	}

	OnOK();
}


void MonitorProcess::OnBnClickedButtonAccept()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (g_hEvent[1]!=NULL)
	{
		SetEvent(g_hEvent[1]);

		ResetEvent(g_hEvent[1]);
	}


	OnOK();
}


BOOL MonitorProcess::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	m_ulCount = 3;

	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);//������չ���ߴ�ģʽ����ֹ��������ʾͼ��
	CRect rectWorkArea;
	SystemParametersInfoW(SPI_GETWORKAREA,0,&rectWorkArea,SPIF_SENDCHANGE);

	//��öԻ����С
	CRect rectDlg;
	GetWindowRect(&rectDlg);
	int nW = rectDlg.Width();
	int nH = rectDlg.Height();

	//���������õ����½�
	::SetWindowPos(this->m_hWnd,HWND_BOTTOM,
		rectWorkArea.right-nW-6,rectWorkArea.bottom-nH,
		nW,nH,
		SWP_NOZORDER);

	SetTimer(0,1000,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void MonitorProcess::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ


	switch (nIDEvent)
	{
	case 0://��ʱ���Զ�����
		{
			if(m_ulCount==0)
			{
				KillTimer(nIDEvent);


				if (g_hEvent[1]!=NULL)
				{
					SetEvent(g_hEvent[1]);

					ResetEvent(g_hEvent[1]);
				}
				SendMessage(WM_CLOSE);
					break;
			}
			
			m_ulCount--;
		
		}
		
	}
	CDialogEx::OnTimer(nIDEvent);
}
