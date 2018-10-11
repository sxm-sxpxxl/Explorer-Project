// ---------------------------------------------------------------------------

#pragma hdrstop

#include "Disk.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)

Disk::Disk(AnsiString path, AnsiString name, AnsiString label,
	AnsiString fileSys, int freeSpace, int allSpace)
	: BaseObject(path, name, TYPE_DISK, true) {
	mLabel = label;
	mFileSystem = fileSys;
	mFreeSpaceInGB = freeSpace;
	mAllSpaceInGB = allSpace;
}

AnsiString Disk::getNameToShow() {
	AnsiString resultName;
	if (mLabel == "") {
		resultName = "Local Disk (" + mName + ")";
	}
	else {
		resultName = mLabel + " (" + mName + ")";
	}
	return resultName;
}

std::list<Disk>Disk::GetAllDisks() {
	std::list<Disk>diskArray;
	DWORD bitMask = GetLogicalDrives(); // функция возвращает битовую маску

	for (int i = 0; i < Disk::MAX_COUNT_DISKS;
	i++) // проходимся циклом по битам
	{
		int valueCurrentBit = ((bitMask >> i) & 1);
		// узнаём значение текущего бита
		if (valueCurrentBit) // если единица - диск с номером x есть
		{
			bool flag;
			AnsiString nameDisk = AnsiString((char)(65 + i)) + ":";
			// получаем литеру диска
			AnsiString pathDisk = AnsiString((char)(65 + i)) + ":\\";

			bool ready = DirectoryExists(pathDisk);
			// пытаемcя открыть корневую директорию
			if (ready) {
				unsigned __int64 FreeBytesAvailable;
				unsigned __int64 TotalNumberOfBytes;
				unsigned __int64 TotalNumberOfFreeBytes;
				char labelDisk[30];
				char fileSystemDisk[30];
				DWORD nameSizeDisk = sizeof(labelDisk);

				// получаем данные о размерах
				flag = ::GetDiskFreeSpaceExA(pathDisk.c_str(),
					(PULARGE_INTEGER) & FreeBytesAvailable,
					(PULARGE_INTEGER) & TotalNumberOfBytes,
					(PULARGE_INTEGER) & TotalNumberOfFreeBytes);
				if (!flag) {
					ShowMessage("Ошибка в GetDiskFreeSpaceEx\n");
				}

				// получаем метку, серинийный номер и пр.
				flag = GetVolumeInformationA(pathDisk.c_str(), labelDisk,
					sizeof(labelDisk), NULL, &nameSizeDisk, NULL,
					fileSystemDisk, sizeof(fileSystemDisk));

				if (!flag) {
					ShowMessage("Ошибка в GetVolumeInformation\n");
				}

				Disk disk(pathDisk, nameDisk, AnsiString(labelDisk),
					AnsiString(fileSystemDisk),
					(int)(TotalNumberOfFreeBytes / 1073741824),
					(int)(TotalNumberOfBytes / 1073741824));

				diskArray.push_back(disk);
			}

		}
	}
	return diskArray;
}
