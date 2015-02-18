#pragma once

struct FileInfo;

class DupeSearcher
{
	NO_COPY_ASSIGN(DupeSearcher);

private:
	using FileGroup = std::list<FileInfo>;
	using FileGroups = std::list<FileGroup>;

public:
	DupeSearcher();
	~DupeSearcher();

	void SearchDupes(const std::wstring& dirPath);
	void PrintResults(std::wostream& out);

private:
	void DupeSearcher::SplitGroup(FileGroups::iterator it);
	static bool IsDummyGroup(const FileGroup& group);
	
private:
	FileGroups mFileGroups;
	uint64_t mChunkSize{};
};
