#include "pch.h"

#include "KConstant.h"
#include "KStoreLog.h"

#include "Platform.h"

#include <tchar.h>
#include <Strsafe.h>
#include <Shlwapi.h>

#ifdef _DEBUG_MEM
#define new DEBUG_NEW
#endif

const char CURRENT_MODULE_NAME_A[] = "VivaRename";
const TCHAR CURRENT_MODULE_NAME[] = _T("VivaRename");

const TCHAR LOG_FOLDER_NAME[] = _T("Log");
const TCHAR LOG_SUBFOLDER_NAME[] = _T("");
const TCHAR LOG_EX_SUBFOLDER_NAME[] = _T("");

BYTE utf8_bom[] = { 0xEF, 0xBB, 0xBF };

#ifdef _USE_SETTING_FILES

int FindLFSeparateStringT(LPTSTR paths, LPCTSTR path)
{
  BOOL found = FALSE;
  if (paths != NULL)
  {
    LPTSTR p = paths;
    LPTSTR e = p + lstrlen(paths);

    while (p < e)
    {
      LPTSTR n = StrChr(p, LF_CHAR);
      if (n != NULL)
      {
        *n = '\0';
        if (StrCmpI(path, p) == 0)
        {
          return (int)(p - paths);
        }
        p = n + 1;
      }
      else
      {
        if (StrCmpI(path, p) == 0)
        {
          return (int)(p - paths);
        }
        p = e;
      }
    }
  }
  return -1;
}

LPSTR AddSettingKeyValue(LPSTR buff, int &buff_length, const char *setting_key, const char *value)
{
  int need_len = (int)(strlen(buff) + strlen(setting_key) + strlen(value) + 20);
  if (need_len > buff_length)
  {
    int incr = need_len - buff_length;
    if(incr < 1024)
      incr = 1024;
    LPSTR nbuff = new char[buff_length + incr];
    memcpy(nbuff, buff, buff_length);
    delete[] buff;
    buff = nbuff;
    buff_length += incr;
  }

  LPSTR d = buff + strlen(buff);
  memcpy(d, setting_key, strlen(setting_key));
  d += strlen(setting_key);
  *(d++) = '=';
  memcpy(d, value, strlen(value));
  d += strlen(value);
  memcpy(d, LF_STR, 1);
  d += 1;
  *d = '\0';
  return buff;
}

BOOL StoreSettingData(LPTSTR setting_file, const char *buff)
{
  MakeWritabeFile(setting_file);
	FILE *f = OpenDelayedFile(setting_file, _T("w"));
  if (f != NULL)
  {
    fwrite(buff, 1, strlen(buff), f);
    fclose(f);
    return TRUE;
  }
  return FALSE;
}

