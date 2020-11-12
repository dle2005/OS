#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <utmp.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ncurses.h>
#include <linux/kdev_t.h>
#include <math.h>
#include <termios.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
    
char **tokenize(char *line);
void f_uptime();
void f_user();
void f_load_average(); 
void f_tasks();
void f_cpu();
void f_kib();
void f_proc(int key_count);
int my_getch();

FILE *fp;
char *line;
char path[MAX_INPUT_SIZE];
size_t len = 0;

int main(int argc, char* argv[]) {

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    struct timespec s_time;
    clock_gettime( CLOCK_REALTIME, &s_time);
    s_time.tv_sec %= 3;
    
    int key_count = 0;

    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    move(0,0);

    printw("top - %02d:%02d:%02d ", tm.tm_hour, tm.tm_min, tm.tm_sec);
    f_uptime();
    f_user();
    f_load_average();
    f_tasks();
    f_cpu();
    f_kib();
    printw("\n  PID USER      PR  NI    VIRT    RES    SHR S  %%CPU %%MEM     TIME+ COMMAND\n");
    f_proc(key_count);

    refresh();

    while(1) {
        move(0,0);

        t = time(NULL);
        tm = *localtime(&t);
        struct timespec sleep_time;
        clock_gettime( CLOCK_REALTIME, &sleep_time);
        sleep_time.tv_sec %= 3;

        if((s_time.tv_sec == sleep_time.tv_sec) && (s_time.tv_nsec/100000000 == sleep_time.tv_nsec/100000000)) {
//            printw("ket_count %d\n", key_count);
            printw("top - %02d:%02d:%02d ", tm.tm_hour, tm.tm_min, tm.tm_sec);
            f_uptime();
            f_user();
            f_load_average();
            f_tasks();
            f_cpu();
            f_kib();
            printw("\n  PID USER      PR  NI    VIRT    RES    SHR S  %%CPU %%MEM     TIME+ COMMAND\n");
            f_proc(key_count);
            
            refresh();
        }
        else {
            int ch = getch();
            switch(ch) {
                case KEY_DOWN:
                    key_count++;
                    printw("top - %02d:%02d:%02d ", tm.tm_hour, tm.tm_min, tm.tm_sec);
                    f_uptime();
                    f_user();
                    f_load_average();
                    f_tasks();
                    f_cpu();
                    f_kib();
                    printw("\n  PID USER      PR  NI    VIRT    RES    SHR S  %%CPU %%MEM     TIME+ COMMAND\n");
                    f_proc(key_count);
                    refresh();
                    break;
                case KEY_UP:
                    key_count--;
                    if(key_count < 0) key_count = 0;
                    printw("top - %02d:%02d:%02d ", tm.tm_hour, tm.tm_min, tm.tm_sec);
                    f_uptime();
                    f_user();
                    f_load_average();
                    f_tasks();
                    f_cpu();
                    f_kib();
                    printw("\n  PID USER      PR  NI    VIRT    RES    SHR S  %%CPU %%MEM     TIME+ COMMAND\n");
                    f_proc(key_count);
                    refresh();
                    break;
           }
            if(ch == 'q') {
                break;
                endwin();
            }
        }

    }

    endwin();
}
int my_getch() {
    int c;
    struct termios oldattr, newattr;

    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    newattr.c_cc[VMIN] = 1;
    newattr.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return c;
}

