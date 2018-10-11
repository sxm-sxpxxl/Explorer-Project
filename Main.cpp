//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------

DWORD WINAPI TMainForm::ThreadProc(LPVOID lpParam) {
	MyVCL* pData = (MyVCL*)lpParam;
	TTreeView* treeView = pData->treeView;
	TListView* listView = pData->listView;
	TEdit* edit = pData->edit;

    while (true)
	{
		if(treeView->Selected->Text == "My Computer") {
			continue;
		}

		LPCSTR lpDir = (*(AnsiString*)(treeView->Selected->Data)).c_str();

		DWORD dwWaitStatus;
		HANDLE dwChangeHandles[2];
		char lpDrive[4];
		char lpFile[_MAX_FNAME];
		char lpExt[_MAX_EXT];

		_splitpath(lpDir, lpDrive, NULL, lpFile, lpExt);

		lpDrive[2] = (TCHAR)'\\';
		lpDrive[3] = (TCHAR)'\0';

		// Watch the directory for file creation and deletion.

		dwChangeHandles[0] = FindFirstChangeNotificationA(
				lpDir,                         // directory to watch
				FALSE,                         // do not watch subtree
				FILE_NOTIFY_CHANGE_FILE_NAME); // watch file name changes

		if (dwChangeHandles[0] == INVALID_HANDLE_VALUE)
		{
			continue;
		}

		// Watch the subtree for directory creation and deletion.

		dwChangeHandles[1] = FindFirstChangeNotificationA(
				lpDrive,                       // directory to watch
				TRUE,                          // watch the subtree
				FILE_NOTIFY_CHANGE_DIR_NAME);  // watch dir name changes

		if (dwChangeHandles[1] == INVALID_HANDLE_VALUE)
		{
			ExitProcess(GetLastError());
		}


		// Make a final validation check on our handles.

		if ((dwChangeHandles[0] == NULL) || (dwChangeHandles[1] == NULL))
		{
			ExitProcess(GetLastError());
		}

		// Change notification is set. Now wait on both notification
		// handles and refresh accordingly.

		dwWaitStatus = WaitForMultipleObjects(2, dwChangeHandles,
											  FALSE, 2000);

        switch (dwWaitStatus)
		{
			case WAIT_OBJECT_0: {

                // A file was created, renamed, or deleted in the directory.
				// Refresh this directory and restart the notification.

				TMainForm::updateUI(treeView, listView, treeView->Selected, edit);
				if ( FindNextChangeNotification(dwChangeHandles[0]) == FALSE )
				{
					ExitProcess(GetLastError());
				}
				break;
			}

			case WAIT_OBJECT_0 + 1: {

				// A directory was created, renamed, or deleted.
				// Refresh the tree and restart the notification.

				TMainForm::updateUI(treeView, listView, treeView->Selected, edit);
                if (FindNextChangeNotification(dwChangeHandles[1]) == FALSE )
				{
					ExitProcess(GetLastError());
				}
                break;
			}

			case WAIT_TIMEOUT:
				break;

			default:
				ExitProcess(GetLastError());
		}
	}
}
//---------------------------------------------------------------------------

__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
	// Base Initialize of variables
	countClickOnListViewItem = 0;
    lastItem = NULL;
	// ------------------------------------------------

	AnsiString nameFile = ChangeFileExt(Application->ExeName, ".INI");
	TIniFile* settingsFile = new TIniFile(nameFile);

	Top = settingsFile->ReadInteger(nameFile, "Top", Top);
	Left = settingsFile->ReadInteger(nameFile, "Left", Left);
	delete settingsFile;

	// ShellExecute - функция для открытия документа в какой-либо программе
}
//---------------------------------------------------------------------------

void TMainForm::createNodeInTreeView(TTreeView* treeView, BaseObject & viewItem, TTreeNode* parentNode) {

	// ---------------------Create TreeView Node-------------------------
	if(viewItem.getIsFolder()) {
        TTreeNode* pChildNode = treeView->Items->AddChild(
			parentNode, viewItem.getName());

		MyData* dataNode = new MyData;
		dataNode->path = viewItem.getPath();
		dataNode->isFolder = viewItem.getIsFolder();
		dataNode->type = viewItem.getType();

		if(dataNode->type == TYPE_DISK) {
			pChildNode->ImageIndex = Icons::disk;
            pChildNode->SelectedIndex = Icons::disk;
		}
		else {
			if(dataNode->isFolder) {
				pChildNode->ImageIndex = Icons::folder;
				pChildNode->SelectedIndex = Icons::folder;
			}
			else {
				pChildNode->ImageIndex = Icons::file;
				pChildNode->SelectedIndex = Icons::file;
			}
		}

		pChildNode->Data = (MyData*) dataNode;
	}
}
//---------------------------------------------------------------------------

