//---------------------------------------------------------------------------

#pragma hdrstop

#include "File.h"
#include <cstring>
//---------------------------------------------------------------------------
#pragma package(smart_init)

File::File(AnsiString path, AnsiString name, bool isFolder, int size) : BaseObject(path, name, TYPE_FILE, isFolder) {
    this->size = size;
}

// --------------------------------------------------------------------------
std::list<File> File::GetAllFiles(AnsiString path) {
	std::list<File> local_fileArray;

	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	char DirSpec[MAX_PATH];
	DWORD dwError;

	std::strncpy(DirSpec, path.c_str(), std::strlen(path.c_str()) + 1);
	std::strncat(DirSpec, "*", 1);
	AnsiString dir = AnsiString(DirSpec);
	dir = dir.Delete(dir.Length(), 1);
	hFind = FindFirstFileA(DirSpec, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		ExitProcess(GetLastError());
	}
	else
	{
		AnsiString path = AnsiString(DirSpec) + AnsiString(FindFileData.cFileName);
		while (FindNextFileA(hFind, &FindFileData) != 0)
		{
			AnsiString name = AnsiString(FindFileData.cFileName);
			AnsiString attrib = AnsiString(FindFileData.dwFileAttributes);

			if(name != "..") {
				AnsiString path = dir + name + "\\";
				int size = FindFileData.nFileSizeLow / 1024; // Размер файла в килобайтах
				DWORD attrib = FindFileData.dwFileAttributes;

				bool isFolder = false;
				if( (attrib >= ATTR_FOLDER && attrib <= ATTR_FOLDER + ATTR_OFFSET) ||
					(attrib >= ATTR_FILE && attrib <= ATTR_FILE + ATTR_OFFSET) ||
					(attrib == ATTR_HIDDEN) ||
					(attrib == ATTR_MEDIA) ||
					(attrib == ATTR_MEDIA_READONLY) )
				{
					if(attrib >= ATTR_FOLDER && attrib <= ATTR_FOLDER + ATTR_OFFSET) {
                        isFolder = true;
					}
					File file(path, name, isFolder, size);
					local_fileArray.push_back(file);
				}
			}
		}

		dwError = GetLastError();
		FindClose(hFind);
		if (dwError != ERROR_NO_MORE_FILES)
		{
			ExitProcess(GetLastError());
		}
	}

	return local_fileArray;
}
// --------------------------------------------------------------------------
