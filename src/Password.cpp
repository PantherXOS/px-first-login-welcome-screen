#include "Password.h"

#include <map>

#define MAX_SALT_SIZE  10

std::map<Password::CryptMethod, std::string> cryptoMethodMap = {
    {Password::CryptMethod::DES,     ""   },
    {Password::CryptMethod::SHA_512, "$6$"},
    {Password::CryptMethod::SHA_256, "$5$"},
    {Password::CryptMethod::MD5,     "$1$"},
};

Password::Password(const std::string &passphrase, CryptMethod method, const std::string &salt) : 
    _salt(!salt.size() ? cryptoMethodMap[method] + generateSalt(MAX_SALT_SIZE) : salt) {
    char *hash = crypt(passphrase.c_str(), _salt.c_str());
    if (!hash || hash[0] == '*') {
        perror ("crypt");
        return;
    }
    _hash = std::string(hash);
}

std::string Password::getHash(){
    return _hash;
}

std::string Password::generateSalt(int len){
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    srand(time(NULL));
    for (int i = 0; i < len; ++i) {
        int r = rand();
        tmp_s += alphanum[r % (sizeof(alphanum) - 1)];
    }
    return tmp_s;
}