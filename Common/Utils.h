// Utils.h: interface for the Utils class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <algorithm>
#pragma warning (push)
#pragma warning (disable: 4702)
#include <vector>
#include <map>
#include <set>
#pragma warning (pop)
#include <afxcmn.h>
#include <wchar.h>

#include "LogEvent.h"

#define countof(_array) ARRAYSIZE(_array)
#define class_sizeof(_class, _member) sizeof(((_class *)0)->_member)
#define class_countof(_class, _array) (class_sizeof(_class, _array)/class_sizeof(_class, _array[0]))

#define member_sizeof(_class, _member) sizeof(RTL_FIELD_TYPE(_class, _member))
#define member_countof(_class, _array) RTL_NUMBER_OF_FIELD(_class, _array)

#define IsCharValue(x) (x >= SCHAR_MIN  &&  x <= SCHAR_MAX)
#define IsByteValue(x) (x >= 0  &&  x <= UCHAR_MAX)
#define IsShortValue(x) (x >= SHRT_MIN  &&  x <= SHRT_MAX)
#define IsUshortValue(x) (x >= 0  &&  x <= USHRT_MAX)

#define ___DEFINE_CPP_OVERLOAD_FUNC_0_0(_ReturnType, _FuncName, _DstType, _Dst) \
    template <size_t _Size> \
    inline _ReturnType _FuncName(_DstType (&_Dst)[_Size]) \
{ \
    return _FuncName(_Dst, _Size); \
}
#define ___DEFINE_CPP_OVERLOAD_FUNC_0_1(_ReturnType, _FuncName, _DstType, _Dst, _TType1, _TArg1) \
    template <size_t _Size> \
    inline _ReturnType _FuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1) \
{ \
    return _FuncName(_Dst, _Size, _TArg1); \
}
#define ___DEFINE_CPP_OVERLOAD_FUNC_0_2(_ReturnType, _FuncName, _DstType, _Dst, _TType1, _TArg1, _TType2, _TArg2) \
    template <size_t _Size> \
    inline _ReturnType _FuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1, _TType2 _TArg2) \
{ \
    return _FuncName(_Dst, _Size, _TArg1, _TArg2); \
}
#define ___DEFINE_CPP_OVERLOAD_FUNC_0_3(_ReturnType, _FuncName, _DstType, _Dst, _TType1, _TArg1, _TType2, _TArg2, _TType3, _TArg3) \
    template <size_t _Size> \
    inline _ReturnType _FuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1, _TType2 _TArg2, _TType3 _TArg3) \
{ \
    return _FuncName(_Dst, _Size, _TArg1, _TArg2, _TArg3); \
}
#define ___DEFINE_CPP_OVERLOAD_FUNC_1_0(_ReturnType, _FuncName, _HType1, _HArg1, _DstType, _Dst) \
    template <size_t _Size> \
    inline _ReturnType _FuncName(_HType1 _HArg1, _DstType (&_Dst)[_Size]) \
{ \
    return _FuncName(_HArg1, _Dst, _Size); \
}
#define ___DEFINE_CPP_OVERLOAD_FUNC_1_1(_ReturnType, _FuncName, _HType1, _HArg1, _DstType, _Dst, _TType1, _TArg1) \
    template <size_t _Size> \
    inline _ReturnType _FuncName(_HType1 _HArg1, _DstType (&_Dst)[_Size], _TType1 _TArg1) \
{ \
    return _FuncName(_HArg1, _Dst, _Size, _TArg1); \
}
#define ___DEFINE_CPP_OVERLOAD_FUNC_2_0(_ReturnType, _FuncName, _HType1, _HArg1, _HType2, _HArg2, _DstType, _Dst) \
    template <size_t _Size> \
    inline _ReturnType _FuncName(_HType1 _HArg1, _HType2 _HArg2, _DstType (&_Dst)[_Size]) \
{ \
    return _FuncName(_HArg1, _HArg2, _Dst, _Size); \
}
#define ___DEFINE_CPP_OVERLOAD_FUNC_2_1(_ReturnType, _FuncName, _HType1, _HArg1, _HType2, _HArg2, _DstType, _Dst, _TType1, _TArg1) \
    template <size_t _Size> \
    inline _ReturnType _FuncName(_HType1 _HArg1, _HType2 _HArg2, _DstType (&_Dst)[_Size], _TType1 _TArg1) \
{ \
    return _FuncName(_HArg1, _HArg2, _Dst, _Size, _TArg1); \
}
#define ___DEFINE_CPP_OVERLOAD_FUNC_3_0(_ReturnType, _FuncName, _HType1, _HArg1, _HType2, _HArg2, _HType3, _HArg3, _DstType, _Dst) \
    template <size_t _Size> \
    inline _ReturnType _FuncName(_HType1 _HArg1, _HType2 _HArg2, _HType3 _HArg3, _DstType (&_Dst)[_Size]) \
{ \
    return _FuncName(_HArg1, _HArg2, _HArg3, _Dst, _Size); \
}
#define ___DEFINE_CPP_OVERLOAD_FUNC_0_4(_ReturnType, _FuncName, _DstType, _Dst, _TType1, _TArg1, _TType2, _TArg2, _TType3, _TArg3, _TType4, _TArg4) \
    template <size_t _Size> \
    inline \
    _ReturnType _FuncName(_DstType (&_Dst)[_Size], _TType1 _TArg1, _TType2 _TArg2, _TType3 _TArg3, _TType4 _TArg4) \
{ \
    return _FuncName(_Dst, _Size, _TArg1, _TArg2, _TArg3, _TArg4); \
}


