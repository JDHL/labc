#include "mlist.h"
#include <stdlib.h>
#include <stdio.h>

#define BUCKETCOUNT 20
#define STEP 10

typedef struct bucketItem 
{
    struct bucketItem *next;
    MEntry *entry;
} Node;
 
 
typedef struct bucket 
{
    int entries;
    struct bucketItem *head;
    struct bucketItem *tail;
} Bucket;
 
 
struct mlist 
{
    unsigned long size;
    struct bucket **buckets;
};

int ml_verbose=1;		/* if true, print diagnostics on stderr */

/* ml_create - created a new mailing list */
MList *ml_create(void)
{	
	// Mallocating MList
	MList *list = malloc(BUCKETCOUNT);
	if (list == NULL) return NULL;
	
	if (ml_verbose)
		fprintf(stderr, "mlist: creating mailing list\n");

	list->size = BUCKETCOUNT;
	unsigned long i;

	// Mallocating bucket array
	list->buckets = malloc(sizeof(Bucket)*BUCKETCOUNT);
	if (list->buckets == NULL) return NULL;

	for (i = 0; i < (list->size); i++)
	{	
		// Mallocating bucket
		list->buckets[i] = malloc(sizeof(Bucket));
		if (list->buckets[i] == NULL) return NULL;

		list->buckets[i]->entries = 0;
		list->buckets[i]->head = NULL;
		list->buckets[i]->tail = NULL;
	}

	return list;
}




int ml_add(MList **ml, MEntry *me)
{	
	MList *list;


	Bucket *bucket;
	
	// Mallocating memory for entry
	Node *bi = malloc(sizeof(Node));
	
	list = *ml;

	if (ml_lookup(list, me) != NULL) {
		free(bi);
		return 1;
	}

	bi->entry = me;
	bi->next = NULL;



	// Computing a new hash for the entry
	unsigned int pos = me_hash(me, list->size);
	bucket = list->buckets[pos];

	
	if(bucket->head == NULL) 
	{
    	// If bucket is empty, then the new entry node is set as the head.
        bucket->head = bi;
    } 
    else 
    {	
    	// Otherwise new entry is the last entry in the bucket.
        bucket->tail->next = bi;
    }

	bucket->tail = bi;

	bucket->entries += 1;

    if((unsigned)(bucket->entries) > (list->size)){
       unsigned long i;
       printf("Starting resizing!\n");

       //Mallocating resized MList
       MList *resizeList = malloc(list->size + STEP);
       resizeList->size = list->size + STEP;
       // Mallocating new bucket Array
       resizeList->buckets = malloc(sizeof(Bucket)*(resizeList->size));
       for (i = 0; i < (resizeList->size); i++) {
           // Mallocating bucket
           resizeList->buckets[i] = malloc(sizeof(Bucket));
           resizeList->buckets[i]->entries = 0;
           resizeList->buckets[i]->head = NULL;
           resizeList->buckets[i]->tail = NULL;
       }
       *ml = resizeList;
       for(i=0; i< (list->size); i++){
           if(list->buckets[i]->head != NULL){
               Node *node = list->buckets[i]->head;
               while(node != NULL){
                   ml_add(ml, node->entry);
                   node = node->next;
               }
           }
       }
       Bucket *bucket;
       Node *nodeItem;
       Node *oldNodeItem;
       unsigned long j;
       for(j=0; j < (list->size); j++){
            bucket = list->buckets[j];
            nodeItem = bucket->head;

            while(nodeItem != NULL){
                oldNodeItem = nodeItem;
                nodeItem = nodeItem->next;
                free(oldNodeItem);
            }
            free(bucket);
       }
       free(list->buckets);
       free(list);
    }
	

	return 1;
	
}


MEntry *ml_lookup(MList *ml, MEntry *me)
{
	Bucket *bucket;
	Node *nodeItem;
	if (ml_verbose)
		fprintf(stderr, "mlist: ml_lookup() entered\n");

	unsigned long i;
	for (i = 0; i < (ml->size); i++)
	{
		bucket = ml->buckets[i];
		for (nodeItem = bucket->head; nodeItem != NULL; nodeItem = nodeItem->next){
			if (me_compare(me, nodeItem->entry) == 0){
				return nodeItem->entry;
			}
		}
	}
	return NULL;

}

void ml_destroy(MList *ml)
{
	Bucket *bucket;
	Node *nodeItem;
	Node *oldNodeItem;

	if (ml_verbose)
		fprintf(stderr, "mlist: ml_destroy() entered\n");

	unsigned long i;
	for (i = 0; i < (ml->size); i++)
	{
		bucket = ml->buckets[i];
		nodeItem = bucket->head;

		while (nodeItem != NULL)
		{
			me_destroy(nodeItem->entry);
			oldNodeItem = nodeItem;
			nodeItem = nodeItem->next;
			free(oldNodeItem);
		}
		free(bucket);
	} 
    free(ml->buckets);
	free(ml);
}
