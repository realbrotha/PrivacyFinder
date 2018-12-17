#include "stdafx.h"
#include "DocumentParser.h"
#include <fstream>

DocumentParser::DocumentParser()
{
}


DocumentParser::~DocumentParser()
{
}

int DocumentParser::TextToChar(const wchar_t* textFilePath, char* &outString)
{
    __int32 stringLength = 0;
    std::ifstream fileStream(textFilePath, std::ifstream::binary);
    if (fileStream)
    {
        fileStream.seekg(0, fileStream.end);
        stringLength = static_cast<__int32>(fileStream.tellg());
        fileStream.seekg(0, fileStream.beg);

        outString = new char[stringLength + 1];
        fileStream.read(outString, stringLength);
        outString[stringLength] = '\0';
        
        fileStream.close();
    }
    return stringLength;
}

bool DocumentParser::CharToSharp(const char* srcString, char* &outString)
{
    bool result = false;
    int stringLength = strlen(srcString);

    outString = new char[stringLength + 1];
    memset(outString, 0x00, stringLength + 1);
    memcpy(outString, srcString, stringLength);

    for (int i = 0; i < stringLength; i++)
    {
        if (outString[i] >= 0x30 && outString[i] <= 0x39)
        {
            outString[i] = '#';
            result = true;
        }
    }
    return result;
}
