#include "sys/sysinfo.h" //For the system info structure
#include <unistd.h> //Sleep, system config, processor information
#include <ctime> //Uptime
#include <fstream> //Uptime/ifstream
#include <chrono> //Uptime
#include <iostream> //Standard in
#include <sys/statvfs.h> //Statistics for storage structure
using namespace std;
using namespace std::chrono;

long blocks = 0;
long used = 0;
long long pMemU = 0;
int pRun = 0;

long long totPhysMem() {
    struct sysinfo info;
    const double meg = 1024 * 1024;
    sysinfo(&info);
    long long totalPMem = info.totalram;
    totalPMem *= info.mem_unit;
    return totalPMem / meg;
}

long long physMemFree() {
    struct sysinfo info;
    const double meg = 1024 * 1024;
    sysinfo(&info);
    long long physMemFree = info.freeram + info.bufferram;
    physMemFree *= info.mem_unit;
    return physMemFree / meg;
}

long long sysUptime() {
    std::chrono::milliseconds uptime(0u);
    double uptime_seconds;
    if (std::ifstream("/proc/uptime", std::ios::in) >> uptime_seconds)
    {
        uptime = std::chrono::milliseconds(
            static_cast<unsigned long long>(uptime_seconds * 60)
            );
    }
    return uptime_seconds;
}

void diskSpace(int prog, char* dir) {
    struct statvfs dSpc;
    char* direct = dir;
    statvfs(direct,&dSpc);

    if (prog == 0) {
        printf("Disk              |   Block Size               |   Total # of Blocks   |   Blocks In Use   |   Space Used   \n");
        printf("%-4s              |   %-10ld               |   %-17ld   |   %-13ld   |   %-3ld bytes   \n\n",dir, dSpc.f_bsize,dSpc.f_blocks,dSpc.f_blocks - dSpc.f_bavail,(dSpc.f_blocks - dSpc.f_bavail) * 4096);
    } else {
        printf("Blocks In Use     |   Space Used   \n");
        printf("%-13ld     |   %-3ld bytes\n\nDifferences: %-3lld  |   %-4lld bytes\n", dSpc.f_blocks - dSpc.f_bavail, (dSpc.f_blocks - dSpc.f_bavail) * 4096, blocks - (1LL * dSpc.f_blocks - dSpc.f_bavail), used  - ((1LL * dSpc.f_blocks - dSpc.f_bavail) * 4096));
    }
    used = (1LL * dSpc.f_blocks - dSpc.f_bavail) * 4096;
    blocks = dSpc.f_blocks - dSpc.f_bavail;
}

void printStats(int status,char* dir) {
    struct sysinfo info;
    sysinfo(&info);
    if (status == 0) {
        printf("\nPhysical Memory   |   Physical Memory In Use   |   Processes Running   |   Processors Online   \n");
        printf("%-12lld MB   |   %-19lld MB   |   %-17d   |   %-17ld\n\n",totPhysMem(),physMemFree(),info.procs,sysconf(_SC_NPROCESSORS_ONLN));
        diskSpace(0,dir);
    } else {
        printf("\nPhysical Memory In Use   |   Processes Running\n");
        printf("%-19lld MB   |   %-17d\n\nDifferences: %-6lld MB   |   %d   \n\n",physMemFree(),info.procs, pMemU - physMemFree(), pRun - info.procs);
        diskSpace(1,dir);
    }
    pMemU = physMemFree();
    pRun = info.procs;
}

int main() {
    int status = 0;
    int i = 0;
    char* dir = new char(8);
    int delay;
    printf("\nProgram that gathers information about the University of Georgia's Computer Science server, Odin (Use program in fullscreen for best experience)\n\n");
    printf("\nCurrent system uptime: %lld seconds\n\n",sysUptime());
    printf("Enter the directory you'd like to check the disk space of: (EX: '/', '/home', '..',etc)\n");
    std::cin >> dir;
    while(true) {
        printf("\nPlease enter the amount of seconds that you'd like to wait between your system checks: \n");
        std::cin >> delay;
        if (i == 0) {
            printStats(0,dir);
        }
        sleep(delay);
        printf("\nStats after pause: (A negative difference statistic signifies a decrease in the resource usage)\n");
        printStats(1,dir);
        printf("\nWould you like to check your system statistics again? 0 for 'Yes', 1 for 'No'.\n");
        std::cin >> status;
        if (status == 1) {
            printf("Program is exiting\n");
            break;
        }
        i++;
    }
    return 0;
}
