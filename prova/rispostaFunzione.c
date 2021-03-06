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

#define PROTOPORT 5193 //numero della porta di default
#define QLEN 6         //grandezza della coda
#define BUFFERSIZE 256

#define DIM 256
#define dim 10

typedef struct
{
    int ID;
    int numero;
    int fila;
    int disponibile; //0=disponibile 1=occupato adesso 2=occupato in futuro 3=occupato sia adesso che in futuro 4=temporaneamente occupato
    int IDclient;
    int data_inizio;
    int data_fine;
} ombrellone;

typedef struct nodo
{
    ombrellone dato;
    struct nodo *next;
} Nodo;

typedef Nodo *lista;

typedef struct
{
    char parola[DIM];
    int ID;
    int ombrellone;
    int fila;
    int data_inizio;
    int data_fine;
    int nparole;
    int IDclient;

} messaggio;

typedef struct
{
    ombrellone Ombrellone[DIM];
    char msg[DIM];
    int ombrelloni_liberi;
    int IDclient;
    lista lista;
    int ombrelloni_Toccupati[101];
} risposta;

risposta Risposta;
messaggio Messaggio;
ombrellone Ombrellone[100];

int uniscidata(char data[]);
messaggio dividiFrase(char msg[]);
char *elaboraRisposta(risposta *Risposta, messaggio Messaggio);
void crealista(lista *l);
void inserimento(lista *l, int ID, int fila, int numero, int IDclient, int data_inizio, int data_fine);
int confrontoDate(int inizioPrenotazione, int finePrenotazione, int inizioRichiesta, int fineRichiesta);
int ricerca(lista *l, int ID, int datainizio, int datafine);
void elimTesta(lista *l);
int eliminaPrenotazione(lista *l, int IDclient, int fila, int numero);
void stampaListaSuFile(lista *l, FILE *f);

int uniscidata(char data[])
{
    char giorno[DIM] = {0};
    char mese[DIM] = {0};
    char anno[DIM] = {0};

    strcpy(giorno, strtok(data, "/"));
    strcpy(mese, strtok(NULL, "/"));
    strcpy(anno, strtok(NULL, "/"));

    strcat(anno, mese);
    strcat(anno, giorno);

    return atoi(anno);
}

messaggio dividiFrase(char msg[])
{
    messaggio Messaggio;
    char frase[dim][DIM] = {0};
    int i = 0;
    int k = 0;
    int j = 0;

    memset(&Messaggio, 0, sizeof(Messaggio));

    while (msg[k] != '\n')
    {
        if (msg[k] != ' ')
        {
            frase[j][i] = msg[k];
            i++;
        }
        else if (msg[k] == ' ' && (msg[k + 1] != '\n' && msg[k + 1] != ' ')) //elimino il problema degli spazi se ce ne sono in più
        {
            frase[j][i] = '\0';
            j++;
            i = 0;
        }
        k++;
    }

    Messaggio.nparole = j + 1;
    strcpy(Messaggio.parola, frase[0]);

    if (strlen(frase[1]) <= 2)
    {
        Messaggio.fila = atoi(frase[1]);
    }
    else if (strlen(frase[1]) > 2)
    {
        if (frase[1][2] != '/' || frase[1][5] != '/') //controllo se la data è nel formato corretto
        {
            strcpy(Messaggio.parola, "ERRORE_DATA");
            return Messaggio;
        }
        Messaggio.data_inizio = uniscidata(frase[1]);
    }
    if (strlen(frase[2]) <= 2)
    {
        Messaggio.ombrellone = atoi(frase[2]);
        Messaggio.ID = (((Messaggio.fila * 10) + Messaggio.ombrellone) - 10);
    }
    else if (strlen(frase[2]) > 2)
    {
        if (frase[2][2] != '/' || frase[2][5] != '/') //controllo se la data è nel formato corretto
        {
            strcpy(Messaggio.parola, "ERRORE_DATA");
            return Messaggio;
        }
        Messaggio.data_fine = uniscidata(frase[2]);
    }
    if (strlen(frase[3]) <= 4)
    {
        Messaggio.IDclient = atoi(frase[3]);
    }
    else if (strlen(frase[3]) > 4)
    {
        if (frase[3][2] != '/' || frase[3][5] != '/') //controllo se la data è nel formato corretto
        {
            strcpy(Messaggio.parola, "ERRORE_DATA");
            return Messaggio;
        }
        Messaggio.data_inizio = uniscidata(frase[3]);
    }
    if (Messaggio.nparole == 6)
    {
        if (frase[3][2] != '/' || frase[3][5] != '/' || frase[4][2] != '/' || frase[4][5] != '/') //controllo se la data è nel formato corretto
        {
            strcpy(Messaggio.parola, "ERRORE_DATA");
            return Messaggio;
        }
        Messaggio.data_inizio = uniscidata(frase[4]);
        Messaggio.data_fine = uniscidata(frase[5]);
    }
    if (Messaggio.nparole == 5)
    {
        if (frase[4][2] != '/' || frase[4][5] != '/') //controllo se la data è nel formato corretto
        {
            strcpy(Messaggio.parola, "ERRORE_DATA");
            return Messaggio;
        }
        Messaggio.data_fine = uniscidata(frase[4]);
    }

    return Messaggio;
}

