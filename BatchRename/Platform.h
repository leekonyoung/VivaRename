/**
* @file Platform.h
* @date 2015.02
* @brief Platform specific functions header file
*/
#ifndef _DESTINY_PLATFORM_SPECFIC_MODULE_H_
#define _DESTINY_PLATFORM_SPECFIC_MODULE_H_

#define MAKE_INT64(lo, hi) ((__int64)lo + ((__int64)hi << 32))

#define IS_EMPTY_STR(sz) ((sz == NULL) || (*sz == 0))


// do not using CoInitialize

#define K_CoInitializeEx(p, c) (S_OK)
#define K_CoUninitialize() 0

#define KMAX_PATH MAX_PATH
#define KMAX_LONGPATH 520

/**
* @brief WSTR을 UTF-8 스트링으로 변환한다.
* @param bstr : 출력 UTF-8 스트링 버퍼(NULL일 때에는 변환하지 않고 출력 스트링 길이를 리턴한다)
* @param buff_size : 버퍼 크기
* @param str : 입력 WSTR 스트링
* @param len : 입력 스트링 길이(-1일때에는 NULL문자까지)
* @return int : 출력 스트링 길이를 리턴한다.
*/
int ToUtf8String(char *bstr, int buff_size, LPCTSTR str, int len);
/**
* @brief 새로 할당한 버퍼에 WSTR을 UTF-8 스트링으로 변환하고 버퍼를 리턴한다.
* @param str : 입력 WSTR 스트링
* @param len : 입력 스트링 길이(-1일때에는 NULL문자까지)
* @return char* : 출력 스트링 버퍼
*/
char *MakeUtf8String(LPCTSTR str, int len = -1);

/**
* @brief UTF-8 스트링을 WSTR으로 변환한다.
* @param dest : 출력 WSTR 스트링 버퍼(NULL일 때에는 변환하지 않고 출력 스트링 길이를 리턴한다)
* @param dest_size : 버퍼 크기
* @param str : 입력 UTF-8 스트링
* @param len : 입력 스트링 길이(-1일때에는 NULL문자까지)
* @return int : 출력 스트링 길이를 리턴한다.
*/
int ToUnicodeString(TCHAR *dest, int dest_size, LPCSTR str, int len);
/**
* @brief 새로 할당한 버퍼에 UTF-8 스트링을 WSTR으로 변환하고 버퍼를 리턴한다.
* @param str : 입력 UTF-8 스트링
* @param len : 입력 스트링 길이(-1일때에는 NULL문자까지)
* @return TCHAR* : 출력 스트링 버퍼
*/
TCHAR *MakeUnicodeString(LPCSTR str, int len = -1);

/**
* @brief 새로 할당한 버퍼에 지정한 스트링을 복사하여 리턴한다.
* @param str : 입력 스트링
* @return LPSTR : 복사된 출력 스트링
*/
LPSTR AllocCString(LPCSTR str, int len = -1);

LPSTR AllocAddCString(LPCSTR dest, char separator, LPCSTR src);

/**
* @brief 새로 할당한 버퍼에 지정한 스트링을 복사하여 리턴한다.
* @param str : 입력 스트링
* @return LPSTR : 복사된 출력 스트링
*/
LPTSTR AllocString(LPCTSTR str, int len = -1);
/**
* @brief 새로 할당한 버퍼에 지정한 path, name 스트링을 붙여 복사하여 리턴한다.
* @param path : 경로
* @param name : 파일명
* @return LPSTR : 복사된 출력 스트링
*/
LPTSTR AllocPathName(LPCTSTR path, LPCTSTR name);

LPTSTR AllocAddString(LPCTSTR dest, TCHAR separator, LPCTSTR src);

LPTSTR AllocAddStringForce(LPCTSTR dest, TCHAR separator, LPCTSTR src);

/**
* @brief 새로 할당한 버퍼에 지정한 스트링을 복사하여 리턴한다.
* @param pDest : 새로 할당한 버퍼 포인터의 포인터
* @param src : 입력 스트링
* @param len : 입력 스트링 길이
*/
void AllocCopyString(LPTSTR *pDest, LPCTSTR src, int len = -1);
/**
* @brief 새로 할당한 버퍼에 지정한 스트링을 복사하여 리턴한다. 소스의 앞 뒤의 white space 글자는 제외한다.
* @param pDest : 새로 할당한 버퍼 포인터의 포인터
* @param src : 입력 스트링
*/
void AllocCopyStringTrim(LPTSTR *pDest, LPCTSTR src);

