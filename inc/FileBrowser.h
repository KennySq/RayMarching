#pragma once
#include<fstream>
#include<string>

class FileBrowser
{
public:
	FileBrowser();
	~FileBrowser();

	void DisplayAssets();

	std::fstream& GetFile() { return mSelectedFileStream; }
	const std::wstring& GetFilePath() { return mSelectedFile; }
	const std::wstring& GetFileName() { return mFileName; }
private:
	void setDirectory(const std::wstring& directory) { mCurrentDirectory = directory; }
	void moveDirectory(const std::wstring& directory) { mCurrentDirectory = mCurrentDirectory + L'\\' + directory; }
	void moveUpperDirectory() { mCurrentDirectory = mCurrentDirectory.substr(0, mCurrentDirectory.find_last_of('\\')); }

	std::wstring mCurrentDirectory;
	std::wstring mSelectedFile;
	std::wstring mFileName;

	void Start();
	void Update();
	void Destroy();

	size_t mDirectoryDepth = 0;

	std::fstream mSelectedFileStream;
};
