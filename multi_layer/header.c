#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "header.h"
#include "timecount.h"

int num_entry, num_query;
unsigned int *query;
struct ENTRY *table;

static void read_table(char *str, unsigned int *ip, int *len, unsigned int *nexthop) {
	char tok[] = "./";
	char buf[100], *str1;
	unsigned int n[4];
	sprintf(buf, "%s\0", strtok(str, tok));
	n[0] = atoi(buf);
	sprintf(buf, "%s\0", strtok(NULL, tok));
	n[1] = atoi(buf);
	sprintf(buf, "%s\0", strtok(NULL, tok));
	n[2] = atoi(buf);
	sprintf(buf, "%s\0", strtok(NULL, tok));
	n[3] = atoi(buf);
	*nexthop = n[2];
	str1 = (char *) strtok(NULL, tok);
	if(str1 != NULL) {
		sprintf(buf, "%s\0", str1);
		*len = atoi(buf);
	}
	else{
		if(n[1] == 0 && n[2] == 0 && n[3] == 0)
			*len = 8;
		else
			if(n[2] == 0 && n[3] == 0)
				*len = 16;
			else
				if(n[3] == 0)
					*len = 24;
	}
	*ip = n[0];
	*ip <<= 8;
	*ip += n[1];
	*ip <<= 8;
	*ip += n[2];
	*ip <<= 8;
	*ip += n[3];
}

void set_table(char *file_name) {
	FILE *fp;
	int len;
	char string[100];
	unsigned int ip, nexthop;

	fp = fopen(file_name, "r");
	while(fgets(string, 50, fp) != NULL) {
		//read_table(string,&ip,&len,&nexthop);
		num_entry++;
	}
	rewind(fp);
	table = (struct ENTRY *) malloc (num_entry * sizeof(struct ENTRY));
	num_entry = 0;
	while(fgets(string, 50, fp) != NULL){
		read_table(string, &ip, &len, &nexthop);
		table[num_entry].ip    = ip;
		table[num_entry].port  = nexthop;
		table[num_entry++].len = len;
	}

    printf("finish\n\n");
}

void set_query(char *file_name) {
	FILE *fp;
	int len;
	char string[100];
	unsigned int ip, nexthop;

    char s[] = "reading table ...";
    printf("%-40s", s);
    fflush(stdout);

	fp=fopen(file_name, "r");
	while(fgets(string, 50, fp) != NULL) {
		num_query++;
	}
	rewind(fp);
	query = (unsigned int *) malloc (num_query*sizeof(unsigned int));
	clock = (unsigned long long int *) malloc (num_query*sizeof(unsigned long long int));
	num_query = 0;
	while(fgets(string, 50, fp) != NULL) {
		read_table(string, &ip, &len, &nexthop);
		query[num_query]   = ip;
		clock[num_query++] = 10000000;
	}
}