/**
* @brief p1, p2 교집합에 해당하는 경로명을 p1에 설정한다.
*/
void AllocMergePath(LPTSTR* p1, LPTSTR p2);

/**
* @brief 문자열에서 특정 문자를 지정한 문자로 바꾼다.
* @param str : 입력 스트링
* @param s : 찾는 문자열
* @param r : 대체할 문자열
* @return int : 바뀐 문자 개수
*/
int ReplaceChar(LPTSTR str, TCHAR s, TCHAR r);

int ReplaceCChar(LPSTR str, char s, char r);

BOOL IsOneOfFolderBelow(LPTSTR stringArray, TCHAR separator, LPCTSTR str);

BOOL IsOneOfString(LPTSTR stringArray, TCHAR separator, LPCTSTR str);

LPCTSTR FindFileExtension(LPCTSTR pathName);

int kstrcatChar(char *d, int buff_len, char c);

/**
* @brief SYSTEMTIME 값을 문자열로 출력한다.
* @param buff : 출력할 버퍼
* @param buff_size : 버퍼 크기
* @param systime : 날짜 및 시간
* @details A. 날짜 및 시간 정보의 연,월,일,시,분,초를 "%d-%.2d-%.2d %.2d:%.2d:%.2d" 형식으로 출력한다.
*/
void SystemTimeToString(char *buff, int buff_size, SYSTEMTIME &systime);

/**
* @brief SYSTEMTIME 값을 문자열로 출력한다.
* @param buff : 출력할 버퍼
* @param buff_size : 버퍼 크기
* @param systime : 날짜 및 시간
* @details A. 날짜 및 시간 정보의 연,월,일,시,분,초,밀리초를 "%d-%.2d-%.2d %.2d:%.2d:%.2d:%.3d" 형식으로 출력한다.
*/
void SystemTimeToStringEx(char *buff, int buff_size, SYSTEMTIME &systime);

/**
* @brief SYSTEMTIME 값을 현재의 타임존에 대한 로컬타임으로 변환하여 문자열로 출력한다.
* @param buff : 출력할 버퍼
* @param buff_size : 버퍼 크기
* @param systime : 날짜 및 시간
* @details A. 날짜 및 시간 정보의 연,월,일,시,분,초를 "%d-%.2d-%.2d %.2d:%.2d:%.2d" 형식으로 출력한다.
*/
void SystemUTCTimeToString(char *buff, int buff_size, SYSTEMTIME &systime);

/**
* @brief FILETIME 값을 문자열로 출력한다.
* @param buff : 출력할 버퍼
* @param buff_size : 버퍼 크기
* @param datetime : 날짜 및 시간
* @details A. 날짜 및 시간 정보의 연,월,일,시,분,초를 "%d-%.2d-%.2d %.2d:%.2d:%.2d" 형식으로 출력한다.
*/
void FileTimeToString(char *buff, int buff_size, FILETIME datetime);

/**
* @brief SYSTEMTIME 값을 문자열로 출력한다.
* @param buff : 출력할 버퍼
* @param buff_size : 버퍼 크기
* @param systime : 날짜 및 시간
* @details A. 날짜 및 시간 정보의 연,월,일,시,분,초를 "%d-%.2d-%.2d %.2d:%.2d:%.2d" 형식으로 출력한다.
*/
void SystemTimeToTString(TCHAR *buff, int buff_size, SYSTEMTIME &systime);
/**
* @brief SYSTEMTIME 값을 문자열로 출력한다.
* @param buff : 출력할 버퍼
* @param buff_size : 버퍼 크기
* @param systime : 날짜 및 시간
* @details A. 날짜 및 시간 정보의 (연,월,일,시,분)을 "%d-%.2d-%.2d %.2d:%.2d" 형식으로 출력한다.
*/
void SystemTimeToTStringHM(TCHAR *buff, int buff_size, SYSTEMTIME &systime);

/*
* @brief SYSTEMTIME 날짜값을 문자열(20181022)로 출력한다.
*/
void SystemTimeToTStringDateNum(TCHAR *buff, int buff_size, SYSTEMTIME &systime);

/**
* @brief SYSTEMTIME 값을 현재의 타임존에 대한 로컬타임으로 변환하여 문자열로 출력한다.
* @param buff : 출력할 버퍼
* @param buff_size : 버퍼 크기
* @param systime : 날짜 및 시간
* @details A. 날짜 및 시간 정보의 연,월,일,시,분,초를 "%d-%.2d-%.2d %.2d:%.2d:%.2d" 형식으로 출력한다.
*/
void SystemUTCTimeToTString(TCHAR *buff, int buff_size, SYSTEMTIME &systime);