void StoreSettingKeyValue(LPTSTR setting_file, const char *setting_key, const char *value)
{
	FILE *f = OpenDelayedFile(setting_file, _T("r"));
  if (f != NULL)
  {
    char key[64];
    char data[1000];
    BOOL update_need = TRUE;
    BOOL item_exist = FALSE;

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buff = new char[size + 2];
    char *dest = new char[size + 1000];
    char *d = dest;
    if ((buff != NULL) && (dest != NULL))
    {
      if (size > 0)
      {
        size = fread(buff, 1, size, f);
        fclose(f);
        f = NULL;

        buff[size] = '\0';
        char *p = buff;

        // UTF8 encoding signature
        if((size > 3) && (memcmp(p, utf8_bom, 3) == 0))
        {
          p += 3;
          size -= 3;
        }
        while(size > 0)
        {
          // find end-of-line
          char *n = strchr(p, LF_CHAR);
          char *s = strchr(p, '=');
          size_t len = 0;

          if(s == NULL)
            break;

          if(n != NULL)
            len = (n - p + 1);
          else
            len = strlen(p);

          if (item_exist || (n < s))
          {
            memcpy(d, p, len);
            d += len;
            /*
            if ((*p != LF_CHAR) || (len > 1))
            {
              memcpy(d, LF_STR, 1);
              d += 1;
            }
            */
          }
          else
          {
            if ((p != NULL) && (s != NULL))
            {
              strncpy_s(key, 64, p, (size_t)(s - p));
              if (n != NULL)
                strncpy_s(data, 1000, s+1, (size_t)(n - s - 1));
              else
                strcpy_s(data, 1000, s+1);

              if (strcmp(key, setting_key) == 0)
              {
                item_exist = TRUE;
                if (strcmp(value, data) == 0) // no need to update
                {
                  update_need = FALSE;
                  break;
                }
                else
                  strcpy_s(data, 1000, value);
              }

              // copy to new buffer
              memcpy(d, key, strlen(key));
              d += strlen(key);
              *(d++) = '=';
              memcpy(d, data, strlen(data));
              d += strlen(data);
              memcpy(d, LF_STR, 1);
              d += 1;
            }
          }
          p += len;
          size -= len;
        }
      }

      if (update_need)
      {
        if (!item_exist)
        {
            memcpy(d, setting_key, strlen(setting_key));
            d += strlen(setting_key);
            *(d++) = '=';
            memcpy(d, value, strlen(value));
            d += strlen(value);
            memcpy(d, LF_STR, 1);
            d += 1;
        }

        // re-open
        /*
		    errno_t err = _tfopen_s(&f, setting_file, _T("w"));
		    if (err == 0)
        */
        MakeWritabeFile(setting_file);
        f = OpenDelayedFile(setting_file, _T("w"));
        if (f != NULL)
        {
          fwrite(dest, 1, (d - dest), f);
          fclose(f);
          f = NULL;
        }
        else
        {
          OutputDebugString(_T("Cannot open file to write : "));
          OutputDebugString(setting_file);
          OutputDebugString(_T("\n"));
        }
      }

      if (buff != NULL)
        delete[] buff;
      if (dest != NULL)
        delete[] dest;
    }
  }
  else
  {
    /*
  	errno_t err = _tfopen_s(&f, setting_file, _T("w"));
    if (err == 0)
    */
    MakeWritabeFile(setting_file);
    f = OpenDelayedFile(setting_file, _T("w"));
    if (f != NULL)
    {
      fwrite(setting_key, 1, strlen(setting_key), f);
      fwrite("=", 1, 1, f);
      fwrite(value, 1, strlen(value), f);
      fwrite(LF_STR, 1, 1, f);
    }
    else
    {
      OutputDebugString(_T("Cannot create file : "));
      OutputDebugString(setting_file);
      OutputDebugString(_T("\n"));
    }
  }
  if (f != NULL)
    fclose(f);
}

void StoreSettingKeyValue(LPTSTR filename, const char *setting_key, int value)
{
  char str[30];
  sprintf_s(str, 30, "%d", value);
  StoreSettingKeyValue(filename, setting_key, str);
}

/*
Should free returned memory by delete[]
*/
// char *ReadSettingKeyValue(TCHAR *filename, const char *setting_key)

