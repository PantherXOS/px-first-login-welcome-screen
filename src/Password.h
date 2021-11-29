#ifndef PX_PASSWORD_H
#define PX_PASSWORD_H
#include <crypt.h>
#include <stdio.h>
#include <iostream>
#include <string>

// https://www.gnu.org/software/libc/manual/html_node/Passphrase-Storage.html#Passphrase-Storage
class Password {
public:
    enum CryptMethod{
        DES,
        SHA_512,
        SHA_256,
        MD5
    };

    Password(const std::string &passphrase, CryptMethod method = CryptMethod::DES, const std::string &salt = "");
    std::string getHash();

private:
    std::string generateSalt(int len);
    std::string _hash;
    std::string _salt;
};
#endif //PX_PASSWORD_H