/**
* @brief 연,월,일,시,분,초,밀리초를 지정한 SYSTEMTIME에 설정한다.
*/
void SetSystemTime(SYSTEMTIME& out_time, WORD y, WORD m, WORD d, WORD h, WORD mm, WORD ss, WORD ms);

/**
* @brief 문자열에서 '연-월-일 시:분:초' 값을 읽어 지정한 SYSTEMTIME에 설정한다.
*/
BOOL StringToSystemTime(char *buff, SYSTEMTIME &out_time);

/**
* @brief 문자열에서 '연-월-일' 값을 읽어 지정한 SYSTEMTIME에 설정한다.
*/
BOOL StringToSystemDate(char *buff, SYSTEMTIME &out_time);

/**
* @brief  SYSTEMTIME의 '연-월-일' 값을 문자열에 저장한다.
*/
BOOL StringFromSystemDate(char* date, int bufflen, SYSTEMTIME &time);

/**
* @brief 문자열에서 '연-월-일' 값을 읽어 지정한 SYSTEMTIME에 설정한다.
*/
BOOL StringToSystemDateT(LPCTSTR buff, SYSTEMTIME &out_time);

/**
* @brief  SYSTEMTIME의 '연-월-일' 값을 문자열에 저장한다.
*/
BOOL StringFromSystemDateT(LPTSTR buff, int bufflen, SYSTEMTIME &out_time);

/**
* @brief FILETIME값을 현재의 타임존에 대한 로컬타임으로 설정한다.
* @param ft : FILETIME값
* @param localTime : 로컬타임 SYSTEMTIME 값
*/
void FileTimeToLocalTime(FILETIME &ft, SYSTEMTIME &localTime);

/**
* @brief SYSTEMTIME 값을 예정시간으로 간략히 표시한다.
* @param buff : 출력할 버퍼
* @param buff_size : 버퍼 크기
* @param time : SYSTEMTIME 값
*/
void GetEllapsedTimeString(TCHAR *buff, int buff_size, SYSTEMTIME &time, BOOL withMS);

/**
* @brief 지정한 파일의 최종 수정 시간과 최종 액세스 시간을 리턴한다.
* @param filename : 대상 파일 경로명
* @param pModifyTime : 최종 수정 시간을 저장할 버퍼
* @param pAccessTime : 최종 액세스 시간을 저장할 버퍼
* @return BOOL : 성공하면 TRUE를 리턴한다.
*/
BOOL GetFileModifiedTime(LPCTSTR filename, SYSTEMTIME *pModifyTime, SYSTEMTIME *pAccessTime);

/**
* @brief src에 초값을 더한 값을 dest에 저장한다.
* @param dest : 대상 날짜 및 시간
* @param src : 소스 날짜 및 시간
* @param sec : 초
*/
void AddSystemTimeMS(SYSTEMTIME& dest, SYSTEMTIME& src, int sec);

/**
* @brief src에 일,시,분을 뺀 값을 dest에 저장한다.
* @param dest : 대상 날짜 및 시간
* @param src : 소스 날짜 및 시간
* @param day : 일
* @param hour : 시
* @param minute : 분
*/
void SubtractSystemTime(SYSTEMTIME& dest, SYSTEMTIME& src, int day, int hour, int minute);

/**
* @brief src에 일,시,분을 더한 값을 dest에 저장한다.
* @param dest : 대상 날짜 및 시간
* @param src : 소스 날짜 및 시간
* @param day : 일
* @param hour : 시
* @param minute : 분
*/
void AddSystemTime(SYSTEMTIME& dest, SYSTEMTIME& src, int day, int hour, int minute);

/**
* @brief 두개의 날짜를 비교하여 a가 크면 1을, 작으면 -1을 리턴한다.
*/
int CompareSystemTimeDate(SYSTEMTIME &a, SYSTEMTIME &b);

// compare daate & time, return 1 if a > b
/**
* @brief 두개의 날짜 및 시간을 비교하여 a가 크면 1을, 작으면 -1을 리턴한다.
*/
int CompareSystemTime(SYSTEMTIME &a, SYSTEMTIME &b);

/**
* @brief 두개의 날짜 및 시간의 차이를 구한다. (out = eng - begin)
* @param out : 결과 날짜
* @param begin : 시작 날짜
* @param end : 끝 날짜
*/
void TimeDifference(SYSTEMTIME& out, SYSTEMTIME& begin, SYSTEMTIME& end);

