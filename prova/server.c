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

#define PROTOPORT 5193 //numero della porta di default
#define QLEN 6         //grandezza della coda
#define BUFFERSIZE 256

int go = 1;
int masterSocket; // descrittore del master socket

void sighand(int sig)
{
    if (sig == SIGINT)
    {
        printf("hai premuto CTRL-C ... chiusura del Master Socket.\n");
        close(masterSocket);
    }
    else if (sig == SIGCHLD)
    {
        printf("ricevuto signale di SIGCHLD.\n");
    }
}

int main(int argc, char *argv[])
{

    int csd; // client socket descriptor
    char msg[256] = {0};
    int port;
    int status; //il parametro status il processo che termina può comunicare al padre informazioni sul suo stato di terminazione (ad es. l’esito della sua esecuzione).

    char buf[BUFFERSIZE]; //array di stringhe che serve come buffer di transito dei dati dai due socket

    pid_t pid;

    if (argc > 1)
    {
        port = atoi(argv[1]); //se come argomento si dà l'indizirizzo di una porta, atoi la converte in binario
    }
    else
        port = PROTOPORT;
    if (port < 0)
    {
        printf("Errore nel numero della porta %s. \n", argv[1]);
    }

    struct sockaddr_in sa;      //struttura per l'indirizzo del server
    memset(&sa, 0, sizeof(sa)); //inizializza tutti i dati della struttura

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
    }
    else
        printf("Socket creata con successo.\n");

    if (bind(masterSocket, (struct sockaddr *)&sa, sizeof(sa)) < 0) //la bind assegan un indirizzo locale ad una socket, e controlla l'esito
    {
        printf("bind() fallita.\n");
        close(masterSocket);
    }
    else
    {
        printf("La Bind ha avuto successo!!!!\n");
    }

    if (listen(masterSocket, QLEN) < 0) //QLEN->la massima lunghezza della code delle connessioni entranti
    {                                   //listen restituisce un valore negativo se fallisce, altrimenti 0
        printf("listen() fallita.\n");
        close(masterSocket);
    }

    else
        printf("In attesa di una connessione da un client....\n");

    /////////////////////////////////DA QUI IN GIU' SERVE SCRIVERE IL CICLO PER LA CONVERSAZIONE///////////////////////////////////////////////
    ///////////////PRENDERE SPUNTO DA CODICI MESSI DAL PROFESSORE SUL SITO//////////////////////////////////////
    while (go)
    {

        // II argomento puntatore indirizzo del client remoto

        if ((csd = accept(masterSocket, NULL, 0)) < 0) //accetta la richiesta di conenssione del socket, e la funzione accept
        {                                              //restituisce il numero del socket se ha successo, altrimenti restituisce -1
            go = 0;
            printf("Accept fallita.\n");
        }
        else
        {
            printf("Connessione riuscita!!!!!\n");
            pid = fork();

            if (pid == 0) //se l'id del processo è 0, significa che il processo è un processo figlio
            {

                close(masterSocket); // chiude il processo padre per continuare sul processo figlio
                while(1)
                {
                if (read(csd, buf, sizeof(buf)) != sizeof(buf)) //legge quello che c'è scritto sul socket figlio, e lo scrive in buf
                {
                    printf("Errore nella lunghezza del messaggio presente sul Socket client.\n");
                    close(csd);
                }

                printf("Il client ha detto: %s\n", buf); //stampa a schermo quello che ha letto dal client
                printf("Scrivi la risposta: ");
                scanf("%s", msg);
                if (write(csd, msg, sizeof(msg)) != sizeof(msg)) //controlla se scrive il messaggio in tutta la sua lunghezza
                {
                    printf("Errore nella ricezione della lunghezza del messaggio.\n");
                    close(csd);
                    printf("Socket chiusa.\n");
                }
                else
                    printf("Invio riuscito.\n");
                    if (strncmp("exit", msg, 4) == 0) { 
                    printf("Server esce...\n"); 
            break; 
        } 
                }

                ///////sul codice del prof qui c'è l'execl, ma non so a cosa serva quindi l'ho tolta
            }
            else //se l'id del processo è maggiore di 0, significa che il processo è padre
            {

                if (wait(&status) < 0) //la funzione wait mette in attesa il processo padre finchè un processo figlio termina o riceve un comando di terminazione
                {
                    printf("Errore nella wait().\n"); //la wait restituisce -1 se fallisce, altrimenti restituisce l'id del processo terminato
                    close(csd);
                    printf("Chiuso il Socket del processo figlio per errore nella wait.\n");
                }
                if (write(csd, &status, sizeof(status)) != sizeof(status)) //scrive sul socket figlio il valore di status
                                                                           //controlla se scrive il messaggio in tutta la sua lunghezza
                {
                    printf("Errore nella ricezione della lunghezza del messaggio scritto dal Socket padre sul Socket figlio\n");
                    close(csd);
                    printf("Socket chiusa.\n");

                    close(masterSocket); //chiude il socket padre e quindi la comunicazione, ricominciando il ciclo
                }
            }
        }

        return 0;
    }
}