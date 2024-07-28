#define UNICODE
#pragma comment(lib, "netapi32.lib")

#include <napi.h>
#include <stdio.h>
#include <assert.h>
#include <windows.h>
#include <lm.h>
#include <clocale>
#include <cstring>
#include <iostream>
#include <string>gf

using namespace std;

string getUserRights(const string username) {
    setlocale(LC_ALL, "Russian");

    string result { "" };
    LPLOCALGROUP_USERS_INFO_0 pBuf = NULL;
    DWORD dwLevel = 0;
    DWORD dwFlags = LG_INCLUDE_INDIRECT;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    NET_API_STATUS nStatus;

    wstring wUsername = wstring(username.begin(), username.end());

    nStatus = NetUserGetLocalGroups(NULL,
                                wUsername.c_str(),
                                dwLevel,
                                dwFlags,
                                (LPBYTE *)&pBuf,
                                dwPrefMaxLen,
                                &dwEntriesRead,
                                &dwTotalEntries);

    if (nStatus == NERR_Success)
    {
        LPLOCALGROUP_USERS_INFO_0 pTmpBuf;
        DWORD i;
        DWORD dwTotalCount = 0;

        if ((pTmpBuf = pBuf) != NULL)
        {
            for (i = 0; i < dwEntriesRead; i++)
            {
                assert(pTmpBuf != NULL);

                if (pTmpBuf == NULL)
                {
                    break;
                }

                int rightNameLength = WideCharToMultiByte(CP_UTF8, 0, pTmpBuf->lgrui0_name, -1, nullptr, 0, nullptr, nullptr);
                string rightName(rightNameLength, 0);
                WideCharToMultiByte(CP_UTF8, 0, pTmpBuf->lgrui0_name, -1, &rightName[0], rightNameLength, nullptr, nullptr);

                result.append(rightName);
                result.append(";");

                pTmpBuf++;
                dwTotalCount++;
            }
        }
    }
    else
        cout << "User not found" << endl;

    if (pBuf != NULL)
        NetApiBufferFree(pBuf);

    if (result.length() > 0) {
        const string empty;
        result.replace(result.length() - 1, 1, empty);
    }

    return result;
}

Napi::Value getUserRightsWrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Username expected").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string username = info[0].As<Napi::String>().Utf8Value();
    string content;

    try {
        content = getUserRights(username);
    } catch (const std::exception& e) {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, content);
}

Napi::Object init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "getUserRights"), Napi::Function::New(env, getUserRightsWrapper));
    return exports;
};

NODE_API_MODULE(test, init);