/**
* @brief 두개의 날짜 및 시간의 차이를 구하고 그 차이를 초 값으로 리턴한다.
* @param d : 끝 날짜
* @param s : 시작 날짜
*/
long TimeDifferenceSec(SYSTEMTIME& d, SYSTEMTIME& s);

/**
* @brief 1970년 1월 1일 0시부터의 밀리초값을 날짜 및 시간값으로 변환한다.
* @param st : 출력되는 날짜 및 시간
* @param time_ms : 밀리초값
*/
BOOL SystemTimeFromEPOCH_MS(SYSTEMTIME& st, __int64 time_ms);

__int64 SystemTimeToEPOCH(WORD yy, WORD mm, WORD dd, WORD h, WORD m, WORD s);

/**
* @brief 날짜 및 시간값을 1970년 1월 1일 0시부터의 밀리초값으로 변환한다.
* @param yy : 년
* @param mm : 월
* @param dd : 일
* @param h : 시
* @param m : 분
* @param s : 초
* @return __int64 : 밀리초값
*/
__int64 SystemTimeToEPOCH_MS(WORD yy, WORD mm, WORD dd, WORD h, WORD m, WORD s);

/**
* @brief 밀리초값을 날 수로 변환한다.
* @param ms : 밀리초값
* @return int : 날 수
*/
int DaysFromEPOCH_MS(__int64 ms);

/**
* @brief 날짜값이 유효하면 TRUE를 리턴한다.
*/
BOOL IsValidSystemTime(SYSTEMTIME &time);
/**
* @brief 날짜 및 시간값이 유효하면 TRUE를 리턴한다.
*/
BOOL IsValidSystemTimeHMS(SYSTEMTIME &time);

/**
* @brief begin - end 날짜수를 리턴한다.
*/
int GetEllapsedDays(SYSTEMTIME& end, SYSTEMTIME& begin);


int GetTommorrowSec();

/**
* @brief GetLastError()에 해당하는 시스템 오류 메시지를 버퍼에 복사한다.
* @param dwLastError : 시스템 오류 코드
* @param buff : 출력할 버퍼
* @param buff_size : 버퍼 크기
*/
BOOL GetLastErrorMessageString(DWORD dwLastError, LPTSTR buff, int buff_size);

/**
* @brief 대상 경로명 없거나 "\" 이면 TRUE를 리턴한다.
*/
BOOL IsEmptyPath(LPCTSTR path);

void AppendSeparateName(LPTSTR dest, int size, LPCTSTR first, TCHAR separator, LPCTSTR second);

void AppendSeparateCString(LPSTR dest, int size, LPCSTR first, char separator, LPCSTR second);

/**
* @brief 폴더명과 파일명을 합친 전체 경로명을 만들어 대상 버퍼에 복사한다.
* @param buff : 출력할 버퍼
* @param buff_size : 버퍼 크기
* @param folder : 폴더명
* @param file : 파일명
*/
void MakeFullPathName(LPTSTR buff, int buff_size, LPCTSTR folder, LPCTSTR file);
/**
* @brief 전체 경로명에서 기준 폴더를 제외한 상대 경로명을 만들어 버퍼에 복사한다.
* @param relative_buff : 출력할 버퍼
* @param buff_size : 버퍼 크기
* @param baseFolder : 기준 폴더명
* @param fullPathName : 전체 경로명
*/
void MakeRelativePathName(LPTSTR relative_buff, int buff_size, LPCTSTR baseFolder, LPCTSTR fullPathName);
/**
* @brief 폴더명과 파일명을 합친 전체 경로명을 만들어 새로 할당한 버퍼에 복사하여 리턴한다.
* @param folder : 폴더명
* @param file : 파일명
* @return LPTSTR : 새로 할당한 전체 경로명 버퍼
*/
LPTSTR AllocMakeFullPathName(LPCTSTR folder, LPCTSTR file);

/**
* @brief 전체 경로명을 짧은 문자열로 만든다.(ex, "c:\abc\...\testFolder")
* @param buff : 짧은 경로명을 저장할 버퍼
* @param length : 버퍼 크기
* @param originPathName : 원래의 전체 경로명
* @return BOOL : 변경되었으면 TRUE를 리턴한다.
*/
BOOL ShortenPathName(LPTSTR buff, int length, LPCTSTR originPathName);

