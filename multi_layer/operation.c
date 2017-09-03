#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "header.h"
#include "structure.h"
#include "timecount.h"

unsigned int B[10000];
btrie A[1000];
int access;

static int search(unsigned int ip) {
    int idx = ip >> 16, mid, i, c = 0;
    element *table = &rootTable[idx];
    int idxl = 0, idxr = table->n - 1, port = 256;
    layer0s *a = table->l0;
    btrie *b = table->array;
    access++;
    unsigned short int ip2 = ip;

    if(idxr != -1) { 
        while(idxl <= idxr) {
            //access++;
            mid = (idxl + idxr) / 2;
            /*if(B[mid] == 0) {
                access++;
                B[mid] = 1;
                B[mid + 1] = 1;
                B[mid + 2] = 1;
                B[mid + 3] = 1;
            }*/

            if(a[mid].ip >> (32 - a[mid].len) == ip2 >> (32 - a[mid].len)) {
                port = b[mid]->port;
                access++;
                return port;
            }
            else if(ip2 < a[mid].ip) {
                if(idxl > mid - 1)
                    break;
                idxr = mid - 1;
            }
            else {
                if(mid + 1 > idxr)
                    break;
                idxl = mid + 1;
            }
        }

        btrie x;
        access++;
        if(b[idxl] != NULL) {
            x = b[idxl]->flink;

            while(x != NULL) {
                access++;
                if(x->ip >> (32 - x->len) == ip >> (32 - x->len)) {
                    port = x->port;
                    return port;
                }

                x = x->flink;
            }
        }

        access++;
        if(b[idxr] != NULL) {
            x = b[idxr]->flink;

            while(x != NULL) {
                access++;
                if(x->ip >> (32 - x->len) == ip >> (32 - x->len)) {
                    port = x->port;
                    return port;
                }

                x = x->flink;
            }
        }
    }

    trietab *t = rootTable[ip >> 16].root;
    access++;

    idx = (ip << 8) >> 24;
    while(idx != 0) {
        access++;
        if(t->port[idx] != 256)
            return t->port[idx];
        idx = t->link[idx];
    }

    return t->port[idx];
}

static void insertL0(btrie n){
    int idx = n->ip >> 16, mid, i, c = 0;
    element *table = &rootTable[idx];
    int idxl = 0, idxr = table->n - 1;
    btrie *a = table->array;
    access += 2;

    if(idxr == -1){
        a[0] = n;
        table->n++;
        return;
    }

    while(idxl <= idxr){
        access++;
        mid = (idxl + idxr) / 2;

        if(a[mid]->len > n->len && a[mid]->ip >> (32 - n->len) == n->ip >> (32 - n->len)){
            c = 1; // n cover a[mid]
            break;
        }
        else if(a[mid]->len < n->len && a[mid]->ip >> (32 - a[mid]->len) == n->ip >> (32 - a[mid]->len)){
            c = 2; // a[mid] cover n
            break;
        }
        else if(n->ip < a[mid]->ip){
            idxr = mid - 1;
        }
        else{
            idxl = mid + 1;
        }
    }

    if(idxl > idxr){
        if(idxl == table->n){
            access++;
            a[idxl] = n;
            table->n++;
            return;
        }

        if(mid == table->n - 1){
            access++;
            a[mid + 1] = n;
            table->n++;
            return;
        }

        if(a[mid]->dlink == NULL){
            access++;
            a[mid]->dlink = n;
        }
        else{
            if(a[mid + 1]->dlink == NULL){
                access++;
                a[mid + 1]->dlink = n;
            }
            else
                printf("it's full!\n");
        }
    }
    else{
        if(c == 1){
            unsigned int ipr = (((n->ip >> (32 - n->len)) + 1) << (32 - n->len)) - 1;
            int endl = mid, endr;

            idxl = mid;
            idxr = table->n - 1;
            while(idxl <= idxr){
                access++;
                mid = (idxl + idxr) / 2;

                if(ipr < a[mid]->ip){
                    idxr = mid - 1;
                }
                else{
                    idxl = mid + 1;
                }
            }
            endr = mid;

            for(i = 0; i <= endr - endl; i++){
                access++;
                A[i] = a[endl + i];
            }

            for(i = 0; i <= endr - endl; i++){
                while(A[i]->flink != NULL && n->len < A[i]->flink->len)
                    A[i] = A[i]->flink;
            }

            for(i = 0; i <= endr - endl; i++){
                if(A[i] != NULL){
                    n->flink = A[i]->flink;
                    A[i]->flink = n;
                }
            }
        }
        else{
            if(a[mid]->clink == NULL){
                access++;
                a[mid]->clink = n;
            }
            else if(a[mid]->clink->len < n->len){
                a[mid]->clink->clink;
            }
            else{
                n->clink = a[mid]->clink;
                a[mid]->clink = n;
            }
        }
    }

    return;
}

