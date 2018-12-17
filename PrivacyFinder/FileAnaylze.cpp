#include "stdafx.h"
#include "FileAnaylze.h"
#include "PrivacyFinderDlg.h"

FileAnalyzer::FileAnalyzer()
{
}

FileAnalyzer::~FileAnalyzer()
{
}

int FileAnalyzer::BreadthSearch(CString strFilePath, char* OrigianlStr, char* ShadowStr, int buffSize, CString& detectedTypeString)
{
	unsigned long detectedType = 0L; // 카테고리 중복을 회피하기 위한 타입변수  
	int totalCount = 0;
	
    std::list<POLICY_INFO> policy = PatternParser::GetInstance().GetPolicyList();

    for (std::list<POLICY_INFO>::iterator policyIt = policy.begin() ;  policyIt != policy.end(); ++policyIt)
	{
		for (std::list<PATTERN_INFO>::iterator patternListIt = policyIt->PatternList.begin(); patternListIt != policyIt->PatternList.end(); ++patternListIt)
		{
			int depthSearchCount = DepthSearch(strFilePath, OrigianlStr, ShadowStr,	buffSize, (TCHAR*)(LPCTSTR)patternListIt->szPattern);

			totalCount += depthSearchCount;

			if (depthSearchCount)
			{			
				switch (policyIt->index)
				{
				case 1:
					if (!(detectedType & 0x00000001))        
					{
                        detectedTypeString += _T("주민 ");
						detectedType |= 0x00000001;
					}
					break;
				case 2:
					if (!(detectedType & 0x00000002))
					{
                        detectedTypeString += _T("카드 ");
						detectedType |= 0x00000002;
					}
					break;
				case 3:
					if (!(detectedType & 0x00000004))
					{
                        detectedTypeString += _T("폰 ");
						detectedType |= 0x00000004;
					}
					break;
				case 4:
					if (!(detectedType & 0x00000010))
					{
                        detectedTypeString += _T("면허 ");
						detectedType |= 0x00000010;
					}
					break;
				case 5:
					if (!(detectedType & 0x00000020))
					{
                        detectedTypeString += _T("유저 ");
						detectedType |= 0x00000020;
					}
					break;
				default:
					break;
				}
			}
		}
	}
	return totalCount;
}

// 패턴 검출 
int FileAnalyzer::DepthSearch(CString strFilePath, char* OrigianlStr, char* ShadowStr, int buffSize, TCHAR * TdoublePattern)
{
	char patternStringAnsi[256] = { 0, };
	int nCount = 0; 

	for (int k = 0; k < buffSize; k++)
	{
		WideCharToMultiByte(CP_ACP, 0, TdoublePattern, 256, patternStringAnsi, 256, NULL, NULL);

		int patternLen = strlen(patternStringAnsi);

        if ((buffSize - k) < patternLen) // 남은 길이가 패턴 길이보다 작으면 skip
            continue;

		if ( 0 == strncmp(ShadowStr + k, patternStringAnsi, patternLen) )
		{
			char originalString[MAX_PATH] = { 0, };
			CString count = _T("");
			DETECT_INFO detectedInfo = { 0, };
				
			count.Format(_T("%d"), k);
			detectedInfo.position = count;								// K 변수 받아감
			detectedInfo.pattern = patternStringAnsi;					// 패턴 형태 받아감
			detectedInfo.filePath = strFilePath;						// 파일 경로 받아감
				
			memcpy(originalString, OrigianlStr + k, patternLen);
			detectedInfo.originalString = (LPCSTR)(LPSTR)originalString;	     // 오리지널 스트링
				
			pThis->detectedInfoList.push_back(detectedInfo);				// 푸시 함

			nCount++;
			k += patternLen;
		}
	}
	return nCount;
}
