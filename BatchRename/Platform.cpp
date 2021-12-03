#include "pch.h"
#include "Platform.h"

#include "KConstant.h"
#include "KStoreLog.h"

#include <tchar.h>
#include <Strsafe.h>
#include <Shellapi.h>
#include <Shlwapi.h>
#include <errno.h>

#include <Netlistmgr.h>
#include <Wininet.h>
#include <VersionHelpers.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char SYSTEM_TIME_FORMAT_MS_STR[] = "%d-%.2d-%.2d %.2d:%.2d:%.2d:%.3d";
char SYSTEM_TIME_FORMAT_STR[] = "%d-%.2d-%.2d %.2d:%.2d:%.2d";

char SYSTEM_DATE_FORMAT_SCAN[] = "%d-%d-%d";
char SYSTEM_TIME_FORMAT_SCAN[] = "%d-%d-%d %d:%d:%d";

TCHAR SYSTEM_TIME_FORMAT_STR_T[] = _T("%d-%.2d-%.2d %.2d:%.2d:%.2d");
TCHAR SYSTEM_TIME_FORMAT_STR_HM_T[] = _T("%d-%.2d-%.2d %.2d:%.2d");
TCHAR SYSTEM_TIME_FORMAT_STR_DATENUM_T[] = _T("%d%.2d%.2d");

TCHAR UNC_FILE_PREFIX[] = _T("\\\\?\\");

int ToUtf8String(char *bstr, int buff_size, LPCTSTR str, int len)
{
#ifdef UNICODE
	int str_len = WideCharToMultiByte(CP_UTF8, 0, str, len, bstr, buff_size, NULL, NULL);
  if ((str_len == 0) && (len > 0))
    OutputDebugString(_T("buffer overrun on ToUtf8String\n"));

  if (bstr != NULL)
	  bstr[str_len] = '\0';
  return str_len;
#else
  if (bstr != NULL)
	  strcpy_s(bstr, buff_size, str);
  return str_len(str);
#endif
}

char *MakeUtf8String(LPCTSTR str, int len)
{
#ifdef UNICODE
  if (str == NULL)
    return NULL;
  if (len < 0)
    len = lstrlen(str);
	int a_len = WideCharToMultiByte(CP_UTF8, 0, str, len, NULL, 0, NULL, NULL);
  if (a_len > 0)
  {
    char *bstr = new char[a_len+1];
    WideCharToMultiByte(CP_UTF8, 0, str, len, bstr, a_len, NULL, NULL);
    bstr[a_len] = '\0';
    return bstr;
  }
  return NULL;
#else
  if (len < 0)
    len = strlen(str);
  char *bstr = new char[len+1];
	strcpy_s(bstr, len+1, str);
  return bstr;
#endif
}

int ToUnicodeString(TCHAR *dest, int dest_size, LPCSTR str, int len)
{
#ifdef UNICODE
  if (len < 0)
    len = strlen(str);
  int str_len = MultiByteToWideChar(CP_UTF8, 0, str, len, dest, dest_size);
  if (dest != NULL)
	  dest[str_len] = '\0';
  return str_len;
#else
  if (dest != NULL)
	  strcpy_s(dest, dest_size, str);
  return strlen(str);
#endif
}

TCHAR *MakeUnicodeString(LPCSTR str, int len)
{
#ifdef UNICODE
  if (str == NULL)
    return NULL;
  if (len < 0)
    len = (int)strlen(str);
  int tstr_len = MultiByteToWideChar(CP_UTF8, 0, str, len, NULL, 0);
  if (tstr_len > 0)
  {
    TCHAR *tstr = new TCHAR[tstr_len+1];
    MultiByteToWideChar(CP_UTF8, 0, str, len, tstr, tstr_len);
    tstr[tstr_len] = '\0';

#ifdef _DEBUG_ALLOC_BUFF
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("%s = 0x%x"), tstr, tstr);
    OutputDebugString(msg);
    OutputDebugString(_T("\n"));
#endif

    return tstr;
  }
  return NULL;
#else
  if (len < 0)
    len = lstrlen(str);
  TCHAR *tstr = new TCHAR[len+1];
  StringCchCopy(tstr, len+1, str);
  return lstrlen(tstr);
#endif
}

LPSTR AllocCString(LPCSTR str, int len)
{
  if ((str != NULL) && (strlen(str) > 0))
  {
    if (len < 0)
      len = (int)strlen(str);
    LPSTR nstr = new char[len+1];
    strncpy_s(nstr, len+1, str, len);
    return nstr;
  }
  return NULL;
}

LPTSTR AllocString(LPCTSTR str, int len)
{
  if ((str != NULL) && (lstrlen(str) > 0))
  {
    if (len < 0)
      len = lstrlen(str);
    LPTSTR nstr = new TCHAR[len+1];
    StringCchCopyN(nstr, len+1, str, len);

    /*
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("A=0x%X, size=%d, str=%s"), nstr, len * sizeof(len), str);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    */
    return nstr;
  }
  return NULL;
}

LPSTR AllocAddCString(LPCSTR dest, char separator, LPCSTR src)
{
  LPSTR nstr = NULL;

  if ((dest != NULL) && (strlen(dest) > 0) &&
    (src != NULL) && (strlen(src) > 0))
  {
    int len = strlen(dest) + strlen(src) + 2;
    nstr = new char[len];
    AppendSeparateCString(nstr, len, dest, separator, src);
  }
  else if ((dest != NULL) && (strlen(dest) > 0))
    nstr = AllocCString(dest);
  else if ((src != NULL) && (strlen(src) > 0))
    nstr = AllocCString(src);
  return nstr;
}

LPTSTR AllocPathName(LPCTSTR path, LPCTSTR name)
{
  LPTSTR nstr = NULL;

  if (IsFullPath(name))
    path = NULL;

  if ((path != NULL) && (name != NULL))
  {
    int len = lstrlen(path) + lstrlen(name) + 2;
    nstr = new TCHAR[len];
    MakeFullPathName(nstr, len, path, name);
  }
  else if(path != NULL)
    nstr = AllocString(path);
  else if(name != NULL)
    nstr = AllocString(name);
  return nstr;
}

LPTSTR AllocAddString(LPCTSTR dest, TCHAR separator, LPCTSTR src)
{
  LPTSTR nstr = NULL;

  if ((dest != NULL) && (lstrlen(dest) > 0) && 
    (src != NULL) && (lstrlen(src) > 0))
  {
    int len = lstrlen(dest) + lstrlen(src) + 2;
    nstr = new TCHAR[len];
    AppendSeparateName(nstr, len, dest, separator, src);
  }
  else if((dest != NULL) && (lstrlen(dest) > 0))
    nstr = AllocString(dest);
  else if((src != NULL) && (lstrlen(src) > 0))
    nstr = AllocString(src);
  return nstr;
}

LPTSTR AllocAddStringForce(LPCTSTR dest, TCHAR separator, LPCTSTR src)
{
  int len = lstrlen(dest) + lstrlen(src) + 2;
  LPTSTR nstr = new TCHAR[len];

  if ((dest != NULL) && (lstrlen(dest) > 0))
    StringCchCopy(nstr, len, dest);
  else
    nstr[0] = '\0';

  int dlen = lstrlen(nstr);
  nstr[dlen++] = separator;
  nstr[dlen++] = '\0';

  if ((src != NULL) && (lstrlen(src) > 0))
    StringCchCat(nstr, len, src);

  return nstr;
}

void AllocCopyString(LPTSTR *pDest, LPCTSTR src, int len)
{
  /*
  if ((src != NULL) && (*src == '#'))
    len=len;
  */
  if (*pDest != NULL)
  {
    delete[] *pDest;
    *pDest = NULL;
  }
  if (src != NULL)
  {
    if (len < 0)
      len = lstrlen(src);
    *pDest = new TCHAR[len+1];

    if ((len > 0) && (src != NULL))
      StringCchCopyN(*pDest, len+1, src, len);
    else
      pDest[0] = '\0';
  }
}

void AllocCopyStringTrim(LPTSTR *pDest, LPCTSTR src)
{
  // trim begin
  while((src != NULL) && (*src != '\0'))
  {
    if (*src >= ' ')
      break;
    src++;
  }

  // trim end
  int len = (int)lstrlen(src);
  while(len > 0)
  {
    if (src[len-1] >= ' ')
      break;
    --len;
  }
  return AllocCopyString(pDest, src, len);
}

// 두 폴더 경로명의 교집합 부분만 p1에 저장.
void AllocMergePath(LPTSTR* p1, LPTSTR p2)
{
  if (*p1 == NULL)
  {
    AllocCopyString(p1, p2);
  }
  else if ((lstrlen(*p1) > 0) && (CompareStringNC(*p1, p2) != 0))
  {
    LPTSTR s1 = *p1;
    LPTSTR s2 = p2;

    while ((s1 != NULL) && (s2 != NULL))
    {
      LPTSTR e1 = StrChr(s1, '\\');
      LPTSTR e2 = StrChr(s2, '\\');
      if ((e1 != NULL) && (e2 != NULL))
      {
        int len1 = e1 - s1;
        int len2 = e2 - s2;

        if ((len1 != len2) ||
          (StrCmpNI(s1, s2, len1) != 0))
        {
          if (s1 > *p1)
            s1--;
          *s1 = '\0';
          break;
        }
      }
      else if ((e1 == NULL) && (e2 == NULL))
      {
        if (s1 > *p1)
          s1--;
        *s1 = '\0';
        break;
      }
      else
      {
        int len1;
        int len2;
        if (e1 != NULL)
          len1 = e1 - s1;
        else
          len1 = lstrlen(s1);
        if (e2 != NULL)
          len2 = e2 - s2;
        else
          len2 = lstrlen(s2);

        if ((len1 != len2) ||
          (StrCmpNI(s1, s2, len1) != 0))
        {
          if (s1 > *p1)
            s1--;
          *s1 = '\0';
          break;
        }

        if (e1 != NULL)
          *e1 = '\0';

        break;
      }

      if (e1 != NULL)
        s1 = e1 + 1;
      else
        s1 = NULL;

      if (e2 != NULL)
        s2 = e2 + 1;
      else
        s2 = NULL;
    }
  }
}

int ReplaceChar(LPTSTR str, TCHAR s, TCHAR r)
{
  int count = 0;
  if (str == NULL)
    return 0;
  while(*str != NULL)
  {
    if (*str == s)
    {
      *str = r;
      count++;
    }
    ++str;
  }
  return count;
}

int ReplaceCChar(LPSTR str, char s, char r)
{
  int count = 0;
  if (str == NULL)
    return 0;
  while (*str != NULL)
  {
    if (*str == s)
    {
      *str = r;
      count++;
    }
    ++str;
  }
  return count;
}

BOOL IsOneOfFolderBelow(LPTSTR stringArray, TCHAR separator, LPCTSTR str)
{
  LPTSTR p = stringArray;
  int child_len = lstrlen(str);

  while(p != NULL)
  {
    LPTSTR e = StrChr(p, separator);
    if (e == NULL)
    {
      if (IsChildFolderOrFile(p, lstrlen(p), str, child_len, TRUE))
      //if (CompareStringICNC(p, str) == 0)
        return TRUE;
      return FALSE;
    }
    else
    {
      int len = e - p;

      if (IsChildFolderOrFile(p, len, str, child_len, TRUE))
      // if ((len == lstrlen(str)) && (StrNCmpI(p, str, len) == 0))
        return TRUE;
      p = e + 1;
    }
  }
  return FALSE;
}

BOOL IsOneOfString(LPTSTR stringArray, TCHAR separator, LPCTSTR str)
{
  LPTSTR p = stringArray;

  while(p != NULL)
  {
    LPTSTR e = StrChr(p, separator);
    if (e == NULL)
    {
      if (CompareStringICNC(p, str) == 0)
        return TRUE;
      return FALSE;
    }
    else
    {
      int len = e - p;
      if ((len == lstrlen(str)) && (StrNCmpI(p, str, len) == 0))
        return TRUE;
      p = e + 1;
    }
  }
  return FALSE;
}

int kstrcatChar(char *d, int buff_len, char c)
{
  int cur_len = strlen(d);
  if (cur_len+2 <= buff_len)
  {
    d[cur_len++] = c;
    d[cur_len++] = '\0';
  }
#ifdef _DEBUG
  else
  {
    OutputDebugString(_T("kstrcatChar buff overflow\n"));
    return 1;
  }
#endif
  return 0;
}

void SystemTimeToString(char *buff, int buff_size, SYSTEMTIME &systime)
{
  sprintf_s(buff, buff_size, SYSTEM_TIME_FORMAT_STR, systime.wYear, systime.wMonth, systime.wDay,
    systime.wHour, systime.wMinute, systime.wSecond); //, systime.wMilliseconds);
}

void SystemTimeToStringEx(char *buff, int buff_size, SYSTEMTIME &systime)
{
  sprintf_s(buff, buff_size, SYSTEM_TIME_FORMAT_MS_STR, systime.wYear, systime.wMonth, systime.wDay,
    systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);
}

void SystemUTCTimeToString(char *buff, int buff_size, SYSTEMTIME &systime)
{
  if (IsValidSystemTime(systime))
  {
    SYSTEMTIME loctime;

    // UTC time to LOCAL time
    TIME_ZONE_INFORMATION tz;
    GetTimeZoneInformation(&tz);
    SystemTimeToTzSpecificLocalTime(&tz, &systime, &loctime);
    SystemTimeToString(buff, buff_size, loctime);
  }
  else
    buff[0] = '\0';
}

