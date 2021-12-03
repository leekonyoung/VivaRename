#pragma once

#include <vector>
#include <string>

#define WM_RENAME_PROCESS (WM_USER + 120)

class KRenameWorker
{
public:
  KRenameWorker(HWND parent);
  ~KRenameWorker();

  HWND mpCaller;
  std::wstring mTargetFolder;
  std::wstring mFindStr;
  std::wstring mReplaceStr;
  BOOL mRecursively;

  int mFileCount;
  int mResultCount;

  BOOL Rename(std::wstring& path, std::wstring& orgFilename, std::wstring& newFilename);

  BOOL RenameFindReplace(std::wstring& path, std::wstring& name);

  void DoFindReplace(std::wstring& targetFolder);

  void DoWorkerThread();

  void Start(LPCTSTR target, LPCTSTR find, LPCTSTR replace, BOOL recurr);



};

