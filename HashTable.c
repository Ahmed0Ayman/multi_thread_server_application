/*
 * HashTable.c
 *
 *  Created on: Jun 21, 2021
 *      Author: Ahmed_Ayman
 */



#include "HashTable.h"


/* hash table data structure */
struct HashTable_t
{
    struct _Entry ** HashTable_Entries ; /* pointer to pointer  to the entry */
    int Totalsize ;                      /* hold the total size of the Hash table */
    int CurrentSize ;                    /* hold the current  available spaces */

} ;


/* entry data structure */
struct _Entry
{
    uint32_t * Key ;	 		/* Hold the Key for the entry */
    client_t * client;
    struct _Entry * Next ;      /* hold the next entry address used if there is no available size in the Hash table  */

};



/* create a new Hash table and return handle to it */
HT_t * HashTable_Create(void)
{
    HT_t * Hash = NULL ;
    CHECK_MEMORYALLOC(Hash = malloc(sizeof(HT_t)));  /* allocate memory for the Hash table data structure */

    /* initialize Hash data members */
    Hash->Totalsize = HashTableLen ;
    Hash->CurrentSize = 0 ;


    /* allocate space for all entries  length pointers */
    Hash->HashTable_Entries = calloc(HashTableLen,sizeof(Entry_t *));

    if(Hash->HashTable_Entries == NULL) /* check for available heap */
    	return NULL ;


    return Hash ;  /* return Hash handler */
}




/* generate a specific Key to given name */
bool HashTable_GenKey(char * name ,uint32_t  * Key )
{
    if((name == NULL )||(strlen(name) == 0 ))   /* check parameter validity */
    	return false ;

    uint32_t Temp_Key = 0 ;

    for(int i =0 ; i< strlen(name);i++)   /* generate random key */
    {
        Temp_Key = Temp_Key *(37*3) + name[i] ;
    }

    Temp_Key %= HashTableLen ;
    *Key = Temp_Key ; /* return Key for the caller */

    return true ;
}




/* free the required all associate key Values  */
bool HashTable_FreeKey(HT_t *Table, uint32_t index) {

	if (Table == NULL) /* check parameter validity */
		return false;

	Entry_t *Curr_entry = Table->HashTable_Entries[index], *Next_enrty = NULL;
	bool Entry_exist = false; /* variable for check exist of entry */

	if (Curr_entry != NULL) /* if exist then free memory */
	{
		do {

			Next_enrty = Curr_entry->Next; /* hold the next entry address */
			/* first free item member */
			free(Curr_entry->Key);
			free(Curr_entry->client->name);

			/* Now we can free item */
			free(Curr_entry);
			Curr_entry = Next_enrty;
			Table->CurrentSize--; /* decrement  currentsize var with each new insertion */

		} while (Curr_entry != NULL);
		Table->HashTable_Entries[index] = NULL; /* assign to null */

	}
	else
	{ /* if not exist then return false to caller */
		return false;
	}

}




/* free all table entries  */
bool HashTable_Free(HT_t * Table)
{

    if(Table == NULL )		/* check parameter validity */
    return false ;

    Entry_t ** entry = NULL ;

    for(uint32_t i =0 ; i < HashTableLen ;i++) /* iterate on all Hash table elements */
    {
    	entry = &(Table->HashTable_Entries[i]) ;
        if(*entry != NULL)
        {
        	HashTable_FreeKey(Table , i);  /* free all entries associated  with this key */

        }
    }

    return true ;

}



/* search for specific entry by specific name */
Entry_t * HashTable_ItemSearch(HT_t * Table , char * name , uint32_t  * Key_Index )
{
    if((Table == NULL)||(name == NULL)||(strlen(name)==0))  		/* check parameter validity */
    	return NULL ;

    uint32_t Temp_Key =0 ;
    Entry_t * entry  = NULL ;
    HashTable_GenKey(name,&Temp_Key);
	entry = Table->HashTable_Entries[Temp_Key] ;
	if(entry == NULL)
	{
		return NULL ;
	}
    do{
		if(!(strcmp(entry->client->name ,name)))
		{
		*Key_Index = Temp_Key ;		/* return key to caller */
		return entry ;

		}
		else
		{
			if(entry->Next != NULL)
			{
				entry = entry->Next ;
			}
			else
			{
				printf("not exist %s\n",name);
				return NULL;
			}
		}
    }while(entry->Next != NULL);



}



/* insert a new item to hash table */
bool HashTable_InsertItem(HT_t * Table ,client_t * client, uint32_t * Key)
{
    if((Table == NULL )||(client == NULL)||(strlen(client->name) == 0 )) 		/* check parameter validity */
    	return false ;

    uint32_t Temp_Key =0 ;
    Entry_t * entry,*Temp_entry ;		/* Temp entry to facilitate handling the entry */

    HashTable_GenKey(client->name , & Temp_Key );

    *Key = Temp_Key ; 		/* return Generated Key to caller */
    printf("%d \n",Temp_Key);
    entry = Table->HashTable_Entries[Temp_Key];


    if(entry == NULL) /* means that first entry assigned to the Key */
    {
    	CHECK_MEMORYALLOC(entry = malloc(sizeof(Entry_t)));
    	CHECK_MEMORYALLOC(entry->Key = malloc(sizeof(uint32_t)));
        entry->client =  client ;        

        *(entry->Key) = Temp_Key ;
        entry->Next = NULL ;
        Table->HashTable_Entries[Temp_Key] = entry ;
    }
    else
    { /* use linked list to link a new entry */
    	CHECK_MEMORYALLOC(Temp_entry = malloc(sizeof(Entry_t)));
    	CHECK_MEMORYALLOC(Temp_entry->Key = malloc(sizeof(uint32_t)));
        entry->client =  client ;        

    	/* initialize the new entry */
    	*(Temp_entry->Key) = Temp_Key ;
    	Temp_entry->Next = NULL ;


    	while(entry->Next != NULL) /* iterate till the end of the list */
    		entry = entry->Next ;

    	entry->Next = Temp_entry ;

        

    }




    Table->CurrentSize++ ;			/* increment current size var with each new insertion */
    return true ;


}




/* print all items stored in hash table */
bool HashTable_Printall(HT_t * Table)
{
    if(Table == NULL )		/* check parameter validity */
    return false ;

    Entry_t * entry;


    for(uint32_t i =0 ; i < HashTableLen ;i++)		/* loop on all hash table entries */
    {
    	entry = Table->HashTable_Entries[i] ;
        if(entry != NULL)
        {
        	do{
				printf( "Table->HashTable[%d]->Key = %d \n",i,*(entry->Key) );
				printf( "Table->HashTable[%d]->Value = %s \n",i,entry->client->name );
				if(entry->Next != NULL) entry = entry->Next ;
        	}while(entry->Next != NULL);

        }
    }

    return true ;

}