void f_proc(int key_count) {
    DIR* dp;
    struct dirent *entry;
    char **tokens;
    unsigned long total_memory;
    char uptime_str[MAX_INPUT_SIZE];
    long Hertz = sysconf(_SC_CLK_TCK);

    fp = fopen("/proc/meminfo", "r");
    getline(&line, &len, fp);
    sscanf(line, "MemTotal:         %lu kB", &total_memory);
    fclose(fp);

    fp = fopen("/proc/uptime", "r");
    getline(&line, &len, fp);
    sscanf(line, "%s ", uptime_str);
    float uptime = atof(uptime_str);
    fclose(fp);

    int proc_count = 0;

    if((dp = opendir("/proc")) == NULL)
        perror("opendir error\n");

    while((entry = readdir(dp)) != NULL) {

        if((atoi(entry->d_name)) <= 0) continue;

        if(proc_count < key_count) { proc_count++; continue; }

        char uid_int_str[8], state, comm[100];
        long cutime, cstime, num_threads, itreavalue;
        int virt = 0, res = 0, rssfile = 0, rssshmen =0, pid, ppid, pgrp, session, tty_nr, tpgid;
        int priority = 0, nice = 0;
        unsigned int flags;
        unsigned long minflt, cminflt, majflt, cmjflt, utime, stime, starttime;
        unsigned long long vmsize, memory_rss;

        strcpy(path, "/proc/");
        strcat(path, entry->d_name);
        strcat(path, "/status");

        fp = fopen(path, "r");

        while(getline(&line, &len, fp) != -1) {
            tokens = tokenize(line);

            if(!strcmp(tokens[0], "Uid:")) strcpy(uid_int_str, tokens[1]);
            else if(!strcmp(tokens[0], "VmSize:")) virt = atoi(tokens[1]);
            else if(!strcmp(tokens[0], "VmRSS:")) res = atoi(tokens[1]);
            else if(!strcmp(tokens[0], "RssFile:")) rssfile = atoi(tokens[1]);
            else if(!strcmp(tokens[0], "RssShmen:")) rssshmen = atoi(tokens[1]);

        }

        struct passwd *pw = getpwuid(atoi(uid_int_str));
        char *user = pw->pw_name;

        fclose(fp);

        strcpy(path, "/proc/");
        strcat(path, entry->d_name);
        strcat(path, "/stat");
            
        fp = fopen(path, "r");
        getline(&line, &len, fp);
        sscanf(line, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %d %d %ld %ld %lu",
                &pid, comm, &state, &ppid, &pgrp, &session, &tty_nr, &tpgid, &flags, &minflt, &cminflt, &majflt,
                &cmjflt, &utime, &stime, &cutime, &cstime, &priority, &nice, &num_threads, &itreavalue, &starttime);
        fclose(fp);

        char priority_str[4];
        if(priority == -100) strcpy(priority_str, "rt");
        else sprintf(priority_str, "%d", priority);

        float total_time = utime + stime;
        total_time = total_time + cutime + cstime;
        float seconds = uptime - (starttime / Hertz);
        float cpu_usage = 100 * ((total_time / Hertz) / seconds);
        
        float mem_usage = 100 * (float)res / total_memory;

        char min_str[4];
//        int min = total_time /6000;
        int min = (utime + stime) / 6000;
        sprintf(min_str, "%d", min);
        int sec = (utime + stime) / 100;
        sec = sec % 60;
        int decimal = (utime + stime) % 100;

        char cmdline[MAX_INPUT_SIZE];
        bzero(cmdline, sizeof(cmdline));
            
        strcpy(path, "/proc/");
        strcat(path, entry->d_name);
        strcat(path, "/comm");
        
        fp = fopen(path, "r");

        getline(&line, &len, fp);
        sscanf(line, " %s", cmdline);
        
        fclose(fp);

        printw("%5s %-8s %3s %3d %7d %6d %6d %c %5.1f %4.1f %3s:%02d.%02d %s\n",
                entry->d_name, user, priority_str, nice, virt, res, rssfile+rssshmen, state, cpu_usage, mem_usage, min_str, sec, decimal, cmdline);
    }

    closedir(dp);
}

