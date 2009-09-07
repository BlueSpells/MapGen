// CriticalSection.h: interface for the CCriticalSection class.
//                    and CCriticalSectionLocker         
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <afxmt.h>

// On its constrctor it locks the section and on its destructor it unlocks
class CCriticalSectionLocker
{
public:
    CCriticalSectionLocker(CCriticalSection& CriticalSection) : 
        m_CriticalSection(CriticalSection)
    {
        m_Locked = m_CriticalSection.Lock() == TRUE;
    }

    void Unlock()
    {
        if(m_Locked)
        {
            m_CriticalSection.Unlock();
            m_Locked = false;
        }
    }

    ~CCriticalSectionLocker()
    {
        Unlock();
    }
private:
    CCriticalSection& m_CriticalSection;
    bool m_Locked;
};