/**
* @brief 지정한 경로 하위의 항목을 모두 삭제한다.
* @param path : 경로명
* @param withItself : TRUE이면 폴더 자체도 삭제한다.
*/
int EmptyDirectory(LPCTSTR path, BOOL withItself);
/**
* @brief 지정한 경로명의 폴더를 생성한다.
* @param pathname : 경로명
*/
BOOL CreateDirectory(LPCTSTR pathname);

int CompareFile(LPCTSTR orgname, LPCTSTR filename);

/**
* @brief 지정한 경로명의 폴더를 상위 폴더부터 생성한다.
* @param pathname : 경로명
*/
BOOL CreateDirectoryCascade(LPTSTR pathname);

BOOL IsFullPath(LPCTSTR pathname);

/* file : 0, folder : 1, none : -1 */
int IsFileOrFolder(LPCTSTR pathname);

/**
* @brief 지정한 경로명의 폴더가 존재하는지 확인한다.
* @param pathname : 경로명
* @return BOOL : 존재하면 TRUE를 리턴한다.
*/
BOOL IsDirectoryExist(LPCTSTR pathname);
/**
* @brief 지정한 경로명의 파일이 존재하는지 확인한다.
* @param pathname : 파일 경로명
* @return BOOL : 존재하면 TRUE를 리턴한다.
*/
BOOL IsFileExistAttribute(LPCTSTR pathname);

/**
* @brief 지정한 경로명의 파일이 존재하는지 확인한다.
* @param pathname : 파일 경로명
* @return int : 0 : not found, 1 : readonly, 2 : normal
*/
int IsFileExistReadOnly(LPCTSTR pathname);

/**
* @brief 지정한 경로명의 파일이 편집중인지 확인한다.
* @param name : 파일 경로명
* @return BOOL : 편집중인 파일이면 TRUE를 리턴한다.
*/
BOOL IsFileEditing(LPCTSTR name);

/**
* @brief 지정한 경로명의 파일이 쓰기 가능한지 확인한다.
* @param name : 파일 경로명
* @return BOOL : 쓰기 가능한 파일이면 TRUE를 리턴한다.
*/
BOOL IsFileWritable(LPCTSTR name);
/**
* @brief 지정한 경로명의 파일이 존재하는지 확인한다.
* @param filename : 파일 경로명
* @return BOOL : 쓰기 가능한 파일이면 TRUE를 리턴한다.
*/
BOOL FindFileExists(LPCTSTR filename);
/**
* @brief 지정한 경로명의 파일이 존재하는지 확인한다.
* 경로명이 긴 경우에는 "//?/" prefix를 붙여서 FindFileExists()를 호출한다
* @param filename : 파일 경로명
* @return BOOL : 쓰기 가능한 파일이면 TRUE를 리턴한다.
*/
BOOL IsFileExist(LPCTSTR filename);

/**
* @brief 지정한 경로명의 폴더가 유효하고 쓰기 가능한지 확인한다.
* @param pathname : 폴더 경로명
* @return BOOL : 유효하면 TRUE를 리턴한다.
*/
BOOL IsDirectoryWritable(LPCTSTR pathname);
/**
* @brief 지정한 경로명의 폴더가 시스템 폴더인지 확인한다.
* 시스템 폴더는 동기화 설정이 안되도록 하는데에 사용된다.
* @param pathname : 폴더 경로명
* @return BOOL : 유효하면 TRUE를 리턴한다.
*/
BOOL IsSystemDirectory(LPCTSTR pathname);

/**
* @brief 지정한 경로명의 폴더가 이동식 디스크가 아님을 확인한다.
* 이동식 디스크 폴더는 동기화 설정이 안되도록 하는데에 사용된다.(USB DISK도 고정식 디스크로)
* @param pathname : 폴더 경로명
* @return BOOL : 고정식 디스크이면 TRUE를 리턴한다.
*/
BOOL IsFixedDriveDisk(LPCTSTR pathname);
/**
* @brief 지정한 경로명의 폴더가 이동식 디스크가 아님을 확인한다.
* 이동식 디스크 폴더는 동기화 설정이 안되도록 하는데에 사용된다.(USB DISK도 이동식 디스크로)
* @param pathname : 폴더 경로명
* @return BOOL : 고정식 디스크이면 TRUE를 리턴한다.
*/
BOOL IsFixedDriveDirectory(LPCTSTR pathname);

BOOL IsDrivePath(LPCTSTR pathname);

