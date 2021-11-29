
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void usage(){
    std::cout << "px-first-login-password-helper -u <username> -p <hash_password> -r <root_hash_password>" << std::endl;
}

int main(int argc, char *argv[]) {
    std::string username = "";
    std::string password = "";
    std::string rootPassword = "";
    
    if(argc==7) {
        for (int i = 1; i < argc; i++) {
            if(!strcmp(argv[i],"-u"))
                username = std::string(argv[i+1]);
            else if(!strcmp(argv[i],"-p"))
                password = std::string(argv[i+1]);
            else if(!strcmp(argv[i],"-r"))
                rootPassword = std::string(argv[i+1]);
        }
    } 
    if(argc !=7 || username.empty() || password.empty()) {
        usage();
        return -1;
    }

    std::string command = "echo '" + username + ":" + password + "' | chpasswd -e;";
    command += "echo 'root:" + rootPassword + "' | chpasswd -e";
    return system(command.c_str()); 
}
