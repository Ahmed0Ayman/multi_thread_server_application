#include "main.h"


    client_t clients[HashTableLen];
     HT_t * Table = NULL ;



void ERROR_FUN(void)
{
    printf("free table before exist \n");
    HashTable_Free(Table);
}

/* in Linux OS every line ended with carriage and line feed so we will remove them */
#define  Remove_Carriage_And_Line_Feed(Message)          {Message[strlen(Message)-2] = 0;}


/* this macro used to add end line form message before send */
#define  Add_Line_Feed(Message)                          {Message[strlen(Message)] = '\n';Message[strlen(Message)-1] = '\0';}   




void * handle_client_1(void * arg)               
{
    char  Message[100] = {0} ;
    long * key_arr = (long*)arg ;
    long key = (key_arr[0]),key_Cliect= (key_arr[1])  ;
    int flag =0 ,tid;
    
    printf("this is key %ld",key );
    printf("this is key_client %ld",key_Cliect );
    
    while(1)
    {
                bzero(Message, sizeof(Message));
                recv(clients[key].clientFD,Message,sizeof(Message),0);
                Remove_Carriage_And_Line_Feed(Message);
                if(!strcmp(Message,"exist")) break;
                Add_Line_Feed(Message);
                send(clients[key_Cliect].clientFD,Message,strlen(Message),0);

    }

    close(clients[key].clientFD);
    printf("thank you %s \n",clients[key].name);

    /* free this key to make it able to accept new member */
    HashTable_FreeKey(Table , key);


    /* release the allocated memory for this thread */
    pthread_detach(pthread_self());


}



void * handle_client_2(void * arg)
{
    char  Message[100] = {0} ;
    long * key_arr = (long*)arg ;
    long key = (key_arr[0]),key_Cliect= (key_arr[1])  ;
    int flag =0 ,tid;
    
    printf("this is key %ld",key );
    printf("this is key_client %ld",key_Cliect );
    
    while(1)
    {
                bzero(Message, sizeof(Message));
                recv(clients[key_Cliect].clientFD,Message,sizeof(Message),0);
                Remove_Carriage_And_Line_Feed(Message);
                if(!strcmp(Message,"exist")) break;
                Add_Line_Feed(Message);
                send(clients[key].clientFD,Message,strlen(Message),0);

    }


    
    close(clients[key_Cliect].clientFD);
    printf("thank you %s \n",clients[key_Cliect].name);

    /* free this key to make it able to accept new member */
    HashTable_FreeKey(Table , key_Cliect);

    /* release the allocated memory for this thread */
    pthread_detach(pthread_self());

}

void * handle_client(void * arg)
{
    char  Message[100] = {0} ;
    long key = (long)arg,key_Cliect =0 ,key_arr[2],CHECK_KEY =0;
    int flag =0  ;
    pthread_t tid[2];

        strcpy(Message,"Welcome in our Messenger based  on socket app \n");
        send(clients[key].clientFD,Message,strlen(Message),0);
        bzero(Message, sizeof(Message));
    while (1)
    {
        strcpy(Message,"please send client Name you want to communicate with or type exist\n");
        send(clients[key].clientFD,Message,strlen(Message),0);

        bzero(Message, sizeof(Message));
        recv(clients[key].clientFD,Message,sizeof(Message),0);
        Remove_Carriage_And_Line_Feed(Message);
        if(!strcmp(Message,"exist"))    
        {
            printf("ok \n");
            break ;
        }
        HashTable_GenKey(Message,&CHECK_KEY);



        /* check valid member and also prevent user from conect with himself */ 
        if((NULL == HashTable_ItemSearch(Table,Message,(uint32_t *)&key_Cliect))||(key != CHECK_KEY )) 
        {
            bzero(Message, sizeof(Message));
            strcpy(Message,"Not exist \n");
            send(clients[key].clientFD,Message,strlen(Message),0);
        } else {
                bzero(Message, sizeof(Message));
                strcpy(Message,"please accept the client request yes/no\n");
                send(clients[key_Cliect].clientFD,Message,strlen(Message),0);
                bzero(Message, sizeof(Message));
                recv(clients[key_Cliect].clientFD,Message,sizeof(Message),0);
                Remove_Carriage_And_Line_Feed(Message);               
                if(!strcmp(Message,"yes"))
                {
                flag = 1 ;
                }
                break ;
        }


    }

    if(flag == 1)
    {
        key_arr[0] = key ;
        key_arr[1] = key_Cliect ;
        pthread_create(&tid[0], NULL, &handle_client_1, (void*)key_arr);
        pthread_create(&tid[1], NULL, &handle_client_2, (void*)key_arr);

    }

    /* wait until return from these threads */
    pthread_join(tid[0],NULL);
    pthread_join(tid[1],NULL);

    /* release the allocated memory for this thread */
    pthread_detach(pthread_self());
    return NULL;
}
 



 /* this handler used to free all allocated memory before exit the application and print exist message  */
