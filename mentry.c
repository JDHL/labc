#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "string.h"
#include "mentry.h"
#define READSIZE 100


/* me_get returns the next file entry, or NULL if end of file*/
MEntry *me_get(FILE *fd){
    MEntry *p = (MEntry *)malloc(sizeof(MEntry));
    int max = 100;
    char buf[READSIZE];

    if(fgets(buf, max, fd) == NULL ){
        free(p);
        return NULL;
    }

    p->full_address = (char *)malloc(READSIZE*sizeof(char));
    p->surname = (char *)malloc(READSIZE*sizeof(char));
    strcpy(p->surname, buf);
    strcpy(p->full_address, buf);

    fgets(buf, max, fd);
    strcat(p->full_address, buf);
    if(isdigit(*buf)){
        char *token;
        token = strtok(buf, " ");
        p->house_number = atoi(token);
    }
    else{
        p->house_number = 0;
    }

    fgets(buf, max, fd);
    p->postcode = (char *)malloc(READSIZE*sizeof(char));
    strcpy(p->postcode, buf);
    strcat(p->full_address, buf);

    return p;
}

unsigned long str_hash(char *str, int num){ 
    unsigned long hash = 0;
    if(num){
        while(*str != ','){
            hash = *str + 31 *hash;
            str++;
        }
    }
    else{
        while(*str != '\0'){
            if(*str != ',' || *str != ' ')
                hash = *str + 31 * hash;
            str++;
         }
    }
    return hash;
}

/* me_hash computes a hash of the MEntry, mod size */
unsigned long me_hash(MEntry *me, unsigned long size){
    unsigned long hashval = 0;
    hashval = str_hash(me->surname, 1);
    hashval = hashval + str_hash(me->postcode, 0) + (unsigned)me->house_number;
    return hashval % size;
}

/* me_print prints the full address on fd */
void me_print(MEntry *me, FILE *fd){
    fputs(me->full_address, fd);
}

/* me_compare compares two mail entries, returning <0, 0, >0 if
 *  * me1<me2, me1==me2, me1>me2
 *   */
int me_compare(MEntry *me1, MEntry *me2){
    unsigned long hashme1, hashme2;
    hashme1 = hashme2 = 0;
    hashme1 = str_hash(me1->surname, 1)+str_hash(me1->postcode, 0);
    hashme2 = str_hash(me2->surname, 1)+str_hash(me2->postcode, 0);
    if(hashme1<hashme2)
        return -1;
    else if(hashme1>hashme2)
        return 1;
    else       
        return 0;
}

/* me_destroy destroys the mail entry
 *  */
void me_destroy(MEntry *me){
    free(me->surname);
    free(me->full_address);
    free(me->postcode);
    free(me);
}

