#define THRES 16

struct list{ 
	unsigned int port, ip, len, l;
	struct list *left, *right, *flink, *clink, *vnode, *dlink;
};
typedef struct list node;
typedef node *btrie;

struct trie{
    unsigned int port[255], link[255];
};
typedef struct trie trietab;

struct layer0s{
    unsigned short int ip;
    unsigned char len;
};
typedef struct layer0s layer0s;

struct seg{
    unsigned int n;
    layer0s * l0;
    btrie *array, def;
    trietab *root;
};
typedef struct seg element;

extern int N, num_node, segnum[65536], segnum2[65536];
extern btrie   root[256];
extern element *rootTable;
extern trietab tab[256];

btrie create_node();
void  create();
void  init8bit();
void  setting_fast();
void  setting_seg();
