/* Coding done by Taylor Hayase and Ryan Premi */

#include "myfw.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


Hashtable htab[26];

int main(int argc, char const *argv[]){

	int cutoff_length = 10;
	List *lst;
	int use_std = 1;
	int start_file = 0;
	int j;
	Node *check;
   Node *temp;
	FILE *file;
   int total_words;

   /*Setup hashtable*/
	for(j = 0; j < 26; j++)
   {
		htab[j].key = (char)(j + 97);
		htab[j].total = 0;
		htab[j].head = NULL;

	}

   /*Error Handling*/
	if(argc != 1)
   {
	   if(argc == 2)
      {
			use_std = 0;
			start_file = 1;
		}
      else if((argv[1][0] == '-' && strcmp(argv[1], "-n") != 0) || (atoi(argv[2]) <= 0 && strcmp(argv[1], "-n") == 0))
      {
         printf("Usage: fw [-n] <num> [files]\n");
         return 0;
      }
      else if(argv[1][0] == '-' && strcmp(argv[1], "-n") == 0)
      {
         cutoff_length = atoi(argv[2]);

         if(argc > 3)
         {
            use_std = 0;
            start_file = 3;
         }

      }
      else
      {
         use_std = 0;
         start_file= 1;
      }
	}
	else
   { 	
	   cutoff_length = 10;
	}
/*Call the proper function and get unsorted List*/

   /*read from stdin*/
	if(use_std == 1)
   {
		read_std();
	}
   /*read from file*/
	else
   {
	   for (j = start_file; j < argc; j++)
      {
			file = fopen(argv[j] , "r");

			if(file == NULL)
         {
				printf("Usage: fw [-n] <num> [files]\n");
				return 0;
			}
			fclose(file);
		}
		read_files((argc - start_file), (argv + start_file));
	}
   /*Setup the list*/
	lst = (List *)malloc(sizeof(List));
	lst -> length = 0; 
	lst -> least = 0;
	lst -> head = NULL;

   /*store list of sorted words and their frequency*/
	lst = sort(lst, cutoff_length);

   /*caclulate how many words*/
   total_words = 0;
   for (j = 0; j < 26; j++)
   {
      total_words += htab[j].total;
   }

   /*Print out finished output*/
	printf("The top %d words (out of %d) are: \n", cutoff_length, total_words);

	check = lst -> head;

   /*print and free sorted list*/
   j = 0;
	while((check != NULL || (check == lst -> head && check != NULL)))
   {
      if(j < cutoff_length)
      {
		   printf("%d %s\n", check -> freq, check -> word);
      }
      temp = check;
		check = check->next;
      free(temp);
      j++;
	}
   /*finally free the list*/
   free(lst);
	return 0;
}

/*hash function, convert a-z into indexes*/
int hash(char f_let)
{
	return (int)f_let - 97;
}

/*insert into hashtable
 * words mapped by frist letter
 * if word in hashtable add to frequency
 * otherwise add to end of linked list to hashtable letter*/
void insert(char *word)
{

   Node *new;
   Node *ptr;
   Node *prev;
   Node *temp;
   int total = 0;

   ptr = htab[hash(word[0])].head;

   /*If no current head, make one*/
   if (htab[hash(word[0])].head == NULL)
   {

      new = (Node *)malloc(sizeof(Node));

      strcpy(new -> word, word);
      new -> freq = 1;
      new -> next = NULL;

      htab[hash(word[0])].head = new;
      total += 1;
      htab[hash(word[0])].total = 1; 
      return;
   }

   prev = htab[hash(word[0])].head;
   /*look until either find word, or reach end of list*/
   while (ptr != NULL || ptr == htab[hash(word[0])].head)
   {
   	/*if found word, increment*/
      if (strcmp(ptr -> word, word) == 0)
      {
         ptr -> freq += 1;
         /*currently at the head*/
         if (prev == ptr)
         {
            return;
         }
         /*higher frequency or ascii code goes first*/
         else if ( (prev->freq < ptr->freq) || (prev->freq == ptr->freq && strcmp(prev->word, ptr->word)<0))
         {
            temp = prev;
            prev = ptr;
            ptr = temp;
         }
         /*else just return as is*/ 
         return;
      }
      /*otherwise at tail, need to add new word*/
      if (ptr->next == NULL) 
      {
         break;
      }
      prev = ptr;
      ptr = ptr -> next;
   }
     
   /*create a new tail node*/
   new = (Node *)malloc(sizeof(Node));

   strcpy(new -> word, word);
   new -> freq = 1;
   new -> next = NULL;

   ptr -> next = new;

   htab[hash(word[0])].total += 1; 

}

