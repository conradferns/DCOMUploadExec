#include "Utils.h"

bool parseArgument(const std::wstring& arg, std::wstring& domain, std::wstring& user, std::wstring& password, std::wstring& address) {

    if (arg == LOCAL_ATTACK_KEYWORD)
    {
        domain = L"";
        user = L"";
        password = L"";
        address = L"";
        return true;
    }

    // Regular expression to match the format [domain]\[user]:[password]@[address]
    std::wregex pattern(LR"(([^\\]+)\\([^:]+):([^@]+)@(.+))");
    std::wsmatch match;

    if (std::regex_match(arg, match, pattern)) {
        // Assign parsed values to the respective variables
        domain = match[1];
        user = match[2];
        password = match[3];
        address = match[4];
        return true;
    }

    return false;
}

size_t GetEnvironmentSizeW(WCHAR* pchEnvironment)
{
    WCHAR* cur = pchEnvironment;
    do {
        // scan for the end of the string
        while (*cur != '\0')
            cur++;

        // move past null 
        cur++;
    } while (*cur != '\0');
    return cur - pchEnvironment + 1;
}