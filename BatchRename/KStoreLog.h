#include "pch.h"

#ifndef _K_APPLICATION_LOG_MODULE_H_
#define _K_APPLICATION_LOG_MODULE_H_

#ifdef _USE_SETTING_FILES
int FindLFSeparateStringT(LPTSTR paths, LPCTSTR path);

LPSTR AddSettingKeyValue(LPSTR buff, int &buff_length, const char *setting_key, const char *value);
BOOL StoreSettingData(LPTSTR setting_file, const char *buff);

void StoreSettingKeyValue(LPTSTR filename, const char *setting_key, int value);
void StoreSettingKeyValue(LPTSTR filename, const char *setting_key, const char *value);

BOOL ReadSettingKeyValue(TCHAR *filename, const char *setting_key, char *buff, int buff_size);

BOOL MakeNextSequenceName(int max_length, LPTSTR candidate);
#endif

#define SYNC_MSG_LOG 1
#define SYNC_MSG_HISTORY 2
#define SYNC_MSG_DONE 4
#define SYNC_MSG_NO_GUI 8
#define SYNC_MSG_SERVICE_LOG 0x10
#define SYNC_MSG_DEBUG 0x20

#define SYNC_EVENT_ERROR 0x0100
#define SYNC_EVENT_WARN 0x0200
#define SYNC_EVENT_SUCCESS 0x0400

void CloseLogFile();
void ClearExpiredLog();

BOOL StoreLogHistory(LPCTSTR module_name, HRESULT hr, DWORD type);

BOOL StoreLogHistory(LPCSTR module_name, HRESULT hr, DWORD type);

BOOL StoreLogHistory(LPCTSTR module_name, LPCTSTR label, HRESULT hr, DWORD type);

BOOL StoreLogHistory(LPCSTR module_name, LPCSTR label, HRESULT hr, DWORD type);

BOOL StoreLogHistory(LPCTSTR module_name, LPCTSTR msg, DWORD type);

BOOL StoreLogHistory(LPCSTR module_name, LPCSTR msg, DWORD type);

void StoreResultLog(LPCSTR function, LPCSTR title, HRESULT hr);
void StoreResultLog(LPCSTR function, LPCTSTR title, HRESULT hr);

#endif

