#pragma once

#include "resource.h"		// main symbols


class CPrivacyFinderApp : public CWinApp
{
public:
    CPrivacyFinderApp();

public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CPrivacyFinderApp theApp;