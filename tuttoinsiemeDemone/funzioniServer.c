#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

void leggoFile(risposta *Risposta, FILE *f_ombrelloni, FILE *f_prenotazioni, FILE *f_aggiornamenti)
{
    int ID, fila, numero, IDclient, data_inizio, data_fine;
    int data;
    char update[DIM];
    aggiornamento Aggiornamento;
    messaggio Messaggio1;
    int i = 0;
    int k, n = 1;
    int j = 1;

    for (k = 1; k <= 100; k++)
    {
        Risposta->Ombrellone[k].ID = k;
        Risposta->Ombrellone[k].fila = j;
        Risposta->Ombrellone[k].numero = n;
        Risposta->Ombrellone[k].disponibile = 0;
        Risposta->Ombrellone[k].IDclient = 0;

        if (n == 10)
        {
            j++;
            n = 0;
        }
        n++;
    }

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
        if (i == 0)
        {
            if (fscanf(f_ombrelloni, "%d", &data) == 1)
                i++;
        }
        else
        {
            if (data == Risposta->data_oggi)
            {
                if (fscanf(f_ombrelloni, "%d %d %d %d %d", &Risposta->Ombrellone[i].ID, &Risposta->Ombrellone[i].fila, &Risposta->Ombrellone[i].numero, &Risposta->Ombrellone[i].disponibile, &Risposta->Ombrellone[i].IDclient) == 5)
                {
                    i++;
                }
            }
            else
                break;
        }
    }

    while (!feof(f_prenotazioni))
    {
        if (fscanf(f_prenotazioni, "%d %d %d %d %d %d", &ID, &fila, &numero, &IDclient, &data_inizio, &data_fine) == 6)
        {
            if (Risposta->data_oggi < data_fine)
            {
                if (data_inizio <= Risposta->data_oggi)
                {
                    inserimento(&Risposta->lista, ID, fila, numero, IDclient, Risposta->data_oggi, data_fine);
                    Risposta->Ombrellone[ID].disponibile = 1;
                    Risposta->Ombrellone[ID].IDclient = IDclient;
                }
                else
                    inserimento(&Risposta->lista, ID, fila, numero, IDclient, data_inizio, data_fine);
            }
        }
    }
    for (k = 1; k <= 100; k++)
    {
        if (Risposta->Ombrellone[k].disponibile == 0)
        {
            Risposta->ombrelloni_liberi++;
        }
    }

    if ((f_aggiornamenti = fopen("aggiornamenti.txt", "r")) == NULL)
    {
        printf("Errore nell'apertura del file aggiornamenti.\n");
        exit(-1);
    }
    else
        printf("File aggiornamenti aperto correttamente.\n");

    printf("Sto controllando se ci sono aggiornamenti.\n");
    while (1)
    {
        if (fgets(update, DIM, f_aggiornamenti) == NULL)
        {
            break;
        }
        else
        {
            Aggiornamento = dividiAggiornamento(update);
            Messaggio1 = dividiFrase(Aggiornamento.parola);
            Risposta->IDclient = Aggiornamento.IDCLient;
            elaboraRisposta(Risposta, Messaggio1);
        }
    }
    fclose(f_ombrelloni);
    fclose(f_prenotazioni);
    fclose(f_aggiornamenti);
}

int controlloData(int giorno, int mese, int anno)
{
    if (giorno < 0 || mese < 0 || anno < 0 || giorno > 31 || mese > 12)
    {
        return 0;
    }
    if (mese == 2)
    {
        if ((anno % 400 == 0 || ((anno % 4 == 0) && (anno % 100 != 0))) && (giorno > 29))
        {
            return 0;
        }
        else if (giorno > 28)
        {
            return 0;
        }
    }
    if ((mese == 4) || (mese == 6) || ((mese == 9)) || (mese == 11))
    {
        if (giorno > 30)
        {
            return 0;
        }
    }
    if ((mese == 1) || (mese == 3) || (mese == 5) || (mese == 7) || (mese == 8) || (mese == 10) || (mese == 12))
    {
        if (giorno > 31)
        {
            return 0;
        }
    }

    return 1;
}

