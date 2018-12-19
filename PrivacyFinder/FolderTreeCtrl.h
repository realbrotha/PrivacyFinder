#pragma once
#include "stdafx.h"

#include <vector>

class FolderTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(FolderTreeCtrl)

public:
	FolderTreeCtrl();
	virtual ~FolderTreeCtrl();

protected:
	DECLARE_MESSAGE_MAP()

public:
	// Member Variable.
	SHFILEINFO			shInfo;
	LPITEMIDLIST		idList;
	TVINSERTSTRUCT		tvi;
	LPSHELLFOLDER		desktopFolder;
	HTREEITEM			destopItem;
	HTREEITEM			currItem;
	LPMALLOC			pMalloc;
	TCHAR				pathFromSH[MAX_PATH];

	// Member Fucntion.
	void		 TreeInitialize();
    BOOL         GetCheckedFolderList(std::vector<CString>& pathList, HTREEITEM item);
    TCHAR*       GetPidlToFolderPath(HTREEITEM item);
	BOOL		 GetItemImage();
	void		 GetDesktopInformation();
	void		 EnumChildItems(LPSHELLFOLDER shFolder, LPITEMIDLIST parentIdList, HTREEITEM parentItem);
	LPITEMIDLIST ILAppend(LPITEMIDLIST idList, LPITEMIDLIST childIdList);
	HTREEITEM	 TreeInsertItem(TVINSERTSTRUCT *pTvi, LPSHELLFOLDER shFolder, LPITEMIDLIST pidl, LPITEMIDLIST pidlRelative, CString &stringBuffer);
	void		 UTStrretToString(LPITEMIDLIST pidl, LPSTRRET pStr, CString &stringBuffer);
    //for user
    void CheckChildItems(HTREEITEM item);
    void UnCheckChildItems(HTREEITEM item);
    void ParentCheckOnOff(HTREEITEM item);

    BOOL IsAllChild(HTREEITEM item, HTREEITEM clickItem);
    void IsAllParent(HTREEITEM parentItem, BOOL check);
};
