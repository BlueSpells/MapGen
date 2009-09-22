// SmartComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "SmartComboBox.h"
#include "Common\Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSmartComboBox

CSmartComboBox::CSmartComboBox()
{
}

CSmartComboBox::~CSmartComboBox()
{
}

void CSmartComboBox::SetPairs(Pair* Pairs, int Count)
{
    for (int i = 0; i < Count; ++i)
		AddItem(Pairs[i].Code, Pairs[i].String);
}

void CSmartComboBox::AddItem(int Code, const char* Str)
{
    int index = AddString(Str);
	SetItemData(index, Code);
}

int CSmartComboBox::GetValue() const
{
    int Index = GetCurSel();
	return GetItemData(Index);}

void CSmartComboBox::SetValue(int Value)
{
    for (int i = 0; i < GetCount(); ++i)
    {
		int ItemData = GetItemData(i);
		if(ItemData == Value)
        {
            SetCurSel(i);
            return;
        }
    }
}

void CSmartComboBox::UpdateList(const std::vector<std::string> & StringVector, const char * CurrentStr /*= ""*/)
{
    for (unsigned int i=0; i < StringVector.size(); ++i)
    {
        if (StringVector[i] != CurrentStr)
            AddString(StringVector[i].c_str());
    }
}

void CSmartComboBox::LoadFromConfiguration(const char* Section, const char* Key)
{
    std::vector<std::string> StringVector;
    GetConfigStringVector(Section, Key, StringVector);
    ResetContent();
    const std::string CurrentStr = std::string("__") + Key + "ThatShouldNotBeFound__";
    UpdateList(StringVector, CurrentStr.c_str());
}

void CSmartComboBox::GetStringList(std::vector<std::string> & StringVector)
{
    for (int i = 0; i < GetCount(); ++i)
    {
        CString CurrentStr;
        GetLBText(i, CurrentStr);
        StringVector.push_back(std::string(CurrentStr));
    }
}
void CSmartComboBox::WriteToConfiguration(const char* Section, const char* Key)
{
    std::vector<std::string> StringVector;
    GetStringList(StringVector);
    WriteConfigStringVector(Section, Key, StringVector);
}

void CSmartComboBox::UpdateLru()
{
    CString Text;
    GetWindowText(Text);
    if (Text == m_LastText)
        return;
    m_LastText = Text;

    std::vector<std::string> StringVector;
    GetStringList(StringVector);
    ResetContent();
    SetWindowText(Text);
    AddString(Text);
    UpdateList(StringVector, Text);
}

BEGIN_MESSAGE_MAP(CSmartComboBox, CComboBox)
	//{{AFX_MSG_MAP(CSmartComboBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmartComboBox message handlers
