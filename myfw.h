#ifndef MYFW_H
#define MYFW_H

#define MAXLINE 101


typedef struct Node {
   char word[MAXLINE];
   int freq;
   struct Node *next;
} Node;

typedef struct List {
   struct Node *head;
   int length;
   int least;
} List;

typedef struct Hashtable {
   char key;
   struct Node *head;
   int total;
} Hashtable;



/*read words from stdin, return head*/
void read_std();

/*read words from multiple files, return head*/
void read_files(int, const char **);

/*list traversal, increment or add, give word, head, and length*/
void insert(char *);

List *n_sert(List *,int, int, char *);
/*just the head, sorted into new list*/
List *sort(List *, int);

int hash(char);

void printHT();

void printList(Node *, int);

/*shorten list to either given n, or 10*/
void cut(Node *, int, int);

#endif
