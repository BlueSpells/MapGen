#pragma once

// SmartComboBox.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CSmartComboBox window

#include <vector>

class CSmartComboBox : public CComboBox
{
// Construction
public:
	CSmartComboBox();
	virtual ~CSmartComboBox();

    struct Pair
    {
        int Code;
        const char* String;
    };
    void SetPairs(Pair* Pairs, int Count);
    int GetValue() const;
    void SetValue(int Value);
    void AddItem(int Code, const char* Str);

    void LoadFromConfiguration(const char* Section, const char* Key);
    void WriteToConfiguration(const char* Section, const char* Key);
    void UpdateLru();

private:
    // Hide copy ctor and assignment operator
    CSmartComboBox(const CSmartComboBox &);
    CSmartComboBox & operator=(const CSmartComboBox &);

    void UpdateList(const std::vector<std::string> & StringVector, const char * CurrentStr = "");
    void GetStringList(std::vector<std::string> & StringVector);

    CString m_LastText;

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmartComboBox)
	//}}AFX_VIRTUAL

	// Generated message map functions

protected:
	//{{AFX_MSG(CSmartComboBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