// Copy a string up to a length and put null at the end
void StrCopy(char* Dest, const char* Source, int Size);

#define StrMinCopy2CharArray(/*char[]*/Target, /*char**/Source) \
    StrCopy(Target, Source, min(sizeof Target, (int)strlen(Source)+1))

#define StrCopy2CharArray(/*char[]*/Target, /*char**/Source) \
    StrCopy(Target, Source, sizeof Target)

#define Str2CharArray(/*char[]*/Target, /*std::string*/Source) \
    StrCopy(Target, Source.c_str(), sizeof Target)

//to be used for printing enum values.
#define RETURN_TYPE_STR(TypeParam) \
        case TypeParam:  \
            return  #TypeParam;
#define RETURN_TYPE_STR1(Prefix, TypeParam) \
        case Prefix ## TypeParam:  \
        return  #TypeParam;
#define RETURN_TYPE_STR2(Prefix, TypeParam, Suffix) \
        case Prefix ## TypeParam ## Suffix:  \
        return  #TypeParam;

#define CHECK_FUNCTION_TIME(exp) \
    DWORD Start = GetTickCount(); \
    ((void)(exp)); \
    DWORD End = GetTickCount(); \
    LogEvent(LE_INFO, "Running " #exp " took %d milli", End - Start);

void DumpData(ELogSeverity Severity, const char* Caption, const void* Data, int DataSize, bool HideNonPrintables = true);

void DumpData(const char* Caption, const void* Data, int DataSize, bool HideNonPrintables = true);

bool CompareNoCase(const std::string& Value, const char* Key);
bool SplitString(const char* StrToSplit, const char* Delimiter, 
                 std::string& FirstPart, std::string& SecondPart);

bool FindParamValue(const char* StrToSearchOn, const char* ParamName, 
                    const char* ParamsDelimiter, const std::string& LogPrefix,
                    std::string& ValueStr, ELogSeverity Severity = LE_INFO);
bool FindParamIntValue(const char* StrToSearchOn, const char* ParamName,
                       const char* ParamsDelimiter, const std::string& LogPrefix,
                       int& Value);
void FindParamIntValueWithDefault(const char* StrToSearchOn, const char* ParamName,
                                  const char* ParamsDelimiter, const std::string& LogPrefix,
                                  int& Value, int DefaultValue);
bool FindParamBoolValue(const char* StrToSearchOn, const char* ParamName,
                        const char* ParamsDelimiter, const std::string& LogPrefix,
                        bool& Value);
void FindParamBoolValueWithDefault(const char* StrToSearchOn, const char* ParamName,
                                  const char* ParamsDelimiter, const std::string& LogPrefix,
                                  bool& Value, bool DefaultValue);

void GetStringTokens(const std::string& String, std::vector<std::string>& Tokens,
                     const std::string& Delimiters = " ");

bool IsValidIp(const char* Ip, bool ShowMessageBox = false);
std::string GetHostName();
std::string GetHostAddress();
DWORD GetHostAddressAsDWORD();
bool GetStandardHostIp(const char* HostName, std::string & StandardHostAddress);
std::string GetStandardIpFromDWORD(DWORD IpInDWORD); // The IpInDWORD should be network order.
DWORD GetHostIPAsDWORD(const char* HostName);
bool CompareHostNames(const char* NameLeft, const char* NameRight);

int sign(int Value);
char HexDigit(int Value);
unsigned char HexValue(char Digit);
std::string HexPrint(const UCHAR* Buffer, UINT BufferLength, UINT LineLength = 32, bool AddSpaces = true);

#define BufferToCharArray(data, len, szDest) \
    BufferToString(data, len, szDest, sizeof szDest)

void BufferToString(
    const void* Data, UINT DataLength, TCHAR* szDest, UINT DestLength);
void StringToBuffer(const TCHAR* Str, BYTE* Data, UINT& len);
int HexStrPartToInt(const char* data, int len);

int ConvertHexToData(
    char* const Target, const int MaxTargetSize,
    const char* const HexSource, const int HexSourceSize);

void StringToWString(const std::string& Src, std::wstring& Target);
void InvertStr(const std::string& Str, std::string& InvertedStr);


bool CompareBuffers(const BYTE* Buffer1, const BYTE* Buffer2, int LengthToCompare);

// Wait for the specified time while pumping messages
void WaitAndPumpMessages(DWORD WaitTimeInMilli);
bool OpenOrSaveFile(CString& FileName, bool Open, const char* Filter);
bool IsFileExists(const char* FileName);

std::string GetFullPathName(const std::string FileName);
bool DoCreateDir(LPCSTR Dir);
CString FormatCurrentTime(SYSTEMTIME* InLocalTime = NULL);

CString GetProgramFullName();
std::string GetFileFolder(const char* FileNamePath);  //returns the path of the folder containing the file
CString GetProgramPath();
void LogEventToDebugOutput(const char* Format, ...);

//Include #define _USE_32BIT_TIME_T in stdafx.h or face an unresolved external error
void GetSecondsAndMillis(time_t& Seconds, int& Millis);
std::string FormatTime(time_t Seconds, int Millis);
time_t ConvertFileTimeToTime(const FILETIME& FileTime);

bool ReadConfigArray(const CString Section, const char* KeyBaseStr,
                     int Array[], int Size, int& ActualSize);

bool WriteConfigArray(const CString Section, const char* KeyBaseStr, 
                      const int Array[], int Size);

void PrintData(const unsigned char* Data, int Size, CString Str="");

void CreateIntString(const int Array[], int Size, CString& Str);

CString GetTimeStr(time_t Time);

class CFileFinder
{
public:
    CFileFinder();
    ~CFileFinder();
    bool GetNextFile(LPCSTR FileName, WIN32_FIND_DATA* pFindData);
    void Close();
private:
    HANDLE m_hFind;
    CString m_FileName;
    CFileFinder(const CFileFinder &); // no implementation
    const CFileFinder & operator =(const CFileFinder &); // no implementation
};

std::vector<std::string> CollectFileName(LPCSTR FileName, bool DirOnly);
//
enum EConvertMonoToStereoType
{
    STEREO_BOTH_CHANNELS,
    STEREO_FIRST_CHANNEL_EMPTY,
    STEREO_SECOND_CHANNEL_EMPTY
};

void Mono16BitLinearToStereo(const short* MonoBuffer, int MonoBufferSize,
                             short* StereoBuffer, EConvertMonoToStereoType ConvertType);

void Mono8BitLinearToStereo(const BYTE* MonoBuffer, int MonoBufferSize,
                            BYTE* StereoBuffer, EConvertMonoToStereoType ConvertType);

enum
{
    MONO_8_BIT_VOLUME_SHIFT = 128
};

bool SplitStereo16BitBuffer(const BYTE* StereoBuffer, int StereoBufferSize, 
                             BYTE* LeftChannelBuffer, BYTE* RightChannelBuffer);

void SetAudioVolume(const BYTE* SourceAudio, BYTE* DestinationAudio, 
                    int AudioSizeInBytes, 
                    int BitsPerSample, 
                    int Volume //0 -100
                    ); 

// helper function for converting formats of IP and handling IP control
std::string IpByteFormat2Str(BYTE (&IpByteFormat)[4]);
bool IpStrFormat2IpByteFormat(const char* IpStrFormat, BYTE (&OutIpByteFormat)[4]);

//DWORD is in Network Order
bool IpStr2DWORD(const char* IpStrFormat, DWORD& DwordIp);
std::string DwordIpFormat2Str(DWORD DwordIp);

void SetIpCtlAddress(CIPAddressCtrl& Ctl, const char* IpStr);
std::string GetIpCtlAddress(CIPAddressCtrl& Ctl);
bool IsValidIpAddress(const char* Ip);

//The GetAllFiles function retrieves the file names found under a
//specific path with a pattern. Can do it recursively.
// Pattern can be any wild card. e.g "*.wav"
// Retuen status false means that the directory was not found.
bool GetAllFiles(const char* BasePath, bool Recursive,
                 std::vector<std::string>& FileNames, const char* Pattern = "*");

void PrintWin32Error(DWORD ErrorCode);
std::string GetWin32Error(DWORD ErrorCode);
std::string GetSystemErrorString(DWORD Error);
void LogGetLastError(const char* Context);

// Converts a boolean value to a TRUE or FALSE string
const char* BooleanStr(bool Val, bool Uppercase = false);
bool IsNumber(const char* Str);
bool IsNumeric(const char* Str); //return true for integral and floating numbers
std::string IntToStr(int Value);

// Parses a string of the format "n1,...,n2-n3,..." to a vector of all the numbers defined by the string
void ParseIntVectorString(const char* Str, std::vector<int> & IntVector);
std::string IntVectorToStr(const std::vector<int>& IntVector, bool UseSpaces = false);
std::string IntArrayToStr(const int Vals[], size_t NumVals);
___DEFINE_CPP_OVERLOAD_FUNC_0_0(std::string, IntArrayToStr, const int, _Dest)
void TraceIntVector(ELogSeverity Severity, const char* Context, const char* Lable, const std::vector<int>& IntVector);
void TraceIntArray(ELogSeverity Severity, const char* Context, const char* Lable, const int Vals[], size_t NumVals);
___DEFINE_CPP_OVERLOAD_FUNC_3_0(void, TraceIntArray, ELogSeverity, _Severity, const char*, _Context, const char*, _Lable, const int, _Dest)
std::string FloatToStr(float Value, int Precision = -1);
std::string FloatArrayToStr(const float FloatArray[], size_t FloatArraySize, int Precision = -1, bool UseSpaces = false);
___DEFINE_CPP_OVERLOAD_FUNC_0_2(std::string, FloatArrayToStr, const float, _Dest, int, _Precision, bool, _UseSpaces)
___DEFINE_CPP_OVERLOAD_FUNC_0_1(std::string, FloatArrayToStr, const float, _Dest, int, _Precision)
___DEFINE_CPP_OVERLOAD_FUNC_0_0(std::string, FloatArrayToStr, const float, _Dest)

int ThreadPriorityStr2Value(const char* Priority);
int PriorityClassStr2Value(const char* Priority);

bool FileExtensionIs(const char* const FileName, const char* const Extension);
void DeleteOldFiles(const char* WorkingDirectory, const char* WildCardCriteria, const char* FileExtention, ULARGE_INTEGER MaxTotalSize, int MaxFileLiveTime = INT_MAX, const char* ActiveFileName = "");
void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName);

