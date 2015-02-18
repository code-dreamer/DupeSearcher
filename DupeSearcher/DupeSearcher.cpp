#include "stdafx.h"
#include "DupeSearcher.h"
#include "FileSysUtils.h"
#include "FileMapper.h"

const size_t HashSize = 16;

struct FileInfo
{
	FileInfo(std::wstring path, uint64_t size)
		: path(path)
		, size(size)
	{}

	FileInfo(const FileInfo& other)
	{
		if (&other != this) {
			path = other.path;
			size = other.size;
		}
	}
	
	FileInfo(FileInfo&& other)
		: path(std::move(other.path))
		, size(other.size)
	{}

	const FileInfo& operator=(FileInfo& other) 
	{
		if (&other != this) {
			path = other.path;
			size = other.size;
		}
	}
	const FileInfo& operator=(FileInfo&& other)
	{
		path = std::move(other.path);
		size = other.size;

		return *this;
	}

	std::wstring path;
	uint64_t size;
};

namespace {
;
void GetHash(unsigned char* from, unsigned char* to, uint64_t readCount)
{
	assert(from);
	assert(to);
	assert(readCount != 0);

	md5_state_t state;
	md5_init(&state);
	assert(readCount <= std::numeric_limits<size_t>::max());
	md5_append(&state, from, (int)readCount);
	md5_finish(&state, to);
}
} // namespace

DupeSearcher::DupeSearcher()
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	const uint64_t chunkSize = 32 * 1024 * 1024;
	if (sysInfo.dwAllocationGranularity > chunkSize)
		mChunkSize = sysInfo.dwAllocationGranularity;
	else
		mChunkSize = sysInfo.dwAllocationGranularity * (chunkSize / sysInfo.dwAllocationGranularity);
}

DupeSearcher::~DupeSearcher()
{}

void DupeSearcher::SearchDupes(const std::wstring& dirPath)
{
	assert(FileSysUtils::IsDirectoryExist(dirPath));

	// Zero files are the same.
	FileGroup zeroFilesGroup;

	FileSysUtils::DirTravel(dirPath, [this, &zeroFilesGroup](const std::wstring& filePath, uint64_t fileSize) {
		assert(!filePath.empty());

		if (fileSize == 0) {
			zeroFilesGroup.push_back(FileInfo{filePath, fileSize});
			return;
		}

		// search group with this size
		auto groupIt = std::find_if(mFileGroups.begin(), mFileGroups.end(), [fileSize](const FileGroup& group) {
			return (!group.empty() && group.front().size == fileSize);
		});

		if (groupIt == mFileGroups.end()) {
			mFileGroups.push_back(FileGroup{});
			groupIt = --mFileGroups.end();
		}

		groupIt->push_back(FileInfo{filePath, fileSize});
	});

	// delete dummy groups
	auto groupIt = mFileGroups.begin();
	while (groupIt != mFileGroups.end()) {
		if (IsDummyGroup(*groupIt)) {
			groupIt = mFileGroups.erase(groupIt);
			continue;
		}

		++groupIt;
	}

	if (!mFileGroups.empty()) {
		SplitGroup(mFileGroups.begin());
	}

	if (zeroFilesGroup.size() > 1)
		mFileGroups.push_back(zeroFilesGroup);
}

void DupeSearcher::SplitGroup(FileGroups::iterator groupIt)
{
	assert(groupIt != mFileGroups.end());
	FileGroup& group = *groupIt;
	assert(group.size() > 1);

	FileGroup newGroup;

	// If this group consist of two files, no need to calculate hash
	const bool comlexGroup = group.size() > 2;

	auto fileIt = group.begin();
	FileInfo& firstFile = *fileIt;
	uint64_t fileSize = firstFile.size;
	uint64_t offset = 0;
	uint64_t toRead = firstFile.size < mChunkSize ? (size_t)fileSize : (size_t)mChunkSize;
	unsigned char firstFileHash[HashSize];
	unsigned char secondFileHash[HashSize];

	FileMapper firstFileMapper;
	if (!firstFileMapper.OpenFile(firstFile.path)) {
		std::wcerr << "Failed to open " << firstFile.path << std::endl;
		if (++groupIt != mFileGroups.end()) {
			SplitGroup(groupIt);
		}
		return;
	}
	FileMapper secondFileMapper;
	for (;;) {
		assert(toRead <= std::numeric_limits<size_t>::max());
		size_t cmpCount = (size_t)toRead;

		unsigned char* firstFilePtr = firstFileMapper.MapChunk(offset, toRead);
		if (!firstFilePtr) {
			std::wcerr << "Failed to map " << firstFile.path << std::endl;
			if (++groupIt != mFileGroups.end()) {
				SplitGroup(groupIt);
			}
			return;
		}
		if (comlexGroup) {
			GetHash(firstFilePtr, firstFileHash, toRead);
			firstFilePtr = firstFileHash;
			cmpCount = HashSize;
		}

		++fileIt;
		while (fileIt != group.end()) {
			secondFileMapper.OpenFile(fileIt->path);
			unsigned char* secondFilePtr = secondFileMapper.MapChunk(offset, toRead);
			if (!secondFilePtr) {
				std::wcerr << "Failed to map " << fileIt->path << std::endl;
				continue;;
			}

			if (comlexGroup) {
				GetHash(secondFilePtr, secondFileHash, toRead);
				secondFilePtr = secondFileHash;
			}

			if (std::memcmp(firstFilePtr, secondFilePtr, cmpCount) != 0) {
				newGroup.push_back(*fileIt);
				fileIt = group.erase(fileIt);
				continue;
			}

			++fileIt;
		}

		if (IsDummyGroup(group))
			break;

		offset += toRead;
		if (offset == fileSize) // eof
			break;

		toRead = std::min(fileSize - offset, mChunkSize);
		assert(toRead > 0);

		fileIt = group.begin();
	}

	if (!IsDummyGroup(newGroup))
		mFileGroups.push_back(newGroup);

	if (IsDummyGroup(group))
		groupIt = mFileGroups.erase(groupIt);
	else
		++groupIt;

	if (groupIt != mFileGroups.end())
		SplitGroup(groupIt);
}

bool DupeSearcher::IsDummyGroup(const FileGroup& group)
{
	return group.size() <= 1;
}

void DupeSearcher::PrintResults(std::wostream& out)
{
	using namespace std;

	if (mFileGroups.empty()) {
		out << L"No duplicates found" << endl;
		return;
	}

	out << L"Duplicates were found:" << endl << endl;

	for (const FileGroup& group : mFileGroups) {
		for (const FileInfo& fileInfo : group) {
			out << fileInfo.path << endl;
		}
		out << L"----------------------------------" << endl;
	}
}