void stampaListaSuFile(lista *l, FILE *f)
{
    while (*l)
    {
        fprintf(f, "%d %d %d %d %d %d \n", (*l)->dato.ID, (*l)->dato.fila, (*l)->dato.numero, (*l)->dato.IDclient, (*l)->dato.data_inizio, (*l)->dato.data_fine);
        l = &(*l)->next;
    }
}

void crealista(lista *l)
{
    *l = NULL;
}

void inserimento(lista *l, int ID, int fila, int numero, int IDclient, int data_inizio, int data_fine)
{
    Nodo *aux = (Nodo *)malloc(sizeof(Nodo));
    aux->dato.ID = ID;
    aux->dato.fila = fila;
    aux->dato.numero = numero;
    aux->dato.IDclient = IDclient;
    aux->dato.data_inizio = data_inizio;
    aux->dato.data_fine = data_fine;
    aux->next = *l;
    *l = aux;
}

int ricerca(lista *l, int ID, int datainizio, int datafine)
{
    int trovato = 0;
    while (*l)
    {
        if ((*l)->dato.ID == ID)
        {
            if (confrontoDate((*l)->dato.data_inizio, (*l)->dato.data_fine, datainizio, datafine) == 1)
            {
                trovato = 1;
                break;
            }
        }
        l = &(*l)->next;
    }
    if (trovato == 0)
    {

        return 0;
    }
    else
        return 1;
}

int confrontoDate(int inizioPrenotazione, int finePrenotazione, int inizioRichiesta, int fineRichiesta)
{
    int trovato = 0;

    //CASO 2
    if ((inizioRichiesta >= inizioPrenotazione) && (fineRichiesta <= finePrenotazione))
    {
        trovato = 1;
    }

    //CASO 1
    if ((inizioRichiesta <= inizioPrenotazione) && (fineRichiesta >= inizioPrenotazione) && (fineRichiesta <= finePrenotazione))
    {
        trovato = 1;
    }

    //CASO 3
    if ((inizioRichiesta <= finePrenotazione) && (inizioRichiesta >= inizioPrenotazione) && (fineRichiesta >= finePrenotazione))
    {
        trovato = 1;
    }

    //CASO 4
    if ((inizioRichiesta <= inizioPrenotazione) && (fineRichiesta >= finePrenotazione))
    {
        trovato = 1;
    }

    return trovato;
}

void elimTesta(lista *l)
{
    Nodo *aux = *l;
    *l = (*l)->next;
    free(aux);
}

int eliminaPrenotazione(lista *l, int IDclient, int fila, int numero)
{
    int trovato = 0;
    while (*l)
    {
        if (((*l)->dato.IDclient == IDclient) && ((*l)->dato.ID == ((fila * 10) + numero) - 10))
        {
            elimTesta(l);
            trovato = 1;
            break;
        }

        l = &(*l)->next;
    }
    if (trovato == 1)
    {

        return trovato;
    }
    else
        return trovato;
}