void FileTimeToString(char *buff, int buff_size, FILETIME datetime)
{
	SYSTEMTIME systime;

	if (FileTimeToSystemTime(&datetime, &systime))
	{
    SystemTimeToString(buff, buff_size, systime);
	}
}

void SystemTimeToTString(TCHAR *buff, int buff_size, SYSTEMTIME &systime)
{
  if (IsValidSystemTime(systime))
  {
    StringCchPrintf(buff, buff_size, SYSTEM_TIME_FORMAT_STR_T, systime.wYear, systime.wMonth, systime.wDay,
      systime.wHour, systime.wMinute, systime.wSecond);//, systime.wMilliseconds);
  }
  else
    buff[0] = '\0';
}

void SystemTimeToTStringHM(TCHAR *buff, int buff_size, SYSTEMTIME &systime)
{
  if (IsValidSystemTime(systime))
  {
    StringCchPrintf(buff, buff_size, SYSTEM_TIME_FORMAT_STR_HM_T, systime.wYear, systime.wMonth,
       systime.wDay, systime.wHour, systime.wMinute);
  }
  else
    buff[0] = '\0';
}

void SystemTimeToTStringDateNum(TCHAR *buff, int buff_size, SYSTEMTIME &systime)
{
  if (IsValidSystemTime(systime))
  {
    StringCchPrintf(buff, buff_size, SYSTEM_TIME_FORMAT_STR_DATENUM_T,
      systime.wYear, systime.wMonth, systime.wDay);
  }
  else
    buff[0] = '\0';
}


void SystemUTCTimeToTString(TCHAR *buff, int buff_size, SYSTEMTIME &systime)
{
  if (IsValidSystemTime(systime))
  {
    SYSTEMTIME loctime;

    // UTC time to LOCAL time
    TIME_ZONE_INFORMATION tz;
    GetTimeZoneInformation(&tz);
    SystemTimeToTzSpecificLocalTime(&tz, &systime, &loctime);
    SystemTimeToTString(buff, buff_size, loctime);
  }
  else
    buff[0] = '\0';
}

void SetSystemTime(SYSTEMTIME& out_time, WORD y, WORD m, WORD d, WORD h, WORD mm, WORD ss, WORD ms)
{
  out_time.wYear = y;
  out_time.wMonth = m;
  out_time.wDay = d;
  out_time.wHour = h;
  out_time.wMinute = mm;
  out_time.wSecond = ss;
  out_time.wMilliseconds = ms;
}

BOOL StringToSystemTime(char *buff, SYSTEMTIME &out_time)
{
  int y, m, d;
  int hh, mm, ss;//, ms;
  int r = sscanf_s(buff, SYSTEM_TIME_FORMAT_SCAN, &y, &m, &d, &hh, &mm, &ss);//, &ms);
  if (r == 6)
  {
    out_time.wYear = y;
    out_time.wMonth = m;
    out_time.wDay = d;
    out_time.wHour = hh;
    out_time.wMinute = mm;
    out_time.wSecond = ss;
    out_time.wMilliseconds = 0;// ms;
    return TRUE;
  }
  else
  {
    OutputDebugString(_T("invalidate date"));
  }
  return FALSE;
}

BOOL StringToSystemDate(char *buff, SYSTEMTIME &out_time)
{
  int y, m, d;
  int r = sscanf_s(buff, SYSTEM_DATE_FORMAT_SCAN, &y, &m, &d);//, &ms);
  if (r == 3)
  {
    out_time.wYear = y;
    out_time.wMonth = m;
    out_time.wDay = d;
    out_time.wHour = 0;
    out_time.wMinute = 0;
    out_time.wSecond = 0;
    out_time.wMilliseconds = 0;// ms;
    return TRUE;
  }
  else
  {
    OutputDebugString(_T("invalidate date"));
  }
  return FALSE;
}

BOOL StringFromSystemDate(char* date, int bufflen, SYSTEMTIME &time)
{
  sprintf_s(date, bufflen, SYSTEM_DATE_FORMAT_SCAN, 
    time.wYear, time.wMonth, time.wDay);
  return TRUE;
}

BOOL StringToSystemDateT(LPCTSTR buff, SYSTEMTIME &out_time)
{
  char date[64];
  ToUtf8String(date, 64, buff, lstrlen(buff));
  return StringToSystemDate(date, out_time);
}

BOOL StringFromSystemDateT(LPTSTR buff, int bufflen, SYSTEMTIME &out_time)
{
  char date[64];
  StringFromSystemDate(date, 64, out_time);

  ToUnicodeString(buff, bufflen, date, strlen(date));
  return TRUE;
}

void FileTimeToLocalTime(FILETIME &ft, SYSTEMTIME &localTime)
{
  SYSTEMTIME sysTime;
  FileTimeToSystemTime(&ft, &sysTime);
  TIME_ZONE_INFORMATION tz;
  GetTimeZoneInformation(&tz);
  SystemTimeToTzSpecificLocalTime(&tz, &sysTime, &localTime);
}

void GetEllapsedTimeString(TCHAR *buff, int buff_size, SYSTEMTIME &time, BOOL withMS)
{
  TCHAR str[32];

  buff[0] = '\0';
  if (time.wYear > 0)
  {
    StringCchPrintf(str, 32, _T("%dYear"), time.wYear);
    StringCchCat(buff, buff_size, str);
  }
  if (time.wMonth > 0)
  {
    StringCchPrintf(str, 32, _T(" %dMonth"), time.wMonth);
    StringCchCat(buff, buff_size, str);
  }
  if (time.wDay > 0)
  {
    StringCchPrintf(str, 32, _T(" %dDay"), time.wDay);
    StringCchCat(buff, buff_size, str);
  }
  if (time.wHour > 0)
  {
    StringCchPrintf(str, 32, _T(" %dHour"), time.wHour);
    StringCchCat(buff, buff_size, str);
  }
  if (time.wMinute > 0)
  {
    StringCchPrintf(str, 32, _T(" %dMin"), time.wMinute);
    StringCchCat(buff, buff_size, str);
  }
  if ((lstrlen(buff) == 0) || (time.wSecond > 0))
  {
    StringCchPrintf(str, 32, _T(" %dSec"), time.wSecond);
    StringCchCat(buff, buff_size, str);
  }
  if (withMS && (time.wMilliseconds > 0))
  {
    StringCchPrintf(str, 32, _T(" %dMS"), time.wMilliseconds);
    StringCchCat(buff, buff_size, str);
  }
}


BOOL GetFileModifiedTime(LPCTSTR filename, SYSTEMTIME *pModifyTime, SYSTEMTIME *pAccessTime)
{
  // GetFileTime is differ with FileFindFirst() in summer-time
  /*
  HANDLE h = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);
  if (h != NULL)
  {
    FILETIME filetime;
    BOOL r = FALSE;
    if (GetFileTime(h, NULL, NULL, &filetime))
    {
      FileTimeToSystemTime(&filetime, time);
      r = TRUE;
    }
    CloseHandle(h);
    return r;
  }
  return FALSE;
  */

  if (pModifyTime != NULL)
    memset(pModifyTime, 0, sizeof(SYSTEMTIME));
  if (pAccessTime != NULL)
    memset(pAccessTime, 0, sizeof(SYSTEMTIME));

	WIN32_FIND_DATA fd;
  int len = lstrlen(filename);

  if (filename[len-1] == ':') // drive folder not work FindFirstFile
  {
    if (pModifyTime != NULL)
    {
      pModifyTime->wYear = 2017;
      pModifyTime->wMonth = 1;
      pModifyTime->wDay = 1;
    }
    if (pAccessTime != NULL)
    {
      pAccessTime->wYear = 2017;
      pAccessTime->wMonth = 1;
      pAccessTime->wDay = 1;
    }
    return TRUE;
  }

  BOOL r = FALSE;
	memset(&fd, 0, sizeof(WIN32_FIND_DATA));
	fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;

  // scan with real folder path if it or parent renamed
	HANDLE h = FindFirstFile(filename, &fd);
	if (h != INVALID_HANDLE_VALUE)
	{
		if ( // ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) &&
      (lstrlen(fd.cFileName) > 0))
    {
      if (pModifyTime != NULL)
        FileTimeToSystemTime(&fd.ftLastWriteTime, pModifyTime);

      if (pAccessTime != NULL)
        FileTimeToSystemTime(&fd.ftLastAccessTime, pAccessTime);

      r = TRUE;
    }
    FindClose(h);
  }
  return r;
}

static int GetDaysOfTheMonth(int year, int month)
{
  int max_days = 31;
  if ((month == 4) || (month == 6) || (month == 9) || (month == 11))
    max_days = 30;
  else if(month == 2)
  {
    if( ((((year % 4) == 0) && (year % 100) != 0)) ||
      ((year % 400) == 0) )
      max_days = 29;
    else
      max_days = 28;
  }
  return max_days;
}

void AddSystemTimeMS(SYSTEMTIME& dest, SYSTEMTIME& src, int sec)
{
  struct tm tm_time;

  tm_time.tm_sec = src.wSecond;
  tm_time.tm_min = src.wMinute;
  tm_time.tm_hour = src.wHour;
  tm_time.tm_mday = src.wDay;
  tm_time.tm_mon = src.wMonth - 1;
  tm_time.tm_year = src.wYear - 1900;
  __int64 ms = _mkgmtime64(&tm_time);

  ms += sec;

  SystemTimeFromEPOCH_MS(dest, ms * 1000);
}

void SubtractSystemTime(SYSTEMTIME& dest, SYSTEMTIME& src, int day, int hour, int minute)
{
  int sec = -(((day * 24) + hour) * 60 + minute) * 60;
  AddSystemTimeMS(dest, src, sec);
}

void AddSystemTime(SYSTEMTIME& dest, SYSTEMTIME& src, int day, int hour, int minute)
{
  int sec = (((day * 24) + hour) * 60 + minute) * 60;
  AddSystemTimeMS(dest, src, sec);

  /*
  dest.wMilliseconds = src.wMilliseconds;
  dest.wSecond = src.wSecond;
  dest.wMinute = src.wMinute + minute;
  if (dest.wMinute >= 60)
  {
    hour += dest.wMinute / 60;
    dest.wMinute %= 60;
  }

  dest.wHour = src.wHour + hour;
  if (dest.wHour >= 24)
  {
    day += dest.wHour / 24;
    dest.wHour %= 24;
  }
  dest.wDay = src.wDay + day;
  dest.wMonth = src.wMonth;
  int max_day = GetDaysOfTheMonth(src.wYear, src.wMonth);
  if (dest.wDay > max_day)
  {
    dest.wMonth++;
    dest.wDay %= max_day;
  }
  dest.wYear = src.wYear;
  if (dest.wMonth > 12)
  {
    dest.wYear++;
    dest.wMonth %= 12;
  }
  */
}

int CompareSystemTimeDate(SYSTEMTIME &a, SYSTEMTIME &b)
{
  if (a.wYear > b.wYear)
    return 1;
  else if (a.wYear < b.wYear)
    return -1;
  if (a.wMonth > b.wMonth)
    return 1;
  else if (a.wMonth < b.wMonth)
    return -1;
  if (a.wDay > b.wDay)
    return 1;
  else if (a.wDay < b.wDay)
    return -1;
  return 0;
}

// return 1 if a > b
int CompareSystemTime(SYSTEMTIME &a, SYSTEMTIME &b)
{
  if (a.wYear > b.wYear)
    return 1;
  else if (a.wYear < b.wYear)
    return -1;
  if (a.wMonth > b.wMonth)
    return 1;
  else if (a.wMonth < b.wMonth)
    return -1;
  if (a.wDay > b.wDay)
    return 1;
  else if (a.wDay < b.wDay)
    return -1;
  if (a.wHour > b.wHour)
    return 1;
  else if (a.wHour < b.wHour)
    return -1;
  if (a.wMinute > b.wMinute)
    return 1;
  else if (a.wMinute < b.wMinute)
    return -1;
  if (a.wSecond > b.wSecond)
    return 1;
  else if (a.wSecond < b.wSecond)
    return -1;
  /*
  if (a.wMilliseconds > b.wMilliseconds)
    return 1;
  else if (a.wMilliseconds < b.wMilliseconds)
    return -1;
    */
  return 0;
}