enum EThreeState
{
    TRS_UNINITIALIZED = -1,
    TRS_FALSE = 0,
    TRS_TRUE = 1
};

bool AreFilesEqual(const char* LeftFileName, const char* RightFileName);

//DataPtr is In and Out parameter pointing to the current data position and being incremented
//DataToGet is the type of data to retrieve from the buffer
template <typename TypeToRetrieve>
bool GetDataFromBuffer(const char* ContextStr,
                       const BYTE* DataOrigin, int DataSize,
                       const BYTE*& DataPtr, TypeToRetrieve& DataToGet)
{
    if(((DataPtr + sizeof(TypeToRetrieve)) - DataOrigin) > DataSize) 
    { 
        LogEvent(LE_ERROR, "%s::GetDataFromBuffer, pass the end of the buffer",
            ContextStr); 
        return false; 
    } 
    DataToGet = *((TypeToRetrieve*)DataPtr); 
    DataPtr += sizeof(TypeToRetrieve);
    return true;
}

bool GetStrFromBuffer(const char* ContextStr,
                      const BYTE* DataOrigin, int DataSize,
                      const BYTE*& DataPtr, std::string & StrToGet);

void flip_bytes(void* pv, size_t len);
double SwitchDoubleBigendianAndLittleEndian(double Val);