char *elaboraRisposta(risposta *Risposta, messaggio Messaggio)
{
    //risposta Risposta_output;
    char *msg = malloc(sizeof(char) * DIM);
    int k;
    int i;

    if (strncmp("ERRORE_DATA", Messaggio.parola, 11) == 0) //se la data non è nel formato corretto ritorna un errore
    {
        strncpy(msg, "Data inserita in un formato non corretto.\n", sizeof(char) * DIM);
        //strncpy(Risposta_output.msg, msg, sizeof(char) * DIM);
        //return Risposta_output;
        return msg;
    }

    if ((strncmp("BOOK", Messaggio.parola, 4) == 0) && (Messaggio.nparole == 1)) //scrive solo BOOK
    {
        if (Risposta->ombrelloni_liberi == 0)
        {
            strncpy(msg, "NAVAILABLE\n", sizeof(char) * DIM); //risponde nok se non ci sono ombrelloni liberi
        }
        else
            strncpy(msg, "OK\n", sizeof(char) * DIM); //risponde ok se va tutto bene
    }
    else if ((strncmp("BOOK", Messaggio.parola, 4) == 0) && (Messaggio.nparole == 3)) //scrive BOOK e fila e numero ombrellone
    {
        if (Messaggio.fila > 10 || Messaggio.ombrellone > 10)
        {
            strncpy(msg, "Fila oppure numero ombrellone inesistente, scrivere un numero da 1 a 10\n", sizeof(char) * DIM);
        }
        else
        {
            if (Risposta->Ombrellone[Messaggio.ID].disponibile == 0) //se l'ombrellone richiesto è libero, scrivo temp. occupato e risponde available
            {
                Risposta->Ombrellone[Messaggio.ID].disponibile = 4;
                Risposta->ombrelloni_liberi--;
                strncpy(msg, "AVAILABLE\nPER CONFERMARE SCRIVERE CONFERMO FILA NUMERO, PER ANNULLARE SCRIVERE NCONFERMO FILA NUMERO \n", sizeof(char) * DIM);
            }
            else
                strncpy(msg, "NAVAILABLE\n", sizeof(char) * DIM); //ombrellone occupato
        }
    }

    else if ((strncmp("BOOK", Messaggio.parola, 4) == 0) && (Messaggio.nparole == 5)) //prenotazione per il futuro, scrive BOOK fila numero e le 2 date
    {

        if (ricerca(&Risposta->lista, ((Messaggio.fila * 10) + Messaggio.ombrellone) - 10, Messaggio.data_inizio, Messaggio.data_fine) == 0)
        {
            Risposta->Ombrellone[Messaggio.ID].disponibile = 4;
            strncpy(msg, "AVAILABLE\nPER CONFERMARE SCRIVERE CONFERMO FILA NUMERO DATAINIZIO DATAFINE, PER ANNULLARE SCRIVERE NCONFERMO FILA NUMERO \n", sizeof(char) * DIM);
        }
        else
            strncpy(msg, "NAVAILABLE\n", sizeof(char) * DIM); //ombrellone occupato
    }
    else if ((strncmp("CONFERMO", Messaggio.parola, 8) == 0) && (Messaggio.nparole == 3))
    {

        if (Risposta->Ombrellone[Messaggio.ID].disponibile == 4)
        {
            Risposta->Ombrellone[Messaggio.ID].disponibile = 1;
            Risposta->Ombrellone[Messaggio.ID].IDclient = Risposta->IDclient;
            sprintf(msg, "PRENOTAZIONE CONFERMATA, IL TUO ID È: %d \n", Risposta->IDclient);
        }
        else
            strncpy(msg, "OMBRELLONE ERRATO\n", sizeof(char) * DIM);
    }
    else if ((strncmp("CONFERMO", Messaggio.parola, 8) == 0) && (Messaggio.nparole == 5))
    {

        if (Risposta->Ombrellone[Messaggio.ID].disponibile == 4)
        {
            inserimento(&Risposta->lista, Messaggio.ID, Messaggio.fila, Messaggio.ombrellone, Risposta->IDclient, Messaggio.data_inizio, Messaggio.data_fine);
            Risposta->Ombrellone[Messaggio.ID].disponibile = 0;
            sprintf(msg, "PRENOTAZIONE CONFERMATA, IL TUO ID È: %d \n", Risposta->IDclient);
        }
        else
            strncpy(msg, "OMBRELLONE ERRATO\n", sizeof(char) * DIM);
    }
    else if ((strncmp("NCONFERMO", Messaggio.parola, 9) == 0) && (Messaggio.nparole == 3))
    {
        if (Risposta->Ombrellone[Messaggio.ID].disponibile == 4)
        {
            Risposta->Ombrellone[Messaggio.ID].disponibile = 0;
            strncpy(msg, "PRENOTAZIONE TEMPORANEA ANNULLATA\n", sizeof(char) * DIM);
        }
        else
            strncpy(msg, "OMBRELLONE ERRATO\n", sizeof(char) * DIM);
    }
    else if (strncmp("AVAILABLE", Messaggio.parola, 9) == 0 && (Messaggio.nparole == 1)) //scrive available per sapere il numero di ombrelloni liberi
    {
        if (Risposta->ombrelloni_liberi == 0) //tutti occupati
        {
            strncpy(msg, "NAVAILABLE\n", sizeof(char) * DIM);
        }
        else
            sprintf(msg, "AVAILABLE %d\n", Risposta->ombrelloni_liberi); //stampa available e il numero di ombrelloni liberi
    }
    else if (strncmp("AVAILABLE", Messaggio.parola, 9) == 0 && (Messaggio.nparole == 2)) //chiede il numero di ombrelloni liberi in una fila
    {
        if (Messaggio.fila > 10)
        {
            strncpy(msg, "Fila Ombrellone inesistente, scrivere una fila da 1 a 10\n", sizeof(char) * DIM);
        }
        else
        {
            int z = 0;
            int liberi_fila[10] = {0};
            int k = 0;
            strncpy(msg, "", sizeof(char) * DIM); //Inizializzo msg così non stampa più tutta la lista
            char *voce = malloc(sizeof(char) * DIM);
            for (int i = 1; i <= 100; i++)
            {
                if (Risposta->Ombrellone[i].disponibile == 0 && Risposta->Ombrellone[i].fila == Messaggio.fila) //conta gli ombrelloni liberi in una fila e li mette in un array
                {
                    sprintf(voce, "%d ", Risposta->Ombrellone[i].numero);
                    strcat(msg, voce);
                    z++;
                }
            }
            if (z == 0) //nessuno libero
            {
                strncpy(msg, "NAVAILABLE\n", sizeof(char) * DIM);
            }
            else
            {
                strcat(msg, "\n");
            }
        }
    }
    else if ((strncmp("CANCEL", Messaggio.parola, 6) == 0) && (Messaggio.nparole == 4))
    {
        if (Risposta->Ombrellone[Messaggio.ID].disponibile == 1)
        {
            if (Risposta->Ombrellone[Messaggio.ID].IDclient == Messaggio.IDclient)
            {
                Risposta->Ombrellone[Messaggio.ID].disponibile = 0;
                Risposta->ombrelloni_liberi++;
                Risposta->Ombrellone[Messaggio.ID].IDclient = 0;
                strncpy(msg, "CANCEL OK\n", sizeof(char) * DIM);
            }
            else
                strncpy(msg, "ID ERRATO\n", sizeof(char) * DIM);
        }
        else
            strncpy(msg, "OMBRELLONE ERRATO\n", sizeof(char) * DIM);
    }

    else if ((strncmp("CANCEL", Messaggio.parola, 6) == 0) && (Messaggio.nparole == 5))
    {
        if (eliminaPrenotazione(&Risposta->lista, Messaggio.IDclient, Messaggio.fila, Messaggio.ombrellone) == 1)
        {

            strncpy(msg, "CANCEL OK\n", sizeof(char) * DIM);
        }
        else
            strncpy(msg, "PRENOTAZIONE INESISTENTE, O ID ERRATO\n", sizeof(char) * DIM);
    }

    /*else if (Messaggio.ombrellone > 10)           //controllo se sono corretti i dati immessi
    {
        strncpy(msg, "Numero Ombrellone inesistente, scrivere un numero da 1 a 10\n", sizeof(char) * DIM);
    }
    else if (Messaggio.fila > 10)
    {
        strncpy(msg, "Fila Ombrellone inesistente, scrivere una fila da 1 a 10\n", sizeof(char) * DIM);
    }*/
    else if (strncmp("EXIT", Messaggio.parola, 4) == 0)
    {
        strncpy(msg, "EXIT", sizeof(char) * DIM);
    }
    else
    {
        strncpy(msg, "Messaggio non valido, scrivere di nuovo\n", sizeof(char) * DIM);
    }
    ///////assegno tutti i valori alla varialbile di ritorno della funzione
    /*strncpy(Risposta_output.msg, msg, sizeof(char) * DIM);
    Risposta_output.ombrelloni_liberi = Risposta->ombrelloni_liberi;
    Risposta_output.IDclient = Risposta->IDclient;
    Risposta_output.lista = Risposta->lista;
    for (i = 1; i <= 100; i++)
    {
        Risposta_output.Ombrellone[i] = Risposta->Ombrellone[i];
    }
    return Risposta->output;*/
    return msg;

    //printf("Dopo strncpy msg: %s Risposta->msg: %s\n", msg, Risposta->msg);
    //return Risposta->msg;
}

