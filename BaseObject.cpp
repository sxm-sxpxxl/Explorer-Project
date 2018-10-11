//---------------------------------------------------------------------------

#pragma hdrstop

#include "BaseObject.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

BaseObject::BaseObject(AnsiString path, AnsiString name, AnsiString type, bool is) {
	mPath = path;
	mName = name;
    mType = type;
	isFolder = is;
}
//---------------------------------------------------------------------------