void TMainForm::createItemInListView(TListView* listView, BaseObject & viewItem) {
	// ---------------------Create ListView Item-------------------------
		TListItem* pItem = listView->Items->Add();
		pItem->Caption = viewItem.getName();

		MyData* dataNode = new MyData;
		dataNode->path = viewItem.getPath();
		dataNode->isFolder = viewItem.getIsFolder();
		dataNode->type = viewItem.getType();

		if(dataNode->type == TYPE_DISK) {
			pItem->ImageIndex = Icons::disk;
		}
		else {
			if(dataNode->isFolder) {
				pItem->ImageIndex = Icons::folder;

			}
			else {
				pItem->ImageIndex = Icons::file;
			}
		}

		pItem->Data = (MyData*) dataNode;
}
//---------------------------------------------------------------------------

void TMainForm::updateUI(TTreeView* treeView, TListView* listView, TTreeNode* node, TEdit* edit) {

	// Если выделен узел "Мой компьютер", происходит обновление списка доступных дисков
	if(node == treeView->Items->GetFirstNode()) {
        edit->Text = "My Computer";
		ListDisk newDiskArray = Disk::GetAllDisks();
		std::vector<Disk> addedDiskArray = TMainForm::searchAddedDisks(node);
		for(int i = 0; i < addedDiskArray.size(); ++i) {
			TMainForm::createNodeInTreeView(treeView, addedDiskArray[i], node);
		}
		listView->Clear();
		for(ListDisk::iterator it_disk = newDiskArray.begin(); it_disk != newDiskArray.end(); ++it_disk) {
			TMainForm::createItemInListView(listView, *it_disk);
		}
		return;
	}
	// -----------------------------------------------------------------------

	// В остальных случаях, когда выделен любой другой узел, происходит обновление папок в директории
	AnsiString path = ((MyData*)(node->Data))->path;
	bool isFolder = ((MyData*)(node->Data))->isFolder;

	edit->Text = path;

	ListFile newFileArray = File::GetAllFiles(path);

	// Поиск добавленных и удаленных элементов
	std::vector<File> addedFileArray = TMainForm::searchAddedFiles(node, path);
	std::vector<TTreeNode*> deletedFileArray = TMainForm::searchDeletedFiles(newFileArray, node);

	// Добавление и удаление найденных элементов
	// -----------------TreeView---------------------
	for(int i = 0; i < addedFileArray.size(); ++i) {
		TMainForm::createNodeInTreeView(treeView, addedFileArray[i], node);
	}

	for(int i = 0; i < deletedFileArray.size(); ++i) {
	  	treeView->Items->Delete(deletedFileArray[i]);
	}
	// ----------------------------------------------

	// Обновление содержимого ListView
	// -----------------ListView---------------------
	if(newFileArray.empty()) {
		if(isFolder) {
			listView->Clear();
		}
		return;
	}
	listView->Clear();
	for(ListFile::iterator it_file = newFileArray.begin(); it_file != newFileArray.end(); ++it_file) {
		TMainForm::createItemInListView(listView, *it_file);
	}
	// ----------------------------------------------
}
//---------------------------------------------------------------------------

