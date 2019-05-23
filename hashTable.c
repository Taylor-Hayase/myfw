#include <limits.h>
#include <assert.h>
#include "hashTable.h"
#include <stdio.h>

typedef struct node
{
   HTEntry *ent;
   struct node *next;
}Node;

typedef struct
{
   Node **table;
   HTFunctions *funcs;
   int uniqueEnt, totalEnt, numSizes, sizeIndex;
   unsigned *sizes;
   float loadFactor;
}HashTable;

void checkError(void* stuff)
{
   if (stuff == NULL)
   {
      perror("");
      exit(EXIT_FAILURE);
   }
}

void* htCreate(
   HTFunctions *functions,
   unsigned sizes[],
   int numSizes,
   float rehashLoadFactor)
{
   int i;
   HashTable *ht;
   assert(numSizes >= 1);
   assert(sizes[0] > 0);
   for (i = 1;i < numSizes; ++i)
      assert(sizes[i-1] < sizes[i]);
   assert(rehashLoadFactor > 0.0 && rehashLoadFactor <= 1.0);

   checkError(ht = (HashTable*)malloc(sizeof(HashTable)));
   ht->sizeIndex = 0;
   checkError(ht->table = (Node**)calloc(sizes[ht->sizeIndex],sizeof(Node*)));
   checkError(ht->sizes = (unsigned*)malloc(sizeof(unsigned)*numSizes));
   checkError(ht->funcs = (HTFunctions*)malloc(sizeof(HTFunctions)));

   ht->funcs->hash = functions->hash;
   ht->funcs->compare = functions->compare;
   ht->funcs->destroy = functions->destroy;
   ht->uniqueEnt = 0;
   ht->totalEnt = 0;

   for (i = 0; i < numSizes; ++i)
      ht->sizes[i] = sizes[i];
   ht->numSizes = numSizes;
   ht->loadFactor = rehashLoadFactor;

   return ht;
}

void htDestroyNodes(Node *n, HashTable *ht)/*in linked list*/
{
   Node *temp;
   while( n != NULL )
   {
      if (ht->funcs->destroy != NULL)
         ht->funcs->destroy(n->ent->data);
      free(n->ent->data);
      free(n->ent);
      temp = n->next;
      free(n);
      n = temp;
   }
}

void htDestroy(void *hashTable)
{
   HashTable* ht = (HashTable*)hashTable;
   Node **table = ht->table;
   int i;
   for (i=0; i < htCapacity(ht); i++)
      if( (table+i) != NULL )/*check each node pointer in table*/
         htDestroyNodes(table[i],ht);  
   free(table);
   free(ht->sizes);
   free(ht->funcs);
   free(ht);
}

void rehashEachNode( Node *n, Node** tempTable, unsigned hashVal)
{
   Node *temp, *prev;
   int notfirst = 0;
   temp = tempTable[hashVal];
   while (temp != NULL)
   {
      notfirst = 1;
      prev = temp;
      temp = temp->next;
   }
   n->next = NULL;
   if (notfirst)
      prev->next = n;
   else
      tempTable[hashVal] = n;
}

void rehashLinkedList(Node *n, Node **tempTable, HashTable *ht)
{
   Node *temp;
   unsigned hashVal;
   while (n != NULL)
   {
      hashVal = ht->funcs->hash(n->ent->data) % *(ht->sizes+(ht->sizeIndex));
      temp = n->next;
      rehashEachNode(n, tempTable, hashVal);    
      n = temp;
   }
}

void rehashCheck(HashTable *ht)
{  
   Node **tempTable;
   int i;
   unsigned ogCapacity = htCapacity(ht);
   if ((ht->loadFactor) != 1.0 && ((ht->sizeIndex) < (ht->numSizes)-1)
      && ((((float)htUniqueEntries(ht))/ogCapacity) > (ht->loadFactor))) 
   {
      (ht->sizeIndex)++;
      checkError(tempTable = (Node**)calloc(htCapacity(ht), sizeof(Node*)));

      for (i=0; i < ogCapacity; ++i)
         rehashLinkedList(ht->table[i], tempTable, ht);

      free(ht->table);
      ht->table = tempTable;
   }
}