int GreatestCommonDivisor(int X, int Y); // GCD
int LeastCommonMultiple(int X, int Y); // LCM

inline bool IsLeapYear(int Year)
{
    return (Year % 4 == 0  &&  Year % 100 != 0)  ||  Year % 400 == 0;
}

time_t mkgmtime(struct tm* _tm); // Same as mktime, but accept gmt time instead of local time

std::string ExceptionCodeToString(UINT Code);
inline int GetExceptionCode(unsigned int* pCode, LPEXCEPTION_POINTERS pEx)
{
    *pCode = pEx->ExceptionRecord->ExceptionCode;
    return EXCEPTION_EXECUTE_HANDLER;
}

std::string VarType2Str(VARTYPE Type);

std::wstring Str2Wchar(LPCSTR Str, size_t sizeInChars = 0, UINT CodePage = CP_ACP);
std::string Wchar2Str(LPCWSTR Wstr, size_t sizeInWchars = 0, UINT CodePage = CP_ACP);

std::string GetCommandLineArg(int Arg);

std::string GetStringValue(const char* StringToParse, const char* Key, const char* Default = "");
int GetIntValue(const char* StringToParse, const char* Key, int Default = 0);
double GetDoubleValue(const char* StringToParse, const char* Key, double Default = 0);

void AddStringValue(std::string &String, const char* Key, std::string Value);
void AddIntValue(std::string &String, const char* Key, int Value);
void AddDoubleValue(std::string &String, const char* Key, double Value, int NumberOfDigitsAfterDecimalPoint = 10);

