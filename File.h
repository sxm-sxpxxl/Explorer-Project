//---------------------------------------------------------------------------

#ifndef FileH
#define FileH
//---------------------------------------------------------------------------

#include <list>
#include <System.Classes.hpp>
#include <vcl.h>

#include "BaseObject.h"

const DWORD ATTR_FOLDER = 16UL;
const DWORD ATTR_FILE = 32UL;
const DWORD ATTR_READONLY = 1UL;
const DWORD ATTR_HIDDEN = 2UL;
const DWORD ATTR_OFFSET = ATTR_READONLY + ATTR_HIDDEN;
const DWORD ATTR_MEDIA = 544UL;
const DWORD ATTR_MEDIA_READONLY = ATTR_MEDIA + ATTR_READONLY;

class File : public BaseObject {
public:
	File(AnsiString path, AnsiString name, bool isFolder, int size);
	static std::list<File> GetAllFiles(AnsiString path);
private:
    int size;
};
#endif
