#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"

int test2(void) {

	List *head = create_node(3);
	head = add_node(head, 4);
	head = remove_node(head, 3);
	char *result = tostring(head);
	if(strcmp(result, "4") != 0) {
		printf("Test2 failed: expected 4 got %s\n", result);
		free(result);
		free_list(head);
		return 0;
	} else {
		printf("Test2 succeeded\n");
		free(result);
		free_list(head);
		return 1;
	}
}