template<class T> std::string VectorToStr(const std::vector<T>& Vector, bool UseSpaces = false)
{
    const char* Separator1 = UseSpaces ? ", " : ",";

    size_t VectorSize = Vector.size();
    std::string Str;
    for (size_t i = 0; i < VectorSize; ++i)
    {
        const T& Current = Vector[i];
        if (i != 0)
        {
            Str += Separator1;
        }
        
        Str += Current.ToString();
    }

    return Str;
}


class CTokenParser
{
public:
    CTokenParser(const char* Str);
	CTokenParser(CTokenParser &TokenParser);
    std::string GetNextToken(const char* Delimiters, bool Trim = false);
    bool MoreTokens() const;
	void CopyParams(const char* &TheString, int &CurrentPlace, int &Length);

private:
    const char* m_TheString;
    int         m_CurrentPlace;
    int         m_Length;
};

class CStopper
{
public:
    CStopper();
    bool Start(bool Force = true);
    bool Stop();
    bool Continue();
    double GetLapsedTimeInMicro();
private:
    static LONGLONG GetFreqency();

    bool m_IsRunning;
    LARGE_INTEGER m_StartTime;
    double m_LapsedTimeInMicro;
};


inline const char* OurItoa(int Val, char* Str, size_t StrSize, int Radix)
{
    errno_t err = _itoa_s(Val, Str, StrSize, Radix);
    if (err == 0)
        return Str;
    else
        return "!!Error!!";
}
___DEFINE_CPP_OVERLOAD_FUNC_1_1(const char*, OurItoa, int, _Value, char, _Dest, int, _Radix)

inline const char* OurUitoa(unsigned int Val, char* Str, size_t StrSize, int Radix)
{
    errno_t err = _ultoa_s(Val, Str, StrSize, Radix);
    if (err == 0)
        return Str;
    else
        return "!!Error!!";
}
___DEFINE_CPP_OVERLOAD_FUNC_1_1(const char*, OurUitoa, unsigned int, _Value, char, _Dest, int, _Radix)