BOOL ReadSettingKeyValue(TCHAR *setting_file, const char *setting_key, char *outbuff, int outbuff_size)
{
  BOOL rtn = FALSE;
  /*
	FILE *f;
	errno_t err = _tfopen_s(&f, setting_file, _T("r"));
	if (err == 0)
  */
  FILE *f = OpenDelayedFile(setting_file, _T("r"));
  if (f != NULL)
  {
    char key[64];
    BOOL update_need = TRUE;

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size > 0)
    {
      char *buff = new char[size + 2];

      if (buff != NULL)
      {
        size = fread(buff, 1, size, f);
        fclose(f);
        f = NULL;
        buff[size] = '\0';
        char *p = buff;

        // UTF8 encoding signature
        if((size > 3) && (memcmp(p, utf8_bom, 3) == 0))
        {
          p += 3;
          size -= 3;
        }
        while(size > 0)
        {
          // find end-of-line
          char *n = strchr(p, LF_CHAR);
          char *s = strchr(p, '=');
          size_t len;
          if(n != NULL)
            len = (n - p + 1);
          else
            len = strlen(p);

          if (s != NULL)
          {
            strncpy_s(key, 64, p, (size_t)(s - p));

            if (strcmp(key, setting_key) == 0)
            {
              size_t value_len;

              if (n != NULL)
                value_len = n - s - 1;
              else
                value_len = strlen(s+1);

              if ((value_len + 1) > (size_t)outbuff_size)
              {
                TCHAR msg[256];
                StringCchPrintf(msg, 256, _T("ReadSettingKeyValue need buffer length %d\n"), value_len+1);
                OutputDebugString(msg);
                value_len = outbuff_size - 1;
              }

              if (value_len > 0)
              {
                memcpy(outbuff, (s + 1), value_len);
                outbuff[value_len] = '\0';
                rtn = TRUE;
              }
              break;
            }

            p += len;
            size -= len;
          }
          else
            break;
        }
      }

      if (buff != NULL)
        delete[] buff;
    }

    if (f != NULL)
      fclose(f);
  }
  else
  {
    DWORD err = GetLastError();
    if (err == 3)
      return rtn;
    if (err != 2)
    {
      TCHAR msg[300];
      StringCchPrintf(msg, 300, _T("fopen error : %d : "), err);
      StringCchCat(msg, 300, setting_file);
      StoreLogHistory(__FUNCTIONW__, msg, SYNC_MSG_LOG);
    }
  }
  return rtn;
}

BOOL MakeNextSequenceName(int max_length, LPTSTR candidate)
{
  int p = (int)lstrlen(candidate) - 1;

  while(p >= 0)
  {
    if (candidate[p] < 'Z')
    {
      ++candidate[p];
      return TRUE;
    }
    else
    {
      candidate[p] = 'A';
    }
    p--;
  }

  // extend length
  p = (int)lstrlen(candidate);
  if (p < max_length)
  {
    candidate[p] = 'A';
    candidate[++p] = '\0';
    return TRUE;
  }
  return FALSE;
}

#endif // _USE_SETTING_FILES


/* 매번 로그를 저장할 때마다 파일을 open/close 하지 않고 프로그램 종료시에만 close한다. */
#define LOGFILE_WITH_NO_CLOSE

#if defined(LOGFILE_WITH_NO_CLOSE)
static HANDLE hLogFile = NULL;
static LPTSTR hLogName = NULL;
#endif

void CloseLogFile()
{
#if defined(LOGFILE_WITH_NO_CLOSE)
  if (hLogFile != NULL)
  {
    CloseLogFileShared(hLogFile);
    hLogFile = NULL;
  }
  if (hLogName != NULL)
  {
    delete[] hLogName;
    hLogName = NULL;
  }
#endif
}

