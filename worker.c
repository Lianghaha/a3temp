#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include "freq_list.h"
#include "worker.h"

char *remove_punc(char *);

/* Complete this function for Task 1. Including fixing this comment.
*/
FreqRecord *get_word(char *word, Node *head, char **file_names) {
	printf("\nIn get_word\n");
	printf("Word: %s\n", word);
	printf("Getting size...\n");
	int size = 0;
	Node* ptr = head;
	while (ptr != NULL) {
		printf("In word: %s\n", ptr->word);
		if (strcmp(word, ptr->word) == 0) {
			printf("Word matched\n");
			//Node contain word found
			int i = 0;
			while (file_names[i] != NULL) {
				printf("file_names[i]: %s\n", file_names[i]);
				printf("freq[i]: %d\n", ptr->freq[i]);
				if (ptr->freq[i] != 0) {
					size++;
					printf("size up\n");
				}
				i++;
			}
		}
		ptr = ptr->next;
	}
	printf("size: %d\n", size);

	printf("\nGetting FreqRecord...\n");
	FreqRecord* frarr = malloc((size + 1) * sizeof(FreqRecord));
	int index = 0;
	ptr = head;
	while (ptr != NULL) {
		printf("In word: %s\n", ptr->word);
		if (strcmp(word, ptr->word) == 0) {
			printf("Word matched\n");
			//Node contain word found
			int i = 0;
			while (file_names[i] != NULL) {
				printf("file_names[i]: %s\n", file_names[i]);
				printf("freq[i]: %d\n", ptr->freq[i]);
				if (ptr->freq[i] != 0) {
					printf("freq[i] != 0\n");
					frarr[index].freq = ptr->freq[i];
					strncpy(frarr[index].filename, file_names[i], PATHLENGTH);
					frarr[index].filename[PATHLENGTH - 1] = '\0';
					printf("FreqRecord %d: freq: %d, filename: %s\n", index, frarr[index].freq, frarr[index].filename);
					index++;
				}
				i++;
			}
		}
		ptr = ptr->next;
	}

	//还要对比MAXFILE和size的大小
	frarr[size].freq = 0; 
	frarr[size].filename[0] = '\0';
	

	printf("\nLoop Over FreqRecord List to check result\n"); 
	print_freq_records(frarr);
	printf("get_word finished\n");

    return frarr;
}

/* Print to standard output the frequency records for a word.
* Use this for your own testing and also for query.c
*/
void print_freq_records(FreqRecord *frp) {
    int i = 0;

    while (frp != NULL && frp[i].freq != 0) {
        printf("%d    %s\n", frp[i].freq, frp[i].filename);
        i++;
    }
    printf("print_freq_records finished\n");
}

/* Complete this function for Task 2 including writing a better comment.
*/

void run_worker(char *dirname, int in, int out) {
	printf("\nIn run_worker\n");
	printf("dirname: %s\n", dirname);
	
	char index_file[PATHLENGTH];
	strncpy(index_file, dirname, PATHLENGTH);
	strncat(index_file, "/index", PATHLENGTH - strlen(index_file));
    index_file[PATHLENGTH - 1] = '\0';
    printf("index_file: %s\n", index_file);

	char filenames_file[PATHLENGTH];
	strncpy(filenames_file, dirname, PATHLENGTH);
	strncat(filenames_file, "/filenames", PATHLENGTH - strlen(filenames_file));
    filenames_file[PATHLENGTH - 1] = '\0';
    printf("filenames_file: %s\n", filenames_file);

    Node* head;
    char **filenames = init_filenames();
    read_list(index_file, filenames_file, &head, filenames);
    printf("display_list: ");
    display_list(head, filenames);

    char word[MAXWORD];
    while (read(in, word, MAXWORD) > 0) {
    	strncpy(word, remove_punc(word), MAXWORD);
    	FreqRecord* frarr = get_word(word, head, filenames);
    	int i = 0;
    	while (frarr[i].freq != 0) {
    		if (write(out, &frarr[i], sizeof(FreqRecord)) == -1) {
    			perror("write to pipe");
    		}
    		//printf("\nWritting...\n");
    		i++;
    	}
    	write(out, &frarr[i], sizeof(FreqRecord));
    	printf("\nFinished writing for word %s\n", word);

    } 
}