void TimeDifference(SYSTEMTIME& out, SYSTEMTIME& begin, SYSTEMTIME& end)
{
  int remain = 0;

  if (end.wMilliseconds < begin.wMilliseconds)
  {
    out.wMilliseconds = end.wMilliseconds + 1000 - begin.wMilliseconds;
    remain = 1;
  }
  else
  {
    out.wMilliseconds = end.wMilliseconds - begin.wMilliseconds;
    remain = 0;
  }

  if (end.wSecond < (begin.wSecond + remain))
  {
    out.wSecond = end.wSecond + 60 - (begin.wSecond + remain);
    remain = 1;
  }
  else
  {
    out.wSecond = end.wSecond - (begin.wSecond + remain);
    remain = 0;
  }

  if (end.wMinute < (begin.wMinute + remain))
  {
    out.wMinute = end.wMinute + 60 - (begin.wMinute + remain);
    remain = 1;
  }
  else
  {
    out.wMinute = end.wMinute - (begin.wMinute + remain);
    remain = 0;
  }

  if (end.wHour < (begin.wHour + remain))
  {
    out.wHour = end.wHour + 24 - (begin.wHour + remain);
    remain = 1;
  }
  else
  {
    out.wHour = end.wHour - (begin.wHour + remain);
    remain = 0;
  }

  if (end.wDay < (begin.wDay + remain))
  {
    out.wDay = end.wDay + 30 - (begin.wDay + remain);
    remain = 1;
  }
  else
  {
    out.wDay = end.wDay - (begin.wDay + remain);
    remain = 0;
  }

  if (end.wMonth < (begin.wMonth + remain))
  {
    out.wMonth = end.wMonth + 12 - (begin.wMonth + remain);
    remain = 1;
  }
  else
  {
    out.wMonth = end.wMonth - (begin.wMonth + remain);
    remain = 0;
  }

  if(end.wYear > (begin.wYear + remain))
    out.wYear = end.wYear - (begin.wYear + remain);
  else
    out.wYear = 0;
}

void TimeDifferenceSec(SYSTEMTIME& out, SYSTEMTIME& begin, SYSTEMTIME& end)
{
  int remain = 0;
  out.wMilliseconds = 0;

  if (end.wSecond < (begin.wSecond + remain))
  {
    out.wSecond = end.wSecond + 60 - (begin.wSecond + remain);
    remain = 1;
  }
  else
  {
    out.wSecond = end.wSecond - (begin.wSecond + remain);
    remain = 0;
  }

  if (end.wMinute < (begin.wMinute + remain))
  {
    out.wMinute = end.wMinute + 60 - (begin.wMinute + remain);
    remain = 1;
  }
  else
  {
    out.wMinute = end.wMinute - (begin.wMinute + remain);
    remain = 0;
  }

  if (end.wHour < (begin.wHour + remain))
  {
    out.wHour = end.wHour + 24 - (begin.wHour + remain);
    remain = 1;
  }
  else
  {
    out.wHour = end.wHour - (begin.wHour + remain);
    remain = 0;
  }

  if (end.wDay < (begin.wDay + remain))
  {
    out.wDay = end.wDay + 30 - (begin.wDay + remain);
    remain = 1;
  }
  else
  {
    out.wDay = end.wDay - (begin.wDay + remain);
    remain = 0;
  }

  if (end.wMonth < (begin.wMonth + remain))
  {
    out.wMonth = end.wMonth + 12 - (begin.wMonth + remain);
    remain = 1;
  }
  else
  {
    out.wMonth = end.wMonth - (begin.wMonth + remain);
    remain = 0;
  }

  if(end.wYear > (begin.wYear + remain))
    out.wYear = end.wYear - (begin.wYear + remain);
  else
    out.wYear = 0;
}

// return d - s as millisecond
long TimeDifferenceSec(SYSTEMTIME& d, SYSTEMTIME& s)
{
  SYSTEMTIME a;
  int negative = 1;

  if (CompareSystemTime(d, s) >= 0)
  {
    TimeDifferenceSec(a, s, d);
  }
  else
  {
    TimeDifferenceSec(a, d, s);
    negative = -1;
  }

  long time_sec = (a.wDay * 24) + a.wHour;
  time_sec = time_sec * 60 + a.wMinute;
  time_sec = time_sec * 60 + a.wSecond;
  return (time_sec * negative);
}

#include <time.h>

BOOL SystemTimeFromEPOCH_MS(SYSTEMTIME& st, __int64 time_ms)
{
  st.wMilliseconds = (WORD) (time_ms % 1000);
  time_ms = time_ms / 1000;

  struct tm tm_time;
  if (_gmtime64_s(&tm_time, &time_ms) == 0)
  {
    st.wSecond = tm_time.tm_sec;
    st.wMinute = tm_time.tm_min;
    st.wHour = tm_time.tm_hour;
    st.wDay = tm_time.tm_mday;
    st.wDayOfWeek = tm_time.tm_wday;
    st.wMonth = tm_time.tm_mon + 1;
    st.wYear = tm_time.tm_year + 1900;
    return TRUE;
  }
  return FALSE;
#if 0
  st.wSecond = (WORD) (time_ms % 60);
  time_ms = time_ms / 60;
  st.wMinute = (WORD) (time_ms % 60);
  time_ms = time_ms / 60;
  st.wHour = (WORD) (time_ms % 24);
  time_ms = time_ms / 24;
  // time_ms = days from 1970/01/01

  // time_ms += 1970 * 365 - (1970 / 4) + (1970 / 100) - (1970 / 400); // days from 1/01/01
  int days_before_1970 = 1970 * 365 - (1970 / 4) + (1970 / 100) - (1970 / 400);

  time_ms += 719468;
  int era = (int)(time_ms / 146097);
  int doe = (int)(time_ms - era * 146097);
  int yoe = (doe - doe / 1460 + doe / 36524 - doe / 146097) / 365;
  st.wYear =(WORD)(yoe + era * 400);
  int doy = doe - (365*yoe + yoe/4 - yoe/100);  // [0, 365]
  int mp = (5*doy + 2)/153; // [0, 11]
  st.wDay = doy - (153*mp+2)/5 + 1; // [1, 31]
  st.wMonth = mp + (mp < 10 ? 3 : -9);
  return TRUE;
#endif
}

__int64 SystemTimeToEPOCH(WORD yy, WORD mm, WORD dd, WORD h, WORD m, WORD s)
{
  struct tm tm_time;

  tm_time.tm_sec = s;
  tm_time.tm_min = m;
  tm_time.tm_hour = h;
  tm_time.tm_mday = dd;
  // tm_time.tm_wday = st.wDayOfWeek;
  tm_time.tm_mon = mm - 1;
  tm_time.tm_year = yy - 1900;

  __int64 ms = _mkgmtime64(&tm_time);
  return ms;
}

__int64 SystemTimeToEPOCH_MS(WORD yy, WORD mm, WORD dd, WORD h, WORD m, WORD s)
{
  __int64 time_sec = SystemTimeToEPOCH(yy, mm, dd, h, m, s);
  return time_sec * 1000;
}

int DaysFromEPOCH_MS(__int64 ms)
{
  int day_s = (int)(ms / (1000 * 60 * 60 * 24));
  return day_s;
}

BOOL IsValidSystemTime(SYSTEMTIME &time)
{
  return ((time.wYear > 1900) && (time.wMonth > 0) && (time.wDay > 0));
}

BOOL IsValidSystemTimeHMS(SYSTEMTIME &time)
{
  return ((time.wYear > 0) || (time.wMonth > 0) || (time.wDay > 0) || (time.wHour > 0) || (time.wMinute > 0) || (time.wSecond > 0));
}

int GetTommorrowSec()
{
  SYSTEMTIME time;
  GetLocalTime(&time);
  return ((long) (60 - time.wSecond) + (59 - time.wMinute + (long) (23 - time.wHour) * 60 ) * 60);
}

int GetEllapsedDays(SYSTEMTIME& end, SYSTEMTIME& begin)
{
  __int64 e = SystemTimeToEPOCH_MS(end.wYear, end.wMonth, end.wDay, 0, 0, 0);
  __int64 b = SystemTimeToEPOCH_MS(begin.wYear, begin.wMonth, begin.wDay, 0, 0, 0);
  int days = 0;
  if (e > b)
  {
    days = DaysFromEPOCH_MS(e - b);
  }
  return days;
}


BOOL GetLastErrorMessageString(DWORD dwLastError, LPTSTR buff, int buff_size)
{
	LPVOID lpMsgBuf = NULL;

	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
				  dwLastError, 
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					(LPTSTR) &lpMsgBuf, 0, NULL );

  if (lpMsgBuf != NULL)
  {
    StringCchCopy(buff, buff_size, (LPTSTR)lpMsgBuf);
		LocalFree(lpMsgBuf);	
    return TRUE;
  }
	return FALSE;
}

BOOL IsEmptyPath(LPCTSTR path)
{
  return ((path == NULL) || (lstrlen(path) == 0) ||
    ((path[0] == '\\') && (path[1] == '\0')));
}

BOOL IsFullPath(LPCTSTR pathName)
{
  if ((lstrlen(pathName) > 2) && (pathName[0] == '\\') && (pathName[1] == '\\'))
    return TRUE;
  return ((lstrlen(pathName) > 1) && (pathName[1] == ':'));
}

void AppendSeparateName(LPTSTR dest, int size, LPCTSTR first, TCHAR separator, LPCTSTR second)
{
  if(dest != first)
  {
    if ((first != NULL) && (lstrlen(first) > 0))
      StringCchCopy(dest, size, first);
    else
      dest[0] = '\0';
  }

  if ((second != NULL) && (lstrlen(second) > 0))
  {
    int dlen = lstrlen(dest);
    if ((dlen > 0) && (dest[dlen-1] != separator))
    {
      dest[dlen++] = separator;
      dest[dlen++] = '\0';
    }
    StringCchCat(dest, size, second);
  }
}

void AppendSeparateCString(LPSTR dest, int size, LPCSTR first, char separator, LPCSTR second)
{
  if (dest != first)
  {
    if ((first != NULL) && (strlen(first) > 0))
      strcpy_s(dest, size, first);
    else
      dest[0] = '\0';
  }

  if ((second != NULL) && (strlen(second) > 0))
  {
    int dlen = strlen(dest);
    if ((dlen > 0) && (dest[dlen - 1] != separator))
    {
      dest[dlen++] = separator;
      dest[dlen++] = '\0';
    }
    strcat_s(dest, size, second);
  }
}

void MakeFullPathName(LPTSTR dest, int size, LPCTSTR folder, LPCTSTR file)
{
  if(dest != folder)
  {
    if ((folder != NULL) && (lstrcmp(folder, _T("\\")) != 0) && !IsFullPath(file))
      StringCchCopy(dest, size, folder);
    else
      dest[0] = '\0';
  }
#ifdef _DEBUG
  int need_len = 0;
  if (folder != NULL)
    need_len += lstrlen(folder);
#endif
  if ((file != NULL) && (lstrlen(file) > 0))
  {
    int dlen = lstrlen(dest);
    if ((dlen > 0) && (dest[dlen-1] != '\\'))
    {
      if (*file != '\\')
      {
        StringCchCat(dest, size, L"\\");
#ifdef _DEBUG
        need_len++;
#endif
      }
    }
    StringCchCat(dest, size, file);

#ifdef _DEBUG
    need_len += lstrlen(file);
    if (need_len >= size)
    {
      TCHAR msg[120];
      StringCchPrintf(msg, 120, _T("Buffer Overflow %d / %d\n"), need_len, size);
      OutputDebugString(msg);
    }
#endif
  }
}

void MakeRelativePathName(LPTSTR relative_buff, int buff_size, LPCTSTR baseFolder, LPCTSTR fullPathName)
{
  int base_length = lstrlen(baseFolder);
  if ((base_length < lstrlen(fullPathName)) &&
    (StrCmpNI(baseFolder, fullPathName, base_length) == 0) &&
    (fullPathName[base_length] == '\\'))
  {
    StringCchCopy(relative_buff, buff_size, &fullPathName[base_length+1]);
  }
  else
  {
    StringCchCopy(relative_buff, buff_size, fullPathName);
  }
}

LPTSTR AllocMakeFullPathName(LPCTSTR folder, LPCTSTR file)
{
  if (IsFullPath(file))
  {
    return AllocString(file);
  }
  else
  {
    int len = lstrlen(folder) + lstrlen(file) + 2;
    LPTSTR fullname = new TCHAR[len];
    MakeFullPathName(fullname, len, folder, file);
    return fullname;
  }
}

BOOL ShortenPathName(LPTSTR buff, int length, LPCTSTR originPathName)
{
  if (lstrlen(originPathName) < length)
  {
    StringCchCopy(buff, length, originPathName);
    return FALSE;
  }

  buff[0] = '\0';
  LPCTSTR end = StrRChr(originPathName, NULL, '\\');
  if (end != NULL)
  {
    ++end;
    int last_name_len = lstrlen(end);

    LPCTSTR p = StrChr(originPathName, ':');
    if (p != NULL)
    {
      int drive_len = (int)(p - originPathName);
      if (drive_len < 4)
      {
        StringCchCopyN(buff, length, originPathName, drive_len+2);
        p += 2;
      }
    }

    LPTSTR d = buff + lstrlen(buff);
    int avail = length - (int)lstrlen(buff);
    while(avail > (last_name_len + 5))
    {
      LPCTSTR e = StrChr(p, '\\');
      int navail = length - (int)lstrlen(buff);
      if ((e != NULL) && ((e - p) < (avail - (last_name_len + 5))))
      {
        int len = (int)(e - p + 1);
        StringCchCopyN(d, avail, p, len);
        d += len;
        p += len;
        avail -= len;
      }
      else
      {
        break;
      }
    }

    StringCchCat(buff, length, _T("..."));
    avail -= 3;

    if (last_name_len > avail)
    {
      end += (last_name_len - avail) + 1;
    }
    else
    {
      StringCchCat(buff, length, _T("\\"));
    }

    StringCchCat(buff, length, end);
  }
  else
  {
    StringCchCat(buff, length, _T("..."));
    int avail = length - (int)lstrlen(buff) - 1;
    end = originPathName;
    int last_name_len = lstrlen(end);

    if (last_name_len > avail)
      end += (last_name_len - avail);

    StringCchCat(buff, length, end);
  }
  return TRUE;
}

