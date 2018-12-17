#pragma once
#include "PrivacyFinderDlg.h"
#include "PatternParser.h"

class FileAnalyzer : public CPrivacyFinderDlg
{
public:
    static FileAnalyzer& GetInstance()
    {
        static FileAnalyzer instance_;
        return instance_;
    }
	~FileAnalyzer();
    int BreadthSearch(CString filePath, char* originalString, char* sharpedString, int buffSize, CString& detectedTypeString);

private:
    FileAnalyzer();
    int DepthSearch(CString filePath, char* origianlString, char* sharpedString, int buffSize, TCHAR * patternString);
};

