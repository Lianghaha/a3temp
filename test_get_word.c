#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freq_list.h"
#include "worker.h"

FreqRecord *get_word(char *word, Node *head, char **file_names) {
	printf("\nIn get_word\n");
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

	frarr[size].freq = 0; 
	frarr[size].filename[0] = '\0';
	

	printf("\nLoop Over FreqRecord List to check result\n"); 
	print_freq_records(frarr);

    return NULL;
}

int main(int argc, char **argv) {
	char **filenames = init_filenames();
    filenames[0] = "Lianghaha";
    filenames[1] = "haha";
    filenames[2] = "hoho.mp3";

	Node* first = create_node("Liang", 5, 0);  
    Node* second = create_node("heihei", 5, 0);
    Node* third = create_node("haohao", 5, 0);
    Node* head = add_word(first, filenames, "A", "hoho.mp3");
    head = add_word(head, filenames, "Liang", "hoho.mp3");

    first->next = second;
    second->next = third;

    get_word("Liang", head, filenames);

    printf("\nDisplay_list: \n");
    display_list(head, filenames);

	return 0;
}