static BOOL EmptyDirectoryFiles(LPCTSTR path)
{
  TCHAR files[KMAX_PATH+3];
	WIN32_FIND_DATA fd;
  int count = 0;

	memset(&fd, 0, sizeof(WIN32_FIND_DATA));
 	fd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
  MakeFullPathName(files, KMAX_PATH, path, L_ASTERISK_STR);
	HANDLE h = FindFirstFile(files, &fd);
	if (h != INVALID_HANDLE_VALUE)
	{
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				if ((lstrlen(fd.cFileName) == 0) ||
          (CompareStringNC(fd.cFileName, L".") == 0) ||
					(CompareStringNC(fd.cFileName, L"..") == 0))
					continue;

        TCHAR subpath[KMAX_LONGPATH];
				MakeFullPathName(subpath, KMAX_LONGPATH, path, fd.cFileName);
        if (KDeleteFile(subpath))
          ++count;
			}
		} while (FindNextFile(h, &fd));
		FindClose(h);
	}
  return(count > 0);
}

int EmptyDirectory(LPCTSTR path, BOOL withItself)
{
  TCHAR files[KMAX_PATH+3];

  if (withItself)
    StringCchCopy(files, KMAX_PATH, path);
  else
    MakeFullPathName(files, KMAX_PATH, path, L_ASTERISK_STR);
  files[lstrlen(files)+1] = '\0'; // make double-NULL terminate

  // delete all files
  SHFILEOPSTRUCT file_op = {
    NULL,
    FO_DELETE,
    files,
    L"",
    FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
    false,
    0,
    L"" };

  int r = 0;

#ifdef _DEBUG
  r = -1;
#else
  r = SHFileOperation(&file_op);
#endif

  if (r != 0)
  {
    // delete sub folders
	  WIN32_FIND_DATA fd;
	  memset(&fd, 0, sizeof(WIN32_FIND_DATA));
 	  fd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
    MakeFullPathName(files, KMAX_PATH, path, L_ASTERISK_STR);
	  HANDLE h = FindFirstFile(files, &fd);
	  if (h != INVALID_HANDLE_VALUE)
	  {
		  do {
			  if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			  {
				  if ((lstrlen(fd.cFileName) == 0) ||
            (CompareStringNC(fd.cFileName, L".") == 0) ||
					  (CompareStringNC(fd.cFileName, L"..") == 0))
					  continue;

          TCHAR subpath[KMAX_PATH];
				  MakeFullPathName(subpath, KMAX_PATH, path, fd.cFileName);
          EmptyDirectory(subpath, TRUE);
			  }
        else // if (lstrlen(fd.cFileName) > 0)
        {
          /* we cannot delete file hear -> cause exception
          TCHAR fullPathName[KMAX_EXTPATH];

          MakeFullPathName(fullPathName, KMAX_EXTPATH, path, fd.cFileName);
          KDeleteFile(fullPathName);
          */
        }
		  } while (FindNextFile(h, &fd));
		  FindClose(h);
	  }
    // delete itself
    if (withItself)
    {
      StringCchCopy(files, KMAX_PATH, path);
      files[lstrlen(files)+1] = '\0'; // make double-NULL terminate
      file_op.pFrom = files;
      r = SHFileOperation(&file_op);
      if (r != 0)
      {
        TCHAR msg[KMAX_LONGPATH+50];
        StringCchPrintf(msg, KMAX_LONGPATH+50, _T("SHFileOperation failure error=0x%X, folder:"), r);
        StringCchCat(msg, KMAX_LONGPATH+50, files);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

        if (r == 0x0CE) // ERROR_FILENAME_EXCED_RANGE
          EmptyDirectoryFiles(path);
      }
    }
  }

  return 0;
}

static DWORD KGetFileAttributes(LPCTSTR name)
{
  DWORD r;
  int name_len = lstrlen(name);
  if (name_len < MAX_PATH)
  {
    r = GetFileAttributes(name);
  }
  else
  {
    LPTSTR pref_filename = new TCHAR[name_len+10];
    StringCchCopy(pref_filename, name_len+10, UNC_FILE_PREFIX);
    StringCchCat(pref_filename, name_len+10, name);
    r = GetFileAttributes(pref_filename);
    delete[] pref_filename;
  }
  return r;
}

/* file : 0, folder : 1, none : -1 */
int IsFileOrFolder(LPCTSTR pathname)
{
  if (pathname != NULL)
  {
    DWORD s = KGetFileAttributes(pathname);
    if (s != INVALID_FILE_ATTRIBUTES)
      return((s & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0);
  }
  return -1;
}

BOOL IsDirectoryExist(LPCTSTR pathname)
{
  if (pathname != NULL)
  {
    DWORD s = KGetFileAttributes(pathname);
    if (s == INVALID_FILE_ATTRIBUTES)
    {
      DWORD last_err = GetLastError();
      return FALSE;
    }
    if (s & FILE_ATTRIBUTE_DIRECTORY)
      return TRUE;
  }
  return FALSE;
  /*
	TCHAR temp[KMAX_PATH];
	BOOL exist = FALSE;

	GetCurrentDirectory(KMAX_PATH, temp);
	if (SetCurrentDirectory(name))
		exist = TRUE;
	SetCurrentDirectory(temp);
	return exist;
  */
}

BOOL IsFileExistAttribute(LPCTSTR pathname)
{
  DWORD s = KGetFileAttributes(pathname);
  if (s == INVALID_FILE_ATTRIBUTES)
    return FALSE;
  return TRUE;
  /*
	TCHAR temp[KMAX_PATH];
	BOOL exist = FALSE;

	GetCurrentDirectory(KMAX_PATH, temp);
	if (SetCurrentDirectory(name))
		exist = TRUE;
	SetCurrentDirectory(temp);
	return exist;
  */
}

// 0 : not found, 1 : readonly, 2 : normal
int IsFileExistReadOnly(LPCTSTR pathname)
{
  DWORD s = KGetFileAttributes(pathname);
  if (s == INVALID_FILE_ATTRIBUTES)
    return 0;

  if (s & FILE_ATTRIBUTE_READONLY)
    return 1;
  return 2;
}

BOOL IsFileEditing(LPCTSTR name)
{
  // readonly file is not editing
  if (IsFileExistReadOnly(name) == 2)
  {
    FILE *f = NULL;
    errno_t err = _tfopen_s(&f, name, _T("a"));
    if (f != NULL)
    {
      fclose(f);
      return FALSE;
    }
    if (err == EACCES) // this file is opened by other app
      return TRUE;
  }
  return FALSE;
}

BOOL IsFileWritable(LPCTSTR name)
{
  DWORD attr = KGetFileAttributes(name);
  if (attr != INVALID_FILE_ATTRIBUTES)
  {
    if (!(attr & FILE_ATTRIBUTE_READONLY))
      return TRUE;
  }
  return FALSE;
}

BOOL CreateDirectory(LPCTSTR pathname)
{
  return CreateDirectory(pathname, NULL);
}

BOOL CreateDirectoryCascade(LPTSTR pathname)
{
  LPTSTR p = pathname;
  int created = 0;
  TCHAR msg[256];

  while(p != NULL)
  {
    LPTSTR e = StrChr(p, '\\');
    if (e != NULL)
    {
      *e = '\0';
      p = e + 1;

      if (!IsDirectoryExist(pathname))
      {
        created++;
        BOOL r = CreateDirectory(pathname);
        DWORD fa = KGetFileAttributes(pathname);
        DWORD last_err = GetLastError();
        StringCchPrintf(msg, 256, _T("%s:%d,%X,%d"), pathname, r, fa, last_err);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }
      *e = '\\';
    }
    else
    {
      p = NULL;
      if (!IsDirectoryExist(pathname))
      {
        created++;
        BOOL r = CreateDirectory(pathname);
        DWORD fa = KGetFileAttributes(pathname);
        DWORD last_err = GetLastError();
        StringCchPrintf(msg, 256, _T("%s:%d,%X,%d"), pathname, r, fa, last_err);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }
    }
  }
  return (created > 0);
}

int CompareFile(LPCTSTR sourceFile, LPCTSTR destFile)
{
  if (GetFileSize(sourceFile) != GetFileSize(destFile))
    return -1;

  int r = 0;
  FILE *sf = NULL;
  errno_t err = _tfopen_s(&sf, sourceFile, L"rb");
  if (err == 0)
  {
    FILE *df = NULL;
    err = _tfopen_s(&df, destFile, L"rb");
    if (err == 0)
    {
      __int64 total = 0;
      char *sBuff = new char[4096];
      char* dBuff = new char[4096];
      if (sBuff != NULL && dBuff != NULL)
      {
        size_t s;
        size_t ds;

        while(!feof(sf))
        {
          s = fread(sBuff, 1, 4096, sf);
          ds = fread(dBuff, 1, 4096, df);
          if (s != ds)
          {
            r = (s > ds) ? 1 : -1;
            break;
          }
          r = memcmp(sBuff, dBuff, s);
          if (r != 0)
          {
            break;
          }
        }

        delete[] sBuff;
        delete[] dBuff;
      }

      fclose(df);
    }
    else
    {
      TCHAR msg[512];
      StringCchPrintf(msg, 512, _T("Failure on fopen to open destFile %s, error=%d"), destFile, err);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }

    fclose(sf);
  }
  else
  {
    TCHAR msg[512];
    StringCchPrintf(msg, 512, _T("Failure on fopen to open sourceFile %s, error=%d"), sourceFile, err);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }

  return r;
}

/*
BOOL CopyFileStream(LPCTSTR orgname, LPCTSTR filename)
{
  BOOL r = FALSE;
  FILE *sf = NULL;
	errno_t err = _tfopen_s(&sf, orgname, L"r");
	if (err == 0)
	{
    FILE *df = NULL;
	  err = _tfopen_s(&df, filename, L"wb");
    if (err == 0)
    {
      __int64 total = 0;
      char *buff = new char[4096];
      if (buff != NULL)
      {
        size_t s;

        fseek(sf, 0, SEEK_SET);
        while(!feof(sf))
        {
          s = fread(buff, 1, 4096, sf);
          fwrite(buff, 1, s, df);
          total += s;
        }

        delete[] buff;
      }
      r = TRUE;
      fclose(df);
    }
    else
    {
      TCHAR msg[512];
      StringCchPrintf(msg, 512, _T("Failure on fopen to write file %s, error=%d"), filename, err);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }
    fclose(sf);
  }
  return r;
}
*/

BOOL FindFileExists(LPCTSTR filename)
{
#if 1
  return PathFileExists(filename);
#else
  // SecurityDisk does not support PathFileExists()
  // 그러나 FindFirstFile()은 속도 문제로 사용이 불가능하다. 다시 PathFileExists()를 사용하기로.
	WIN32_FIND_DATA fd;

	memset(&fd, 0, sizeof(WIN32_FIND_DATA));
	fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;

  BOOL rtn = FALSE;
	HANDLE h = FindFirstFile(filename, &fd);
	if ((h != INVALID_HANDLE_VALUE) &&
		(lstrlen(fd.cFileName) > 0))
    rtn = TRUE;
	FindClose(h);
  return rtn;
#endif
}

BOOL IsFileExist(LPCTSTR fileName)
{
  int name_len = lstrlen(fileName);
  if (name_len < MAX_PATH)
    return FindFileExists(fileName);

  LPTSTR ext_filename = new TCHAR[name_len+10];
  StringCchCopy(ext_filename, name_len+10, UNC_FILE_PREFIX);
  StringCchCat(ext_filename, name_len+10, fileName);
  BOOL r = FindFileExists(ext_filename);
  delete[] ext_filename;
  return r;
}

BOOL IsDirectoryWritable(LPCTSTR pathname)
{
  DWORD attr = KGetFileAttributes(pathname);
  if (attr != INVALID_FILE_ATTRIBUTES)
  {
    if (attr & FILE_ATTRIBUTE_DIRECTORY)
    {
      // 폴더의 READONLY 속성은 의미없음.
      /*
      if (attr & FILE_ATTRIBUTE_READONLY)
      {
      TCHAR msg[300] = _T("ReadOnly Directory : ");
      StringCchCat(msg, 300, pathname);
      OutputDebugString(msg);
      return FALSE;
      }
      */

      /* TODO : check policy
      else if (attr & FILE_ATTRIBUTE_SYSTEM)
      {
      }
      else if (attr & FILE_ATTRIBUTE_TEMPORARY)
      {
      }
      */

      return TRUE;
    }
    /*
    else
    {
      TCHAR msg[300] = _T("Not a directory : ");
      StringCchCat(msg, 300, pathname);
      OutputDebugString(msg);
    }
    */
  }
  return FALSE;
}

BOOL IsSystemDirectory(LPCTSTR pathname)
{
  // check windows folder
  TCHAR windowsDir[KMAX_PATH];
  if (GetWindowsDirectory(windowsDir, KMAX_PATH))
  {
    int win_len= lstrlen(windowsDir);
    if ((lstrlen(pathname) >= win_len) &&
      (StrNCmpI(pathname, windowsDir, win_len) == 0)) // _tcsncmp
      return TRUE;
  }

#ifdef _USE_DESTINY_SYSTEM
  int len = lstrlen(DESTINY_ROOT_FOLDER);
  if ((lstrlen(pathname) >= len) &&
    (StrNCmpI(pathname, DESTINY_ROOT_FOLDER, len) == 0))
    return TRUE;
  if ((len > 4) && (pathname[1] == ':') && (pathname[2] == '\\') && (pathname[3] == '$'))
    return TRUE;
#endif

  LPTSTR path = AllocString(pathname);
  BOOL rtn = FALSE;

  while ((lstrlen(path) > 0) && (path[lstrlen(path)-1] != ':'))
  {
    DWORD attr = KGetFileAttributes(path);
    if ((attr != INVALID_FILE_ATTRIBUTES) &&
      (attr & (FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN)))
    {
      rtn = TRUE;
      break;
    }

    LPTSTR e = StrRChr(path, NULL, '\\');
    if (e != NULL)
    {
      *e = NULL;
    }
    else
    {
      break;
    }
  }
  delete[] path;
  return rtn;
}

#include <WinIoCtl.h>

BOOL GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
{
  STORAGE_PROPERTY_QUERY Query; // input param for query
  DWORD dwOutBytes; // IOCTL output length
  BOOL bResult; // IOCTL return val

  // specify the query type
  Query.PropertyId = StorageDeviceProperty;
  Query.QueryType = PropertyStandardQuery;

  // Query using IOCTL_STORAGE_QUERY_PROPERTY 
  bResult = ::DeviceIoControl(hDevice, // device handle
    IOCTL_STORAGE_QUERY_PROPERTY, // info of device property
    &Query, sizeof(STORAGE_PROPERTY_QUERY), // input data buffer
    pDevDesc, pDevDesc->Size, // output data buffer
    &dwOutBytes, // out's length
    (LPOVERLAPPED)NULL); 

  return bResult;
}

// include USB
BOOL IsFixedDriveDisk(LPCTSTR pathname)
{
  TCHAR driveName[KMAX_PATH+1];

#ifdef _DEBUG_DRIVE_TYPE
  LPTSTR p = _T("C:\\새 폴더\\f\\jfk\\ll\\jl4\\jkl\\kl;\\kl\\jklj\\jkl;\\hh\\jj\\dfjdskfjel;jfopwejds;lfkl;dkf;d'kafkf'wq[wdk[fk;lgjrjg;lsdkafd;fkd\\hhh\\jnhg\\djfdjfkldjfkdsl;jfjrioewgfgjlfjd;fjdska;fj;wlejflkdjadljfdlsfg jkfld;jldajfdsfjs;jfeifjwepfja\\jhgg\\hhh\\ggg\\hhhhh\\ffff\\hh\\hjkkk\\tyuy\\hhh");
  int p_len = lstrlen(p);
#endif

  StringCchCopy(driveName, KMAX_PATH, pathname);
  int len = lstrlen(driveName);
  if (driveName[len - 1] != '\\')
  {
    driveName[len] = '\\';
    driveName[len+1] = '\0';
  }

  UINT type = GetDriveType(driveName);
  return (type == DRIVE_FIXED);
}

BOOL IsFixedDriveDirectory(LPCTSTR pathname)
{
  TCHAR driveName[KMAX_PATH+1];
  LPTSTR e = StrChr(pathname, '\\');
  if (e != NULL)
    StringCchCopyN(driveName, KMAX_PATH, pathname, (e - pathname));
  else
    StringCchCopy(driveName, KMAX_PATH, pathname);

  int len = lstrlen(driveName);
  if (driveName[len - 1] != '\\')
  {
    driveName[len] = '\\';
    driveName[len+1] = '\0';
  }

  UINT type = GetDriveType(driveName);

  // check USBHDD
  if (type == DRIVE_FIXED)
  {
    TCHAR volumeName[8] = _T("\\\\.\\A:");
    volumeName[4] = driveName[0];
    HANDLE hDevice = CreateFile(volumeName, GENERIC_READ, 
      FILE_SHARE_READ | FILE_SHARE_WRITE, 
      NULL, OPEN_EXISTING, NULL, NULL);
    if (hDevice != INVALID_HANDLE_VALUE)
    {
      STORAGE_DEVICE_DESCRIPTOR DevDesc;
      DevDesc.Version = sizeof(STORAGE_DEVICE_DESCRIPTOR);
      DevDesc.Size = sizeof(STORAGE_DEVICE_DESCRIPTOR);
      if(GetDisksProperty(hDevice, &DevDesc))
        {
         if(DevDesc.BusType == BusTypeUsb)
         {
          type = DRIVE_REMOVABLE;
         }
        }
      CloseHandle(hDevice);
    }
  }

  return (type == DRIVE_FIXED);
}

BOOL IsDrivePath(LPCTSTR pathname)
{
  int len = lstrlen(pathname);
  if ((len > 0) &&
    (pathname[len-1] == ':'))
    return TRUE;
  return FALSE;
}

BOOL SetHiddenDirectory(LPCTSTR folder_name)
{
  DWORD a = KGetFileAttributes(folder_name); 
  return SetFileAttributes(folder_name, a | FILE_ATTRIBUTE_HIDDEN);
}

BOOL GetFolderNameFromFullPath(LPTSTR folder_name, LPCTSTR full_path)
{
  LPTSTR p = StrRChr(full_path, NULL, '\\');
  if ((p == NULL) || (lstrlen(p) <= 1))
  {
    StringCchCopy(folder_name, KMAX_PATH, full_path);
  }
  else
  {
    StringCchCopy(folder_name, KMAX_PATH, p + 1);
  }
  return TRUE;
}

BOOL IsChildFolderOrFile(LPCTSTR parent, LPCTSTR child, BOOL itself)
{
  if ((parent == NULL) || (lstrlen(parent) == 0)) // root folder
  {
    return TRUE;
  }

  return IsChildFolderOrFile(parent, lstrlen(parent), child, lstrlen(child), itself);
}

BOOL IsChildFolderOrFile(LPCTSTR parent, int parent_len, LPCTSTR child, int child_len, BOOL itself)
{
  if ((parent == NULL) || (lstrlen(parent) == 0)) // root folder
    return TRUE;

  if (parent_len < 0)
    parent_len = lstrlen(parent);
  if (child_len < 0)
    child_len = lstrlen(child);

  if ((parent_len < child_len) &&
    (StrCmpNI(parent, child, parent_len) == 0) &&
    (child[parent_len] == '\\'))
    return TRUE;

  if (itself && 
    (CompareStringICNC(parent, child) == 0))
    return TRUE;

  return FALSE;
}

BOOL IsChildFolderOrFile(LPCSTR parent, LPCSTR child, BOOL itself)
{
  if ((parent == NULL) || (strlen(parent) == 0)) // root folder
  {
    return TRUE;
  }

  size_t parent_len = strlen(parent);
  if ((parent_len < strlen(child)) &&
    (_strnicmp(parent, child, parent_len) == 0) &&
    (child[parent_len] == '\\'))
    return TRUE;

  if (itself && 
    (_strcmpi(parent, child) == 0))
    return TRUE;

  return FALSE;
}

// return 0 : no available space, -1 : error, 1 : success
int CheckDiskFreeSpace(LPCTSTR fullname, __int64 fileSize)
{
  ULARGE_INTEGER freeBytes;
  ULARGE_INTEGER totalBytes;
  ULARGE_INTEGER totalFreeBytes;
  TCHAR path[KMAX_PATH];

  SplitPathName(fullname, path, NULL);
  if (GetDiskFreeSpaceEx(path, &freeBytes, &totalBytes, &totalFreeBytes))
  {
    __int64 free = MAKE_INT64(freeBytes.LowPart, freeBytes.HighPart);
    __int64 tfree = MAKE_INT64(totalFreeBytes.LowPart, totalFreeBytes.HighPart);

    return (free > fileSize + 10 * 1024 * 1024);
  }
  else
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("GetDiskFreeSpaceEx error : \n"), GetLastError());
    OutputDebugString(msg);
  }
  return -1;
}