Node* newNodeEnt(HashTable *ht, void *data)
{
   HTEntry *entry;
   Node *n;
   (ht->uniqueEnt)++;
   checkError(n = (Node*)malloc(sizeof(Node)));
   checkError(entry = (HTEntry*)malloc(sizeof(HTEntry)));
   entry->data = data;
   entry->frequency = 1;
   n->ent = entry;
   n->next = NULL;
   return n;
}

unsigned htAdd(void *hashTable, void *data)
{
   int notfirst = 0;
   unsigned hashVal;
   HashTable* ht = (HashTable*)hashTable;
   Node *n, *prev;
   assert(data != NULL);

   rehashCheck(ht);
   
   hashVal = (ht->funcs->hash(data)) % (ht->sizes[ht->sizeIndex]);
   n = ht->table[hashVal]; 
   (ht->totalEnt)++; 

   while (n != NULL)
   {
      notfirst = 1;
      if (ht->funcs->compare(n->ent->data, data) == 0)
         return ++(n->ent->frequency);
      prev = n;
      n = n->next;
   }
   if (notfirst)
      prev->next = newNodeEnt(ht, data);
   else
      ht->table[hashVal] = newNodeEnt(ht, data);

   return 1;
}

void checkList(Node *n, HashTable *ht, HTEntry *entry, void *data)
{
   while (n != NULL )
   {
      if (ht->funcs->compare(n->ent->data, data) == 0)
      {
         entry->data = n->ent->data;
         entry->frequency = n->ent->frequency;
         return;
      }
      n = n->next;
   }
}

HTEntry htLookUp(void *hashTable, void *data)
{
   HashTable* ht = (HashTable*)hashTable;
   Node *n;
   HTEntry entry;
   assert(data != NULL);
   entry.data = NULL;
   entry.frequency = 0;
   n = ht->table[ht->funcs->hash(data) % (ht->sizes[ht->sizeIndex])];
   checkList(n, ht, &entry, data);
   return entry;
}

void fillArr(HTEntry *entryArr, Node *n, int *entIndex)
{
   while (n != NULL)
   {
      entryArr[*entIndex].data = n->ent->data;
      entryArr[(*entIndex)++].frequency = n->ent->frequency;
      n = n->next;
   }
}

HTEntry* htToArray(void *hashTable, unsigned *size)
{
   HTEntry *entryArr;
   int entIndex = 0, i;
   HashTable *ht = (HashTable*)hashTable;
   *size = htUniqueEntries(ht);
   if (*size == 0)
      return NULL;
   checkError(entryArr = (HTEntry*)calloc((*size),sizeof(HTEntry)));
   for(i = 0; i < htCapacity(ht) ; ++i)
      if (ht->table[i] != NULL)
         fillArr(entryArr, ht->table[i], &entIndex);
   return entryArr;
}

unsigned htCapacity(void *hashTable)
{
   return ((HashTable*)hashTable)->sizes[((HashTable*)hashTable)->sizeIndex];
}

unsigned htUniqueEntries(void *hashTable)
{
   return ((HashTable*)hashTable)->uniqueEnt;
}

unsigned htTotalEntries(void *hashTable)
{
   return ((HashTable*)hashTable)->totalEnt;
}

void sumChains(int *maxChecker, int *sum, Node *n)
{
   while (n != NULL)
   {
      (*sum)++;
      (*maxChecker)++;
      n = n->next;
   }
}

void metricsCalc(Node *n, int *chain, int *max, int *maxChecker, int *sum)
{
   if (n != NULL)
   {
      (*chain)++;
      *maxChecker = 0;
      sumChains(maxChecker, sum, n);
      if (*maxChecker > *max)
         *max = *maxChecker;
   }
}

HTMetrics htMetrics(void *hashTable)
{
   HTMetrics metrics;
   HashTable *ht = (HashTable*)hashTable;
   int chain=0, i=0, max = 0, maxChecker = 0, sum = 0;
   for(;i<htCapacity(ht);++i)
      metricsCalc( ht->table[i], &chain, &max, &maxChecker, &sum);
   metrics.numberOfChains = chain;   
   metrics.maxChainLength = max;
   if (chain == 0)
      metrics.avgChainLength = 0;
   else
      metrics.avgChainLength = ((float)sum/chain);

   return metrics;
}
