#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int ID;
    int fila;
    int numero;
    int data_inizio;
    int data_fine;

} Ombrellone;

int main()
{

    Ombrellone ombrellone[100] = {0};

    int i, n = 1;
    int j = 1;

    FILE *f;

    if ((f = fopen("prenotazioni.txt", "w")) == NULL)
    {
        printf("Errore apertura file\n");
        exit(-1);
    }

    for (i = 0; i < 100; i++)
    {
        ombrellone[i].ID = i + 1;
        ombrellone[i].fila = j;
        ombrellone[i].numero = n;
        ombrellone[i].data_inizio = 0;
        ombrellone[i].data_fine = 0;

        if (n == 10)
        {
            j++;
            n = 0;
        }
        n++;
    }

    for (i = 0; i < 100; i++)
    {
        (fprintf(f, "%d %d %d %d %d \n",
                 ombrellone[i].ID,
                 ombrellone[i].fila,
                 ombrellone[i].numero,
                 ombrellone[i].data_inizio,
                 ombrellone[i].data_fine));
        if (n == 10)
        {
            j++;
            n = 0;
        }
        n++;
    }

    if (fclose(f))
    {
        printf("Errore chiusura file\n");
        exit(-2);
    }
    return 0;
}