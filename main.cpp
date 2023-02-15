#include <algorithm>
#include <ctime>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>

const std::string resetColour = "\033[0m";
const std::string titleColour = "\033[1;31m";

std::string prettyName() {
  std::ifstream file("/etc/os-release");
  std::string line;
  std::string pretty_name = "N/A";
  while (std::getline(file, line)) {
    if (line.find("PRETTY_NAME=") == 0) {
      pretty_name = line.substr(13, line.size() - 14);
      pretty_name.erase(
          std::remove(pretty_name.begin(), pretty_name.end(), '\"'),
          pretty_name.end());
      break;
    }
  }
  return pretty_name;
}

std::string cleanseWhitespace(std::string str) {
  int i = 0;
  while (str[i] == ' ' | str[i] == '\t') {
    i++;
  }
  return str.substr(i);
}

std::string getGpuFolder() {
  std::string gpuFolderPath = "/proc/driver/nvidia/gpus/";
  DIR *dir = opendir(gpuFolderPath.c_str());
  if (!dir) {
    return "";
  }

  struct dirent *entry = readdir(dir);
  while (entry) {
    if (entry->d_type == DT_DIR && entry->d_name[0] != '.') {
      std::string folderName = entry->d_name;
      closedir(dir);
      return gpuFolderPath + folderName;
    }
    entry = readdir(dir);
  }

  closedir(dir);
  return "";
}

int main() {
  struct utsname unameData;
  uname(&unameData);
  struct sysinfo sys_info;
  sysinfo(&sys_info);

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
  std::cout << titleColour << getenv("USER") << "@" << unameData.nodename
            << resetColour << std::endl;
  std::cout << "-----------------------------------"
            << "\n"
            << std::endl;

  std::cout << titleColour << "OS:" << resetColour << " " << unameData.sysname
            << " " << unameData.release << std::endl;

  std::cout << titleColour << "Distro:" << resetColour << " " << prettyName()
            << std::endl;

  std::cout << titleColour << "Kernel:" << resetColour << " "
            << unameData.version << std::endl;
  std::cout << titleColour << "Hostname:" << resetColour << " "
            << unameData.nodename << std::endl;
  std::cout << titleColour << "Architecture:" << resetColour << " "
            << unameData.machine << std::endl;

  std::cout << titleColour << "CPU:" << resetColour << " ";
  std::ifstream cpuInfo("/proc/cpuinfo");
  if (cpuInfo.is_open()) {
    std::string line;
    while (std::getline(cpuInfo, line)) {
      if (line.find("model name") != std::string::npos) {
        std::cout << line.substr(line.find(":") + 2) << std::endl;
        break;
      }
    }
    cpuInfo.close();
  }

  std::cout << titleColour << "GPU:" << resetColour << " ";
  std::ifstream gpuInfo(getGpuFolder() + "/information");
  if (gpuInfo.is_open()) {
    std::string line;
    while (std::getline(gpuInfo, line)) {
      if (line.find("Model") != std::string::npos) {
        std::cout << cleanseWhitespace(line.substr(line.find(":") + 2))
                  << std::endl;
        break;
      }
    }
    gpuInfo.close();
  } else {
    std::cout << "N/A" << std::endl;
  }

  std::ifstream meminfo("/proc/meminfo");
  std::string line;
  long long total_memory = 0, free_memory = 0, used_memory = 0;
  while (std::getline(meminfo, line)) {
    std::istringstream iss(line);
    std::string name;
    long long value;
    iss >> name >> value;
    if (name == "MemTotal:") {
      total_memory = value;
    } else if (name == "MemFree:" || name == "Buffers:" || name == "Cached:") {
      free_memory += value;
    }
  }
  used_memory = total_memory - free_memory;

  std::cout << titleColour << "Memory: " << resetColour << used_memory / 1024
            << "MiB/" << total_memory / 1024 << "MiB" << std::endl;

  std::cout << titleColour << "Uptime:" << resetColour << " ";
  std::ifstream uptime("/proc/uptime");
  if (uptime.is_open()) {
    std::string line;
    if (std::getline(uptime, line)) {
      int uptimeSecs = std::stoi(line.substr(0, line.find(".")));
      int minutes = uptimeSecs / 60 % 60;
      int hours = uptimeSecs / 3600 % 24;
      int days = uptimeSecs / 86400;
      std::cout << days << "d " << hours << "h " << minutes << "m" << std::endl;
    }
    uptime.close();
  }

  std::cout << titleColour << "Current time:" << resetColour << " ";
  time_t now = time(0);
  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&now);
  strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
  std::cout << buf << std::endl;
  std::cout << "\n";

  return 0;
}