static void insert(unsigned int ip, int len, int port){
	btrie current = create_node();
    access++;

    current->port = port;
    current->ip = ip;
    current->len = len;
    //current->p = q;

    if(len > 16)
        insertL0(current);
    else if(len == 16){
        rootTable[ip >> 16].def = current;
        access++;
    }
    else{
        int i, k = 0, link;
        trietab *t = rootTable[ip >> 16].root;
        access++;

	    for(i = 8; i < len; i++){
            if(t->port[k] != 256)
                link = k;

		    if(ip & (1 << (31 - i)))
                k = k * 2 + 1;
            else
                k = k * 2 + 2;
        }

        t->port[k] = port;
        t->link[k] = link;

        if(k < 127)
            for(i = k * 2 + 1; i < 255; i++)
                if(t->link[i] == link)
                    t->link[i] = k;
    }
}

static void deleteL0(btrie n){
    int idx = n->ip >> 16, mid, i, c = 0;
    element *table = &rootTable[idx];
    int idxl = 0, idxr = table->n - 1;
    btrie *a = table->array;
    access += 2;

    if(idxr == -1){
        return;
    }

    while(idxl <= idxr){
        access++;
        mid = (idxl + idxr) / 2;

        if(a[mid]->len == n->len && a[mid]->ip == n->ip){
            c = 1; // n is layer 0
            break;
        }
        else if(a[mid]->len > n->len && a[mid]->ip >> (32 - n->len) == n->ip >> (32 - n->len)){
            c = 2; // n is a[mid] fastlink
            break;
        }
        else if(n->ip < a[mid]->ip){
            idxr = mid - 1;
        }
        else{
            idxl = mid + 1;
        }
    }

    if(idxl > idxr){
        if(a[mid]->dlink != NULL && a[mid]->dlink->ip == n->ip){
            access++;
            a[mid]->dlink = NULL;
            return;
        }

        if(a[mid + 1] != NULL && a[mid + 1]->dlink != NULL && a[mid + 1]->dlink->ip == n->ip){
            access++;
            a[mid + 1]->dlink = NULL;
        }
    }
    else{
        if(c == 2){
            unsigned int ipr = (((n->ip >> (32 - n->len)) + 1) << (32 - n->len)) - 1;
            int endl = mid, endr;

            idxl = mid;
            idxr = table->n - 1;
            while(idxl <= idxr){
                access++;
                mid = (idxl + idxr) / 2;

                if(ipr < a[mid]->ip){
                    idxr = mid - 1;
                }
                else{
                    idxl = mid + 1;
                }
            }
            endr = mid;

            for(i = 0; i <= endr - endl; i++){
                access++;
                A[i] = a[endl + i];
            }

            for(i = 0; i <= endr - endl; i++){
                while(A[i]->flink != NULL && A[i]->flink != n)
                    A[i] = A[i]->flink;
                if(A[i]->flink == n){
                    A[i]->flink = A[i]->flink->flink;
                }
            }
        }
        else{
            a[mid]->port = 256;
        }
    }

    return;
}

static void delete(unsigned int ip, int len, int port){
	btrie current = create_node();
    access++;

    current->port = port;
    current->ip = ip;
    current->len = len;
    //current->p = q;

    if(len > 16)
        deleteL0(current);
    else if(len == 16 && rootTable[ip >> 16].def != NULL){
        rootTable[ip >> 16].def = NULL;
        access++;
    }
    else{
        int i, k = 0, link;
        trietab *t = rootTable[ip >> 16].root;
        access++;

	    for(i = 8; i < len; i++){
            if(t->port[k] != 256)
                link = k;

		    if(ip & (1 << (31 - i)))
                k = k * 2 + 1;
            else
                k = k * 2 + 2;
        }

        t->port[k] = 256;

        if(k < 127)
            for(i = k * 2 + 1; i < 255; i++)
                if(t->link[i] == k)
                    t->link[i] = link;
    }
}

int search_process() {
    unsigned int ran, rant;
    int i, j;

    char s1[] = "random input header ...";
    printf("%-40s", s1);
    fflush(stdout);

    srand(time(NULL));

    for(i = 0; i < num_query; i++){
        ran = rand() % num_query;
        rant = query[i];
        query[i] = query[ran];
        query[ran] = rant;
    }

    printf("finish\n");

    char s2[] = "start search process ...";
    printf("%-40s", s2);
    fflush(stdout);

    float cc = 0;
    int qwe[50] = {0}, k;
	for(j = 0; j < 100; j++) {
		for(i = 0; i < num_query; i++) {
            //printf("%6d: %u\n", i, query[i]);
            access = 0;
            //for(k = 0; k < 10000; k++)
                //B[k] = 0;
			begin = rdtsc();
			search(query[i]);
			end = rdtsc();
			if(clock[i] > (end - begin))
				clock[i] = (end - begin);
            if(j == 0){
                cc += access;
                if(access < 50)
                    qwe[access]++;
                else
                    qwe[49]++;
            }
		}
	}

    printf("finish\n\n");

	total = 0;
	for(j = 0; j < num_query; j++)
		total += clock[j];
	printf("Avg. Search: %llu\n", total / num_query);
    //printf("Avg. memory access%f\n", cc / num_query);
	
    CountClock();
	
	/*for(i = 0; i < 50; i++) {
		printf("%d\n", NumCntClock[i]);
	}
    printf("\n");*/

    //for(i = 0; i < 50; i++)
        //printf("%d\n", qwe[i]);
}
