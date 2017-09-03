struct ENTRY{
	unsigned int ip;
	unsigned char len;
	unsigned char port;
};

extern int num_entry, num_query;
extern unsigned int *query;
extern struct ENTRY *table;

void set_table(char *);
void set_query(char *);