void ClearExpiredLog()
{
  TCHAR filename[KMAX_PATH];

  if (lstrlen(gModulePathName) == 0)
  {
    GetModuleFileName(NULL, gModulePathName, KMAX_PATH);
    LPTSTR e = StrRChr(gModulePathName, NULL, '\\');
    if (e != NULL)
    {
      e++;
      *e = '\0';
    }
  }
  LPCTSTR baseFolder = gModulePathName;
  MakeFullPathName(filename, KMAX_PATH, baseFolder, LOG_FOLDER_NAME);

  if (IsDirectoryExist(filename))
  {
    TCHAR filemask[KMAX_PATH];
    MakeFullPathName(filemask, KMAX_PATH, filename, _T("*"));

    WIN32_FIND_DATA fd;
    memset(&fd, 0, sizeof(WIN32_FIND_DATA));
    fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;

    __int64 fifteen_day_epoch = (__int64)15 * 24 * 60 * 60 * 1000;
    SYSTEMTIME currentTime;
    GetLocalTime(&currentTime);
    __int64 base_epoch = SystemTimeToEPOCH_MS(currentTime.wYear, currentTime.wMonth,
      currentTime.wDay, currentTime.wHour, currentTime.wMinute, currentTime.wSecond) - fifteen_day_epoch;

    // scan with real folder path if it or parent renamed
    HANDLE h = FindFirstFile(filemask, &fd);
    if (h != INVALID_HANDLE_VALUE)
    {
      do
      {
        if (((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) &&
          (lstrlen(fd.cFileName) > 0))
        {
          SYSTEMTIME createTime;
          FileTimeToSystemTime(&fd.ftCreationTime, &createTime);
          __int64 file_epoch = SystemTimeToEPOCH_MS(createTime.wYear, createTime.wMonth,
            createTime.wDay, createTime.wHour, createTime.wMinute, createTime.wSecond);

          if (file_epoch < base_epoch)
          {
            // we can remove this file
            TCHAR fullname[KMAX_PATH];
            MakeFullPathName(fullname, KMAX_PATH, filename, fd.cFileName);
            KDeleteFile(fullname);
          }
        }
      } while (FindNextFile(h, &fd));

      FindClose(h);
    }


  }
}

static BOOL AppendMessageToFile(const TCHAR *filename, char *buff, BOOL bClose)
{
  BOOL r = FALSE;
#if defined(LOGFILE_WITH_NO_CLOSE)
  HANDLE f = NULL;

  if (bClose)
  {
    f = OpenLogFileShared(filename, TRUE);
  }
  else
  {
    // check if filename changed
    if ((hLogName != NULL) && (StrCmpI(filename, hLogName) != 0))
    {
      if (hLogFile != NULL)
      {
        CloseLogFileShared(hLogFile);
        hLogFile = NULL;
      }

      delete[] hLogName;
      hLogName = NULL;
    }

    if (hLogFile == NULL)
    {
      hLogFile = OpenLogFileShared(filename, TRUE);
      if (hLogName != NULL)
        delete[] hLogName;
      hLogName = AllocString(filename);
    }
    f = hLogFile;
  }

  if (f != NULL)
  {
    DWORD written = 0;
    int bufflen = strlen(buff);
    LONG ptr = 0;

    DWORD p = SetFilePointer(f, 0, &ptr, FILE_END);
    if (!WriteFile(f, buff, bufflen, &written, NULL))
    {
      TCHAR msg[64];
      StringCchPrintf(msg, 64, _T("Write log fail : 0x%X\n"), GetLastError());
      OutputDebugString(msg);
    }
    else
    {
      r = TRUE;
    }

    if (bClose)
      CloseLogFileShared(f);
  }
#else
  FILE *f = OpenDelayedFile(filename, _T("a+"));
  if (f != NULL)
  {
    fseek(f, 0, SEEK_END);
    fwrite(buff, 1, strlen(buff), f);
    fclose(f);
    r = TRUE;
  }
#endif
  return r;
}

static void AppendMessageToFile(const TCHAR *filename, TCHAR *buff, BOOL bClose)
{
  char *p = MakeUtf8String(buff);
  if (p != NULL)
  {
    AppendMessageToFile(filename, p, bClose);
    delete[] p;
  }
}

BOOL AppendLogMessageEx(char *buff, SYSTEMTIME &systime, LPCTSTR logFolderName, BOOL bClose)
{
  TCHAR filename[KMAX_PATH];
  BOOL r = FALSE;

  if (lstrlen(gModulePathName) == 0)
  {
    GetModuleFileName(NULL, gModulePathName, KMAX_PATH);
    LPTSTR e = StrRChr(gModulePathName, NULL, '\\');
    if (e != NULL)
    {
      e++;
      *e = '\0';
    }
  }
  LPCTSTR baseFolder = gModulePathName;
  MakeFullPathName(filename, KMAX_PATH, baseFolder, LOG_FOLDER_NAME);

	if (IsDirectoryExist(filename) || CreateDirectory(filename))
  {
#if 0 // do not use sub folder
    MakeFullPathName(filename, KMAX_PATH, filename, logFolderName);
  	if (IsDirectoryExist(filename) || CreateDirectory(filename))
#endif
    {
      // make date filename
      TCHAR name[64];
      StringCchPrintf(name, 64, _T("%d%.2d%.2d.log"), systime.wYear, systime.wMonth, systime.wDay);
      MakeFullPathName(filename, KMAX_PATH, filename, name);
      r = AppendMessageToFile(filename, buff, bClose);
    }
  }
  return r;
}

BOOL AppendLogMessage(char *buff, SYSTEMTIME &systime)
{
  return AppendLogMessageEx(buff, systime, LOG_SUBFOLDER_NAME, FALSE);
}

#if 0
void StoreLogHistory(LPCTSTR module_name, HRESULT hr, DWORD type)
{
  TCHAR msg[256];

  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
    NULL, hr,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR)msg, 256, NULL);
  StoreLogHistory(module_name, msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
}

