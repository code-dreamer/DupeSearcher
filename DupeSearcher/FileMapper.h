#pragma once

class FileMapper
{
	NO_COPY_ASSIGN(FileMapper);

public:
	FileMapper();
	~FileMapper();

	bool OpenFile(const std::wstring& path);
	unsigned char* MapChunk(uint64_t offset, uint64_t readCount);

private:
	void CloseFile();
	void UnmapCurrent();

private:
	HANDLE mFile{};
	HANDLE mFileMapping{};
	void* mFileMappingPtr{};
};
