#pragma  once

#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include "LogEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#define THIS_FILE __FILE__
#endif


template <class T>
class SmartPtr
{
public:
    SmartPtr<T>(T* Ptr  = NULL): m_ThePtr(NULL), m_RefCount(NULL)
    {
        Create(Ptr);
    }

    SmartPtr<T>(const SmartPtr<T>& Other): m_ThePtr(NULL), m_RefCount(NULL)
    {
        Set(Other);
    }

    ~SmartPtr<T>()
    {
        Release();
    }

    const SmartPtr<T>& operator=(const SmartPtr<T>& Other) throw()
    {
        if(&Other != this)
        {
            Release();
            Set(Other);
        }
        return *this;
    }

    const SmartPtr<T>& operator=(T* Ptr) throw()
    {
        if (IsNull())
        {
            Create(Ptr);
        }
        else if(Ptr != Get())
        {
            Release();
            Create(Ptr);
        }
        return *this;
    }

    bool operator==(const SmartPtr<T>& Other)
    {
        return m_ThePtr == Other.m_ThePtr  &&  m_RefCount == Other.m_RefCount;
    }

    T* operator->() const
    {
        if(IsNull())
        {
            LogEvent(LE_ERROR, "SmartPtr::operator->, uninitialized data");
            Assert(false);
            throw;
        }
        return Get();
    }

    T& operator*() const
    {
        if(IsNull())
        {
            LogEvent(LE_ERROR, "SmartPtr::operator->, uninitialized data");
            Assert(false);
            throw;
        }
        return *Get();
    }

    operator T*() const
    {
        return Get();
    }

    int GetRefCount() const
    {
        if(IsNull())
            return 0;
        return *m_RefCount;
    }

    bool IsNull() const
    {
        return m_ThePtr == NULL;
    }

private:
    void Create(T* Ptr)
    {
        if(Ptr != NULL)
        {
            Assert(_CrtIsValidHeapPointer(Ptr));
            m_ThePtr = Ptr;
            m_RefCount = new RefCountType;
            *m_RefCount = 1;
        }
    }

    void Release()
    {
        if(IsNull())
            return;

        if(InterlockedDecrement(m_RefCount) == 0)
        {
            delete m_ThePtr;
            delete m_RefCount;
        }
        m_ThePtr = NULL;
        m_RefCount = NULL;
    }

    void Set(const SmartPtr<T>& Other)
    {
        m_ThePtr = Other.m_ThePtr;
        m_RefCount = Other.m_RefCount;
        if (m_RefCount != NULL) // check that the pointer is not set to null
        {
            Assert(m_ThePtr != NULL);
            InterlockedIncrement(m_RefCount);
        }
    }

    T* Get() const
    {
        return m_ThePtr;
    }

private:
    T* m_ThePtr;
    typedef LONG RefCountType;
    volatile RefCountType* m_RefCount;
};


#ifdef _DEBUG
#define THIS_FILE __FILE__
#undef new
#endif
