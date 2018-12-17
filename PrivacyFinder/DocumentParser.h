#pragma once
class DocumentParser
{
public:
    static DocumentParser& GetInstace()
    {
        static DocumentParser _instance;
        return _instance;
    }
    int TextToChar(const wchar_t* textFilePath, char* &original);
    bool CharToSharp(const char* srcString, char* &outString);

    virtual ~DocumentParser();
private:
    DocumentParser();
    
};

