#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include "freq_list.h"
#include "worker.h"

char *remove_punc(char *);

void add_sort_records(FreqRecord** frarr_ptr, FreqRecord* fr_ptr, int* num_record_ptr) {
    /*
    int count = 0;
    if (*num_record_ptr < MAXRECORDS) {
        count = *num_record_ptr;
    } else {
        count = MAXRECORDS;
    }
    */

    // find the index to insert
    int i = 0;
    while (i < *num_record_ptr)
    {
        if ((*fr_ptr).freq <= (*frarr_ptr)[i].freq) {
            i++;
        }else {
            break;
        }
    }
    //printf("index is: %d\n", i);
    //printf("num_record: %d\n", *num_record_ptr);
    //printf("count: %d\n", count);

    if (i == *num_record_ptr) {
        (*frarr_ptr)[i] = *fr_ptr;
        //printf("added new fr to position: %d\n", i);
    }
    else if (i < *num_record_ptr) {
        FreqRecord temp_curr = (*frarr_ptr)[i];
        FreqRecord temp_next;
        (*frarr_ptr)[i] = *fr_ptr;
        while (i < *num_record_ptr) {
            i++;
            temp_next = (*frarr_ptr)[i];
            (*frarr_ptr)[i] = temp_curr;
            temp_curr = temp_next;
        }
    }
    if (*num_record_ptr < MAXRECORDS) {
        (*num_record_ptr)++;
    }
}

/* Complete this function for Task 1. Including fixing this comment.
*/
FreqRecord *get_word(char *word, Node *head, char **file_names) {
	//printf("\nIn get_word\n");
	//printf("Word: %s\n", word);
	//printf("Getting size...\n");
	int size = 0;
	Node* ptr = head;
	while (ptr != NULL) {
		//printf("In word: %s\n", ptr->word);
		if (strcmp(word, ptr->word) == 0) {
			//printf("Word matched\n");
			//Node contain word found
			int i = 0;
			while (file_names[i] != NULL) {
				if (ptr->freq[i] != 0) {
					size++;
					//printf("file_names[i]: %s\n", file_names[i]);
					//printf("freq[i]: %d\n", ptr->freq[i]);
					//printf("size up\n");
				}
				i++;
			}
		}
		ptr = ptr->next;
	}
	//printf("size: %d\n", size);

	//printf("\nGenerating FreqRecord...\n");
	FreqRecord* frarr = malloc((size + 1) * sizeof(FreqRecord));
	int index = 0;
	ptr = head;
	while (ptr != NULL) {
		//printf("In word: %s\n", ptr->word);
		if (strcmp(word, ptr->word) == 0) {
			//printf("Word matched\n");
			//Node contain word found
			int i = 0;
			while (file_names[i] != NULL) {
				if (ptr->freq[i] != 0) {
					//printf("file_names[i]: %s\n", file_names[i]);
					//printf("freq[i]: %d\n", ptr->freq[i]);
					frarr[index].freq = ptr->freq[i];
					strncpy(frarr[index].filename, file_names[i], PATHLENGTH);
					frarr[index].filename[PATHLENGTH - 1] = '\0';
					//printf("FreqRecord %d: freq: %d, filename: %s\n", index, frarr[index].freq, frarr[index].filename);
					index++;
				}
				i++;
			}
		}
		ptr = ptr->next;
	}

	frarr[size].freq = 0; 
	frarr[size].filename[0] = '\0';
	

	//printf("\nLoop Over FreqRecord List to check result\n"); 
	//print_freq_records(frarr);
	//printf("get_word finished\n");

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
}

/* Complete this function for Task 2 including writing a better comment.
*/

void run_worker(char *dirname, int in, int out) {
	//printf("In run_worker\n");
	//printf("dirname: %s\n", dirname);
	
	char index_file[PATHLENGTH];
	strncpy(index_file, dirname, PATHLENGTH);
	strncat(index_file, "/index", PATHLENGTH - strlen(index_file));
    index_file[PATHLENGTH - 1] = '\0';

	char filenames_file[PATHLENGTH];
	strncpy(filenames_file, dirname, PATHLENGTH);
	strncat(filenames_file, "/filenames", PATHLENGTH - strlen(filenames_file));
    filenames_file[PATHLENGTH - 1] = '\0';

    Node* head;
    char **filenames = init_filenames();
    read_list(index_file, filenames_file, &head, filenames);
    //printf("display_list: \n");
    //display_list(head, filenames);

    char word[MAXWORD];
    while (read(in, word, MAXWORD) > 0) {
    	strncpy(word, remove_punc(word), MAXWORD);
    	FreqRecord* frarr = get_word(word, head, filenames);
    	int i = 0;
    	while (frarr[i].freq != 0) {
    		//printf("\nwriting into pipe: %d\n", i);
    		if (write(out, &frarr[i], sizeof(FreqRecord)) == -1) {
    			perror("write to pipe");
    		}
    		//printf("wrote to pipe: %d    %s\n", frarr[i].freq, frarr[i].filename);
    		i++;
    	}
    	write(out, &frarr[i], sizeof(FreqRecord));
    	free(frarr);
    	//printf("\nFinished writing for word %s\n", word);
    } 
    //printf("run_worker done\n");
}
