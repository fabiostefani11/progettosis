#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h> /* wait */
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include "thpool.h"
#include <malloc.h>
#include <semaphore.h>

sem_t mutex;
void aggiornaFile(void *);
void connection_handler(void *);
int goo = 1;
int masterSocket; // descrittore del master socket
int c12;
int client_sock;
risposta Risposta;
int csd; // client socket descriptor
messaggio Messaggio;
char buf[BUFFERSIZE]; //array di stringhe che serve come buffer di transito dei dati dai due socket

int port;
int status; //il parametro status il processo che termina può comunicare al padre informazioni sul suo stato di terminazione (ad es. l’esito della sua esecuzione).
pid_t pid;
FILE *f_ombrelloni, *f_prenotazioni, *f_aggiornamenti;

//questa funzione non la metto nel .h e nel .c perchè dà errore sulla variabile mastersocket
void sighand(int sig)
{
    printf("\n");
    if (sig == SIGINT)
    {
        printf(RED "hai premuto CTRL-C ... chiusura del Master Socket.\n" CRESET);
        close(masterSocket);
    }
    else if (sig == SIGCHLD)
    {
        printf("ricevuto signale di SIGCHLD.\n");
    }
    if (sig == SIGTERM)
    {
        printf("SIGTERM received ... GAME OVER ! \n");
    }
    if (sig == SIGQUIT)
    {
        printf("SIGQUIT received ... GAME OVER ! \n");
    }
    if (sig == SIGHUP)
    {
        printf("SIGHUP received ... GAME OVER ! \n");
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    sem_init(&mutex, 0, 1);

    char data_odierna[DIM];
    crealista(&Risposta.lista);
    memset(&Risposta, 0, sizeof(Risposta));

    printf("\nPid figlio server: %d\nPid del padre: %d\n", (int)getpid(), (int)getppid());

    printf("Inserisci la data odierna: ");
    scanf("%s", data_odierna);
    Risposta.data_oggi = uniscidata(data_odierna);

    leggoFile(&Risposta, f_ombrelloni, f_prenotazioni, f_aggiornamenti);

    if (argc > 1) //da togliere
    {
        port = atoi(argv[1]); //se come argomento si dà l'indizirizzo di una porta, atoi la converte in binario
    }
    else
        port = PROTOPORT;
    if (port < 0)
    {
        printf("Errore nel numero della porta %s. \n", argv[1]);
    }

    struct sockaddr_in sa, client; //struttura per l'indirizzo del server
    memset(&sa, 0, sizeof(sa));    //inizializza tutti i dati della struttura

    sa.sin_family = AF_INET;                     //famiglia indirizzi
    sa.sin_addr.s_addr = inet_addr("127.0.0.1"); //ip del server  inet_addr->converte numero in notazione puntata in numero a 32 bit
    sa.sin_port = htons(port);                   //porta del server  htons->converte dall'ordine di 16-bit dell'host all'ordine del network

    signal(SIGINT, sighand);
    signal(SIGCHLD, sighand);

    // creazione del master socket
    masterSocket = socket(AF_INET, SOCK_STREAM, 0); //af_inet=ipv4 stream->socket tcp  0->protocollo di default
    if (masterSocket < 0)
    {
        printf("Fallimento nella creazione della Socket.\n");
        close(masterSocket);
        exit(0);
    }
    else
        printf("Socket creata con successo.\n");

    if (bind(masterSocket, (struct sockaddr *)&sa, sizeof(sa)) < 0) //la bind assegan un indirizzo locale ad una socket, e controlla l'esito
    {
        printf("bind() fallita.\n");
        close(masterSocket);
        exit(0);
    }
    else
    {
        printf("La Bind ha avuto successo!!!!\n");
    }

    if (listen(masterSocket, QLEN) < 0) //QLEN->la massima lunghezza della code delle connessioni entranti
    {                                   //listen restituisce un valore negativo se fallisce, altrimenti 0
        printf("listen() fallita.\n");
        close(masterSocket);
        exit(0);
    }

    else
    {
        printf("In attesa di una connessione da un client....\n");
    }

    c12 = sizeof(struct sockaddr_in);
    pthread_t thread_id;
    threadpool thpool = thpool_init(QLEN);
    thpool_add_work(thpool, aggiornaFile, ((void *)&Risposta));

    /////////////////////////////////DA QUI IN GIU' IL CICLO PER LA CONVERSAZIONE///////////////////////////////////////////////
    while ((client_sock = accept(masterSocket, (struct sockaddr *)&client, (socklen_t *)&c12)))
    {
        puts("Connessione accettata");
        while (thpool_num_threads_working(thpool) >= QLEN)
        {
        }

        thpool_add_work(thpool, connection_handler, (void *)&client_sock);

        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
        puts("Gestione assegnata ad un thread");
    }

    if (client_sock < 0)
    {
        perror("accept fallita");
        return 1;
    }

    return 0;
}

void connection_handler(void *socket_desc)
{

    //Get the socket descriptor
    int sock = *(int *)socket_desc;
    char msg[256] = {0};
    srand(time(0));
    int id = 1 + rand() % 10000;
    Risposta.IDclient = id;
    int ombrellone_attuale[10] = {0};
    char mid[DIM] = "Il tuo id è ";
    char conv[DIM];
    int go = 1;
    sprintf(conv, "%d", id);
    strcat(mid, conv);

    printf(GREEN "Client %d conesso\n" CRESET, id);

    if (write(sock, mid, sizeof(mid)) != sizeof(mid)) //controlla se scrive il messaggio in tutta la sua lunghezza
    {
        printf("Errore nella ricezione della lunghezza del messaggio.\n");
        close(sock);
        printf("Socket chiusa.\n");
    }

    // pid = fork();

    //if (pid == 0) //se l'id del processo è 0, significa che il processo è un processo figlio
    //{

    // close(sock); // chiude il processo padre per continuare sul processo figlio
    while (go)
    {

        if (read(sock, buf, sizeof(buf)) != sizeof(buf)) //legge quello che c'è scritto sul socket figlio, e lo scrive in buf
        {
            printf("Errore nella lunghezza del messaggio presente sul Socket client.\n");
            for (int i = 0; i < 10; i++)
            {
                if (Risposta.Ombrellone[ombrellone_attuale[i]].disponibile == 4)
                {
                    Risposta.Ombrellone[ombrellone_attuale[i]].disponibile = 0;
                    Risposta.ombrelloni_liberi++;
                }
                if (Risposta.ombrelloni_Toccupati[ombrellone_attuale[i]] == 1)
                {
                    Risposta.ombrelloni_Toccupati[ombrellone_attuale[i]] = 0;
                }
            }

            printf(RED "Client %d disconnesso\n" CRESET, id);
            go = 0;
            close(sock);
        }
        else

        {
            printf("Il client ha detto: %s", buf); //stampa a schermo quello che ha letto dal client

            //divide la frase in una parola e 4 interi//
            Risposta.IDclient = id;
            //wait
            sem_wait(&mutex);
            if ((f_aggiornamenti = fopen("aggiornamenti.txt", "a")) == NULL)
            {
                printf("Errore nell'apertura del file Aggiornamenti.\n");
                exit(-1);
            }
            else
                printf("File Aggiornamenti aperto correttamente.\n");
            fprintf(f_aggiornamenti, "%d-%s", id, buf);
            fclose(f_aggiornamenti);
            //signal
            sem_post(&mutex);

            Messaggio = dividiFrase(buf);

            strncpy(msg, elaboraRisposta(&Risposta, Messaggio), sizeof(char) * DIM);

            int j = 0;

            if ((strncmp("AVAILABLE", msg, 9) == 0) && (strncmp("BOOK", Messaggio.parola, 4) == 0))
            {
                while (ombrellone_attuale[j] != 0)
                {
                    j++;
                }
                ombrellone_attuale[j] = Messaggio.ID;
            }

            //confronta la parola con le varie possibilità e scrive la risposta nella socket

            if (write(sock, msg, sizeof(msg)) != sizeof(msg)) //controlla se scrive il messaggio in tutta la sua lunghezza
            {
                printf("Errore nella ricezione della lunghezza del messaggio.\n");
                close(sock);
                printf("Socket chiusa.\n");
            }
        }
        if (strncmp("USCITA", msg, 6) == 0)
        {
            for (int i = 0; i < 10; i++)
            {
                if (Risposta.Ombrellone[ombrellone_attuale[i]].disponibile == 4)
                {
                    Risposta.Ombrellone[ombrellone_attuale[i]].disponibile = 0;
                    Risposta.ombrelloni_liberi++;
                }
                if (Risposta.ombrelloni_Toccupati[ombrellone_attuale[i]] == 1)
                {
                    Risposta.ombrelloni_Toccupati[ombrellone_attuale[i]] = 0;
                }
            }
            go = 0;
            printf(RED "Client %d disconnesso\n" CRESET, id);
            close(sock);
        }
    }
}

void aggiornaFile(void *Risposta)
{

    while (1)
    {
        sleep(10);
        risposta Ris = *(risposta *)Risposta;
        int ok = 0;
        int i;
        if ((f_ombrelloni = fopen("ombrelloni.txt", "w")) == NULL)
        {
            printf("errore apertura file ombrelloni.\n");
        }

        for (i = 0; i <= 100; i++)
        {
            if (i == 0)
            {
                fprintf(f_ombrelloni, "%d \n", Ris.data_oggi);
            }
            else
            {
                if (Ris.Ombrellone[i].disponibile == 4)
                {
                    (fprintf(f_ombrelloni, "%d %d %d %d %d \n",
                             Ris.Ombrellone[i].ID,
                             Ris.Ombrellone[i].fila,
                             Ris.Ombrellone[i].numero,
                             0,
                             Ris.Ombrellone[i].IDclient));
                }
                else
                {
                    (fprintf(f_ombrelloni, "%d %d %d %d %d \n",
                             Ris.Ombrellone[i].ID,
                             Ris.Ombrellone[i].fila,
                             Ris.Ombrellone[i].numero,
                             Ris.Ombrellone[i].disponibile,
                             Ris.Ombrellone[i].IDclient));
                }
            }
        }
        if ((f_prenotazioni = fopen("prenotazioni.txt", "w")) == NULL)
        {
            printf("errore apertura file prenotazioni.\n");
        }

        stampaListaSuFile(&Ris.lista, f_prenotazioni);

        fclose(f_prenotazioni);
        fclose(f_ombrelloni);

        //wait
        sem_wait(&mutex);
        if ((f_aggiornamenti = fopen("aggiornamenti.txt", "w")) == NULL)
        {
            printf("errore apertura file aggiornamenti.\n");
        }
        fclose(f_aggiornamenti);
        //signal
        sem_post(&mutex);
    }
}
