#pragma once

namespace FileSysUtils {
;
// Recursively call handler for all files in path
//
using FileHandler = std::function <void(const std::wstring&, uint64_t)>;
void DirTravel(const std::wstring& path, FileHandler handler);

bool IsDirectoryExist(const std::wstring& path);

std::wstring GetDirFromFullpath(const std::wstring& fullPath);
std::wstring GetDirFromFullpath(const wchar_t* fullPath);
std::wstring GetFilenameFromFullpath(const std::wstring& fullPath);
std::wstring GetFilenameFromFullpath(const wchar_t* fullPath);

} // namespace FileSysUtils
