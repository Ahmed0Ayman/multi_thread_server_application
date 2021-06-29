/*
 * HashTable.h
 *
 *  Created on: Jun 21, 2021
 *      Author: Ahmed_Ayman
 */

#ifndef INC_HASHTABLE_H_
#define INC_HASHTABLE_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "main.h"






typedef struct {
    char * name;
    int clientFD ;
    struct sockaddr_in client_addr ;
}client_t ;




/* macro for check memory allocation return value */
#define CHECK_MEMORYALLOC(X)    		\
{										\
if((X)  == NULL){						\
printf("fail to allocate memory \n");	\
exit(-2) ;								\
}										\
}										\




/* macro to define the required length for hash table */
#define HashTableLen 50


/* define our data structure as incomplete data type to protect our library form corruption from client */

/* hash table data structure */
typedef struct HashTable_t 	HT_t ;


/* entry data structure */
typedef struct _Entry  	Entry_t ;


/* create a new Hash table and return handle to it */
HT_t * HashTable_Create(void);


/* generate a specific Key to given value */
bool HashTable_GenKey(char * Value ,uint32_t  * Key );


/* free the required entry */
bool HashTable_FreeKey(HT_t * Table , uint32_t index );


/* free all table */
bool HashTable_Free(HT_t * Table);


/* search for specific entry by specific value */
Entry_t * HashTable_ItemSearch(HT_t * Table , char * Value , uint32_t * Key_Index );


/* insert a new item to hash table */
bool HashTable_InsertItem(HT_t * Table ,client_t * client, uint32_t *  Key);


/* print all items stored in hash table */
bool HashTable_Printall(HT_t * Table);
















#endif /* INC_HASHTABLE_H_ */