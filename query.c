#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "freq_list.h"
#include "worker.h"

void perror_and_exit(char* str) {
    perror(str);
    exit(1);
}

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
            fprintf(stderr, "Usage: query [-d DIRECTORY_NAME]\n");
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

    int num_child = 0;
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

        // Only call run_worker if it is a directory
        // Otherwise ignore it.
        if (S_ISDIR(sbuf.st_mode)) {
            if (pipe(p1[num_child]) == -1) {
                perror_and_exit("pipe");
            }
            if (pipe(p2[num_child]) == -1) {
                perror_and_exit("pipe");
            }
            int r = fork();
            if (r > 0) {
                //parent
                if ((close(p1[num_child][0])) == -1){
                    perror_and_exit("close");
                }
                if ((close(p2[num_child][1])) == -1){
                    perror_and_exit("close");
                }
            }
            else if (r == 0) {
                //child
                for (int i = 0; i < num_child; i++) {
                    if ((close(p1[i][1])) == -1){
                        perror_and_exit("close");
                    }
                    if ((close(p2[i][0])) == -1){
                        perror_and_exit("close");
                    }
                }
                if ((close(p1[num_child][1])) == -1){
                    perror_and_exit("close");
                }
                if ((close(p2[num_child][0])) == -1){
                    perror_and_exit("close");
                }
                
                run_worker(path, p1[num_child][0], p2[num_child][1]);
                
                if ((close(p1[num_child][0])) == -1){
                    perror_and_exit("close");
                }
                if ((close(p2[num_child][1])) == -1){
                    perror_and_exit("close");
                }
                exit(0);
            } else {
                perror_and_exit("fork");
            }
            num_child++;
        }      
    }
    //printf("fork finished, num_child: %d found\n", num_child);
    if (num_child > MAXWORKERS) {
        fprintf(stderr, "Subdirectories more than MAXWORKER\n");
        exit(1);
    }

    while (1) {
        char input[MAXWORD];
        FreqRecord fr;

        //init frarr
        FreqRecord* frarr;
        if ((frarr = malloc((MAXRECORDS + 1) * sizeof(FreqRecord))) == NULL) {
        perror("malloc for init_FreqRecodList");
        exit(1);
        }

        int num_record = 0;
        fgets(input, MAXWORD, stdin);
        //check error
        //printf("Input: %s\n", input);
        
        for (int i = 0; i < num_child; i++) {
            //printf("writting input from pipe for child #: %d\n", i);
            if (write(p1[i][1], input, MAXWORD) == -1) {
                perror_and_exit("write to pipe");
            }
        }
        for (int i = 0; i < num_child; i++) {
            //printf("reading output to pipe for child #: %d\n", i);
            while (1) {
                read(p2[i][0], &fr, sizeof(FreqRecord));
                //printf("Chile process #: %d, return filenme: %s\n", i, fr.filename);
                if (fr.freq != 0) {
                    add_sort_records(&frarr, &fr, &num_record); 
                } else {
                    break;
                }
            }
        }
        fr.freq = 0;    
        frarr[num_record] = fr;

        //printf("Final print_freq_records\n");
        print_freq_records(frarr);
        free(frarr);
        //check error
        
    }

    if (closedir(dirp) < 0)
        perror("closedir");

    return 0;
}
