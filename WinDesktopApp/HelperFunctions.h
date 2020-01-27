#pragma once

template<class T> void SafeDelete(T*& pVal)
{
    delete pVal;
    pVal = nullptr;
}

template<class T> void SafeDeleteArray(T*& pVal)
{
    delete[] pVal;
    pVal = nullptr;
}

template<class T> void SafeReleaseAndNull(T*& pVal)
{
    if(pVal)
    { 
        pVal->Release(); 
        pVal = nullptr; 
    }
}

template<class T> void SafeShutdounAndDelete(T*& pVal)
{
    if(pVal)
    { 
        pVal->Shutdown();
        delete pVal;
        pVal = nullptr;
    }
}
