// MenuManager.h: interface for the CMenuManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class IMenuEventHandler
{
public:
    virtual void OnMenuSelected(int Id) = 0;
};

#pragma warning (push)
#pragma warning (disable: 4702)
#include <map>
#pragma warning (pop)

class CMenuManager
{
public:
    // Called by the main application
    void SetMainMenu(CMenu* MainMenu);
    bool OnMenuSelected(int Id);

    bool AddMenuItem(
        IMenuEventHandler* Handler,
        const char* PopupMenuText, 
        const char* SubMenuText, 
        int Id
    );
    bool AddSeparator(const char* PopupMenuText = NULL);

    bool CheckMenuItem(
        IMenuEventHandler* Handler,
        int Id,
        bool On
    );

    static CMenuManager& GetTheMenuManager();
private:
    CMenuManager(); // This is a single-tone so we hide the constructor
    bool MenuExists(const char* Caption);
    bool AddItemToItemMap(
        int PopupMenuIndex, int Id, IMenuEventHandler* Handler, int OriginalId);
    CMenu* SearchPopupMenu(const char* PopupMenuText, int& PopupMenuIndex);

    struct ItemData
    {
        int                PopupMenuIndex;
        IMenuEventHandler* Handler;
        int                OriginalId; // We work with our own id to prevent collision
    };
    typedef std::map<int/*Id*/, ItemData> ItemMap;
    typedef ItemMap::iterator ItemIterator;

    ItemMap m_Items;
    bool    m_FirstItem;
    CMenu   m_MainMenu;
    int     m_CurrentMenuId;
};

