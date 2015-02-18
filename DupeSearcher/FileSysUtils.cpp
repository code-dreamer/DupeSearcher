#include "stdafx.h"
#include "FileSysUtils.h"

namespace FileSysUtils {
;
void DoDirTravel(LPCWSTR lpFolder, FileHandler handler)
{
	wchar_t szFullPattern[MAX_PATH];
	WIN32_FIND_DATA findFileData;

	PathCombine(szFullPattern, lpFolder, L"*");
	HANDLE hFindFile = FindFirstFile(szFullPattern, &findFileData);

	if (hFindFile == INVALID_HANDLE_VALUE) {
		return;
	}

	do {
		if (wcscmp(findFileData.cFileName, L".") == 0 || wcscmp(findFileData.cFileName, L"..") == 0) {
			continue;
		}

		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// found a subdirectory, recurse into it
			PathCombine(szFullPattern, lpFolder, findFileData.cFileName);
			DoDirTravel(szFullPattern, handler);
		}
		else {
			ULARGE_INTEGER fileSize;
			fileSize.HighPart = findFileData.nFileSizeHigh;
			fileSize.LowPart = findFileData.nFileSizeLow;
			
			PathCombine(szFullPattern, lpFolder, findFileData.cFileName);
			handler(szFullPattern, fileSize.QuadPart);
		}
	} while (FindNextFile(hFindFile, &findFileData));

	FindClose(hFindFile);
}

void DirTravel(const std::wstring& path, FileHandler handler)
{
	assert(IsDirectoryExist(path));
	assert(handler != nullptr);

	DoDirTravel(path.c_str(), handler);
}

bool IsDirectoryExist(const std::wstring& path)
{
	if (path.empty())
		return false;

	return (PathIsDirectory(path.c_str()) == FILE_ATTRIBUTE_DIRECTORY );
}

std::wstring GetDirFromFullpath(const std::wstring& fullPath)
{
	assert(!fullPath.empty());
	
	return GetDirFromFullpath(fullPath.c_str());
}

std::wstring GetDirFromFullpath(const wchar_t* fullPath)
{
	assert(fullPath && wcslen(fullPath) > 0);

	std::wstring result;

	wchar_t path[MAX_PATH];
	wcscpy(path, fullPath);
	BOOL ok = PathRemoveFileSpec(path);
	assert(ok == TRUE);
	if (ok == TRUE) {
		result = path;
	}
	else {
		assert(false);
	}
	
	return result;
}

std::wstring GetFilenameFromFullpath(const std::wstring& fullPath)
{
	assert(!fullPath.empty());

	return GetFilenameFromFullpath(fullPath.c_str());
}

std::wstring GetFilenameFromFullpath(const wchar_t* fullPath)
{
	std::wstring result;

	PWSTR path = PathFindFileName(fullPath);
	assert(path != fullPath);
	if (path != fullPath) {
		result = path;
	}
	else {
		assert(false);
	}

	return path;
}

} // namespace FileSysUtils