void * Handle_EXIT(void * arg)
{
    char Mess[10];
    printf("the Handle_EXIT is running at any time enter exist to leave \n");
    while(1)
    {
        scanf("%s",Mess);
        if(!strcmp(Mess,"exist"))
        exit(0);
    }


}




int main(int argc ,char * argv[])
{
    atexit(ERROR_FUN); /* register the error function */

    if(argc < 2 )  exit(-2); /* port number always selected by user */

    pthread_t tid ; 
    int server_socketID, child_socketID;
    struct sockaddr_in sock_serveraddr ,sock_clientaddr;


    Table = HashTable_Create() ;
    if(Table == NULL)
    exit(-1 );

    long Key =0 , index=0 ;
    char  Message[50]  = {0};
    socklen_t sock_len;






    server_socketID = socket(AF_INET, SOCK_STREAM, 0);
    sock_serveraddr.sin_family = AF_INET;
    sock_serveraddr.sin_addr.s_addr = INADDR_ANY;
    sock_serveraddr.sin_port = htons(atoi(argv[1]));


    if(bind(server_socketID , (struct sockaddr *)&sock_serveraddr , sizeof(sock_serveraddr)) == -1) exit(-1);

    /* maximun number of clients will equal to the hashtable length */
    if(listen(server_socketID,HashTableLen) == -1) exit(-1);

    /* create exist thread */
    if(pthread_create(&tid, NULL, &Handle_EXIT, (void*)Key)  == -1) exit(-1);
   


    while(1)
    {
        
        do{ /* loop until get valid user name key from hashtable  */

        strcpy(Message , "send your name \n");
        sock_len = sizeof(sock_clientaddr);


        /* accept function block until receive a request on the bind port number */
        child_socketID = accept(server_socketID,(struct sockaddr*)&sock_clientaddr, &sock_len);
        send(child_socketID,Message,strlen(Message),0);
        bzero(Message, sizeof(Message));
        recv(child_socketID,Message,sizeof(Message),0);
        Remove_Carriage_And_Line_Feed(Message);
        HashTable_GenKey(Message , (uint32_t *)&Key);


        /* if HashTable return null this mean valid key this step to prevent to clients with the same name */
        }while (HashTable_ItemSearch(Table,Message,(uint32_t *)&Key) != NULL);   

        clients[Key].name = malloc(strlen(Message)+1);
        strcpy(clients[Key].name ,Message);
        clients[Key].clientFD = child_socketID ;
        clients[Key].client_addr = sock_clientaddr;
        HashTable_InsertItem(Table,&clients[Key],(uint32_t *)&Key);
        strcat(Message , "  -> welcome\n");
        send(child_socketID,Message,strlen(Message),0);




        pthread_create(&tid, NULL, &handle_client, (void*)Key);





    }


    exit(0) ;
}