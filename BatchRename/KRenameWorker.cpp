#include "pch.h"
#include "KRenameWorker.h"
#include "ThreadPool.h"
#include "Platform.h"

#include <strsafe.h>

KRenameWorker::KRenameWorker(HWND parent)
{
  mpCaller = parent;
}

KRenameWorker::~KRenameWorker()
{

}

BOOL KRenameWorker::Rename(std::wstring& path, std::wstring& orgFilename, std::wstring& newFilename)
{
  TCHAR srce[260];
  TCHAR dest[260];

  MakeFullPathName(srce, 260, path.c_str(), orgFilename.c_str());
  MakeFullPathName(dest, 260, path.c_str(), newFilename.c_str());
  
  return KMoveFile(srce, dest);
}

BOOL KRenameWorker::RenameFindReplace(std::wstring& path, std::wstring& orgFilename)
{
  BOOL r = FALSE;
  size_t p = orgFilename.find(mFindStr);
  if (p != std::string::npos)
  {
    std::wstring newFilename;
    if (p > 0)
      newFilename = orgFilename.substr(0, p);

    if (mReplaceStr.length() > 0)
      newFilename += mReplaceStr;

    size_t end = p + mFindStr.length();
    if (orgFilename.length() > end)
      newFilename += orgFilename.substr(end, orgFilename.length() - end);

    r = Rename(path, orgFilename, newFilename);

    if (r)
      mResultCount++;

    TCHAR msg[260];
    StringCchPrintf(msg, 260, _T("%s => %s : "), orgFilename.c_str(), newFilename.c_str());
    StringCchCat(msg, 260, (r ? _T("Ok") : _T("Fail")));
    StringCchCat(msg, 260, _T("\r\n"));

    if (mpCaller != NULL)
      SendMessage(mpCaller, WM_RENAME_PROCESS, 0, (LPARAM)msg);

    OutputDebugString(msg);
  }
  mFileCount++;
  return r;
}

void KRenameWorker::DoFindReplace(std::wstring& targetFolder)
{
  std::vector<std::wstring> subFolders;

  TCHAR findMask[260];
  MakeFullPathName(findMask, 260, targetFolder.c_str(), _T("*"));

  WIN32_FIND_DATA fd;
  memset(&fd, 0, sizeof(WIN32_FIND_DATA));
  fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;

  HANDLE h = FindFirstFile(findMask, &fd);
  if (h != INVALID_HANDLE_VALUE)
  {
    do {
      if (lstrlen(fd.cFileName) <= 0)
        continue;
      if ((StrCmp(fd.cFileName, L".") == 0) ||
        (StrCmp(fd.cFileName, L"..") == 0))
        continue;

      int isFolder = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;

      if (isFolder)
      {
        if (mRecursively)
        {
          TCHAR path[260];

          MakeFullPathName(path, 260, targetFolder.c_str(), fd.cFileName);
          subFolders.push_back(std::wstring(path));
        }
      }
      else
      {
        /*
        TCHAR path[260];
        MakeFullPathName(path, 260, targetFolder.c_str(), fd.cFileName);
        std::wstring wpath(path);
        RenameFindReplace(wpath);
        */
        RenameFindReplace(targetFolder, std::wstring(fd.cFileName));
      }

      /*
      if (IsExcludedFileName(fd.cFileName, isFolder, syncPolicy))
        continue;
      */
    } while (FindNextFile(h, &fd));
    FindClose(h);
  }

  for (std::wstring sub : subFolders)
  {
    TCHAR path[260];
    MakeFullPathName(path, 260, targetFolder.c_str(), sub.c_str());
    DoFindReplace(std::wstring(path));
  }
}

void KRenameWorker::DoWorkerThread()
{
  DoFindReplace(mTargetFolder);

  TCHAR msg[260];
  StringCchPrintf(msg, 260, _T("Total files=%d, replaced=%d\r\n"), mFileCount, mResultCount);
  OutputDebugString(msg);

  if (mpCaller != NULL)
    SendMessage(mpCaller, WM_RENAME_PROCESS, 1, (LPARAM)msg);
}

void KRenameWorker::Start(LPCTSTR target, LPCTSTR find, LPCTSTR replace, BOOL recurr)
{
  mTargetFolder.assign(target);
  mFindStr.assign(find);
  mReplaceStr.assign(replace);
  mRecursively = recurr;

  mFileCount = 0;
  mResultCount = 0;

  CThreadPool::QueueUserWorkItem(&KRenameWorker::DoWorkerThread, this);
}