void StoreLogHistory(LPCTSTR module_name, LPCTSTR msg, DWORD type)
{
  TCHAR *buff;
  char time_str[64];
  int buff_len = lstrlen(msg) + 200;

  buff = new TCHAR[buff_len];
  SYSTEMTIME time;
  GetLocalTime(&time);
  SystemTimeToStringEx(time_str, 64, time); // HH:MM:SS:MSC
  ToUnicodeString(buff, buff_len, time_str, (int)strlen(time_str));

  StringCchCat(buff, buff_len, L",");
  StringCchCat(buff, buff_len, CURRENT_MODULE_NAME);
  StringCchCat(buff, buff_len, L"::");
  StringCchCat(buff, buff_len, module_name);
  
  // system error or warning
  if (type & SYNC_EVENT_ERROR)
    StringCchCat(buff, buff_len, L"[?ER]");
  if (type & SYNC_EVENT_WARN)
    StringCchCat(buff, buff_len, L"[?WN]");
  if (type & SYNC_EVENT_SUCCESS)
    StringCchCat(buff, buff_len, L"[?OK]");

  StringCchCat(buff, buff_len, L",");
  StringCchCat(buff, buff_len, msg);
  StringCchCat(buff, buff_len, L_CRLF_STR);

  if (type & SYNC_MSG_SERVICE_LOG)
  {
    // Log/Service폴더에 로그 저장
    AppendLogMessageEx(buff, time, LOG_EX_SUBFOLDER_NAME, TRUE);
  }
  else if (type & SYNC_MSG_LOG)
  {
    AppendLogMessage(buff, time);
  }

#ifdef _DEBUG_SHOW_LOG
  if (!(type & SYNC_MSG_DEBUG))
  {
    CMainFrame *pMain = (CMainFrame*) AfxGetMainWnd();
    if ((pMain != NULL) && !(type & SYNC_MSG_NO_GUI))
    {
      pMain->AppendOutputString(buff);
    }
  }
#endif // _DEBUG_SHOW_LOG
  if (buff != NULL)
    delete[] buff;

#ifdef _DEBUG
  OutputDebugString(msg);
  OutputDebugString(_T("\n"));
#endif
}
#else

BOOL StoreLogHistory(LPCTSTR module_name, LPCTSTR label, HRESULT hr, DWORD type)
{
  TCHAR msg[256];

  StringCchCopy(msg, 256, label);
  StringCchCat(msg, 256, _T(" : "));
  int len = lstrlen(msg);
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
    NULL, hr,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR)&msg[len], 256 - len, NULL);

  return StoreLogHistory(module_name, msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
}

BOOL StoreLogHistory(LPCSTR module_name, LPCSTR label, HRESULT hr, DWORD type)
{
  char msg[256];

  StringCchCopyA(msg, 256, label);
  StringCchCatA(msg, 256, " : ");
  int len = strlen(msg);

  TCHAR wstr[256];
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
    NULL, hr,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR)wstr, 256, NULL);
  LPSTR cmsg = MakeUtf8String(wstr);
  if (cmsg != NULL)
  {
    StringCchCatA(msg, 256, cmsg);
    delete[] cmsg;
  }

  return StoreLogHistory(module_name, msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
}

BOOL StoreLogHistory(LPCTSTR module_name, HRESULT hr, DWORD type)
{
  TCHAR msg[256];

  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
    NULL, hr,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR)msg, 256, NULL);
  return StoreLogHistory(module_name, msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
}

