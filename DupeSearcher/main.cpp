// DupeSearcher.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FileSysUtils.h"
#include "DupeSearcher.h"
#include "TimeUtils.h"

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace std;

	// Fix incorrect cyrillic output in console
	setlocale( LC_ALL , "russian_russia.866" );

	std::wstring dirPath;

	if (argc == 1) {
		wcout << "Note: no cmd args provided, checking current directory." << endl;
		dirPath = FileSysUtils::GetDirFromFullpath(argv[0]);
		if (dirPath.empty()) {
			wcerr << "Failed to process path " << argv[0] << endl;
			return EXIT_FAILURE;
		}
	}
	else if (argc == 2) {
		dirPath = argv[1];
		if (!FileSysUtils::IsDirectoryExist(dirPath)) {
			wcerr << "Requested directory '" << dirPath << "' not exist!" << endl;
			return EXIT_SUCCESS;
		}
	}
	else {
		std::wstring filename = FileSysUtils::GetFilenameFromFullpath(argv[0]);
		if (filename.empty()) {
			wcerr << "Failed to process path " << argv[0] << endl;
			return EXIT_FAILURE;
		}
		wcout << "Usage:" << endl << filename << " <search_dir_path>" << endl << endl;
		return EXIT_SUCCESS;
	}

	wcout << "Searching in '" << dirPath << "'... ";

	DupeSearcher ds;
	
	chrono::milliseconds durationMs = TimeUtils::Measure<>::TimedCall([&]() {
		ds.SearchDupes(dirPath);
	});

	wcout << endl;
	if (durationMs.count() > 0)
		wcout << L"Done in " << TimeUtils::ToHumanString(durationMs);
	else
		wcout << L"Done.";

	wcout << endl;

	ds.PrintResults(wcout);
	wcout << endl;

#ifdef DEBUG
	system("pause");
#endif

	return EXIT_SUCCESS;
}