BOOL FileAccessWrite(LPCTSTR filename)
{
  int err = _waccess_s(filename, 2);
  if (err == EACCES) // no permission
    return FALSE;

  if (err == ENOENT) // file not exist
  {
    TCHAR path[MAX_PATH];
    SplitPathName(filename, path, NULL);
    err = _waccess_s(path, 2);
    if (err == EACCES) // no permission
      return FALSE;
  }
  return TRUE;
}

static void GetUnaccessableItemsEx(LPCTSTR path, LPTSTR buff)
{
	TCHAR filename[KMAX_PATH];
	WIN32_FIND_DATA fd;

	memset(&fd, 0, sizeof(WIN32_FIND_DATA));
	MakeFullPathName(filename, KMAX_PATH, path, L_ASTERISK_STR);

	// find normal files
	fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;

	HANDLE h = FindFirstFile(filename, &fd);
	LPTSTR server_id = NULL;
  BOOL need_more = TRUE;

	if (h != INVALID_HANDLE_VALUE)
	{
		do {
			if ((CompareStringNC(fd.cFileName, L".") == 0) ||
				(CompareStringNC(fd.cFileName, L"..") == 0))
				continue;

      if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
      {
        if (lstrlen(buff) + lstrlen(fd.cFileName) < 500)
        {
          if (lstrlen(buff) > 0)
            StringCchCat(buff, 512, L"|"); // DA_VALUE_SEPARATOR
          StringCchCat(buff, 512, fd.cFileName);
        }
        else
        {
          need_more = FALSE;
        }
      }

			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
        int len = lstrlen(path) + lstrlen(fd.cFileName) + 1;
        if (len < KMAX_PATH)
        {
      	  TCHAR subfolder[KMAX_PATH];
      	  MakeFullPathName(subfolder, KMAX_PATH, path, fd.cFileName);
          GetUnaccessableItemsEx(subfolder, buff);
        }
      }
		} while (need_more && FindNextFile(h, &fd));

		FindClose(h);
	}
}

LPTSTR GetUnaccessableItems(LPCTSTR pathname)
{
  LPTSTR buff = new TCHAR[512];

  buff[0] = '\0';
  GetUnaccessableItemsEx(pathname, buff);
  if (lstrlen(buff) <= 0)
  {
    delete[] buff;
    buff = NULL;
  }
  return buff;
}

__int64 GetFileSize(LPCTSTR filename)
{
  __int64 size = 0;
  /*
  FILE *f;
	errno_t err = _tfopen_s(&f, filename, _T("rb"));
	if (err == 0)
  */

#if 0 // ftell return -1 for large files(3GByte)
  FILE *f = OpenDelayedFile(filename, _T("rb"));
  if (f != NULL)
  {
    fseek(f, 0, SEEK_END);   // non-portable
    size = ftell(f);
    fclose(f);
  }
#else
	WIN32_FIND_DATA fd;
  BOOL r = FALSE;
	memset(&fd, 0, sizeof(WIN32_FIND_DATA));
	fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;

  // scan with real folder path if it or parent renamed
  TCHAR file_mask[KMAX_PATH+6];
  StringCchCopy(file_mask, KMAX_PATH+6, UNC_FILE_PREFIX);
  StringCchCat(file_mask, KMAX_PATH+6, filename);

  HANDLE h = FindFirstFile(file_mask, &fd);
	if (h != INVALID_HANDLE_VALUE)
	{
		if (lstrlen(fd.cFileName) > 0)
    {
      size = MAKE_INT64(fd.nFileSizeLow, fd.nFileSizeHigh);
    }
    FindClose(h);
  }
#endif
  return size;
}


#define KMAX_PATH_EX 300

__int64 GetFolderSize(LPCTSTR pathname)
{
  __int64 total_size = 0;
	WIN32_FIND_DATA fd;

	memset(&fd, 0, sizeof(WIN32_FIND_DATA));

  int len = lstrlen(pathname) + 10;
  LPTSTR filename = new TCHAR[len];
  if (len > (MAX_PATH-4))
  {
    int pref_len = lstrlen(UNC_FILE_PREFIX);
    StringCchCopy(filename, len, UNC_FILE_PREFIX);
    MakeFullPathName(&filename[pref_len], len-pref_len, pathname, L_ASTERISK_STR);
  }
  else
  {
	  MakeFullPathName(filename, len, pathname, L_ASTERISK_STR);
  }

	// find normal files
	fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;

	HANDLE h = FindFirstFile(filename, &fd);
	LPTSTR server_id = NULL;
  BOOL need_more = TRUE;

	if (h != INVALID_HANDLE_VALUE)
	{
		do {
			if ((CompareStringNC(fd.cFileName, L".") == 0) ||
				(CompareStringNC(fd.cFileName, L"..") == 0))
				continue;

			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
        int path_length = lstrlen(pathname) + lstrlen(fd.cFileName) + 2;
        LPTSTR subfolder = new TCHAR[path_length];
      	MakeFullPathName(subfolder, path_length, pathname, fd.cFileName);
        total_size += GetFolderSize(subfolder);
        delete[] subfolder;
      }
      else
      {
        //total_size += (__int64)fd.nFileSizeLow + ((__int64)fd.nFileSizeHigh << 32);
        total_size += MAKE_INT64(fd.nFileSizeLow, fd.nFileSizeHigh);
      }
		} while (FindNextFile(h, &fd));

		FindClose(h);
	}
  delete[] filename;
  return total_size;
}