/**
* @brief 지정한 경로명의 폴더나 파일을 숨김으로 설정한다.
* @param folder_name : 파일 경로명
* @return BOOL : 성공하면 TRUE를 리턴한다.
*/
BOOL SetHiddenDirectory(LPCTSTR folder_name);
/**
* @brief 전체 경로명에서 폴더 이름을 가져온다.
* @param folder_name : 폴더 이름을 저장할 버퍼
* @param full_path : 전체 경로명
* @return BOOL : 성공하면 TRUE를 리턴한다.
*/
BOOL GetFolderNameFromFullPath(LPTSTR folder_name, LPCTSTR full_path);
/**
* @brief child가 parent의 하위 폴더인지 확인한다.
* @param parent : 부모 폴더 경로명
* @param child : child 전체 경로명
* @param itself : 동일한 것도 포함한다.
* @return BOOL : 경로가 같거나 하위 폴더이면 TRUE를 리턴한다.
*/
BOOL IsChildFolderOrFile(LPCTSTR parent, LPCTSTR child, BOOL itself);

BOOL IsChildFolderOrFile(LPCTSTR parent, int parent_len, LPCTSTR child, int child_len, BOOL itself);

BOOL IsChildFolderOrFile(LPCSTR parent, LPCSTR child, BOOL itself);

/**
* @brief 디스크의 여유 공간을 확인한다. 지정한 크기 이상의 여유공간이 있으면 1을 리턴한다.
* @param fullname : 대상 경로명
* @param fileSize : 필요한 크기
* @return int : return 0 : no available space, -1 : error, 1 : success
*/
int CheckDiskFreeSpace(LPCTSTR fullname, __int64 fileSize);

/**
* @brief 지정한 파일에 대해 쓰기 권한이 있는지 확인한다.
* @param filename : child 전체 경로명
* @return BOOL : 대해 쓰기 권한이 있으면 TRUE를 리턴한다.
*/
BOOL FileAccessWrite(LPCTSTR filename);
/**
* @brief 지정한 경로명 하위 항목 중에서 읽기전용의 파일이 있으면 그 이름을 리턴한다.
* @param pathname : 대상 경로명
* @return LPTSTR : 읽기전용의 파일 이름들을 콤마로 분리한 스트링.
*/
LPTSTR GetUnaccessableItems(LPCTSTR pathname);

/**
* @brief 지정한 경로명의 파일의 크기를 리턴한다.
* FindFirstFile()로 대상 파일의 64비트 크기값을 받아 라틴한다.
* @param filename : 대상 경로명
* @return LPTSTR : 읽기전용의 파일 이름들을 콤마로 분리한 스트링.
*/
__int64 GetFileSize(LPCTSTR filename);

/**
* @brief 지정한 경로명의 하위 파일의 전체 크기를 리턴한다.
* @param pathname : 대상 경로명
* @return __int64 : 하위 파일의 전체 크기.
*/
__int64 GetFolderSize(LPCTSTR pathname);

/**
* @brief 파일 크기를 간략한 표시 문자열로 출력한다.
* @param buff : 출력할 버퍼
* @param length : 버퍼 크기
* @param size : 파일의 크기.
*/
void FormatFileSize(LPTSTR buff, int length, __int64 size);

/**
* @brief 더블 형식으로 문자열에서 크기값을 읽어온다.
* @param str : 입력 문자열
* @return __int64 : 크기값.
*/
__int64 FileSizeFromStringDouble(const char *str);
/**
* @brief 더블 형식으로 문자열에서 크기값을 읽어온다.
* @param str : 입력 문자열
* @return __int64 : 크기값.
*/
__int64 FileSizeFromStringDoubleT(const TCHAR *str);

/**
* @brief 정수 형식으로 문자열에서 크기값을 읽어온다.
* @param str : 입력 문자열
* @return __int64 : 크기값.
*/
__int64 FileSizeFromString(const char *str);
/**
* @brief 정수 형식으로 문자열에서 크기값을 읽어온다.
* @param str : 입력 문자열
* @return __int64 : 크기값.
*/
__int64 FileSizeFromStringT(const TCHAR *str);

/**
* @brief 탐색기에서 대상 경로의 아이콘이 업데이트되도록 업데이트 요청을 보낸다.
* @param pathname : 대상 경로명
* @param bRecursiveChild : TRUE이면 하위의 항목도 업데이트를 요청함.
*/
void ShellNotifyIconChange(LPCTSTR pathname, BOOL bRecursiveChild);
/**
* @brief 탐색기에서 대상 경로의 아이콘이 업데이트되도록 업데이트 요청을 보낸다.
* @param pathname : 대상 경로명
* @param event_id : 대상 경로명
* @param bRecursiveChild : TRUE이면 하위의 항목도 업데이트를 요청함.
*/
void ShellNotifyIconChangeEx(LPCTSTR pathname, int event_id, BOOL bRecursiveChild);
/**
* @brief 탐색기에서 ECM 서버측 대상 경로의 아이콘이 업데이트되도록 업데이트 요청을 보낸다.
* @param ecmFullPath : 대상 경로명
* 동작하지 않아 사용하지 않음.
*/
void ShellNotifyECMChange(LPCTSTR ecmFullPath);

