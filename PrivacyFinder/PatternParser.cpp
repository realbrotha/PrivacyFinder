#include "stdafx.h"
#include "PatternParser.h"
#include "PrivacyFinderDlg.h"

#include <strsafe.h>
#include <list>

PatternParser::PatternParser()
{
}

PatternParser::~PatternParser()
{
}

bool PatternParser::Initialize()
{
    const TCHAR patterPolicy[] = _T("patternPolicy.ini");
    TCHAR currentPath[MAX_PATH] = _T("");
    DWORD size = ::GetModuleFileName(NULL, currentPath, _countof(currentPath));

    if (size < _countof(currentPath))
    {
        LPTSTR lastReverseSlash = _tcsrchr(currentPath, _T('\\'));

        if (lastReverseSlash)
        {
            *lastReverseSlash = _T('\0');
            lastReverseSlash = _tcsrchr(currentPath, _T('\\'));
            policyFilePath_.Format(_T("%s\\%s"), currentPath, patterPolicy);
        }
    }
    else
    {
        return false;
    }

	for (int i = 1; i < 6; i++)        // 카테고리 종류의 개수 (예, 주민 
	{
        GetPatternList(i, 50);
	}
    return true;
}
std::list<POLICY_INFO> PatternParser::GetPolicyList()
{
    return policyInfoList_;
}

void PatternParser::GetPatternList(int indexCount, int stringLen)
{
	TCHAR* APPName = NULL;
	TCHAR* KeyName = NULL;
	
	switch(indexCount)
	{
	case 1:
		APPName = _T("JUMIN");
		KeyName = _T("jumin");
		break;
	case 2:
		APPName = _T("CARD");
		KeyName = _T("card");
		break;
	case 3:
		APPName = _T("PHONE");
		KeyName = _T("phone");
		break;
	case 4:
		APPName = _T("DRIVER_LICENSE");
		KeyName = _T("driver_license");
		break;
    case 5:
        APPName = _T("USER");
        KeyName = _T("user");
        break;
	}
	int dwSize = 0;
	dwSize = GetPrivateProfileInt(APPName, _T("count"), 0, policyFilePath_);

	for (int nCount = 0; nCount < dwSize; nCount++)
	{
        PATTERN_INFO patterBuffer = { 0, };

        TCHAR buffer[256] = { 0, };
        TCHAR listName[MAX_PATH] = { 0, };

		StringCbPrintf(listName, stringLen, _T("%s%d"),KeyName, nCount + 1);
		DWORD dwLastError = GetPrivateProfileString(APPName, listName, _T("0"),buffer,sizeof(buffer), policyFilePath_);
		patterBuffer.szPattern.Format(_T("%s"), buffer);

        patternInfoList_.push_back(patterBuffer);
	}
	//여기서 정보를 넣는다
    POLICY_INFO infoBuffer = { 0, };
	infoBuffer.count = dwSize;
	infoBuffer.index = indexCount;
	infoBuffer.PatternList = patternInfoList_;

	policyInfoList_.push_back(infoBuffer);
    patternInfoList_.clear();
}
