#pragma once

#ifdef RESOURCE_EXPORTS
#define RESOURCE_EXPORTS_API __declspec(dllexport)
#else
#define RESOURCE_EXPORTS_API __declspec(dllimport)
#endif

void RESOURCE_EXPORTS_API ResourceDummyFunc();

void RESOURCE_EXPORTS_API DumpResources();
typedef void (*DumpResourcesTraceOutput)(int Severity, const char* Text);
void RESOURCE_EXPORTS_API SetDumpResourcesTraceOutput(DumpResourcesTraceOutput TheOutput);

static const char* RESOURCE_DLL_NAME = "AppResources.dll";

#define IDD_ABOUTBOX                    100
#define IDD_DAL_DIALOG                  102
#define IDI_DAL                         129
#define IDC_TRACE_LIST                  1000
#define IDD_SEARCH_DIALOG               3002
