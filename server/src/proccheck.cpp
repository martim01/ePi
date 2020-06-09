#include "proccheck.h"
#include "log.h"
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>

using namespace pml;

bool IsProcRunning(const std::string& sProc)
{
    DIR* dir;
    struct dirent* ent;
    char* endptr;
    char buf[512];

    bool bExec(false);
    bool bSNMP(false);
    bool bDecoder(false);

    if (!(dir = opendir("/proc")))
    {
        Log::Get(Log::LOG_ERROR) << "can't open /proc" << std::endl;
        return true;
    }

    while((ent = readdir(dir)) != NULL)
    {
        /* if endptr is not a null character, the directory is not
         * entirely numeric, so ignore it */
        long lpid = strtol(ent->d_name, &endptr, 10);
        if (*endptr != '\0')
        {
            continue;
        }

        /* try to open the cmdline file */
        snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
        FILE* fp = fopen(buf, "r");

        if (fp)
        {
            if (fgets(buf, sizeof(buf), fp) != NULL)
            {
                /* check the first token in the file, the program name */
                char* first = strtok(buf, " ");
                if (strstr(first, sProc.c_str())!=NULL)
                {
                    return true;
                }
            }
            fclose(fp);
        }
    }

    closedir(dir);
    return false;
}