void f_kib() {
    unsigned long total, free, cached, buffers, reclaim, s_total, s_free, available;
    char **tokens;

    fp = fopen("/proc/meminfo", "r");

    while(getline(&line, &len, fp) != -1) {
        tokens = tokenize(line);

        if(!strcmp(tokens[0], "MemTotal:")) total = atoi(tokens[1]);
        else if(!strcmp(tokens[0], "MemFree:")) free = atoi(tokens[1]);
        else if(!strcmp(tokens[0], "Cached:")) cached = atoi(tokens[1]);
        else if(!strcmp(tokens[0], "Buffers:")) buffers = atoi(tokens[1]);
        else if(!strcmp(tokens[0], "SReclaimable:")) reclaim = atoi(tokens[1]);

        else if(!strcmp(tokens[0], "SwapTotal:")) s_total = atoi(tokens[1]);
        else if(!strcmp(tokens[0], "SwapFree:")) s_free = atoi(tokens[1]);
        else if(!strcmp(tokens[0], "MemAvailable:")) available = atoi(tokens[1]);
    }

    printw("KiB Mem : %7ld total,  %7ld free,  %7ld used,  %7ld buff/cache\n",
            total, free, total - cached + free + reclaim, buffers + cached);

    printw("KiB Swap: %7ld total,  %7ld free,  %7ld used,  %7ld avail Mem\n",
            s_total, s_free, s_total - s_free, available);

    fclose(fp);
}

void f_cpu() {
    fp = fopen("/proc/stat", "r");

    float total, us, sy, ni, id, wa, hi, si, st;
    char cpu_name[5];

    getline(&line, &len, fp);
    sscanf(line, "%s %f %f %f %f %f %f %f %f",
            cpu_name, &us, &ni, &sy, &id, &wa, &hi, &si, &st);
    
    total = us + sy + ni + id + wa + hi + si + st;

    us = us / total * 100;
    sy = sy / total * 100;
    ni = ni / total * 100;
    id = id / total * 100;
    wa = wa / total * 100;
    hi = hi / total * 100;
    si = si / total * 100;
    st = st / total * 100;

    printw("%%Cpu(s):  %.1f us,  %.1f sy,  %.1f ni,  %.1f id,  %.1f wa,  %.1f hi,  %.1f si,  %.1f st\n",
            us, sy, ni, id, wa, hi, si, st);

    fclose(fp);
}


void f_tasks() {
    DIR *dp = NULL;
    struct dirent *entry;
    int total = 0, running = 0, sleeping = 0, stopped = 0, zombie = 0;

    if((dp = opendir("/proc")) == NULL)
        perror("opendir error\n");
        
    while((entry = readdir(dp)) != NULL) {
        if(atoi(entry->d_name) <= 0) continue;

        total++;

        strcpy(path, "/proc/");
        strcat(path, entry->d_name);
        strcat(path, "/stat");

        fp = fopen(path, "r");
        getline(&line, &len, fp);

        char comm[100], state;
        int pid;

        sscanf(line, "%d %s %c", &pid, comm, &state);

        switch(state) {
            case 'R' : running++; break;
            case 'S' : sleeping++; break;
            case 'D' : sleeping++; break;
            case 'T' : stopped++; break;
            case 'Z' : stopped++; break;
        }

        fclose(fp);
    }
    
    printw("Tasks: %3d total, %3d running, %3d sleeping, %3d stopped, %3d zombie\n",
            total, running, sleeping, stopped, zombie);

    closedir(dp);
}

void f_load_average() {
    fp = fopen("/proc/loadavg", "r");
    getline(&line, &len, fp);

    char **tokens;
    tokens = tokenize(line);

    printw("load average: %s, %s, %s\n", tokens[0], tokens[1], tokens[2]);

    fclose(fp);
}

void f_user() {
    int user = 0;
    struct utmp* utmp;

    setutent();

    while((utmp = getutent()) != NULL) {
        if(utmp->ut_type == USER_PROCESS)
            user++;
    }

    printw("%d user,  ", user);
}


char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

void f_uptime() {
    int boot_t, day, hour, min;

    fp = fopen("/proc/uptime", "r");

    getline(&line, &len, fp);
    boot_t = atoi(line);

    min = boot_t / 60;
    hour = min / 60;
    day = hour / 24;
    min = min % 60;
    hour = hour % 24;
    day = boot_t / (24 * 60 * 60);

    if(day == 0) {
        if(hour == 0) {
            printw("up %02d, ", min);
        } else {
            printw("up %d:%02d, ", hour, min);
        }
    } else {
        if(hour == 0) {
            printw("up %d day, %02d, ", day, min);
        } else {
            printw("up %d day, %d:%02d, ", day, hour, min);
        }
    } 

    fclose(fp);
}