/**
* @brief 대상 경로명을 첫번째 폴더명과 하위의 경로명으로 분리한다.
* @param i_pathname : 입력 대상 경로명
* @param o_name : 첫번째 폴더 이름
* @param o_sub : 경로
*/
void SplitNameSubPath(LPCTSTR i_pathname, LPTSTR o_name, LPTSTR o_sub);

/**
* @brief 대상 경로명을 상위 폴더명과 마지막 항목 이름으로 분리한다.
* @param i_pathname : 입력 대상 경로명
* @param o_path : 경로
* @param o_name : 마지막 이름
*/
void SplitPathName(LPCTSTR i_pathname, LPTSTR o_path, LPTSTR o_name);
/**
* @brief 대상 경로명을 상위 폴더명과 마지막 항목 이름으로 분리하고 새로 할당하여 리턴한다.
* @param i_pathname : 입력 대상 경로명
* @param o_path : 경로
* @param o_name : 마지막 이름
*/
void SplitPathNameAlloc(LPCTSTR i_pathname, LPTSTR* o_path, LPTSTR* o_name);

void SplitPathNameAlloc_D(LPCTSTR i_pathname, LPTSTR* o_path, LPTSTR* o_name);

LPCTSTR FindFileExtension(LPCTSTR pathName);

LPTSTR ChangeFileExtension(LPCTSTR pathName, LPCTSTR ext);

BOOL TrimFileExtension(LPTSTR filename);

/**
* @brief 두 경로명 중에서 상위 폴더 부부만 비교하여 p1 > p2이면 1을 리턴한다.
* @param p1 : 첫번째 경로명
* @param p2 : 두번째 경로명
* @return int : p1 > p2이면 1을, p1 < p2이면 -1을, 같으면 0을 리턴한다.
*/
int CompareParentFolderPath(LPTSTR p1, LPTSTR p2);

/**
* Make unique filename which does not exist
* ex) LPWSTR fullPathName[] = _T("C:\Temp\abc"); ReadyUniqueFilename(fullPathName, 256, _T(".pdf")
*/
BOOL ReadyUniqueFilename(LPTSTR fullPathName, int len, LPTSTR dotExt);

/**
* @brief 문자열에서 ' ' 보다 작은 문자들을 모두 ' '로 변경한다.
* @param str : 대상 문자열
*/
void ReplaceControlChars(LPTSTR str);

/**
* @brief 문자열 앞뒤의 white space 문자들을 모두 제거한다.
* @param str : 대상 문자열
* @return LPTSTR : 결과 문자열
*/
LPTSTR TrimStr(LPTSTR str);

/*
* CompareStringwithNullCheck
*/
int CompareStringNC(LPCTSTR s1, LPCTSTR s2);

/*
* CompareStringIgnoreCasewithNullCheck
*/
int CompareStringICNC(LPCTSTR s1, LPCTSTR s2);

/**
* @brief 대상 파일의 읽기전용 속성을 제거한다.
* @param setting_file : 대상 파일 경로명
*/
void MakeWritabeFile(LPCTSTR setting_file);
/**
* @brief 대상 파일을 삭제한다.
* @param fileName : 대상 파일 경로명
*/
BOOL KDeleteFile(LPCTSTR fileName);
/**
* @brief 대상 파일 또는 폴더를 휴지통으로 이동한다.
* @param filename : 대상 파일 경로명
* @param isFolder : TRUE이면 대상 경로명이 폴더임
*/
BOOL KShellMoveToTrash(LPCTSTR filename, BOOL isFolder);
/**
* @brief 대상 파일 또는 다른 폴더로 이동 또는 이름 변경을 한다.
* @param prevFileName : 대상 파일 경로명
* @param newFileName : 새 파일 경로명
*/
BOOL KMoveFile(LPCTSTR prevFileName, LPCTSTR newFileName);

/**
* @brief FILE_SHARE_READ 모드로 로그 파일 오픈한다.
*/
HANDLE OpenLogFileShared(LPCTSTR fileName, BOOL addBOM);