BOOL StoreLogHistory(LPCSTR module_name, HRESULT hr, DWORD type)
{
  TCHAR msg[256];

  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
    NULL, hr,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR)msg, 256, NULL);
  LPSTR smsg = MakeUtf8String(msg);

  BOOL r = StoreLogHistory(module_name, smsg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
  if (smsg != NULL)
    delete[] smsg;
  return r;
}

BOOL StoreLogHistory(LPCTSTR module_name, LPCTSTR msg, DWORD type)
{
  LPSTR modulestr = MakeUtf8String(module_name);
  LPSTR msgstr = MakeUtf8String(msg);
  BOOL r = StoreLogHistory(modulestr, msgstr, type);
  if (modulestr != NULL)
    delete[] modulestr;
  if (msgstr != NULL)
    delete[] msgstr;
  return r;
}

BOOL StoreLogHistory(LPCSTR module_name, LPCSTR msg, DWORD type)
{
  char* buff;
  int buff_len = 200;
  if (msg != NULL)
    buff_len += strlen(msg);

  buff = new char[buff_len];
  SYSTEMTIME time;
  GetLocalTime(&time);
  SystemTimeToStringEx(buff, 64, time); // HH:MM:SS:MSC

  StringCchCatA(buff, buff_len, ",");
  StringCchCatA(buff, buff_len, CURRENT_MODULE_NAME_A);
  StringCchCatA(buff, buff_len, "::");
  StringCchCatA(buff, buff_len, module_name);

  // system error or warning
  if (type & SYNC_EVENT_ERROR)
    StringCchCatA(buff, buff_len, "[?ER]");
  if (type & SYNC_EVENT_WARN)
    StringCchCatA(buff, buff_len, "[?WN]");

  StringCchCatA(buff, buff_len, ",");
  if (msg != NULL)
    StringCchCatA(buff, buff_len, msg);
  StringCchCatA(buff, buff_len, CRLF_STR);

  BOOL rtn = FALSE;
  if (type & SYNC_MSG_SERVICE_LOG)
  {
    // Log/Service폴더에 로그 저장
    rtn = AppendLogMessageEx(buff, time, LOG_EX_SUBFOLDER_NAME, TRUE);
  }
  else if (type & SYNC_MSG_LOG)
  {
    rtn = AppendLogMessage(buff, time);
  }

#ifndef HISTORY_BY_SQLITE
  if (type & SYNC_MSG_HISTORY)
  {
#ifdef USE_SYNC_ENGINE
    AppendHistoryMessage(buff, time);
#endif
  }
#endif

  // #define _DEBUG_ALL_MESSAGE

#ifdef _DEBUG_SHOW_LOG
  {
    CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
    if ((pMain != NULL) && !(type & SYNC_MSG_NO_GUI))
    {
      LPTSTR tmsg = MakeUnicodeString(msg);
      pMain->AppendOutputString(tmsg);
      delete[] tmsg;
    }
  }
# else
  {
    OutputDebugStringA(msg);
    OutputDebugStringA("\n");
  }
#endif

  if (buff != NULL)
    delete[] buff;
  return rtn;
}

void StoreResultLog(LPCSTR function, LPCSTR title, HRESULT hr)
{
  char msg[256];
  int type = SYNC_MSG_LOG | SYNC_MSG_DEBUG;

  StringCchCopyA(msg, 256, title);
  if (FAILED(hr))
  {
    StringCchCatA(msg, 256, " Fail, ");

    TCHAR buff[256];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
      NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR)buff, 256, NULL);

    LPSTR cstr = MakeUtf8String(buff);
    if (cstr != NULL)
    {
      StringCchCatA(msg, 256, cstr);
      delete[] cstr;
    }
    type |= SYNC_EVENT_ERROR;
  }
  else
  {
    StringCchCatA(msg, 256, " Ok");
  }
  StoreLogHistory(function, msg, type);
}

void StoreResultLog(LPCSTR function, LPCTSTR title, HRESULT hr)
{
  LPSTR titlea = MakeUtf8String(title);
  StoreResultLog(function, titlea, hr);
  if (titlea != NULL)
    delete[] titlea;
}

#endif
