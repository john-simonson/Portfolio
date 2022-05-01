// from code prof Moloney showed in class
#define _FILE_OFFSET_BITS 64

#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>        // file type/prot macros
#include<sys/sysmacros.h>   // major/minor macros
#include<errno.h>
#include<stdlib.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>
#include<stdio.h>

/*********************************************************************
struct stat {
    dev_t       st_dev;         // device inode resides on 
    ino_t       st_ino;         // this inode's number 
    mode_t      st_mode;        // protection 
    nlink_t     st_nlink;       // number or hard links to the file 
    uid_t       st_uid;         // user-id of owner 
    gid_t       st_gid;         // group-id of owner 
    dev_t       st_rdev;        // dev type, for inode that is dev 
    off_t       st_size;        // total size of file 
    time_t      st_atime;       // file lastaccess time 
    time_t      st_mtime;       // file last modify time 
    time_t      st_ctime;       // file last status change time 
    uint_t      st_blksize;     // opt blocksize for file sys i/o ops 
    int         st_blocks;      // actual number of blocks allocated 
    //........... etc...........
    };
***********************************************************************/

extern int errno;
void print_details(struct stat);

int main(int argc, char* argv[]){

    struct stat stat_buf;
    int i, j;
    DIR * dirptr;
    char dirspc[1024];
    struct dirent * entptr;

        if( argc > 1 ){
            for( i = 1; i < argc; i++){
                if(lstat(argv[i], &stat_buf) == -1){
                    perror("bad file name for stat");
                    exit(1);
                }
                printf("FILENAME : \t\t\t%s\n", argv[i]);
                print_details(stat_buf);
            }
        }else{ /* get file names from current working directory */
            if((dirptr = opendir(".")) == NULL){
                perror("failed to open current directory");
                exit(2);
            }
            while((entptr = readdir(dirptr)) != NULL ){
                if(lstat(entptr->d_name, &stat_buf) == -1){
                    perror("bad file name for stat");
                    exit(1);
                }
                printf("FILENAME : \t\t\t%s\n", entptr->d_name);
                print_details(stat_buf);
            }
        }
}

void print_details(struct stat stat_buf){
    char prot_buf[10];
    struct passwd * pwptr;
    struct group * grptr;
    int j;

    /******** macros and bit names from /usr/include/sys/stat.h *********/
    prot_buf[9] = '\0';
    if (S_ISDIR(stat_buf.st_mode))printf("FILE TYPE:\t\t\tdirectory\n");
    if (S_ISREG(stat_buf.st_mode))printf("FILE TYPE:\t\t\tordinary\n");
    if (S_ISCHR(stat_buf.st_mode))printf("FILE TYPE:\t\t\tchar dev\n");
    if (S_ISBLK(stat_buf.st_mode))printf("FILE TYPE:\t\t\tblock dev\n");
    if (S_ISFIFO(stat_buf.st_mode))printf("FILE TYPE:\t\t\tpipe\n");
    if (S_ISSOCK(stat_buf.st_mode))printf("FILE TYPE:\t\t\tUD socket\n");
    if (S_ISLNK(stat_buf.st_mode))printf("FILE TYPE:\t\t\tsymbolic link\n");
    for ( j = 0; j < 9; j++ ) prot_buf[j] = '-';
        if ( stat_buf.st_mode & S_IRUSR ) prot_buf[0] = 'r';
        if ( stat_buf.st_mode & S_IWUSR ) prot_buf[1] = 'w';
        if ( stat_buf.st_mode & S_IXUSR ) prot_buf[2] = 'x';
        if ( stat_buf.st_mode & S_IRGRP ) prot_buf[3] = 'r';
        if ( stat_buf.st_mode & S_IWGRP ) prot_buf[4] = 'w';
        if ( stat_buf.st_mode & S_IXGRP ) prot_buf[5] = 'x';
        if ( stat_buf.st_mode & S_IROTH ) prot_buf[6] = 'r';
        if ( stat_buf.st_mode & S_IWOTH ) prot_buf[7] = 'w';
        if ( stat_buf.st_mode & S_IXOTH ) prot_buf[8] = 'x';
        printf("PERMISSIONS: \t\t\t%s\n", prot_buf);
        if( (pwptr = getpwuid(stat_buf.st_uid)) == NULL){
            printf("OWNER_NAME:\t\t\t%d\n", stat_buf.st_uid);
        } else {
            printf("OWNER_NAME:\t\t\t%s\n", pwptr->pw_name);
        }
        if( (grptr = getgrgid(stat_buf.st_gid)) == NULL){
            printf("GROUP_NAME:\t\t\t%d\n", stat_buf.st_gid);
        } else {
            printf("GROUP_NAME:\t\t\t%s\n", grptr->gr_name);
        }
        printf("DATE_OF_LAST_MODIFICATIONS:\t%s", ctime(&(stat_buf.st_mtime)));
//      printf("NLINK SIZEOF:\t\t\t%d\n", sizeof(stat_buf.st_nlink));
        printf("LINK_COUNT:\t\t\t%ld\n", stat_buf.st_nlink);
        if( (S_ISCHR(stat_buf.st_mode)) || (S_ISBLK(stat_buf.st_mode))  ){
/********* major and minor lines 65 and 66 in /usr/include/sys/sysmacros.h ******/
        printf("MAJOR - MINOR CODES:\t\t%d, %d\n", major(stat_buf.st_rdev), minor(stat_buf.st_rdev));
        }else{
//          printf("FILE SIZEOF:\t\t\t%d bytes\n", sizeof(stat_buf.st_size));
            printf("FILE_SIZE:\t\t\t%ld\n", stat_buf.st_size);
        }
//      printf("INUM SIZE:\t\t\t%d\n", sizeof(stat_buf.st_ino));
        printf("INODE_NUMBER:\t\t\t%lu\n\n", stat_buf.st_ino);
        return;
}