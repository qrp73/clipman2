/*
    clipman2 - A basic clipboard manager for Wayland, with support 
               for persisting copy buffers after an application exits.
    Copyright (C) 2024-2025  qrp73
    https://github.com/qrp73/clipman2

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

// gcc -o clipman2 clipman2.c
// sudo cp clipman2 /usr/bin/
// wayfire: ~/.config/wayfire.ini:     clipman-store = wl-paste -t "text/plain;charset=utf-8" --watch /usr/bin/clipman2
// labwc:   ~/.config/labwc/autostart: /usr/bin/wl-paste -t "text/plain;charset=utf-8" --watch /usr/bin/clipman2 & 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>


ssize_t readWithTimeout(FILE *file, char *buffer, size_t size, int timeout_msec) {
    int fd = fileno(file);
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);
    struct timeval timeout;
    timeout.tv_sec = timeout_msec / 1000;
    timeout.tv_usec = (timeout_msec-timeout.tv_sec*1000) * 1000;
    int ret = select(fd + 1, &read_fds, NULL, NULL, &timeout);
    if (ret == -1) {
        perror("select");
        return -1;
    } else if (ret == 0) {
//printf("select(): timeout\n");
        return 0;           // timeout expired
    }
    return read(fd, buffer, size);
}

char* readTextToEnd(FILE* file) {
    size_t size = 4096;
    char* text = (char *)malloc(size+1);
    size_t index = 0;
    for (;!feof(file);) {
        ssize_t readBytes = readWithTimeout(file, text+index, size-index, 200);
        if (readBytes == 0 && index == 0) {
            free(text);
            return NULL;
        }
        if (readBytes <= 0) break;
        index += readBytes;
        if (index < size-1) continue;
        // not enough buffer, reallocate twice more
        size *= 2;
        char* buf = (char*)malloc(size+1);
        memcpy(buf, text, index);
        free(text);
        text = buf;
    }
    text[index] = 0;
    return text;
}

char *file_readAllText(const char* fileName) {
    char *text = NULL;
    FILE *file = fopen(fileName, "r");
    if (file) {
        text = readTextToEnd( file );
        fclose(file);
    }
    return text;
}

void file_writeAllText(const char* fileName, const char* text) {
    FILE *file = fopen(fileName, "w");
    if (file) {
        fwrite(text, 1, strlen(text), file);
        fclose(file);
    }
}

int file_exists(const char* fileName) {
    return access(fileName, F_OK) == 0;
    //struct stat   buffer;   
    //return (stat (fileName, &buffer) == 0);
}


void path_combine(char *result, size_t result_size, const char *folder, const char *filename) {
    if (folder[strlen(folder) - 1] == '/') {
        snprintf(result, result_size, "%s%s", folder, filename);
    } else {
        snprintf(result, result_size, "%s/%s", folder, filename);
    }    
}

void path_getHome(char *result, size_t result_size) {
    const char *home = getenv("HOME");
    if (home) {
        snprintf(result, result_size, "%s", home);
    } else {
        fprintf(stderr, "Failed to get HOME environment variable\n");
        exit(EXIT_FAILURE);
    }
}

void serveText(const char *cmd, const char *args, const char *text) {
    size_t size = strlen(cmd) + strlen(args) + 8;
    char argv[size];
    snprintf(argv, size, "%s %s", cmd, args);
    FILE *pipe = popen(argv, "w");
    if (pipe == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }
    fwrite(text, sizeof(char), strlen(text), pipe);
    pclose(pipe);
}


int main() {
    char folder[PATH_MAX] = "/dev/shm";
    char filePath[PATH_MAX];
    char lockPath[PATH_MAX];

/*
#if HOME_FOLDER
    char homePath[PATH_MAX];
    path_getHome(homePath, sizeof(homePath));
    path_combine(folder,   sizeof(folder),   homePath,  ".local/share");
#endif
*/
    path_combine(filePath, sizeof(filePath), folder,    "clipman2.txt");
    path_combine(lockPath, sizeof(lockPath), folder,    "clipman2.txt.lock");

    //printf("notify\n");

    char *text = readTextToEnd( stdin );
    if (text == NULL) {
        return EXIT_FAILURE;
    }

    char *textOld = file_readAllText(filePath);
    

    if (!textOld || strcmp(text, textOld) != 0) {
        file_writeAllText(filePath, text);

        if (!file_exists(lockPath)) {
            remove(lockPath);
        }
    }

    if (!file_exists(lockPath)) {
        file_writeAllText(lockPath, "");

        // TODO: find the path for wl-copy
        serveText("/bin/wl-copy", "--type text/plain", text);
    } else {
        remove(lockPath);
    }

    if (textOld) {
        free(textOld);
        textOld = NULL;
    }
    if (text) {
        free(text);
        text = NULL;
    }
    return EXIT_SUCCESS;
}
