#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "freq_list.h"
#include "worker.h"

/* A program to model calling run_worker and to test it. Notice that run_worker
 * produces binary output, so the output from this program to STDOUT will 
 * not be human readable.  You will need to work out how to save it and view 
 * it (or process it) so that you can confirm that your run_worker 
 * is working properly.
 */
int main(int argc, char **argv) {
    char ch;
    char path[PATHLENGTH];
    char *startdir = ".";

    /* this models using getopt to process command-line flags and arguments */
    while ((ch = getopt(argc, argv, "d:")) != -1) {
        switch (ch) {
        case 'd':
            startdir = optarg;
            break;
        default:
            fprintf(stderr, "Usage: queryone [-d DIRECTORY_NAME]\n");
            exit(1);
        }
    }

    // Open the directory provided by the user (or current working directory)
    DIR *dirp;
    if ((dirp = opendir(startdir)) == NULL) {
        perror("opendir");
        exit(1);
    }

    /* For each entry in the directory, eliminate . and .., and check
     * to make sure that the entry is a directory, then call run_worker
     * to process the index file contained in the directory.
     * Note that this implementation of the query engine iterates
     * sequentially through the directories, and will expect to read
     * a word from standard input for each index it checks.
     */

    int index = 0;
    int p1[MAXWORKERS][2];
    int p2[MAXWORKERS][2];
    struct dirent *dp;
    while ((dp = readdir(dirp)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 ||
            strcmp(dp->d_name, "..") == 0 ||
            strcmp(dp->d_name, ".svn") == 0 ||
            strcmp(dp->d_name, ".git") == 0) {
                continue;
        }

        strncpy(path, startdir, PATHLENGTH);
        strncat(path, "/", PATHLENGTH - strlen(path));
        strncat(path, dp->d_name, PATHLENGTH - strlen(path));
        path[PATHLENGTH - 1] = '\0';

        struct stat sbuf;
        if (stat(path, &sbuf) == -1) {
            // This should only fail if we got the path wrong
            // or we don't have permissions on this entry.
            perror("stat");
            exit(1);
        }

        int count = 0;
        // Only call run_worker if it is a directory
        // Otherwise ignore it.
        if (S_ISDIR(sbuf.st_mode)) {
            count++;
            if (pipe(p1[index]) == -1) {
                perror("pipe");
            }
            if (pipe(p2[index]) == -1) {
                perror("pipe");
            }
            int r = fork();
            if (r > 0) {
                printf("In parent process\n");
                if ((close(p1[index][0])) == -1){
                    perror("close");
                }
                if ((close(p2[index][1])) == -1){
                    perror("close");
                }
            }
            else if (r == 0) {
                printf("In child process\n");
                if ((close(p1[index][1])) == -1){
                    perror("close");
                }
                if ((close(p2[index][0])) == -1){
                    perror("close");
                }
                run_worker(path, p1[index][0], p2[index][1]);
                exit(0);
            } else {
                perror("fork");
                exit(1);
            }
        }
        index++;
    }

    char input[MAXWORD];
    FreqRecord fr;
    FreqRecord frarr[MAXWORKERS];
    int frarr_index = 0;
    while (1) {
        fgets(input, MAXWORD, stdin);
        printf("Input: %s\n", input);
        for (int i = 0; i < 1; i++) {
            printf("writting input\n");
            if (write(p1[i][1], input, MAXWORD) == -1) {
                perror("write to pipe");
            }    
            while (1) {
                read(p2[i][0], &fr, sizeof(FreqRecord));
                printf("Reading from child process\n");
                printf("Chile process return filenme: %s\n", fr.filename);
                if (fr.freq != 0) {
                    frarr[frarr_index] = fr;
                    frarr_index++;
                } else {
                    break;
                }
            }
            printf("reach this line\n");
        }

        printf("Final print_freq_records\n");
        print_freq_records(frarr);
        
    }

    if (closedir(dirp) < 0)
        perror("closedir");

    return 0;
}