void FormatFileSize(LPTSTR buff, int length, __int64 size)
{
  int unit = 0;
  int remain = 0;
  static TCHAR *SIZE_UNIT[] = 
  {
    _T("Byte"),
    _T("KB"),
    _T("MB"),
    _T("GB"),
    _T("TB")
  };

  while (size > 1024)
  {
    remain = (int)(size % 1024);
    size = (size / 1024);

    ++unit;
    if(unit == 4)
      break;
  }
  remain = remain * 100 / 1024;
  if (remain > 0)
    StringCchPrintf(buff, length, _T("%d.%.2d"), (int)size, remain);
  else
    StringCchPrintf(buff, length, _T("%d"), (int)size);

  StringCchCat(buff, length, SIZE_UNIT[unit]);
}

__int64 FileSizeFromStringDouble(const char *str)
{
  float d;
  if (sscanf_s(str, "%f", &d) == 1)
    return ((__int64)d);
  return 0;
}

__int64 FileSizeFromStringDoubleT(const TCHAR *str)
{
  char buff[24];

  if (ToUtf8String(buff, 24, str, lstrlen(str)) > 0)
    return FileSizeFromStringDouble(buff);
  return 0;
}

__int64 FileSizeFromString(const char *str)
{
  char buff[24];
  int p = 0;

  if(str == NULL)
    return 0;

  while(*str != '\0')
  {
    if ((*str >= '0') && (*str <= '9'))
      buff[p++] = *str;
    else if(*str != ',')
      break;
    if(p >= 20)
      break;
    str++;
  }
  if (p > 0)
  {
    __int64 value = 0;
    buff[p++] = '\0';
    if (sscanf_s(buff, "%lld", &value) >= 1)
      return value;
  }
  return 0;
}

__int64 FileSizeFromStringT(const TCHAR *str)
{
  char buff[24];

  if (ToUtf8String(buff, 24, str, lstrlen(str)) > 0)
    return FileSizeFromString(buff);
  return 0;
}

#include <Shlobj.h>

/*
BOOL CALLBACK RefreshFolderSelectionCB(HWND hWnd, LPARAM lParam)
{
  WCHAR sBuffer[KMAX_PATH] = {0};

  ::GetClassName(hWnd, sBuffer, KMAX_PATH);

  if (wcscmp(L"SysTreeView32", sBuffer) == 0)
  {
    ::InvalidateRect(hWnd, NULL, TRUE);
  }

  return TRUE;
}

void SHUpdateExplorerWindow()
{
  HWND hExplorer = FindWindowEx(GetDesktopWindow(), NULL, L"ExploreWClass", NULL);

  while(hExplorer != NULL)
  {
    EnumChildWindows(hExplorer, RefreshFolderSelectionCB, (LPARAM)&packet);
    hExplorer = FindWindowEx(GetDesktopWindow(), hExplorer, L"ExploreWClass", NULL);
  }
}
*/

void ShellNotifyIconChange(LPCTSTR pathname, BOOL bRecursiveChild)
{
#ifndef NO_USE_OVERLAY_ICON
  if (pathname != NULL)
  {
    LONG event_id = IsDirectoryExist(pathname) ? SHCNE_UPDATEDIR : SHCNE_UPDATEITEM;

    UINT flag = SHCNF_PATH | SHCNF_FLUSHNOWAIT;
    if (bRecursiveChild)
      flag |= SHCNF_NOTIFYRECURSIVE;

    SHChangeNotify(event_id, flag, pathname, 0);

    // update itself
    if (event_id == SHCNE_UPDATEDIR)
      SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH | SHCNF_FLUSHNOWAIT, pathname, 0);
  }
#endif // NO_USE_OVERLAY_ICON
}

void ShellNotifyIconChangeEx(LPCTSTR pathname, int event_id, BOOL bRecursiveChild)
{
#ifndef NO_USE_OVERLAY_ICON
  UINT flag = SHCNF_PATH | SHCNF_FLUSHNOWAIT;
  if (bRecursiveChild)
    flag |= SHCNF_NOTIFYRECURSIVE;

  SHChangeNotify(event_id, flag, pathname, 0);
#endif // NO_USE_OVERLAY_ICON
}

void ShellNotifyECMChange(LPCTSTR ecmFullPath)
{
#if 0 // should be run on explorer process.
  if (lstrlen(ecmFullPath) > 0)
  {
    //TCHAR shPath[KMAX_PATH + 100] = _T("::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{01F256E4-53BD-301F-2975-1B9AC3670110}\\");
    TCHAR shPath[KMAX_PATH + 100] = _T("S:\\");
    int start_of_path = lstrlen(shPath);
    StringCchCat(shPath, KMAX_PATH + 100, ecmFullPath);
    ReplaceChar(&(shPath[start_of_path]), '>', '\\');

    LONG event_id = SHCNE_ALLEVENTS; // SHCNE_UPDATEDIR; // : SHCNE_UPDATEITEM;
    UINT flag = SHCNF_PATH | SHCNF_FLUSHNOWAIT; // SHCNF_NOTIFYRECURSIVE;

    SHChangeNotify(event_id, flag, shPath, 0);
  }
#endif
}

void SplitNameSubPath(LPCTSTR i_pathname, LPTSTR o_name, LPTSTR o_sub)
{
  LPCTSTR n = StrChr(i_pathname, '\\');
  if (n != NULL)
  {
    int len = (int)(n - i_pathname);
    if (o_name != NULL)
    {
      memcpy(o_name, i_pathname, sizeof(TCHAR) * len);
      o_name[len] = '\0';
    }
    n++;
    len = lstrlen(n);
    if(o_sub != NULL)
    {
      memcpy(o_sub, n, sizeof(TCHAR) * len);
      o_sub[len] = '\0';
    }
  }
  else
  {
    if(o_name != NULL)
      StringCchCopy(o_name, KMAX_PATH, i_pathname);
    if (o_sub != NULL)
      o_sub[0] = '\0';
  }
}

void SplitPathName(LPCTSTR i_pathname, LPTSTR o_path, LPTSTR o_name)
{
  if (i_pathname != NULL)
  {
    LPCTSTR n = StrRChr(i_pathname, NULL, '\\');
    if(n != NULL)
    {
      int len = (int)(n - i_pathname);
      if(o_path != NULL)
      {
        memcpy(o_path, i_pathname, sizeof(TCHAR) * len);
        o_path[len] = '\0';
      }
      n++;
      len = lstrlen(n);
      if(o_name != NULL)
      {
        memcpy(o_name, n, sizeof(TCHAR) * len);
        o_name[len] = '\0';
      }
    }
    else
    {
      if (o_path != NULL)
        o_path[0] = '\0';
      if(o_name != NULL)
        StringCchCopy(o_name, KMAX_PATH, i_pathname);
    }
  }
  else
  {
    o_path[0] = '\0';
    o_name[0] = '\0';
  }
}

void SplitPathNameAlloc_D(LPCTSTR i_pathname, LPTSTR* o_path, LPTSTR* o_name)
{
  LPCTSTR n = StrRChr(i_pathname, NULL, '\\');
  if(n != NULL)
  {
    if (o_path != NULL)
    {
      if (*o_path != NULL)
        delete[] *o_path;

      int len = (int)(n - i_pathname);
      *o_path = new TCHAR[len+1];
      StringCchCopyN(*o_path, len+1, i_pathname, len);
    }
    n++;
    if (o_name != NULL)
    {
      if (*o_name != NULL)
        delete[] *o_name;

      int len = lstrlen(n) + 1;
      *o_name = new TCHAR[len];
      StringCchCopy(*o_name, len, n);
    }
  }
  else
  {
    if (o_name != NULL)
    {
      if (*o_name != NULL)
        delete[] *o_name;

      int len = lstrlen(i_pathname) + 1;
      *o_name = new TCHAR[len];
      StringCchCopy(*o_name, len, i_pathname);
    }
  }
}

void SplitPathNameAlloc(LPCTSTR i_pathname, LPTSTR* o_path, LPTSTR* o_name)
{
  LPCTSTR n = StrRChr(i_pathname, NULL, '\\');
  if(n != NULL)
  {
    if (o_path != NULL)
    {
      if (*o_path != NULL)
        delete[] *o_path;

      int len = (int)(n - i_pathname);
      *o_path = new TCHAR[len+1];
      StringCchCopyN(*o_path, len+1, i_pathname, len);
    }
    n++;
    if (o_name != NULL)
    {
      if (*o_name != NULL)
        delete[] *o_name;

      int len = lstrlen(n) + 1;
      *o_name = new TCHAR[len];
      StringCchCopy(*o_name, len, n);
    }
  }
  else
  {
    if (o_name != NULL)
    {
      if (*o_name != NULL)
        delete[] *o_name;

      int len = lstrlen(i_pathname) + 1;
      *o_name = new TCHAR[len];
      StringCchCopy(*o_name, len, i_pathname);
    }
  }
}

LPCTSTR FindFileExtension(LPCTSTR pathName)
{
  LPCTSTR filename = StrRChr(pathName, NULL, '\\');
  if (filename == NULL)
    filename = pathName;
  else
    filename++;

  LPCTSTR ext = StrRChr(filename, NULL, '.');
  if (ext != NULL)
  {
    ++ext;
    if (*ext != '\0')
      return ext;
  }
  return NULL;
}

LPTSTR ChangeFileExtension(LPCTSTR pathName, LPCTSTR ext)
{
  LPCTSTR originExt = FindFileExtension(pathName);
  if ((originExt != NULL) && (StrCmpI(originExt, ext) == 0))
  {
    return AllocString(pathName);
  }

  if (originExt == NULL)
  {
    return AllocAddString(pathName, '.', ext);
  }

  int len = originExt - pathName + lstrlen(ext);
  LPTSTR newPathName = new TCHAR[len];
  StringCchCopyN(newPathName, len, pathName, (int)(originExt - pathName));
  StringCchCat(newPathName, len, ext);

  return newPathName;
}

BOOL TrimFileExtension(LPTSTR filename)
{
  LPTSTR e = StrRChr(filename, NULL, '.');
  if (e != NULL)
  {
    *e = '\0';
  }
  return (e != NULL);
}

// return 1 if p1 > p2
int CompareParentFolderPath(LPTSTR p1, LPTSTR p2)
{
  LPCTSTR s1 = StrRChr(p1, NULL, '\\');
  LPCTSTR s2 = StrRChr(p2, NULL, '\\');
  if (s1 == NULL)
  {
    if (s2 == NULL)
      return 0;
    return -1;
  }
  else
  {
    if (s2 == NULL)
      return 1;

    int n1 = (int)(s1 - p1);
    int n2 = (int)(s2 - p2);

    if (n1 > n2)
      return 1;
    else if(n1 < n2)
      return -1;

    return StrCmpNI(p1, p2, n1);
  }
}

BOOL ReadyUniqueFilename(LPTSTR fullPathName, int len, LPTSTR dotExt)
{
  LPTSTR fullName = new TCHAR[len];

  StringCchCopy(fullName, len, fullPathName);
  StringCchCat(fullName, len, dotExt);
  if (!IsFileExistAttribute(fullName))
  {
    StringCchCopy(fullPathName, len, fullName);
    delete[] fullName;
    return TRUE;
  }
  else
  {
    int index = 1;
    while (index < 9999)
    {
      StringCchPrintf(fullName, len, _T("%s(%d)"), fullPathName, index);
      StringCchCat(fullName, len, dotExt);
      if (!IsFileExistAttribute(fullName))
      {
        StringCchCopy(fullPathName, len, fullName);
        delete[] fullName;
        return TRUE;
      }
      index++;
    }
  }
  delete[] fullName;
  return FALSE;
}

void ReplaceControlChars(LPTSTR str)
{
  if (str != NULL)
  {
    while(*str != '\0')
    {
      if (*str < ' ')
        *str = ' ';
      str++;
    }
  }
}

LPTSTR TrimStr(LPTSTR str)
{
  while(*str != 0)
  {
    if (isspace(*str))
      ++str;
    else
      break;
  }

  int len = lstrlen(str);
  while(--len >= 0)
  {
    if ( isspace(str[len]))
      str[len] = '\0';
    else
      break;
  }

  return str;
}

int CompareStringNC(LPCTSTR s1, LPCTSTR s2)
{
  if ((s1 == NULL) && (s2 == NULL))
    return 0;
  if (s1 == NULL)
    return 1;
  if (s2 == NULL)
    return -1;

  return lstrcmp(s1, s2);
}

int CompareStringICNC(LPCTSTR s1, LPCTSTR s2)
{
  if ((s1 == NULL) && (s2 == NULL))
    return 0;
  if (s1 == NULL)
    return 1;
  if (s2 == NULL)
    return -1;

  return lstrcmpi(s1, s2);
}

void MakeWritabeFile(LPCTSTR setting_file)
{
  DWORD attr = KGetFileAttributes(setting_file);
  if (attr & FILE_ATTRIBUTE_READONLY)
    SetFileAttributes(setting_file, (attr & ~FILE_ATTRIBUTE_READONLY));
}

