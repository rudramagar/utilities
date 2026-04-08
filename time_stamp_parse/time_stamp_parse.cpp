#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <ctime>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <timestamp>\n", argv[0]);
        fprintf(stderr, "  %s 1775592023563597085   (nanoseconds since epoch)\n", argv[0]);
        fprintf(stderr, "  %s 563597085             (nanoseconds since midnight)\n", argv[0]);
        return 1;
    }

    uint64_t val = strtoull(argv[1], nullptr, 10);
    time_t sec;
    uint32_t nsec;

    if (strlen(argv[1]) <= 15) {
        // Nanoseconds since midnight
        sec = val / 1000000000ULL;
        nsec = val % 1000000000ULL;

        unsigned hours = sec / 3600;
        unsigned mins = (sec % 3600) / 60;
        unsigned secs = sec % 60;

        printf("%02u:%02u:%02u.%09u JST (since midnight)\n", hours, mins, secs, nsec);
    } else {
        // Nanoseconds since epoch
        sec = val / 1000000000ULL;
        nsec = val % 1000000000ULL;

        // JST = UTC + 9
        sec += 9 * 3600;

        struct tm tm;
        gmtime_r(&sec, &tm);

        printf("%04d-%02d-%02d %02d:%02d:%02d.%09u JST\n",
               tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
               tm.tm_hour, tm.tm_min, tm.tm_sec, nsec);
    }

    return 0;
}
