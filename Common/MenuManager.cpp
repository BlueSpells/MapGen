// MenuManager.cpp: implementation of the CMenuManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MenuManager.h"
#include "LogEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// class CMenuManager

    // Called by the main application
void CMenuManager::SetMainMenu(CMenu* MainMenu)
{
    m_MainMenu.Attach(MainMenu->GetSafeHmenu());
}

bool CMenuManager::OnMenuSelected(int Id)
{
    ItemIterator Iter = m_Items.find(Id);
    if (Iter == m_Items.end())
        return false;

    ItemData& Data = Iter->second;
    Data.Handler->OnMenuSelected(Data.OriginalId);
    return true;
}

bool CMenuManager::AddMenuItem(
    IMenuEventHandler* Handler,
    const char* PopupMenuText, 
    const char* SubMenuText, 
    int Id
)
{
    if (!MenuExists("AddMenuItem"))
        return false;

    if (m_FirstItem)
    {
        AddSeparator();
        m_FirstItem = false;
    }

    CMenu* CurrentMenu = &m_MainMenu;
    int PopupMenuIndex = -1;
    if (PopupMenuText != NULL) // Search for the popup menu 
        CurrentMenu = SearchPopupMenu(PopupMenuText, PopupMenuIndex);

    if (CurrentMenu == NULL)
    {
        LogEvent(LE_ERROR, "CMenuManager::AddMenuItem: Could not create %s/%s", 
            PopupMenuText, SubMenuText);
        return false;
    }

    CurrentMenu->AppendMenu(MF_STRING | MF_ENABLED, m_CurrentMenuId, SubMenuText);
    if (!AddItemToItemMap(PopupMenuIndex, m_CurrentMenuId, Handler, Id))
        return false;

    ++m_CurrentMenuId;
    return true;
}

bool CMenuManager::AddSeparator(const char* PopupMenuText)
{
    if (!MenuExists("AddMenuItem"))
        return false;

    CMenu* CurrentMenu = &m_MainMenu;
    int PopupMenuIndex = -1;
    if (PopupMenuText != NULL) // Search for the popup menu 
        CurrentMenu = SearchPopupMenu(PopupMenuText, PopupMenuIndex);

	if(CurrentMenu == NULL)
		return false;

	CurrentMenu->AppendMenu(MF_SEPARATOR);
    return true;
}

bool CMenuManager::CheckMenuItem(
    IMenuEventHandler* Handler,
    int Id,
    bool On
)
{
    // Search for entry
    ItemIterator Iter = m_Items.begin();
    ItemIterator End = m_Items.end();
    for (; Iter != End; ++Iter)
    {
        ItemData& Data = Iter->second;
        if (Data.Handler == Handler && Data.OriginalId == Id)
        {
            int Id = Iter->first;
            int PopupMenuIndex = Data.PopupMenuIndex;
            CMenu* CurrentMenu = &m_MainMenu;
            if (PopupMenuIndex != -1)
                CurrentMenu = m_MainMenu.GetSubMenu(PopupMenuIndex);
            CurrentMenu->CheckMenuItem(
                Id, MF_BYCOMMAND | (On ? MF_CHECKED : MF_UNCHECKED));
            return true;
        }
    }
    LogEvent(LE_ERROR, 
        "CMenuManager::CheckMenuItem: Item 0x%x %d was not found", Handler, Id);
    return false;
}

CMenuManager& CMenuManager::GetTheMenuManager()
{
    static CMenuManager TheMenuManager;
    return TheMenuManager;
}

// private methods

 // This is a single-tone so we hide the constructor
CMenuManager::CMenuManager() : 
    m_FirstItem(true), m_CurrentMenuId(0x1000)
{
}

bool CMenuManager::MenuExists(const char* Caption)
{
    if (m_MainMenu != NULL)
        return true;

    LogEvent(LE_ERROR, "CMenuManager::%s: NULL Menu", Caption);
    return false;
}

bool CMenuManager::AddItemToItemMap(
    int PopupMenuIndex, int Id, IMenuEventHandler* Handler, int OriginalId)
{
    ItemData Data = { PopupMenuIndex, Handler, OriginalId };
    ItemMap::value_type Value(Id, Data);
    std::pair<ItemIterator, bool> Result = m_Items.insert(Value);
    if (!Result.second)
    {
        // Should not happen
        LogEvent(LE_ERROR, ": Duplicate Id %d", Id);
        return false;
    }
    return true;
}

CMenu* CMenuManager::SearchPopupMenu(
    const char* PopupMenuText, int& PopupMenuIndex)
{
    CString CurrentMenuText;

    int i;
    for (i = 0; ; ++i)
    {
        UINT State = m_MainMenu.GetMenuState(i, MF_BYPOSITION);
        if (State == (UINT)-1)
            break;

        int Actual = m_MainMenu.GetMenuString(i, CurrentMenuText, MF_BYPOSITION);
        if (Actual != 0 && CurrentMenuText == PopupMenuText)
        {
            PopupMenuIndex = i;
            return m_MainMenu.GetSubMenu(i);
        }
    }
    // Add a new pop-up menu
    HMENU hNewMenu = ::CreatePopupMenu();
    m_MainMenu.AppendMenu(MF_POPUP, (UINT)hNewMenu, PopupMenuText);
    PopupMenuIndex = i;
    return 	CMenu::FromHandle(hNewMenu);
}

//////////////////////////////////////////////////////////////////////////
#ifdef _TEST

enum 
{
    MAIN_EVENT_1 = 0,
    POPUP_EVENT_1,
    POPUP_EVENT_2,
};

class CMenuEventTester: public IMenuEventHandler
{
public:
    CMenuEventTester(): m_Checked(false)
    {
    }
    virtual void OnMenuSelected(int Id)
    {
        switch (Id)
        {
        case MAIN_EVENT_1:
            LogEvent(LE_INFOHIGH, "CMenuEventTester::OnMenuSelected: MAIN_EVENT_1");
            m_Checked = !m_Checked;
            CMenuManager::GetTheMenuManager().CheckMenuItem(this, MAIN_EVENT_1, m_Checked);
            break;
        case POPUP_EVENT_1:
            LogEvent(LE_INFOHIGH, "CMenuEventTester::OnMenuSelected: POPUP_EVENT_1");
            break;
        case POPUP_EVENT_2:
            LogEvent(LE_INFOHIGH, "CMenuEventTester::OnMenuSelected: POPUP_EVENT_2");
            break;
        default:
            LogEvent(LE_INFOHIGH, "CMenuEventTester::OnMenuSelected: Unknown event");
            break;
        }
    }
private:
    bool m_Checked;
};

void TestMenuManager()
{
    CMenuManager& MenuManager = CMenuManager::GetTheMenuManager();
    static CMenuEventTester MenuEventTester;

    // Called by the main application
    void SetMainMenu(CMenu* MainMenu);
    bool OnMenuSelected(int Id);

    MenuManager.AddMenuItem(&MenuEventTester, NULL, "Main Event 1", MAIN_EVENT_1);
    MenuManager.AddMenuItem(&MenuEventTester, "Popup", "Popup Event 1", POPUP_EVENT_1);
    MenuManager.AddMenuItem(&MenuEventTester, "Popup", "Popup Event 2", POPUP_EVENT_2);
}

#endif // _TEST

