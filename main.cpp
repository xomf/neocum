#include <iostream>
#include <unistd.h>
#include <sys/utsname.h>
#include <fstream>
#include <string>

const std::string resetColour = "\033[0m";
const std::string titleColour = "\033[1;31m"; //the colour of my semen, you might want to change this. 

int main() {
    struct utsname unameData;
    uname(&unameData);

    char *home = getenv("HOME");
    if (!home) {
        std::cerr << "Error: HOME environment variable is not set." << std::endl;
        return 1;
    }

    std::string customMessageFilePath = std::string(home) + "/.config/cum.txt";
    std::string customMessage;
    std::ifstream customMessageFile(customMessageFilePath);
    if (customMessageFile.is_open()) {
        std::string line;
        while (std::getline(customMessageFile, line)) {
            customMessage += line + '\n';
        }
        customMessageFile.close();
    }

    if (!customMessage.empty()) {
        std::cout << "\n" << customMessage << std::endl;
    }

    std::cout << "-----------------------------------" << std::endl;
    std::cout << titleColour << getenv("USER") << "@" << unameData.nodename << resetColour << std::endl;
    std::cout << "-----------------------------------" << "\n" << std::endl;

    std::cout << titleColour << "OS:" << resetColour << " " << unameData.sysname << " " << unameData.release << std::endl;
    std::cout << titleColour << "Kernel:" << resetColour << " " << unameData.version << std::endl;
    std::cout << titleColour << "Hostname:" << resetColour << " " << unameData.nodename << std::endl;
    std::cout << titleColour << "Architecture:" << resetColour << " " << unameData.machine << std::endl;

    return 0;
}