void TMainForm::updateUI(TTreeView* treeView, TListView* listView, TListItem* item) {
	// Переход по директориям с помощью ListView
	TTreeNode* node = matchListItemToTreeNode(treeView, item);
	if(node != treeView->Selected) {
		treeView->Selected = node;
	}
	else {
		item->Selected = false;

		// *** reserve ***
		//treeView->Selected = node->Parent;
		//TTreeNode* nod = matchListItemToTreeNode(treeView, item);
		//treeView->Selected = nod;
		// *** ------- ***
	}
	// ------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

TTreeNode* TMainForm::matchListItemToTreeNode(TTreeView* treeView, TListItem* item) {
	AnsiString pathListItem = ((MyData*)(item->Data))->path;
	TTreeNode* selectedNode = treeView->Selected;

	for(int i = 0; i < selectedNode->Count; i++) {
		TTreeNode* node = selectedNode->operator [](i);
		AnsiString pathTreeNode = ((MyData*)(node->Data))->path;

		if(pathListItem == pathTreeNode) {
			return node;
		}
	}
    return selectedNode;
}
//---------------------------------------------------------------------------

std::vector<File> TMainForm::searchAddedFiles(TTreeNode* node, AnsiString path) {

	std::vector<File> addedFilesArray;
	ListFile newFilesArray = File::GetAllFiles(path);

	for(ListFile::iterator it_file = newFilesArray.begin(); it_file != newFilesArray.end(); ++it_file) {
		if(!isFileContain(node, (BaseObject&) *it_file)) {
			addedFilesArray.push_back(*it_file);
		}
	}
	return addedFilesArray;
}
//---------------------------------------------------------------------------

std::vector<Disk> TMainForm::searchAddedDisks(TTreeNode* node) {
	std::vector<Disk> addedDisksArray;
	ListDisk newDisksArray = Disk::GetAllDisks();

	for(ListDisk::iterator it_disk = newDisksArray.begin(); it_disk != newDisksArray.end(); ++it_disk) {
		if(!isFileContain(node, (BaseObject&) *it_disk)) {
			addedDisksArray.push_back(*it_disk);
		}
	}
	return addedDisksArray;
}
//---------------------------------------------------------------------------

std::vector<TTreeNode*> TMainForm::searchDeletedFiles(ListFile fileArray, TTreeNode* node) {
    std::vector<TTreeNode*> deletedFileArray;
	for(int i = 0; i < node->Count; i++) {
		if(find(fileArray.begin(), fileArray.end(), node->operator [](i)) == fileArray.end()) {
			deletedFileArray.push_back(node->operator [](i));
		}
	}
	return deletedFileArray;
}
//---------------------------------------------------------------------------

bool TMainForm::isFileContain(TTreeNode* node, BaseObject& bsItem) {
	for(int i = 0; i < node->Count; i++) {
		if(bsItem == node->operator [](i)) {
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------

TTreeNode* TMainForm::findChildNodeByPath(TTreeNode* node, AnsiString path) {
	for(int i = 0; i < node->Count; i++) {
		TTreeNode* currentNode = node->operator [](i);
		AnsiString pathCurrentNode = ((MyData*)(currentNode->Data))->path;

		if(path == pathCurrentNode) {
			return currentNode;
		}
		else {
			 TTreeNode* resultNode = findChildNodeByPath(currentNode, path);
			 if (resultNode == NULL) {
				 continue;
			 }
			 else {
				return resultNode;
			 }
        }
	}
	return NULL;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::pTreeViewChange(TObject *Sender, TTreeNode *Node)
{
	updateUI(pTreeView, pListView, Node, Edit1);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::pTreeViewDeletion(TObject *Sender, TTreeNode *Node)
{
	MyData* dataNode = (MyData*)Node->Data;
	delete dataNode;
	dataNode = 0;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::pListViewDeletion(TObject *Sender, TListItem *Item)
{
	MyData* dataItem = (MyData*)Item->Data;
	delete dataItem;
	dataItem = 0;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormCreate(TObject *Sender)
{
	HANDLE hThread;
	CONST HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwThreadId;

	MyVCL* pData = new MyVCL;
	pData->treeView = pTreeView;
	pData->listView = pListView;
    pData->edit = Edit1;

	hThread = CreateThread(NULL, 0,  ThreadProc, (MyVCL*)pData, 0, &dwThreadId);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::BitBtn1Click(TObject *Sender)
{
	TTreeNode* node = pTreeView->Selected;
	if(node->Parent != NULL) {
		pTreeView->Selected = node->Parent;
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Edit1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if(Key == vkReturn) {
		UnicodeString str = Edit1->Text;
		wchar_t ch = str.operator [](str.Length());

		if(ch != (wchar_t)"\\") {
			str += "\\";
            Edit1->Text = str;
		}

        TTreeNode* node = findChildNodeByPath(pTreeView->Items->GetFirstNode(), Edit1->Text);

		if(node != NULL) {
            pTreeView->Selected = node;
		}
		else {
			if(Edit1->Text == "") {
				Edit1->Text = "My Computer";
                pTreeView->Selected = pTreeView->Items->GetFirstNode();
			}
			else {
				ShowMessage("Uncorrect path. Try again!");
				MyData* data = (MyData*)pTreeView->Selected->Data;
				Edit1->Text = data->path;
			}
        }
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button1Click(TObject *Sender)
{
    updateUI(pTreeView, pListView, pTreeView->Selected, Edit1);
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
	AnsiString nameFile = ChangeFileExt(Application->ExeName, ".INI");
	TIniFile* settingsFile = new TIniFile(nameFile);
	settingsFile->WriteInteger(nameFile, "Top", Top);
	settingsFile->WriteInteger(nameFile, "Left", Left);
    delete settingsFile;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::pListViewMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	TListView* list = dynamic_cast<TListView*>(Sender);
	if(lastItem == list->Selected && lastItem != NULL) {
		if(countClickOnListViewItem + 1 == 2) {
			updateUI(pTreeView, pListView, list->Selected);
			countClickOnListViewItem = 0;
		}
		else {
			countClickOnListViewItem++;
		}
	}
	else {
		lastItem = list->Selected;
		countClickOnListViewItem = 0;
		countClickOnListViewItem++;
	}
}
//---------------------------------------------------------------------------

