//---------------------------------------------------------------------------

#ifndef BaseObjectH
#define BaseObjectH
//---------------------------------------------------------------------------

#include <System.Classes.hpp>
#include <vcl.h>

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

const AnsiString TYPE_DISK = "Disk";
const AnsiString TYPE_FILE = "File";

class BaseObject {
protected:
	AnsiString mPath;
	AnsiString mName;
    AnsiString mType;
	bool isFolder;
public:
	BaseObject(AnsiString path, AnsiString name, AnsiString type, bool is);

    bool operator == (TTreeNode* node) {
		if(mPath == *(AnsiString*)node->Data) {
			return true;
		}
		return false;
	}

	AnsiString getName() { return mName; }
	AnsiString getPath() { return mPath; }
    AnsiString getType() { return mType; }
	bool getIsFolder() { return isFolder; }
private:
};
#endif
