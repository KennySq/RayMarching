#include"pch.h"
#include"FileBrowser.h"

using namespace std::filesystem;

FileBrowser::FileBrowser()
{
	wchar_t buffer[256];
	GetModuleFileName(nullptr, buffer, 256);

	std::wstring directory = buffer;

	directory = directory.substr(0, directory.find_last_of('\\') + 1);
	mCurrentDirectory = directory;
}

FileBrowser::~FileBrowser()
{
}

void FileBrowser::DisplayAssets()
{
	USES_CONVERSION;
	
	ImGui::Begin("Asset Browser");

	directory_iterator endItr;
	path directory(mCurrentDirectory);

	bool bExists = exists(directory);
	assert(bExists == true);
	std::wstring currentDirectory = mCurrentDirectory.substr(mCurrentDirectory.find_last_of('\\') + 1);
	ImGui::Text("%s", W2A(currentDirectory.c_str()));
	ImGui::SameLine();
	if (ImGui::ArrowButton("Upper", ImGuiDir_::ImGuiDir_Up) && mDirectoryDepth > 0)
	{
		mDirectoryDepth--;
		moveUpperDirectory();
	}
	ImGui::Separator();
	// cannot iterate, idk why tho.
	for (directory_iterator itr(directory); itr != endItr; itr++)
	{
		std::string itrDirectory = itr->path().string();
		itrDirectory = itrDirectory.substr(itrDirectory.find_last_of('\\') + 1);

		if (itr->is_directory() == true)
		{
			ImGui::Selectable(itrDirectory.c_str());
			ImVec2 itemRectMin = ImGui::GetItemRectMin();
			ImVec2 itemRectMax = ImGui::GetItemRectMax();

			if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsMouseHoveringRect(itemRectMin, itemRectMax, false))
			{
				std::wstring wPath;
				wPath.assign(itrDirectory.begin(), itrDirectory.end());
				moveDirectory(wPath);
				mDirectoryDepth++;
			}
		}

		if (itr->is_regular_file() == true)
		{
			ImGui::Selectable(itrDirectory.c_str());
			ImVec2 itemRectMin = ImGui::GetItemRectMin();
			ImVec2 itemRectMax = ImGui::GetItemRectMax();

			if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsMouseHoveringRect(itemRectMin, itemRectMax, false))
			{
				std::wcout << L"Selected File : " << itr->path().c_str() << '\n';

				if (mSelectedFileStream.is_open() == true)
				{
					mSelectedFileStream.close();
				}
				
				std::wstring pathStr = itr->path().wstring();
				mSelectedFile = pathStr;
				mFileName = mSelectedFile.substr(mSelectedFile.find_last_of('\\') + 1);
				mSelectedFileStream.open(pathStr);
			}
		}
	}

	ImGui::End();
}

void FileBrowser::Start()
{
}

void FileBrowser::Update()
{
	DisplayAssets();
}

void FileBrowser::Destroy()
{
}
