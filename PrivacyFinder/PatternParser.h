#pragma once

#include <list>

typedef struct PATTERN_INFO_
{
	CString szPattern;
}PATTERN_INFO;

typedef struct POLICY_INFO_
{
	int		count;			
	ULONG   index;
	std::list<PATTERN_INFO> PatternList;
}POLICY_INFO;

class PatternParser
{
public:
    static PatternParser& GetInstance()
    {
        static PatternParser instance_;
        return instance_;
    }
	~PatternParser();

    bool Initialize();
    std::list<POLICY_INFO> GetPolicyList();
private:
    PatternParser();
    void GetPatternList(int indexCount, int stringLength);

    CString policyFilePath_;

    std::list<POLICY_INFO> policyInfoList_;      // 최상위 정책
    std::list<PATTERN_INFO> patternInfoList_;    // 패턴 리스트
};