int uniscidata(char data[])
{
    int giorno, mese, anno;
    int data_fine;
    int i = 0;

    char *GIORNO, *MESE, *ANNO;

    char *token;
    token = strtok(data, "/");
    while (token != NULL)
    {
        if (i == 0)
        {
            GIORNO = token;
        }
        if (i == 1)
        {
            MESE = token;
        }
        if (i == 2)
        {
            ANNO = token;
        }
        token = strtok(NULL, "/");

        i++;
    }

    giorno = atoi(GIORNO);
    mese = atoi(MESE);
    anno = atoi(ANNO);

    if (controlloData(giorno, mese, anno) == 0)
    {
        data_fine = -1;
    }
    else
    {
        strcat(ANNO, MESE);
        strcat(ANNO, GIORNO);
        data_fine = atoi(ANNO);
    }

    return data_fine;
}

aggiornamento dividiAggiornamento(char msg[])
{
    aggiornamento Aggiornamento;
    char numero[3];
    int k = 0;
    int i = 0;
    while (msg[k] != '-')
    {
        numero[k] = msg[k];
        k++;
    }
    k++;
    Aggiornamento.IDCLient = atoi(numero);

    while (msg[k] != '\n')
    {
        Aggiornamento.parola[i] = msg[k];
        k++;
        i++;
    }
    return Aggiornamento;
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
    if ((Messaggio.nparole == 4) && (strncmp("AVAILABLE", Messaggio.parola, 9) == 0))
    {
        if (frase[2][2] != '/' || frase[2][5] != '/' || frase[3][2] != '/' || frase[3][5] != '/') //controllo se la data è nel formato corretto
        {
            strcpy(Messaggio.parola, "ERRORE_DATA");
            return Messaggio;
        }
        Messaggio.data_inizio = uniscidata(frase[2]);
        Messaggio.data_fine = uniscidata(frase[3]);
        return Messaggio;
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
    if ((Messaggio.nparole == 4) && (strncmp("BOOK", Messaggio.parola, 4) == 0))
    {
        if (frase[3][2] != '/' || frase[3][5] != '/') //controllo se la data è nel formato corretto
        {
            strcpy(Messaggio.parola, "ERRORE_DATA");
            return Messaggio;
        }
        Messaggio.data_fine = uniscidata(frase[3]);
        return Messaggio;
    }
    if ((Messaggio.nparole == 4) && (strncmp("CONFERMO", Messaggio.parola, 8) == 0))
    {
        if (frase[3][2] != '/' || frase[3][5] != '/') //controllo se la data è nel formato corretto
        {
            strcpy(Messaggio.parola, "ERRORE_DATA");
            return Messaggio;
        }
        Messaggio.data_fine = uniscidata(frase[3]);
        return Messaggio;
    }
    if ((Messaggio.nparole == 4) && (strncmp("NCONFERMO", Messaggio.parola, 9) == 0))
    {
        if (frase[3][2] != '/' || frase[3][5] != '/') //controllo se la data è nel formato corretto
        {
            strcpy(Messaggio.parola, "ERRORE_DATA");
            return Messaggio;
        }
        Messaggio.data_fine = uniscidata(frase[3]);
        return Messaggio;
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
        if (frase[4][2] != '/' || frase[4][5] != '/' || frase[5][2] != '/' || frase[5][5] != '/') //controllo se la data è nel formato corretto
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

char *ricercaAvailable(lista *l, int fila, int datainizio, int datafine)
{
    int z = 0;
    int liberi_fila[11] = {0};
    int k = 0;
    char *stringa = malloc(sizeof(char) * DIM);
    char *voce = malloc(sizeof(char) * DIM);

    for (int n = 1; n < 11; n++)
    {
        liberi_fila[n] = n;
    }

    while (*l)
    {
        if ((*l)->dato.fila == fila)
        {
            if (confrontoDate((*l)->dato.data_inizio, (*l)->dato.data_fine, datainizio, datafine) == 1)
            {
                liberi_fila[((*l)->dato.numero)] = 0;
            }
        }
        l = &(*l)->next;
    }

    for (int i = 1; i < 11; i++)
    {
        if (liberi_fila[i] != 0)
        {
            sprintf(voce, "%d ", liberi_fila[i]);
            strcat(stringa, voce);
            z++;
        }
    }
    if (z == 0) //nessuno libero
    {
        strncpy(stringa, "NAVAILABLE\n", sizeof(char) * DIM);
    }
    else
    {
        strcat(stringa, "\n");
    }
    return stringa;
}

char *ricercaAvailableNumero(lista *l, int datainizio, int datafine)
{
    int z = 0;
    int liberi_fila[101] = {0};
    int k = 0;
    char *stringa = malloc(sizeof(char) * DIM);
    char *voce = malloc(sizeof(char) * DIM);

    for (int n = 1; n < 101; n++)
    {
        liberi_fila[n] = n;
    }

    while (*l)
    {
        if (confrontoDate((*l)->dato.data_inizio, (*l)->dato.data_fine, datainizio, datafine) == 1)
        {
            liberi_fila[((*l)->dato.ID)] = 0;
        }

        l = &(*l)->next;
    }
    for (int i = 1; i < 101; i++)
    {
        if (liberi_fila[i] != 0)
        {
            z++;
        }
    }
    if (z == 0) //nessuno libero
    {
        strncpy(stringa, "NAVAILABLE\n", sizeof(char) * DIM);
    }
    else
    {
        sprintf(stringa, "AVAILABLE %d\n", z);
    }
    return stringa;
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

int eliminaPrenotazione(lista *l, int IDclient, int fila, int numero, int data_inizio)
{
    int trovato = 0;
    while (*l)
    {
        if (((*l)->dato.IDclient == IDclient) && ((*l)->dato.ID == ((fila * 10) + numero) - 10) && ((*l)->dato.data_inizio == data_inizio))
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
    char *msg = malloc(sizeof(char) * DIM);
    strncpy(msg, "", sizeof(char) * DIM);
    int k;
    int i;

    if ((Messaggio.data_fine == -1) || (Messaggio.data_inizio == -1)) //se la data non è nel formato corretto ritorna un errore
    {
        strncpy(msg, "Data inesistente.\n", sizeof(char) * DIM);
        return msg;
    }
    if ((Messaggio.data_inizio = !0) && (Messaggio.data_fine != 0))
    {
        if ((Messaggio.nparole > 3) && ((Messaggio.data_fine < Risposta->data_oggi) || (Messaggio.data_inizio < Risposta->data_oggi))) //se la data non è nel formato corretto ritorna un errore
        {
            strncpy(msg, "Inserita una data precedente alla data odierna.\n", sizeof(char) * DIM);
            return msg;
        }
    }

    if (strncmp("ERRORE_DATA", Messaggio.parola, 11) == 0) //se la data non è nel formato corretto ritorna un errore
    {
        strncpy(msg, "Data inserita in un formato non corretto.\n", sizeof(char) * DIM);
        return msg;
    }
    if ((Messaggio.data_inizio = !0) && (Messaggio.data_fine != 0))
    {
        if (Messaggio.data_fine < Messaggio.data_inizio) //se la data non è nel formato corretto ritorna un errore
        {
            strncpy(msg, "Data di fine precedente alla data di inizio.\n", sizeof(char) * DIM);
            return msg;
        }
    }

    if (Messaggio.ombrellone > 10) //controllo se sono corretti i dati immessi
    {
        strncpy(msg, "Numero Ombrellone inesistente, scrivere un numero da 1 a 10\n", sizeof(char) * DIM);
        return msg;
    }

    if (Messaggio.fila > 10)
    {
        strncpy(msg, "Fila Ombrellone inesistente, scrivere una fila da 1 a 10\n", sizeof(char) * DIM);
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
            {
                if (Risposta->Ombrellone[Messaggio.ID].disponibile == 4)
                    strncpy(msg, "OMBRELLONE TEMPORANEAMENTE OCCUPATO\n", sizeof(char) * DIM); //ombrellone occupato
                else
                    strncpy(msg, "NAVAILABLE\n", sizeof(char) * DIM);
            }
        }
    }
    else if ((strncmp("BOOK", Messaggio.parola, 4) == 0) && (Messaggio.nparole == 4)) //scrive BOOK e fila e numero ombrellone
    {
        if (Messaggio.fila > 10 || Messaggio.ombrellone > 10)
        {
            strncpy(msg, "Fila oppure numero ombrellone inesistente, scrivere un numero da 1 a 10\n", sizeof(char) * DIM);
        }
        else
        {
            if (Risposta->ombrelloni_Toccupati[Messaggio.ID] == 0)
            {
                Risposta->ombrelloni_Toccupati[Messaggio.ID] = 1;
                if ((ricerca(&Risposta->lista, ((Messaggio.fila * 10) + Messaggio.ombrellone) - 10, Risposta->data_oggi, Messaggio.data_fine) == 0) && (Risposta->Ombrellone[Messaggio.ID].disponibile == 0))
                {
                    Risposta->Ombrellone[Messaggio.ID].disponibile = 4;
                    Risposta->ombrelloni_liberi--;
                    strncpy(msg, "AVAILABLE\nPER CONFERMARE SCRIVERE CONFERMO FILA NUMERO DATAFINE, PER ANNULLARE SCRIVERE NCONFERMO FILA NUMERO DATAFINE \n", sizeof(char) * DIM);
                }
                else
                {
                    strncpy(msg, "NAVAILABLE\n", sizeof(char) * DIM); //ombrellone occupato
                    Risposta->ombrelloni_Toccupati[Messaggio.ID] = 0;
                }
            }
            else
            {
                strncpy(msg, "OMBRELLONE TEMPORANEAMENTE OCCUPATO\n", sizeof(char) * DIM);
            }
        }
    }

    else if ((strncmp("BOOK", Messaggio.parola, 4) == 0) && (Messaggio.nparole == 5)) //prenotazione per il futuro, scrive BOOK fila numero e le 2 date
    {
        if (Risposta->ombrelloni_Toccupati[Messaggio.ID] == 0)
        {
            Risposta->ombrelloni_Toccupati[Messaggio.ID] = 1;
            if (ricerca(&Risposta->lista, ((Messaggio.fila * 10) + Messaggio.ombrellone) - 10, Messaggio.data_inizio, Messaggio.data_fine) == 0)
            {

                strncpy(msg, "AVAILABLE\nPER CONFERMARE SCRIVERE CONFERMO FILA NUMERO DATAINIZIO DATAFINE, PER ANNULLARE SCRIVERE NCONFERMO FILA NUMERO DATAINIZIO DATAFINE \n", sizeof(char) * DIM);
            }
            else
            {
                strncpy(msg, "NAVAILABLE\n", sizeof(char) * DIM); //ombrellone occupato
                Risposta->ombrelloni_Toccupati[Messaggio.ID] = 0;
            }
        }
        else
        {
            strncpy(msg, "OMBRELLONE TEMPORANEAMENTE OCCUPATO\n", sizeof(char) * DIM);
        }
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
    else if ((strncmp("CONFERMO", Messaggio.parola, 8) == 0) && (Messaggio.nparole == 4))
    {
        if ((Risposta->Ombrellone[Messaggio.ID].disponibile == 4) && (Risposta->ombrelloni_Toccupati[Messaggio.ID] == 1))
        {
            if (ricerca(&Risposta->lista, ((Messaggio.fila * 10) + Messaggio.ombrellone) - 10, Risposta->data_oggi, Messaggio.data_fine) == 0)
            {
                inserimento(&Risposta->lista, Messaggio.ID, Messaggio.fila, Messaggio.ombrellone, Risposta->IDclient, Risposta->data_oggi, Messaggio.data_fine);
                Risposta->ombrelloni_Toccupati[Messaggio.ID] = 0;
                Risposta->Ombrellone[Messaggio.ID].disponibile = 1;
                Risposta->Ombrellone[Messaggio.ID].IDclient = Risposta->IDclient;
                sprintf(msg, "PRENOTAZIONE CONFERMATA, IL TUO ID È: %d \n", Risposta->IDclient);
            }
        }
        else
            strncpy(msg, "OMBRELLONE ERRATO\n", sizeof(char) * DIM);
    }
    else if ((strncmp("CONFERMO", Messaggio.parola, 8) == 0) && (Messaggio.nparole == 5))
    {
        if (Risposta->ombrelloni_Toccupati[Messaggio.ID] == 1)
        {
            if (ricerca(&Risposta->lista, ((Messaggio.fila * 10) + Messaggio.ombrellone) - 10, Messaggio.data_inizio, Messaggio.data_fine) == 0)
            {
                inserimento(&Risposta->lista, Messaggio.ID, Messaggio.fila, Messaggio.ombrellone, Risposta->IDclient, Messaggio.data_inizio, Messaggio.data_fine);
                sprintf(msg, "PRENOTAZIONE CONFERMATA, IL TUO ID È: %d \n", Risposta->IDclient);
                Risposta->ombrelloni_Toccupati[Messaggio.ID] = 0;
            }
            else
                strncpy(msg, "ERRORE NELLA DATA O NELL'OMBRELLONE\n", sizeof(char) * DIM);
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
    else if ((strncmp("NCONFERMO", Messaggio.parola, 9) == 0) && (Messaggio.nparole == 4))
    {
        if ((Risposta->Ombrellone[Messaggio.ID].disponibile == 4) && (Risposta->ombrelloni_Toccupati[Messaggio.ID] == 1))
        {
            Risposta->ombrelloni_Toccupati[Messaggio.ID] = 0;
            Risposta->Ombrellone[Messaggio.ID].disponibile = 0;
            strncpy(msg, "PRENOTAZIONE TEMPORANEA ANNULLATA\n", sizeof(char) * DIM);
        }
        else
            strncpy(msg, "OMBRELLONE ERRATO\n", sizeof(char) * DIM);
    }
    else if ((strncmp("NCONFERMO", Messaggio.parola, 9) == 0) && (Messaggio.nparole == 5))
    {
        if (Risposta->ombrelloni_Toccupati[Messaggio.ID] == 1)
        {
            Risposta->ombrelloni_Toccupati[Messaggio.ID] = 0;
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
    else if (strncmp("AVAILABLE", Messaggio.parola, 9) == 0 && (Messaggio.nparole == 4)) //scrive available per sapere il numero di ombrelloni liberi
    {
        if (Messaggio.fila > 10)
        {
            strncpy(msg, "Fila Ombrellone inesistente, scrivere una fila da 1 a 10\n", sizeof(char) * DIM);
        }
        else
        {
            strncpy(msg, ricercaAvailable(&Risposta->lista, Messaggio.fila, Messaggio.data_inizio, Messaggio.data_fine), sizeof(char) * DIM);
        }
    }
    else if (strncmp("AVAILABLE", Messaggio.parola, 9) == 0 && (Messaggio.nparole == 3)) //scrive available per sapere il numero di ombrelloni liberi
    {
        if (Messaggio.fila > 10)
        {
            strncpy(msg, "Fila Ombrellone inesistente, scrivere una fila da 1 a 10\n", sizeof(char) * DIM);
        }
        else
        {
            strncpy(msg, ricercaAvailableNumero(&Risposta->lista, Messaggio.data_inizio, Messaggio.data_fine), sizeof(char) * DIM);
        }
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
        if (eliminaPrenotazione(&Risposta->lista, Messaggio.IDclient, Messaggio.fila, Messaggio.ombrellone, Messaggio.data_fine) == 1)
        {

            strncpy(msg, "CANCEL OK\n", sizeof(char) * DIM);
        }
        else
            strncpy(msg, "PRENOTAZIONE INESISTENTE, O ID ERRATO\n", sizeof(char) * DIM);
    }
    else if (strncmp("EXIT", Messaggio.parola, 4) == 0)
    {
        strncpy(msg, "USCITA", sizeof(char) * DIM);
    }
    else
    {
        strncpy(msg, "Messaggio non valido, scrivere di nuovo\n", sizeof(char) * DIM);
    }

    return msg;
}