#pragma once
#if !defined(AFX_IDD_DIALOG1_H__E7DB4047_84FC_45CB_BFAE_8B8DE36B869B__INCLUDED_)
#define AFX_IDD_DIALOG1_H__E7DB4047_84FC_45CB_BFAE_8B8DE36B869B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IDD_DIALOG1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// IDD_DIALOG1 dialog

class IDD_DIALOG1 : public CDialog
{
	// Construction
public:
	IDD_DIALOG1(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(IDD_DIALOG1)
	enum { IDD = IDD_DIALOG1 };
	// NOTE: the ClassWizard will add data members here
//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(IDD_DIALOG1)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(IDD_DIALOG1)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IDD_DIALOG1_H__E7DB4047_84FC_45CB_BFAE_8B8DE36B869B__INCLUDED_)