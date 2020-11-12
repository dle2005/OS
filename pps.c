#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h> //DIR 함수 헤더 파일
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/kdev_t.h>
#include <pwd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

int main(int argc, char* argv[]) {

    DIR* dp; //디렉토리 포인터
    struct dirent *entry; //디렉토리 구조체
    struct stat stat; //파일 정보 구조체
    FILE *fp; //파일 포인터
    long Hertz = sysconf(_SC_CLK_TCK);

    char path[MAX_INPUT_SIZE];
    char proc_file[40];
    char* line;
    size_t len = 0;

    int t_major, t_minor;

    int user_uid = getuid();

    if((dp = opendir("/proc")) == NULL) { //proc디렉토리를 연다.
        perror("opendir error\n");
        return 0; 
    }

    char uptime_str[MAX_INPUT_SIZE];

    strcpy(path, "/proc/");
    strcat(path, "uptime");

    fp = fopen(path, "r");

    getline(&line, &len, fp);
    sscanf(line, "%s ", uptime_str);

    float uptime = atof(uptime_str);

    unsigned long long total_memory;

    strcpy(path, "/proc/");
    strcat(path, "meminfo");

    fp = fopen(path, "r");

    getline(&line, &len, fp);
    sscanf(line, "MemTotal:        %llu kB", &total_memory);

    if(argc < 2) { // default ps
        printf("  PID TTY          TIME CMD\n");

        // 현재 프로세스의 major minor값 구하기
        char t_path[MAX_INPUT_SIZE];
        int kernel_pid = getpid(); //현재 프로세스의 pid구함
        sprintf(t_path, "%d", kernel_pid); //문자열로 저장

        strcpy(path, "/proc/");
        strcat(path, t_path);
        strcat(path, "/stat"); //현재 프로세스의 stat으로 경로 설정
        
        fp = fopen(path, "r");
        getline(&line, &len, fp);

        char t_comm[10], t_state;
        unsigned int t_flags;
        int t_pid, t_ppid, t_pgrp, t_session, t_tty_nr, t_tpgid;
        unsigned long t_minflt, t_cminflt, t_majflt, t_cmjflt, t_utime, t_stime;
        unsigned long long t_starttime;
        long t_cutime, t_cstime, t_priority, t_nice, t_num_threads, t_itreavalue;

        //현재 프로세스의 stat 정보를 가져옴
        sscanf(line, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu",
                &t_pid, t_comm, &t_state, &t_ppid, &t_pgrp, &t_session, &t_tty_nr, &t_tpgid, &t_flags, &t_minflt, &t_cminflt, &t_majflt, &t_cmjflt,
                &t_utime, &t_stime, &t_cutime, &t_cstime, &t_priority, &t_nice, &t_num_threads, &t_itreavalue, &t_starttime);

        t_major = MAJOR(t_tty_nr); //현재 프로세스의 major minor 번호 저장
        t_minor = MINOR(t_tty_nr);
    }
    else if(strcmp(argv[1], "a") == 0) {
        printf("  PID TTY      STAT   TIME COMMAND\n");
    }
    else if(strcmp(argv[1], "u") == 0) {
        printf("USER       PID %%CPU %%MEM    VSZ   RSS TTY      STAT START   TIME COMMAND\n");
    }
    else if(strcmp(argv[1], "x") == 0) {
        printf("  PID TTY      STAT   TIME COMMAND\n");
    }
    else if((strcmp(argv[1], "au") == 0) || (strcmp(argv[1], "ua") == 0)) {
        printf("USER       PID %%CPU %%MEM    VSZ   RSS TTY      STAT START   TIME COMMAND\n");
    }
    else if((strcmp(argv[1], "ax") == 0) || (strcmp(argv[1], "xa") == 0)) {
        printf("  PID TTY      STAT   TIME COMMAND\n");
    }
    else if((strcmp(argv[1], "ux") == 0) || (strcmp(argv[1], "xu") == 0)) {
        printf("USER       PID %%CPU %%MEM    VSZ   RSS TTY      STAT START   TIME COMMAND\n");
    }
    else if((strcmp(argv[1], "aux") == 0) || (strcmp(argv[1], "axu") == 0) || (strcmp(argv[1], "uax") == 0)
            || (strcmp(argv[1], "uxa") == 0) || (strcmp(argv[1], "xua") == 0) || (strcmp(argv[1], "xau") == 0)) {
        printf("USER       PID %%CPU %%MEM    VSZ   RSS TTY      STAT START   TIME COMMAND\n");
    }
    else printf("SSUShell : Incorrect command\n");

    while((entry = readdir(dp)) != NULL) { //디렉토리를 읽어 들인다.
        
        lstat(entry->d_name, &stat); //파일의 state정보를 가져옴
        if(!S_ISDIR(stat.st_mode)) continue; //디렉토리가 아니라면 continue

        if((atoi(entry->d_name)) <= 0) continue; //숫자인 경우만 파싱

        strcpy(path, "/proc/");
        strcat(path, entry->d_name);
        strcat(path, "/stat");

        fp = fopen(path, "r");
        getline(&line, &len, fp);

        char comm[100], state;
        unsigned int flags;
        int pid, ppid, pgrp, session, tty_nr, tpgid;
        unsigned long minflt, cminflt, majflt, cmjflt, utime, stime;
        unsigned long long starttime, vmsize, memory_rss;
        long cutime, cstime, priority, nice, num_threads, itreavalue;

        sscanf(line, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu",
                &pid, comm, &state, &ppid, &pgrp, &session, &tty_nr, &tpgid, &flags, &minflt, &cminflt, &majflt, &cmjflt,
                &utime, &stime, &cutime, &cstime, &priority, &nice, &num_threads, &itreavalue, &starttime);

        strcpy(path, "/proc/");
        strcat(path, entry->d_name);
        strcat(path, "/status");

        char uid_int_str[6] = {0};
        char stat_state[10];

        fp = fopen(path, "r");
        
        getline(&line, &len, fp);
        getline(&line, &len, fp);
        getline(&line, &len, fp);
        sscanf(line, "State:  %s ", stat_state);
        getline(&line, &len, fp);
        getline(&line, &len, fp);
        getline(&line, &len, fp);
        getline(&line, &len, fp);
        getline(&line, &len, fp);
        getline(&line, &len, fp);
        sscanf(line, "Uid:    %s ", uid_int_str);
        getline(&line, &len, fp);
        getline(&line, &len, fp);
        getline(&line, &len, fp);
        getline(&line, &len, fp);
        getline(&line, &len, fp);
        getline(&line, &len, fp);
        getline(&line, &len, fp);
        getline(&line, &len, fp);
        getline(&line, &len, fp);
        sscanf(line, "VmSize:     %llu kB", &vmsize);
        getline(&line, &len, fp);
        getline(&line, &len, fp);
        getline(&line, &len, fp);
        getline(&line, &len, fp);
        sscanf(line,  "VmRSS:      %llu kB", &memory_rss);

        char ax_comm[MAX_INPUT_SIZE];

        strcpy(path, "/proc/");
        strcat(path, entry->d_name);
        strcat(path, "/cmdline");

        fp = fopen(path, "r");
        
        char temp[MAX_INPUT_SIZE];
        bzero(ax_comm, sizeof(ax_comm));
        bzero(temp, sizeof(temp));
        while(feof(fp) == 0) {
            fgets(temp, sizeof(temp), fp);
            strcat(ax_comm, temp);
            bzero(temp, sizeof(temp));
        }

        if(fp < 0) {
            strcpy(path, "/proc/");
            strcat(path, entry->d_name);
            strcat(path, "/comm");

            fp = fopen(path, "r");

            getline(&line, &len, fp);
            strcpy(ax_comm, line);
        }


        float memory_usage = 100 * (float)memory_rss / total_memory;
        float total_time = utime + stime;
        total_time = total_time + (unsigned long)cutime + (unsigned long)cstime;
        float seconds = uptime - (starttime / Hertz);
        float cpu_usage = 100 * ((total_time/ Hertz) / seconds);

        if(isnan(cpu_usage)) cpu_usage = 0.0;
        if(isnan(memory_usage)) memory_usage = 0.0;

        uid_t entry_uid = atoi(uid_int_str);
        struct passwd *pw = getpwuid(atoi(uid_int_str));
        char* user = pw->pw_name;

        int major = MAJOR(tty_nr);
        int minor = MINOR(tty_nr);

        unsigned long time = ((unsigned long)utime + (unsigned long)stime) / Hertz;
        unsigned long c_time = (unsigned long)(cutime + cstime) / Hertz;
        unsigned long s_time = itreavalue / Hertz;
//        unsigned long s_time = starttime / Hertz;
        int min = time / 60;
        int hour = min / 60;
        int sec = time % 60;
        min = min % 60;
        int c_min = c_time / 60;
        int c_hour = c_min / 60;
        int c_sec = c_time % 60;
        c_min = c_min % 60;
        int s_min = s_time / 60;
        int s_hour = s_min / 60;
        s_min = s_min % 60;

        char tty[7], s_minor[2];
        if(major == 4) strcpy(tty, "tty");
        else strcpy(tty, "pts/");
        sprintf(s_minor, "%d", minor);
        strcat(tty, s_minor);
        if(major != 4 && major != 136) strcpy(tty, "?");


        if(argc < 2) {
            if( major == t_major && minor == t_minor) 
                printf("%5s pts/%d    %02d:%02d:%02d %s\n", entry->d_name, minor, c_hour, c_min, sec, ax_comm);
        }
        else if((strcmp(argv[1], "a") == 0) && major != 0) {
            printf("%5d %-8s %-5s %2d:%02d %s\n", atoi(entry->d_name), tty, stat_state, min, sec, ax_comm);
        }
        else if((strcmp(argv[1], "u") == 0) && major == 136) {
            printf("%-8s %5s %4.1f %4.1f %6llu %5llu %-8s %4s %02d:%02d  %2d:%02d %s\n",
                    user, entry->d_name, cpu_usage, memory_usage, vmsize, memory_rss, tty, stat_state, s_hour, s_min, min, sec, ax_comm);
        }
        else if((strcmp(argv[1], "x") == 0) && user_uid == entry_uid) {
            printf("%5s %-8s %-5s %2d:%02d %s\n", entry->d_name, tty, stat_state, min, sec, ax_comm);
        }
        else if((strcmp(argv[1], "au") == 0) || (strcmp(argv[1], "ua") == 0)) {
            if(major != 0)
                printf("%-8s %5s %4.1f %4.1f %6llu %5llu %-8s %4s %02d:%02d %2d:%02d %s\n",
                    user, entry->d_name, cpu_usage, memory_usage, vmsize, memory_rss, tty, stat_state, s_hour, s_min, min, sec, ax_comm);
        }
        else if((strcmp(argv[1], "ax") == 0) || (strcmp(argv[1], "xa") == 0)) {
            printf("%5d %-8s %-5s %2d:%02d %s\n", atoi(entry->d_name), tty, stat_state, min, sec, ax_comm);
        }
        else if((strcmp(argv[1], "ux") == 0) || (strcmp(argv[1], "xu") == 0)) {
            if(user_uid == entry_uid)
                printf("%-8s %5s %4.1f %4.1f %6llu %5llu %-8s %-4s %02d:%02d  %2d:%02d %s\n",
                    user, entry->d_name, cpu_usage, memory_usage, vmsize, memory_rss, tty, stat_state, s_hour, s_min, min, sec, ax_comm);
        }
        else if((strcmp(argv[1], "aux") == 0) || (strcmp(argv[1], "axu") == 0) || (strcmp(argv[1], "uax") == 0)
                || (strcmp(argv[1], "uxa") == 0) || (strcmp(argv[1], "xua") == 0) || (strcmp(argv[1], "xau") == 0)) {
            printf("%-8s %5s %4.1f %4.1f %6llu %5llu %-8s %-4s %02d:%02d  %2d:%02d %s\n",
                    user, entry->d_name, cpu_usage, memory_usage, vmsize, memory_rss, tty, stat_state, s_hour, s_min, min, sec, ax_comm);
        }


    }

}