int main()
{
    //messaggio Messaggio;
    //ombrellone Ombrellone[100];
    //risposta Risposta;
    char frase[DIM] = {0};
    char msg[DIM] = {0};
    int ombrelloni_liberi = 0;
    FILE *f_ombrelloni, *f_prenotazioni;
    int i = 1;
    int ID, fila, numero, IDclient, data_inizio, data_fine;

    crealista(&Risposta.lista);

    memset(&Risposta, 0, sizeof(Risposta));

    if ((f_ombrelloni = fopen("ombrelloni.txt", "r")) == NULL)
    {
        printf("Errore nell'apertura del file ombrelloni.\n");
        exit(-1);
    }
    else
        printf("File ombrelloni aperto correttamente.\n");

    if ((f_prenotazioni = fopen("prenotazioni.txt", "r")) == NULL)
    {
        printf("Errore nell'apertura del file prenotazioni.\n");
        exit(-1);
    }
    else
        printf("File prenotazioni aperto correttamente.\n");

    while (!feof(f_ombrelloni))
    {
        if (fscanf(f_ombrelloni, "%d %d %d %d %d", &Risposta.Ombrellone[i].ID, &Risposta.Ombrellone[i].fila, &Risposta.Ombrellone[i].numero, &Risposta.Ombrellone[i].disponibile, &Risposta.Ombrellone[i].IDclient) == 5)
        {
            if (Risposta.Ombrellone[i].disponibile == 0)
            {
                Risposta.ombrelloni_liberi++;
            }
            i++;
        }
    }

    while (!feof(f_prenotazioni))
    {
        if (fscanf(f_prenotazioni, "%d %d %d %d %d %d", &ID, &fila, &numero, &IDclient, &data_inizio, &data_fine) == 6)
        {
            inserimento(&Risposta.lista, ID, fila, numero, IDclient, data_inizio, data_fine);
        }
    }
    fclose(f_ombrelloni);
    fclose(f_prenotazioni);

    srand(time(0));
    int id = 1 + rand() % 1000;
    int ombrellone_attuale = 0;
    Risposta.IDclient = id;
    printf("Il tuo ID è: %d\n", Risposta.IDclient);
    printf("Inserisci la frase: ");
    fgets(frase, sizeof(frase), stdin);
    Messaggio = dividiFrase(frase);
    if (Messaggio.nparole > 1 && (strncmp("BOOK", Messaggio.parola, 4) == 0))
    {
        ombrellone_attuale = Messaggio.ID;
    }
    //Risposta = elaboraRisposta(Risposta, Messaggio);
    strncpy(msg, elaboraRisposta(&Risposta, Messaggio), sizeof(msg));
    //strncpy(msg, Risposta->msg, sizeof(msg));
    //printf("La frase è: %s \n", Risposta->msg);
    printf("La frase è: %s \n", msg);

    if ((f_ombrelloni = fopen("ombrelloni.txt", "w")) == NULL)
    {
        printf("Errore nell'apertura del file ombrelloni.\n");
        exit(-1);
    }
    else
        printf("File ombrelloni aperto correttamente.\n");

    /*if (Risposta.Ombrellone[ombrellone_attuale].disponibile == 4)
    {
        Risposta.Ombrellone[ombrellone_attuale].disponibile = 0;
    };*/

    for (i = 1; i <= 100; i++)
    {
        (fprintf(f_ombrelloni, "%d %d %d %d %d \n",
                 Risposta.Ombrellone[i].ID,
                 Risposta.Ombrellone[i].fila,
                 Risposta.Ombrellone[i].numero,
                 Risposta.Ombrellone[i].disponibile,
                 Risposta.Ombrellone[i].IDclient));
    }
    if ((f_prenotazioni = fopen("prenotazioni.txt", "w")) == NULL)
    {
        printf("Errore nell'apertura del file prenotazioni.\n");
        exit(-1);
    }
    else
        printf("File prenotazioni aperto correttamente.\n");

    stampaListaSuFile(&Risposta.lista, f_prenotazioni);

    fclose(f_ombrelloni);
    fclose(f_prenotazioni);
}