/**
* @brief 로그 파일 클로즈한다.
*/
void CloseLogFileShared(HANDLE h);


/**
* @brief 대상 파일을 지정한 모드로 오픈한다. 파일이 존재하면서 오픈을 실패하는
* 경우에는 잠시 딜레이를 두고 다시 시도한다.
* @param fileName : 대상 파일 경로명
* @param openMode : 파일 오픈 모드
* @return FILE* : 오픈된 파일 핸들
*/
FILE *OpenDelayedFile(LPCTSTR fileName, LPCTSTR openMode);

/**
* @brief 대상 파일을 임시 파일명으로 변경한다.
* @param filename : 대상 파일 경로명
* @param dotExt : '.'을 포함하는 확장자명
*/
BOOL Ready4SerialUniqueFilename(LPTSTR filename, LPCTSTR dotExt);

int LoadLocaleString(HINSTANCE hInst, int id, WORD lang, LPTSTR buff, int buff_size);

/**
* @brief 문자열에서 앞뒤의 white space 문자열을 제거하고 대상 버퍼에 복사한다.
* @param buff : 대상 문자열 버퍼
* @param buff_size : 버퍼의 크기
* @param src_string : 원본 문자열
*/
void CopyStringTrim(LPTSTR buff, int buff_size, LPCTSTR src_string);

/**
* @brief 문자열에서 앞뒤의 white space 문자열을 제거하고, 인용 부호로 둘러싸여 있으면 이것도 제거한다.
* @param buff : 원본 문자열
*/
void TrimQuoteString(LPTSTR buff);

/**
* @brief 대상 경로와 파일명에 대한 임시 파일명을 만든다.
* @param path : 경로명
* @param originalFilename : 원본 파일 이름
* @param Fullname : 결과 전체 경로명
*/
BOOL ReadyTempFileName(LPCTSTR path, LPCTSTR originalFilename, LPTSTR Fullname);

/**
* @brief 시스템의 OS 버전이 WindowXP인지 확인한다.
*/
BOOL IsWindowXP();

BOOL IsWindow10_1607_More();

/**
* @brief 경로명의 depth를 계산하여 리턴한다.
*/
int getPathDepth(LPCSTR IN path);

/**
* @brief p1, p2 두 상대 경로명을 비교한다.
*/
int CompareRelativePath(LPCTSTR p1, LPCTSTR p2);

/**
* @brief 대상 문자열에 파일명에 부합하지 않는 문자가 포함되어 있는지 확인한다.
* 문자열에 \,/,:,*,?,\,",<,>,| 등의 문자는 사용할 수 없다.
*/
BOOL IsValidFilenameString(LPCTSTR filename);

/**
* @brief 전체 경로명에서 파일의 확장자 부분을 리턴한다.
*/
LPCTSTR GetFileExtension(LPCTSTR filePathName);

/**
* @brief 컴퓨터 이름을 리턴한다.
*/
void GetComputerNameStr(LPTSTR name, int bufferLength);

/**
* @brief 컴퓨터 네트워크 트래픽 모니터링을 시작한다
*/
HANDLE ReadyNetworkTrafficLevel(OUT LPTSTR msg, IN int length);

/**
* @brief 컴퓨터 네트워크 트래픽 값을 리턴한다
*/
int GetNetworkTrafficLevel(HANDLE Query);

/**
* @brief 컴퓨터 네트워크 트래픽 모니터링을 종료한다
*/
void CloseNetworkTrafficLevel(HANDLE Query);

/**
* @brief 지정한 시간 구간동안 네트워크 트래픽을 모니터링하여 초당 전송 바이트 수를 리턴한다.
* @return int : 성공한 때에는  초당 전송 바이트 수(0보다 크거나 같음), 오류가 발생하면 아래의 오류값이 리턴된다.
* @n -1 : PdhOpenQuery 실패
* @n -2 : PdhAddCounter 실패
* @n -3 : PdhCollectQueryData 실패
* @n -4 : PdhCollectQueryData 실패
* @n -5 : PdhGetFormattedCounterValue 실패
*/
int MeasureNetworkTrafficLevel(int timeDelaySec);

/**
* @brief 시스템의 정의된 폴더 경로를 리턴한다.
*/
BOOL GetKnownFolderPath(REFKNOWNFOLDERID refid, LPTSTR* pPath);

/**
* @brief 시스템의 인터넷 연결 상태를 확인한다.
*/
BOOL IsInternetConnected();


#endif

