#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "header.h"
#include "structure.h"
#include "timecount.h"

int N = 0; //number of nodes
int num_node = 0; //total number of nodes in the binary trie
int segnum[65536] = {0}, segnum2[65536] = {0};
btrie   root[256];
element *rootTable;
trietab tab[256];

btrie create_node() {
	btrie temp;
	num_node++;
	temp = (btrie) malloc (sizeof(node));
	temp->right = NULL;
	temp->left  = NULL;
    temp->flink = NULL;
    temp->clink = NULL;
    temp->dlink = NULL;
    temp->vnode = NULL;
    temp->l = 0;
	temp->port = 256; //default port
	return temp;
}

static void add_node(unsigned int ip, unsigned char len, unsigned char nexthop) {
	btrie ptr = root[ip >> 24];
	int i;
	for(i = 8; i < len; i++) {
		if(ip & (1 << (31 - i))) {
			if(ptr->right == NULL)
				ptr->right = create_node();
			ptr = ptr->right;
			if((i == len - 1) && (ptr->port == 256)) {
				ptr->port = nexthop;
                ptr->ip   = ip;
                ptr->len  = len;
                ptr->l    = 0;
            }
		}
		else {
			if(ptr->left == NULL)
				ptr->left = create_node();
			ptr = ptr->left;
			if((i == len - 1) && (ptr->port == 256)) {
				ptr->port = nexthop;
                ptr->ip   = ip;
                ptr->len  = len;
                ptr->l    = 0;
            }
		}
	}
}

void create() {
	int i;

    char s[] = "binary trie ...";
    printf("start setting structure ... \n    %-36s", s);

    for(i = 0; i < 256; i++)
	    root[i] = create_node();

	begin = rdtsc();
	for(i=0;i<num_entry;i++)
        //if(i % KKK != RAN || table[i].len < 16)
		add_node(table[i].ip, table[i].len, table[i].port);
	end=rdtsc();

    printf("finish\n");
}

static void one_leafpushing(btrie r) {
    if(r->port != 256){
        if(r->left != NULL && r->left->port != 256 && r->right != NULL && r->right->port != 256) {
            r->port = 256;
        }
        else if(r->left != NULL && r->left->port != 256) {
            if(r->right == NULL)
                r->right = create_node();
            r->right->port = r->port;
            r->right->len  = r->len + 1;
            r->right->ip   = (((r->ip >> (32 - r->len)) * 2) + 1) << (31 - r->len);
            r->port = 256;
        }
        else if(r->right != NULL && r->right->port != 256) {
            if(r->left == NULL)
                r->left = create_node();
            r->left->port = r->port;
            r->left->len  = r->len + 1;
            r->left->ip   = r->ip;
            r->port = 256;
        }
    }

    if(r->left != NULL)
        one_leafpushing(r->left);
    if(r->right != NULL)
        one_leafpushing(r->right);
}

static void plist(btrie r, btrie n) {
    if(r->port != 256 && r->len > 16) {
        r->flink = n;
        n = r;
    }
    
    if(r->left != NULL)
        plist(r->left, n);

    if(r->right != NULL)
        plist(r->right, n);
}

static void layer0(btrie r, int op) {
    if(op == 2 && r->port != 256 && r->len == 16) {
        rootTable[r->ip >> 16].def = r;
    }

    if(r->left == NULL && r->right == NULL && r->len > 16) {
        int i;

        if(op == 1) {
            r->l = 1;
            
            segnum[r->ip >> 16]++;
        }
        if(op == 2) {
            rootTable[r->ip >> 16].l0[segnum2[r->ip >> 16]].ip  = r->ip;
            rootTable[r->ip >> 16].l0[segnum2[r->ip >> 16]].len = r->len;
            rootTable[r->ip >> 16].array[segnum2[r->ip >> 16]++] = r;
        }
    }

    if(r->left != NULL)
        layer0(r->left, op);

    if(r->right != NULL)
        layer0(r->right, op);
}

void setting_seg() {
    int i, j, k, m, thres = THRES;

    char s[] = "building segment ...";
    printf("    %-36s", s);

    for(i = 0; i < 256; i++)
        layer0(root[i], 1);
    
    rootTable = (element *) malloc(65536 * sizeof(element ));

    for(i = 0; i < 65536; i++) {
            rootTable[i].n     = segnum[i];
            rootTable[i].root  = &tab[i / 256];
            rootTable[i].array = (btrie *) malloc((segnum[i] + 100) * sizeof(btrie));
            rootTable[i].l0    = (layer0s *) malloc((segnum[i] + 100) * sizeof(layer0s));
        }

    for(i = 0; i < 256; i++)
        layer0(root[i], 2);
    
    btrie t, tmp, down;
    int piece[500], n1, n2, max;
    for(i = 0; i < 65536; i++) {
        for(j = 0; j < segnum[i]; j += thres) {
            down = create_node();
            t    = rootTable[i].array[j]->flink;
            n1   = 1;
            n2   = 1;
            max  = 0;

            for(k = j + 1; k < j + thres && k < segnum[i]; k++) {
                if(rootTable[i].array[k]->flink != t) {
                    n1++;
                    if(n2 > max){
                        max = n2;
                        tmp = t;
                    }
                    t = rootTable[i].array[k]->flink;
                    n2 = 1;
                }   
                else {
                    n2++;
                }
            }

            if(n2 > max)
                tmp = t;
            down->flink = tmp;
            
            if(n1 == 1)
                down->l = 1;  

            for(k = j; k < j + thres && k < segnum[i]; k++) {
                if(n1 == 1 || rootTable[i].array[k]->flink == tmp) {
                    rootTable[i].array[k]->vnode = down;
                }
            }
        }
    }

    printf("finish\n\n");
}

static void setting8bit(btrie n, int i, int k, int link) {
    if(n->port != 256) {
        tab[i].port[k] = n->port;
        tab[i].link[k] = link;
        link = k;
    }

    if(n->left != NULL && k < 127)
        setting8bit(n->left, i, k * 2 + 1, link);

    if(n->right != NULL && k < 127)
        setting8bit(n->right, i, k * 2 + 2, link);
}

void init8bit() {
    int i, j;

    char s[] = "8-bits reverse trie ...";
    printf("    %-36s", s);

    for(i = 0; i < 256; i++) {
        for(j = 0; j < 255; j++) {
            tab[i].port[j] = 256;
            tab[i].link[j] = 0;
            setting8bit(root[i], i, 0, 0);
        }
    }

    printf("finish\n");
}

void setting_fast() {
    int i;
    
    char s1[] = "one-level leaf pushing ...";
    printf("    %-36s", s1);
    for(i = 0; i < 256; i++) {
        one_leafpushing(root[i]);
    }
    printf("finish\n");

    char s2[] = "fast link ...";
    printf("    %-36s", s2);
    for(i = 0; i < 256; i++) {
        plist(root[i], NULL);
    }
    printf("finish\n");
}
