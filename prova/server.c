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

    if (bind(masterSocket, (struct sockaddr *)&sa, sizeof(sa)) < 0) //la bind assegan un indirizzo locale ad una socket, e controlla l'esito
    {
        printf("bind() fallita.\n");
        close(masterSocket);
    }

    if (listen(masterSocket, QLEN) < 0) //QLEN->la massima lunghezza della code delle connessioni entranti
    {                                   //listen restituisce un valore negativo se fallisce, altrimenti 0
        printf("listen() fallita.\n");
        close(masterSocket);
    }
    /////////////////////////////////DA QUI IN GIU' SERVE SCRIVERE IL CICLO PER LA CONVERSAZIONE///////////////////////////////////////////////
    ///////////////PRENDERE SPUNTO DA CODICI MESSI DAL PROFESSORE SUL SITO//////////////////////////////////////
    while (go)
    {

        // II argomento puntatore indirizzo del client remoto

        if ((csd = accept(masterSocket, NULL, 0)) < 0) //accetta la richiesta di conenssione del socket, e la funzione accept
        {                                              //restituisce il numero del socket se ha successo, altrimenti restituisce -1

            go = 0;
        }
        else
        {
            pid = fork();

            if (pid == 0) //se l'id del processo è 0, significa che il processo è un processo figlio
            {

                close(masterSocket); // chiude il processo padre per continuare sul processo figlio

                if (read(csd, buf, sizeof(buf)) != sizeof(buf)) //legge quello che c'è scritto sul socket figlio, e lo scrive in buf
                {
                    printf("Errore nella lunghezza del messaggio presente sul Socket client");
                    close(csd);
                }

                printf("Il client ha detto: %s\n", buf); //stampa a schermo quello che ha letto dal client

                ///////sul codice del prof qui c'è l'execl, ma non so a cosa serva quindi l'ho tolta
            }
            else //se l'id del processo è maggiore di 0, significa che il processo è padre
            {

                if (wait(&status) < 0) //la funzione wait mette in attesa il processo padre finchè un processo figlio termina o riceve un comando di terminazione
                {
                    printf("Errore nella wait()."); //la wait restituisce -1 se fallisce, altrimenti restituisce l'id del processo terminato
                    close(csd);
                    printf("Chiuso il Socket del processo figlio per errore nella wait.");
                }
                if (write(csd, &status, sizeof(status)) != sizeof(status)) //scrive sul socket figlio il valore di status
                                                                           //controlla se scrive il messaggio in tutta la sua lunghezza
                {
                    printf("Errore nella ricezione della lunghezza del messaggio scritto dal Socket padre sul Socket figlio");
                    close(csd);
                    printf("Socket chiusa.\n");

                    close(masterSocket); //chiude il socket padre e quindi la comunicazione, ricominciando il ciclo
                }
            }
        }

        return 0;
    }
}