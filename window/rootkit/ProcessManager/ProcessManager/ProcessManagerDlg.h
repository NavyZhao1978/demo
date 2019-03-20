
// ProcessManagerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "ProcessManager.h"
#include "Process.h"
#include "CEnumProcess.h"
#include "Monitor.h"


#define UM_ICONNOTIFY   WM_USER+1
// CProcessManagerDlg �Ի���
class CProcessManagerDlg : public CDialogEx
{
// ����
public:
	CProcessManagerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PROCESSMANAGER_DIALOG };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	NOTIFYICONDATA  m_nid;
	VOID CProcessManagerDlg::ContructNotifyConData();
// ʵ��
protected:
	HICON m_hIcon;
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnIconNotify(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_TabMain;
	CMonitor Monitor;
	CEnumProcess EnumProcess;
	afx_msg void OnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnCancel();
};
