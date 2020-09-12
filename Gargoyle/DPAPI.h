#pragma once

#include <Windows.h>
#include <dpapi.h>
#include <string>

#pragma comment(lib, "Crypt32.lib")

// To encrypt bytes using DPAPI
// ------------------------------------------------------------------------

std::string env_encrypt(std::string message) {
    DATA_BLOB plaintextBlob = { 0 };
    DATA_BLOB ciphertextBlob = { 0 };

    BYTE* dataBytes = (BYTE*)message.c_str();

    plaintextBlob.pbData = dataBytes;
    plaintextBlob.cbData = message.length();

    if(!CryptProtectData(&plaintextBlob, NULL, NULL, NULL, NULL, CRYPTPROTECT_LOCAL_MACHINE, &ciphertextBlob))
        return "";

    std::string ciphertext(reinterpret_cast<char const*>(ciphertextBlob.pbData), ciphertextBlob.cbData);

    return ciphertext;
}