inline const char* OurItoa64(__int64 Val, char* Str, size_t StrSize, int Radix)
{
    errno_t err = _i64toa_s(Val, Str, StrSize, Radix);
    if (err == 0)
        return Str;
    else
        return "!!Error!!";
}
___DEFINE_CPP_OVERLOAD_FUNC_1_1(const char*, OurItoa64, __int64, _Value, char, _Dest, int, _Radix)

inline const char* OurUitoa64(unsigned __int64 Val, char* Str, size_t StrSize, int Radix)
{
    errno_t err = _ui64toa_s(Val, Str, StrSize, Radix);
    if (err == 0)
        return Str;
    else
        return "!!Error!!";
}
___DEFINE_CPP_OVERLOAD_FUNC_1_1(const char*, OurUitoa64, unsigned __int64, _Value, char, _Dest, int, _Radix)


inline const char* OurFcvt(char* Buffer, size_t SizeInBytes,
                            double Value, int Count, int* Dec, int* Sign)
{
    errno_t err = _fcvt_s(Buffer, SizeInBytes, Value, Count, Dec, Sign);
    if (err == 0)
        return Buffer;
    else
        return "!!Error!!";
}
___DEFINE_CPP_OVERLOAD_FUNC_0_4(const char*, OurFcvt, char, _Dest, double, _Value, int, _Count, int*, _Dec, int*, _Sign)


bool GetLogicalDrives(std::vector<std::string>& StrVector, UINT Type, bool IgnoreSubsts = true);
const char* DriveTypeToString(UINT Type);
double GetDriveFreeSpacePercentage(const char* Drive);

class CMemoryInfo
{
public:
    CMemoryInfo(UCHAR Unit = 'K'); // Legal vales: B, K or M. Other values will treated like B
    bool GetInfo(ULONG& MemUsage, ULONG& VMSize);
private:
    HANDLE m_hProcess;
    UINT m_Divisor;
};

class CCpuUsage
{
public:
    CCpuUsage();
    double GetAverageCpuUsage();
private:
    typedef LONG (WINAPI* PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
    PROCNTQSI m_NtQuerySystemInformation;
    int m_NumberOfProcessors;
    double m_OldIdleTime;
    double m_OldSystemTime;
};


class IMultimediaTimerEvent
{
public:
    virtual void OnMultimediaTimeout(DWORD UserData) = 0;
};

class CMultimediaTimer
{
public:
    CMultimediaTimer(UINT ResolutionInMilli = 1);
    ~CMultimediaTimer();

    enum ETimerType { OneShotTimer, PeriodicTimer };
    bool SetTimer(UINT DelayInMilli, ETimerType TimerType, DWORD UserData, IMultimediaTimerEvent* Sink);
    bool KillTimer(bool Force = false);

private:
    static void CALLBACK TimerFunc(UINT TimerID, UINT /*msg*/, DWORD dwUser, DWORD /*dw1*/, DWORD /*dw2*/);

    struct EventData
    {
        DWORD UserData;
        IMultimediaTimerEvent* Sink;
    };
    EventData m_EventData;
    UINT m_TimerRes;
    DWORD m_TimerID;
};


template <typename _T>
void CompareVectors(const std::vector<_T>& Src, const std::vector<_T>& Other, std::vector<_T>& Added, std::vector<_T>& Removed)
{
    Added.clear();
    Removed.clear();

    if (Src.size() == 0)
    {
        Added = Other;
        return;
    }
    if(Other.size() == 0)
    {
        Removed = Src;
        return;
    }
    size_t SrcSize = Src.size();
    size_t OtherSize = Other.size();
    std::set<_T> SrcSet;
    for (size_t i = 0; i < SrcSize; ++i)
        SrcSet.insert(Src[i]);
    for (size_t i = 0; i < OtherSize; ++i)
    {
        const _T& t = Other[i];
        std::set<_T>::iterator Iter = SrcSet.find(t);
        if (Iter == SrcSet.end())
            Added.push_back(t);
        else
            SrcSet.erase(Iter);
    }
    for (std::set<_T>::const_iterator Iter = SrcSet.begin(); Iter != SrcSet.end(); ++Iter)
        Removed.push_back(*Iter);
}