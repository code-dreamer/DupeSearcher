#include "stdafx.h"
#include "FileMapper.h"

FileMapper::FileMapper()
{}

FileMapper::~FileMapper()
{
	UnmapCurrent();
	BOOL ok = CloseHandle(mFile);
	assert(ok == TRUE);
}

bool FileMapper::OpenFile(const std::wstring& path)
{
	assert(!path.empty());

	CloseFile();

	mFile = CreateFile(
		path.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);
	assert(mFile != INVALID_HANDLE_VALUE);

	return (mFile != NULL);
}

unsigned char* FileMapper::MapChunk(uint64_t offset, uint64_t readCount)
{
	assert(mFile != NULL);
	assert(readCount > 0);

	UnmapCurrent();

	uint64_t maximumSize = offset + readCount;
	mFileMapping = CreateFileMapping(
		mFile,
		NULL,
		PAGE_READONLY,
		HIDWORD(maximumSize), LODWORD(maximumSize),
		NULL);
	if (mFileMapping == NULL) {
		assert(false);
		return nullptr;
	}

	assert(readCount <= std::numeric_limits<size_t>::max());
	mFileMappingPtr = MapViewOfFile(mFileMapping, FILE_MAP_READ, HIDWORD(offset), LODWORD(offset), (size_t)readCount);
	if (mFileMappingPtr == nullptr) {
		assert(false);
		BOOL ok = CloseHandle(mFileMapping);
		assert(ok == TRUE);
		return nullptr;
	}
	
	return reinterpret_cast<unsigned char*>(mFileMappingPtr);
}

void FileMapper::UnmapCurrent()
{
	if (mFileMappingPtr != NULL) {
		BOOL ok = UnmapViewOfFile(mFileMappingPtr);
		assert(ok == TRUE);
		mFileMappingPtr = NULL;
	}

	if (mFileMapping != NULL) {
		BOOL ok = CloseHandle(mFileMapping);
		assert(ok == TRUE);
		mFileMapping = NULL;
	}
}

void FileMapper::CloseFile()
{
	if (mFile != NULL) {
		UnmapCurrent();

		BOOL ok = CloseHandle(mFile);
		assert(ok == TRUE);
	}
}