BOOL KDeleteFile(LPCTSTR fileName)
{
  // TCHAR ext_filename[KMAX_EXTPATH];
  int n_len = lstrlen(fileName);
  int buff_len = n_len + 16;
  LPTSTR ext_filename = NULL;

  if (n_len >= MAX_PATH)
  {
    ext_filename = new TCHAR[buff_len];
    StringCchCopy(ext_filename, buff_len, UNC_FILE_PREFIX);
    StringCchCat(ext_filename, buff_len, fileName);
    fileName = ext_filename;
  }

  if (DeleteFile(fileName) == 0)
  {
    TCHAR msg[KMAX_LONGPATH+50];
    StringCchPrintf(msg, KMAX_LONGPATH+50, _T("DeleteFile failure error=0x%X, file:"), GetLastError());
    StringCchCat(msg, KMAX_LONGPATH+50, fileName);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }

  if (ext_filename != NULL)
    delete[] ext_filename;

  return TRUE;
}

BOOL KShellMoveToTrash(LPCTSTR filename, BOOL isFolder)
{
  TCHAR filepathname[KMAX_LONGPATH+10];

  if (lstrlen(filename) > MAX_PATH)
  {
    if (lstrlen(filename) > KMAX_LONGPATH)
    {
      TCHAR msg[100];
      StringCchPrintf(msg, 100, _T("KShellMoveToTrash failure so long file length(%d)"), lstrlen(filename));
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }
    StringCchCopy(filepathname, KMAX_LONGPATH+10, UNC_FILE_PREFIX);
    StringCchCat(filepathname, KMAX_LONGPATH+10, filename);
  }
  else
  {
    StringCchCopy(filepathname, KMAX_LONGPATH+1, filename);
  }

  filepathname[lstrlen(filepathname)+1] = '\0'; // double-null terminator
  SHFILEOPSTRUCT sf = { 0 };
  sf.hwnd = NULL;       // handle to window that will own generated windows, if applicable
  sf.wFunc = FO_DELETE;
  sf.pFrom = filepathname;
  sf.pTo = NULL;       // not used for deletion operations
  sf.fFlags = FOF_ALLOWUNDO|FOF_NO_UI|FOF_NORECURSION; // use the recycle bin

  const int retVal = SHFileOperation(&sf);
  if (retVal != 0)
  {
    /*
    if (IsDirectoryExist(filename))
      EmptyDirectory(filename, FALSE);
    */
    /*
    BOOL rtn = FALSE;
    try
    {
      if (isFolder)
        rtn = RemoveDirectory(filename);
      else
        rtn = DeleteFile(filepathname);
    }
    catch (CFileException *e)
    {
      TCHAR msg[300];
      TCHAR errmsg[256];
      e->GetErrorMessage(errmsg, 256, NULL);
      StringCchPrintf(msg, 300, _T("RemoveDirectory or DeleteFile (%s) error : %s"), filename, errmsg);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

  #ifdef _DEBUG__
      StringCchCat(msg, 300, _T("\n"));
      OutputDebugString(msg);
  #endif

      e->Delete();
    }
    return rtn;
    */
    return FALSE;
  }
  return TRUE;
}

BOOL KMoveFile(LPCTSTR prevFileName, LPCTSTR newFileName)
{
  LPTSTR ext_prev = NULL;
  LPTSTR ext_new = NULL;

  if (lstrlen(prevFileName) >= MAX_PATH)
  {
    int len = lstrlen(prevFileName) + 16;
    ext_prev = new TCHAR[len];
    StringCchCopy(ext_prev, len, UNC_FILE_PREFIX);
    StringCchCat(ext_prev, len, prevFileName);
    prevFileName = ext_prev;
  }
  if (lstrlen(newFileName) >= MAX_PATH)
  {
    int len = lstrlen(newFileName) + 16;
    ext_new = new TCHAR[len];
    StringCchCopy(ext_new, len, UNC_FILE_PREFIX);
    StringCchCat(ext_new, len, newFileName);
    newFileName = ext_new;
  }

  int retry_count = 0;
  int err = 0;
  BOOL rtn = FALSE;

  while(retry_count < 3)
  {
    if (MoveFile(prevFileName, newFileName))
    {
      rtn = TRUE;
      break;
    }
    err = GetLastError();
    if ((err == ERROR_FILE_NOT_FOUND) ||
      (err == ERROR_ACCESS_DENIED) ||
      (err == ERROR_SHARING_VIOLATION))
    {
      retry_count++;
    }
    else
    {
      break;
    }
    Sleep(20);
  }

  if (!rtn)
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Retry=%d, error=%d : "), retry_count, err);
    StringCchCat(msg, 256, prevFileName);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }

  if (ext_prev != NULL)
    delete[] ext_prev;
  if (ext_new != NULL)
    delete[] ext_new;
  return rtn;
}

extern BYTE utf8_bom[];

HANDLE OpenLogFileShared(LPCTSTR fileName, BOOL addBOM)
{
  LPTSTR ext_filename = NULL;

  if (lstrlen(fileName) >= MAX_PATH)
  {
    int len = lstrlen(fileName) + 6;
    ext_filename = new TCHAR[len];
    StringCchCopy(ext_filename, len, UNC_FILE_PREFIX);
    StringCchCat(ext_filename, len, fileName);
    fileName = ext_filename;
  }

  HANDLE h = CreateFile(fileName,  GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 
    FILE_ATTRIBUTE_NORMAL, NULL); // FILE_FLAG_NO_BUFFERING
  if (h == INVALID_HANDLE_VALUE)
  {
    h = NULL;
    OutputDebugString(_T("Cannot open log file\n"));
  }
  else if (addBOM)
  {
    LONG hptr = 0;
    DWORD p = SetFilePointer(h, 0, &hptr, FILE_END);
    if ((p != INVALID_SET_FILE_POINTER) && (p == 0) && (hptr == 0))
    {
      DWORD written = 0;
      WriteFile(h, utf8_bom, 3, &written, NULL);
    }
  }
  return h;
}

void CloseLogFileShared(HANDLE h)
{
  CloseHandle(h);
}

FILE *OpenDelayedFile(LPCTSTR fileName, LPCTSTR openMode)
{
	FILE *f = NULL;
  errno_t err;
  int retry = 0;
  LPTSTR ext_filename = NULL;

  if (lstrlen(fileName) >= MAX_PATH)
  {
    int len = lstrlen(fileName) + 16;
    ext_filename = new TCHAR[len];
    StringCchCopy(ext_filename, len, UNC_FILE_PREFIX);
    StringCchCat(ext_filename, len, fileName);
    fileName = ext_filename;
  }

	while ((err = _tfopen_s(&f, fileName, openMode)) != 0)
  {
    if ((err == 2) || (err == 3)) // file not found
      return NULL;
    if (err == 13)
      return NULL;

    if(retry > 30)
    {
      TCHAR errstr[64];
      StringCchPrintf(errstr, 64, _T(", Error=%d"), err);
      TCHAR msg[300] = _T("Cannot open to setting-file:");
      StringCchCat(msg, 300, fileName);
      StringCchCat(msg, 300, errstr);
      StringCchCat(msg, 300, _T("\n"));
      OutputDebugString(msg);
      break;
    }
    retry++;
    ::Sleep(2);
  }

  if (ext_filename != NULL)
    delete[] ext_filename;
  return f;
}

BOOL readyNextSerialName(LPTSTR name, int length)
{
  int p = length - 1;
  if (p >= 0)
  {
    if (name[p] == '9')
    {
      name[p] = 'a';
    }
    else if (name[p] == 'z')
    {
      name[p] = '0';
      if (!readyNextSerialName(name, p))
        return FALSE;
    }
    else
    {
      name[p]++;
    }
    return TRUE;
  }
  return FALSE;
}

BOOL Ready4SerialUniqueFilename(LPTSTR filename, LPCTSTR dotExt)
{
  TCHAR numbers[6] = _T("0000");
  int end = lstrlen(filename);
  StringCchCat(filename, KMAX_PATH, _T("0000"));
  int p = lstrlen(filename) - 1;
  StringCchCat(filename, KMAX_PATH, dotExt);

  // total 3359230 count
  while(end <= p)
  {
    if (!IsFileExist(filename))
      return TRUE;

    if (!readyNextSerialName(&filename[end], 4))
      return FALSE;

    // count++;
  }
  return FALSE;
}

int LoadLocaleString(HINSTANCE hInst, int id, WORD lang, LPTSTR buff, int buff_size)
{
  HRSRC hResource = ::FindResourceEx(hInst, (LPWSTR) RT_STRING, 
    MAKEINTRESOURCE( (((id>>4)+1) & static_cast<WORD>(~0)) ), lang);
  int offset = (id & 0x00F);

  if (hResource != NULL)
  {
    HGLOBAL g = LoadResource(hInst, hResource);
    if (g)
    {
      int size = SizeofResource(hInst, hResource);
      LPBYTE s = (LPBYTE)LockResource(g);

      WORD len = *((WORD *) s);

      while ((offset > 0) && (size > 0))
      {
        s += (len + 1) * 2;

        len = *((WORD *) s);

        size -= (len + 1) * 2;
        offset--;
      }

      s += 2;

      if (len >= buff_size)
        len = buff_size - 1;
      memcpy(buff, s, len * sizeof(TCHAR));
      buff[len] = 0;
      // StringCchCopy(buff, buff_size, (LPTSTR)s);
      return lstrlen(buff);
    }
  }
  return 0;
}

void CopyStringTrim(LPTSTR buff, int buff_size, LPCTSTR src_string)
{
  LPCTSTR p = src_string;
  while(*p != 0)
  {
    if (*p > ' ')
      break;
    p++;
  }
  StringCchCopy(buff, buff_size, p);
  int i = lstrlen(buff) - 1;
  while(i >= 0)
  {
    if (buff[i] > ' ')
      break;
    buff[i] = 0;
    --i;
  }
  int len = lstrlen(buff);
  for(i = 0; i < len; i++)
  {
    if (buff[i] < ' ')
      buff[i] = ' ';
  }
}

void TrimQuoteString(LPTSTR buff)
{
  int buff_len = lstrlen(buff);
  LPTSTR p = TrimStr(buff);

  if (*p == '\'')
  {
    LPTSTR e = StrRChr(p+1, NULL, '\'');
    if (e != NULL)
    {
      *e = '\0';
      p++;
    }
  }
  else if (*p == '"')
  {
    LPTSTR e = StrRChr(p+1, NULL, '"');
    if (e != NULL)
    {
      *e = '\0';
      p++;
    }
  }

  if (p != buff)
    StringCchCopy(buff, buff_len, p);
}

BOOL ReadyTempFileName(LPCTSTR path, LPCTSTR originalFilename, LPTSTR Fullname)
{
  TCHAR filename[MAX_PATH];
  TCHAR body[MAX_PATH];

  MakeFullPathName(Fullname, MAX_PATH, path, originalFilename);

  if ( !IsFileExist(Fullname) )
  {
    return TRUE;
  }

  StringCchCopy(filename, MAX_PATH, originalFilename);
  // LPTSTR ext = StrRChr(filename, NULL, '.');
  LPTSTR ext = (LPTSTR)GetFileExtension(filename);

  if (ext != NULL)
  {
    --ext;
    *ext = '\0';
  }

  MakeFullPathName(body, MAX_PATH, path, filename);
  StringCchCat(body, MAX_PATH, _T("*"));
  if (ext != NULL)
  {
    StringCchCat(body, MAX_PATH, _T("."));
    StringCchCat(body, MAX_PATH, ext);
  }

  // FindFirst
	WIN32_FIND_DATA fd;
	memset(&fd, 0, sizeof(WIN32_FIND_DATA));
	fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
  int can_len = lstrlen(filename);
  int purename_len = can_len;
  if (ext != NULL)
  {
    can_len += lstrlen(ext) + 6;
  }

  // scan with real folder path if it or parent renamed
	HANDLE h = FindFirstFile(body, &fd);
  TCHAR m_index_str[8] = _T("");

	if (h != INVALID_HANDLE_VALUE)
	{
    do 
    {
		  if ( ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) &&
        (lstrlen(fd.cFileName) == can_len))
      {
        LPTSTR index_str = &fd.cFileName[purename_len+1];

        if (m_index_str[0] == '\0')
        {
          StringCchCopyN(m_index_str, 8, index_str, 4);
        }
        else
        {
          if (StrCmpN(index_str, m_index_str, 4) > 0)
            StringCchCopyN(m_index_str, 8, index_str, 4);
        }
      }
		} while (FindNextFile(h, &fd));

    FindClose(h);
  }

  MakeFullPathName(Fullname, MAX_PATH, path, filename);
  if (m_index_str[0] == '\0')
  {
    StringCchCat(Fullname, MAX_PATH, _T("-0000"));
  }
  else
  {
    StringCchCat(Fullname, MAX_PATH, _T("-"));
    int p = lstrlen(Fullname);
    StringCchCat(Fullname, MAX_PATH, m_index_str);

    readyNextSerialName(&Fullname[p], 4);
  }

  if (ext != NULL)
  {
    StringCchCat(Fullname, MAX_PATH, _T("."));
    StringCchCat(Fullname, MAX_PATH, ext);
  }
  return TRUE;
}

/*
BOOL isWindowsSpecialFolder()
{
  return SHGetSpecialFolderPath(CSIDL_STARTMENU);
}
*/

BOOL IsWindowXP()
{
  /*
  OSVERSIONINFO ovi;
  ovi.dwOSVersionInfoSize = sizeof(ovi);
  GetVersionEx(&ovi);
  return (ovi.dwMajorVersion < 6);
  */
  return FALSE;
}

BOOL IsWindow10_1607_More()
{
  return IsWindows10OrGreater();
}

