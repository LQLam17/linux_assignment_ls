#include <stdio.h>         
#include <stdlib.h>        // exit, malloc, free,...
#include <dirent.h>        // opendir, readdir, closedir
#include <sys/stat.h>      // stat, lstat, struct stat
#include <unistd.h>       
#include <pwd.h>           
#include <grp.h>           
#include <time.h>          
#include <string.h>        


void show_file_info(const char *dirpath, const char *filename) {
    char absolute_path[1024];
    sprintf(absolute_path, "%s/%s", dirpath, filename);

    // get file information 
    struct stat file_infor;
    if (lstat(absolute_path, &file_infor) == -1) {
        return;
    }

    // determine the file's type
    char type = '?';
    if (S_ISREG(file_infor.st_mode)) type = '-';        // regular file
    else if (S_ISDIR(file_infor.st_mode)) type = 'd';   // directory
    else if (S_ISLNK(file_infor.st_mode)) type = 'l';   // soft link

    // get user and group
    struct passwd *pw = getpwuid(file_infor.st_uid);
    struct group  *gr = getgrgid(file_infor.st_gid);

    // get creation time and modification time
    char ctime_str[64], mtime_str[64];
    strftime(ctime_str, sizeof(ctime_str), "%Y-%m-%d %H:%M", localtime(&file_infor.st_ctime));
    strftime(mtime_str, sizeof(mtime_str), "%Y-%m-%d %H:%M", localtime(&file_infor.st_mtime));

    // show result
    printf("%c %-20s %-10s %-10s creat: %s | modify: %s\n", type, filename, pw ? pw->pw_name : "???", gr ? gr->gr_name : "???", ctime_str, mtime_str);
}

int main(int argc, char *argv[]) {
    // if have no argument, set path as ./
    const char *path = (argc > 1) ? argv[1] : "."; 

    // get information of file, stored in stat object
    struct stat file_infor;
    if (stat(path, &file_infor) != 0) {
        return 1;
    }

    // check if the path is directory
    if (!S_ISDIR(file_infor.st_mode)) {
        fprintf(stderr, "%s isn't directory.\n", path);
        return 1;
    }

    // open directory
    DIR *dir = opendir(path);
    if (!dir) {
        return 1;
    }

    // information of entry in dir
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Dont show information of "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
        }

        // Show file's information
        show_file_info(path, entry->d_name);
    }

    closedir(dir);
    return 0;
}