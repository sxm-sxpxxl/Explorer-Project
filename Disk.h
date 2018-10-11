//---------------------------------------------------------------------------

#ifndef DiskH
#define DiskH
//---------------------------------------------------------------------------

#include <list>
#include <System.Classes.hpp>
#include <vcl.h>

#include "BaseObject.h"

class Disk : public BaseObject {
public:
	Disk(AnsiString path, AnsiString name, AnsiString label, AnsiString fileSys,
	int freeSpace, int allSpace);
	static std::list<Disk> GetAllDisks();

	AnsiString getNameToShow();
private:
	static const int MAX_COUNT_DISKS = 10;
	AnsiString mLabel;
	AnsiString mFileSystem;
	int mFreeSpaceInGB;
	int mAllSpaceInGB;
};

#endif