int getPathDepth(LPCSTR IN path)
{
  int path_depth = 0;
  while((path != NULL) && (*path != 0))
  {
    if (*path == '\\')
      path_depth++;
    path++;
  }
  return path_depth;
}

int CompareRelativePath(LPCTSTR p1, LPCTSTR p2)
{
  if ((p1 == NULL) || (lstrlen(p1) == 0))
  {
    if ((p2 == NULL) || (lstrlen(p2) == 0))
      return 0;
    return 1;
  }
  else
  {
    if ((p2 == NULL) || (lstrlen(p2) == 0))
      return -1;

    return CompareStringICNC(p1, p2);
  }
}

const TCHAR gInvalidFilenameChar[] = _T("\\/:*?\"<>|");

BOOL IsValidFilenameString(LPCTSTR filename)
{
  if (lstrlen(filename) >= KMAX_PATH)
    return FALSE;

  int len = lstrlen(filename);
  for (int i = 0; i < len; i++)
  {
    if ((*filename < ' ') ||
      (StrChr(gInvalidFilenameChar, *filename) != NULL))
      return FALSE;
    ++filename;
  }
  return TRUE;
}

LPCTSTR GetFileExtension(LPCTSTR filePathName)
{
  LPCTSTR last = StrRChr(filePathName, NULL, '\\');
  LPCTSTR p = StrRChr(filePathName, NULL, '.');
  if ((p != NULL) && (last < p))
  {
    ++p;
    return p;
  }
  return  NULL;
}

void GetComputerNameStr(LPTSTR name, int bufferLength)
{
  DWORD length = bufferLength;
  GetComputerName(name, &length);
}

#include <pdh.h>
#include <pdhmsg.h>
#pragma comment(lib, "pdh.lib")

#define NUMBER_OF_PATHS 1
HCOUNTER Counter[NUMBER_OF_PATHS];

static LPCTSTR netCounterPaths[NUMBER_OF_PATHS] = 
{
  //_T("\\TCPv4\\Segments/sec"),
  //_T("\\TCPv6\\Segments/sec"),
  //_T("\\UDPv4\\Datagrams/sec"),
  //_T("\\UDPv6\\Datagrams/sec"),
  _T("\\Network Interface(*)\\Bytes Total/sec")
};

HANDLE ReadyNetworkTrafficLevel(OUT LPTSTR msg, IN int length)
{
  PDH_STATUS Status;
  HQUERY Query = NULL;
  int avail = 0;

  Status = PdhOpenQuery(NULL, NULL, &Query);
  if (Status != ERROR_SUCCESS)
  {
    if (msg != NULL)
      StringCchPrintf(msg, length, _T("PdhOpenQuery failed with status 0x%x."), Status);
    goto Cleanup;
  }

  StringCchPrintf(msg, length, _T("NetworkTrafficPaths:"));
  for (int i = 0; i < NUMBER_OF_PATHS; i++)
  {
    Status = PdhAddCounter(Query, netCounterPaths[i], 0, &Counter[i]);
    if (msg != NULL)
      StringCchCat(msg, length, netCounterPaths[i]);

    if (Status != ERROR_SUCCESS)
    {
      Counter[i] = NULL;
      if (msg != NULL)
      {
        TCHAR str[64];
        StringCchPrintf(str, 64, _T(":failed with status 0x%x, "), Status);
        StringCchCat(msg, length, str);
      }
    }
    else
    {
      ++avail;
      if (msg != NULL)
        StringCchCat(msg, length, _T(", "));
    }
  }

  Status = PdhCollectQueryData(Query);
  if (Status != ERROR_SUCCESS)
  {
    if (msg != NULL)
      StringCchPrintf(msg, length, _T("PdhCollectQueryData failed with status 0x%x."), Status);
    avail = 0;
  }

Cleanup:
  if ((avail == 0) && (Query != NULL))
    PdhCloseQuery(Query);

  if (msg != NULL)
  {
    OutputDebugString(msg);
    OutputDebugString(_T("\n"));
  }
  return Query;
}

int GetNetworkTrafficLevel(HANDLE Query)
{
  PDH_FMT_COUNTERVALUE DisplayValue;
  PDH_STATUS Status;
  DWORD CounterType;
  int level = 0;

  if (Query)
  {
    Status = PdhCollectQueryData(Query);
    if (Status == ERROR_SUCCESS)
    {
      int out[NUMBER_OF_PATHS];
      for(int i = 0; i < NUMBER_OF_PATHS; i++)
      {
        if (Counter[i] != NULL)
        {
          Status = PdhGetFormattedCounterValue(Counter[i],
                                                   PDH_FMT_DOUBLE,
                                                   &CounterType,
                                                   &DisplayValue);
          if (Status == ERROR_SUCCESS)
          {
            if (DisplayValue.doubleValue < 2147483646.0) // 0x7FFFFFFE
              out[i] = (int) DisplayValue.doubleValue;
            else
              out[i] = 2147483646;
            level += out[i];
          }
          else
          {
            out[i] = -1;
          }
        }
      }
      /*
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("%d, %d, %d, %d = %d\n"), out[0], out[1], out[2], out[3], out[4] );
      OutputDebugString(msg);
      */
    }
  }
  return level;
}

void CloseNetworkTrafficLevel(HANDLE Query)
{
  if (Query)
    PdhCloseQuery(Query);
}


int MeasureNetworkTrafficLevel(int timeDelaySec)
{
  PDH_STATUS Status;
  HQUERY Query = NULL;
  HCOUNTER Counter;
  int r = 0;
  TCHAR netCounterPath[] = _T("\\Network Interface(*)\\Bytes Total/sec");

  Status = PdhOpenQuery(NULL, NULL, &Query);
  if (Status == ERROR_SUCCESS)
  {
    Status = PdhAddCounter(Query, netCounterPath, 0, &Counter);
    if (Status == ERROR_SUCCESS)
    {
      Status = PdhCollectQueryData(Query);
      if (Status == ERROR_SUCCESS)
      {
        if (timeDelaySec > 0)
          Sleep(timeDelaySec * 1000);

        Status = PdhCollectQueryData(Query);
        if (Status == ERROR_SUCCESS)
        {
          PDH_FMT_COUNTERVALUE DisplayValue;
          DWORD CounterType;

          Status = PdhGetFormattedCounterValue(Counter,
                                                   PDH_FMT_DOUBLE,
                                                   &CounterType,
                                                   &DisplayValue);
          if (Status == ERROR_SUCCESS)
          {
            if (DisplayValue.doubleValue < 2147483646.0) // 0x7FFFFFFE
              r = (int) DisplayValue.doubleValue;
            else
              r = 2147483646;
          }
          else
          {
            r = -5;
          }
        }
        else
        {
          r = -4;
        }
      }
      else
      {
        r = -3;
      }
    }
    else
    {
      r = -2;
    }
  }
  else
  {
    r = -1;
  }

  if (Query)
    PdhCloseQuery(Query);
  return r;
}

BOOL GetKnownFolderPath(REFKNOWNFOLDERID refid, LPTSTR* pPath)
{
  HRESULT hr = SHGetKnownFolderPath(refid, KF_FLAG_NO_ALIAS, NULL, pPath);
  return SUCCEEDED(hr);
}

void BytesToString(LPTSTR str, int strLength, BYTE *pBuffer, int bufferLength)
{
  if (strLength <= (bufferLength * 2))
    bufferLength = (strLength - 1) / 2;

  for(int i = 0; i < bufferLength; i++)
  {
    StringCchPrintf(str, 3, _T("%.2X"), *pBuffer);
    pBuffer++;
    str += 2;
  }
}

#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")

BOOL GetSystemMacAddress(LPTSTR address, int length)
{
  BOOL rtn = FALSE;
  ULONG size = sizeof(IP_ADAPTER_INFO);
  IP_ADAPTER_INFO* pAdapterInfo = (IP_ADAPTER_INFO*) new BYTE[size];
  if (GetAdaptersInfo(pAdapterInfo, &size) == ERROR_BUFFER_OVERFLOW)
  {
    delete pAdapterInfo;
    pAdapterInfo = (IP_ADAPTER_INFO*) new BYTE[size];
    GetAdaptersInfo(pAdapterInfo, &size);
  }

  IP_ADAPTER_INFO* pAdapters = pAdapterInfo;
  while (pAdapters != NULL)
  {
    if (pAdapters->AddressLength > 0)
    {
      BytesToString(address, length, pAdapters->Address, pAdapters->AddressLength);
      rtn = TRUE;
      break;
    }
    pAdapters = pAdapters->Next;
  }

  if (pAdapterInfo != NULL)
    delete[] pAdapterInfo;
  return rtn;
}

BOOL IsMatchSystemMacAddress(LPCTSTR pMacAddress)
{
  BOOL rtn = FALSE;
  ULONG size = sizeof(IP_ADAPTER_INFO);
  IP_ADAPTER_INFO* pAdapterInfo = (IP_ADAPTER_INFO*) new BYTE[size];
  if (GetAdaptersInfo(pAdapterInfo, &size) == ERROR_BUFFER_OVERFLOW)
  {
    delete pAdapterInfo;
    pAdapterInfo = (IP_ADAPTER_INFO*) new BYTE[size];
    GetAdaptersInfo(pAdapterInfo, &size);
  }

  IP_ADAPTER_INFO* pAdapters = pAdapterInfo;
  TCHAR address[32];

  while (pAdapters != NULL)
  {
    if (pAdapters->AddressLength > 0)
    {
      BytesToString(address, 32, pAdapters->Address, pAdapters->AddressLength);
      if (CompareStringNC(address, pMacAddress) == 0)
      {
        rtn = TRUE;
        break;
      }
    }
    pAdapters = pAdapters->Next;
  }

  if (pAdapterInfo != NULL)
    delete[] pAdapterInfo;
  return rtn;
}

#ifdef _USE_INET
BOOL IsInternetConnected()
{
  // #ifdef _USE_InternetGetConnectedState
  BOOL networkConnected = FALSE;

#ifdef USE_SYNC_ENGINE
  if (IsWindowXP())
  {
    DWORD dwFlag = 0;
    networkConnected = InternetGetConnectedState(&dwFlag, 0);
  }
  else
  {
    CComPtr<INetworkListManager> pNLM;

    HRESULT hr = CoCreateInstance(CLSID_NetworkListManager, NULL, 
      CLSCTX_ALL, __uuidof(INetworkListManager), (LPVOID*)&pNLM);
    if (SUCCEEDED(hr))
    {
      NLM_CONNECTIVITY con = NLM_CONNECTIVITY_DISCONNECTED;
      hr = pNLM->GetConnectivity(&con);
      if (SUCCEEDED(hr))
      {
        // con = 0x042; // NLM_CONNECTIVITY_IPV6_NOTRAFFIC	| NLM_CONNECTIVITY_IPV4_INTERNET
        networkConnected = !!(con & (NLM_CONNECTIVITY_IPV4_INTERNET | NLM_CONNECTIVITY_IPV6_INTERNET));
        /*
        if (con & (NLM_CONNECTIVITY_IPV4_NOTRAFFIC | NLM_CONNECTIVITY_IPV6_NOTRAFFIC))
          networkConnected = FALSE;
        else
        networkConnected = (con != NLM_CONNECTIVITY_DISCONNECTED);
        */
      }
    }
  }
#endif
  return networkConnected;
}
#endif

#pragma comment(lib, "version.lib")

BOOL GetFileVersionInfoString(LPCTSTR moduleName, LPTSTR pBuff, int length)
{
  DWORD size = GetFileVersionInfoSize(moduleName, 0);
  if (size > 0)
  {
    LPBYTE buff = new BYTE[size];
    if (GetFileVersionInfo(moduleName, 0, size, buff))
    {
      UINT cbValue = 0;
      VS_FIXEDFILEINFO* fileinfo;

      if (VerQueryValue(buff, _T(""), (LPVOID*)&fileinfo, &cbValue) &&
        (cbValue == sizeof(VS_FIXEDFILEINFO)))
      {
        StringCchPrintf(pBuff, length, _T("%d.%d.%d.%d"), 
          HIWORD(fileinfo->dwFileVersionMS), LOWORD(fileinfo->dwFileVersionMS),
          HIWORD(fileinfo->dwFileVersionLS), LOWORD(fileinfo->dwFileVersionLS));
      }
    }
    delete[] buff;
  }
  return (lstrlen(pBuff) > 0);
  return FALSE;
}

BOOL GetFileVersionInfoString(LPCTSTR moduleName, LPSTR pBuff, int length)
{
  DWORD size = GetFileVersionInfoSize(moduleName, 0);
  if (size > 0)
  {
    LPBYTE buff = new BYTE[size];
    if (GetFileVersionInfo(moduleName, 0, size, buff))
    {
      UINT cbValue = 0;
      VS_FIXEDFILEINFO* fileinfo;

      if (VerQueryValue(buff, _T(""), (LPVOID*)&fileinfo, &cbValue) &&
        (cbValue == sizeof(VS_FIXEDFILEINFO)))
      {
        StringCchPrintfA(pBuff, length, "%d.%d.%d.%d",
          HIWORD(fileinfo->dwFileVersionMS), LOWORD(fileinfo->dwFileVersionMS),
          HIWORD(fileinfo->dwFileVersionLS), LOWORD(fileinfo->dwFileVersionLS));
      }
    }
    delete[] buff;
  }
  return (strlen(pBuff) > 0);
  return FALSE;
}
