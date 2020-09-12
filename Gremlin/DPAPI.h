#pragma once

#include <Windows.h>
#include <dpapi.h>
#include <string>

#pragma comment(lib, "Crypt32.lib")

// To decrypt the DPAPI encrypted blob on the same host
// ------------------------------------------------------------------------

std::string env_decrypt(std::string ciphertext) {
    DATA_BLOB plaintextBlob = { 0 };
    DATA_BLOB ciphertextBlob = { 0 };

    BYTE* dataBytes = (BYTE*)ciphertext.c_str();

    ciphertextBlob.pbData = dataBytes;
    ciphertextBlob.cbData = ciphertext.length();

    if(!CryptUnprotectData(&ciphertextBlob, NULL, NULL, NULL, NULL, 0, &plaintextBlob))
        return "";

    std::string plaintext(reinterpret_cast<char const*>(plaintextBlob.pbData), plaintextBlob.cbData);

    return plaintext;
}