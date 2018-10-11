//---------------------------------------------------------------------------

#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Buttons.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>

#include <algorithm>
#include <vector>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#include "BaseObject.h"
#include "Disk.h"
#include "File.h"
#include <System.IniFiles.hpp>
//---------------------------------------------------------------------------
typedef std::list<Disk> ListDisk;
typedef std::list<File> ListFile;

enum Icons {
	folder = 0,
	file = 1,
	disk = 2
};

struct MyData {
	AnsiString path;
	AnsiString type;
    bool isFolder;
};

struct MyVCL {
	TTreeView* treeView;
	TListView* listView;
    TEdit* edit;
};

class TMainForm : public TForm
{
__published:	// IDE-managed Components
	TTreeView *pTreeView;
	TListView *pListView;
	TPanel *Panel1;
	TEdit *Edit1;
	TButton *Button1;
	TBitBtn *BitBtn1;
	TEdit *Edit2;
	TBitBtn *BitBtn2;
	TBitBtn *BitBtn3;
	TMainMenu *MainMenu1;
	TMenuItem *File1;
	TMenuItem *Optio1;
	TMenuItem *About1;
	TImageList *ImageList1;

	void __fastcall pTreeViewChange(TObject *Sender, TTreeNode *Node);
	void __fastcall pTreeViewDeletion(TObject *Sender, TTreeNode *Node);
	void __fastcall pListViewDeletion(TObject *Sender, TListItem *Item);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall BitBtn1Click(TObject *Sender);
	void __fastcall Edit1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall pListViewMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);

private:	// User declarations

	int countClickOnListViewItem;
    TListItem* lastItem;

	static std::vector<File> searchAddedFiles(TTreeNode* node, AnsiString path = "MyComputer");
	static std::vector<Disk> searchAddedDisks(TTreeNode* node);
	static std::vector<TTreeNode*> searchDeletedFiles(ListFile fileArray, TTreeNode* node);

	static void createNodeInTreeView(TTreeView* treeView, BaseObject & viewItem, TTreeNode* parentNode);
	static void createItemInListView(TListView* listView, BaseObject & viewItem);

	static void updateUI(TTreeView* treeView, TListView* listView, TTreeNode* node, TEdit* edit);
	static void updateUI(TTreeView* treeView, TListView* listView, TListItem* item);

    static TTreeNode* matchListItemToTreeNode(TTreeView* treeView, TListItem* item);
	static bool isFileContain(TTreeNode* node, BaseObject & item);
	static DWORD WINAPI ThreadProc(LPVOID lpParam);

    TTreeNode* TMainForm::findChildNodeByPath(TTreeNode* node, AnsiString path);

public:		// User declarations
	__fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