/*Decides if word whould be in final list*/
List *n_sert(List *lst, int c_off, int freq, char *word)
{

   Node *ptr;
   Node *prev = NULL;
   Node *new;
   int incre;
   int found;
   int counter = 0;

   ptr = lst -> head;

   /*if no list, make a head*/
   if (ptr == NULL)
   {
      new = (Node *)malloc(sizeof(Node));
      
      strcpy(new -> word, word);
      new -> freq = freq;
      new -> next = NULL;

      lst -> head = new;

      lst -> length = 1;
      lst -> least = freq;

      return lst;
   }

   /*If the list is already full, and frequency doesn't meet minimum*/
   if( (lst -> least > freq) && (lst -> length == c_off ))
   {
      return lst ;
   }
   found = 0;
   
   /*go through linked list at each letter*/
   while ( (ptr != NULL) || (ptr == lst->head) || (ptr -> freq > lst -> least) )
   {
      incre = 0;

      /*prevent unsesseary inserts*/
      if (lst -> length == c_off && counter == c_off - 1)
      {
         break;
      }

      /*the the new node 
 *    has a greater frequency than the current 
 *    or has a higher ascii value
 *    then it deserves to be put into list in front of current*/
   	if (!found && (ptr->freq < freq || (ptr->freq == freq && strcmp(ptr->word, word)<0)))
      {
      	new = (Node *)malloc(sizeof(Node));
     		new -> freq = freq;
      	strcpy(new -> word, word);


      	if(incre + 1 < c_off )
         {
      		if(lst->length < c_off)
            {
      		   lst->length += 1;
            }

            /*create a new head, place in front of current head*/
      		if(prev == NULL)
            {
               prev = lst -> head;
      			new -> next = prev;
      			lst -> head = new;
               found = 1;
      		}
            /*otherwise place between current and previous*/
            else
            {
      			new -> next = prev -> next;
					prev -> next = new;
               found = 1;
      		}	
         }
         /*else put at the end*/
      	else
         {
            ptr -> next = new;
      		new -> next = NULL;
            found = 1;
         }
      }

      /*if the list is too long, cut it off*/
      if (lst -> length == c_off && incre == c_off)
      {
      	ptr -> next= NULL;
   	}

      /*if the new smallest frequency is less than the current one, replace it*/
      if (ptr -> freq < lst -> least)
      {
         lst -> least = ptr->freq;
      }

      /*if there isn't a next node in list, break*/
   	if (ptr -> next == NULL)
      {
         /*if list isn't at cutoff length, add it anyways*/
         if (lst -> length < c_off && found == 0)
         {
            new = (Node *)malloc(sizeof(Node));

            strcpy(new -> word, word);
            new -> freq = freq;
            new -> next = NULL;

            ptr -> next = new;
            lst -> least = freq;
         }
         break;
      }
      
      /*go to the next pointer on list*/
   	incre += 1;
   	prev = ptr;
      counter += 1;
   	ptr = ptr -> next;
   }
   /*return list constructed by function*/
   return lst;
}

/*Make a list of most frequent by looking though entire hashtable*/
List *sort(List *lst, int c_off)
{
	int i;
	Node *ptr;
	Node *temp;

	for (i = 0; i < 26; i++)
   {
		ptr = htab[i].head;
		
		while( ptr != NULL || (ptr == htab[i].head && ptr!= NULL))
		{
		   lst = n_sert( lst, c_off, ptr->freq, ptr->word);
		   temp = ptr -> next;
		   free(ptr);

		   if( temp == NULL) 
         {
            break;
         }
		   ptr = temp;
		}
	}
	return lst;
}

/*read in words from stdin*/
void read_std()
{
	char word[101];
	char c;
	int curr=0;

	for (c = getchar(); c != EOF;  c =getchar())
   {
      /*check if letter*/
		if (isalpha(c))
      {
			word[curr] = tolower(c);
			curr += 1;
		}
		else
      {
			if(curr != 0)
         {
			   word[curr] = 0;
			   insert(word);
			   curr=0;
			}
		}

	}
}

void read_files(int num_files, const  char *files[] )
{
	char word[101];
	char c;
	int curr = 0;
	FILE *file;
	int i;

	for (i = 0; i < num_files; i++)
   {
		file = fopen(files[i] , "r");

		for (c = fgetc(file); c != EOF;  c = fgetc(file))
      {
			if (isalpha(c))
         {
				word[curr] = tolower(c);
				curr += 1;
			}
			else
         {   
				if(curr != 0)
            {
               word[curr] = 0;
				   insert(word);
				   curr = 0;
				}
			}
		}
		fclose(file);
	}
}
