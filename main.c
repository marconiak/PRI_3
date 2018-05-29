#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct naglowek
{
    short type;
    int fileSize;
    short reserved0;
    short reserved1;
    int dataOffset;
}naglowek;

typedef struct naglowek_info
{
    int hdrSize;
    int width;
    int height;
    short planes;
    short depth;
    int compression;
    int bmpDataSize;
    int hResolution;
    int vResolution;
    int nColors;
    int nImportantColors;
}naglowek_info;

typedef struct RGB
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
}
kolory;
int menu();
int poprawnosc_wyboru();
int wielkosc();
void sortuj(int tab[],int lewy,int prawy);
int mediana(int tab[],int n);
int mini(int tab[],int n);
int maksi(int tab[],int n);
int szer,wys;

int main()
{
    FILE *plik, *plik2;
    naglowek FileInfo;
    naglowek_info PictureInfo;
    int i,j,k,m,n;
    int mianownik=0,sumaR,sumaG,sumaB;
    char szary;
    short padding;
    kolory** tablica_kolorow;


    if((plik = fopen("wejscie.bmp", "rb")))
    {
        /*wczytanie naglowka*/
        fread(&FileInfo.type, 2, 1, plik);
        fread(&FileInfo.fileSize, 4, 1, plik);
        fread(&FileInfo.reserved0, 2, 1, plik);
        fread(&FileInfo.reserved1, 2, 1, plik);
        fread(&FileInfo.dataOffset, 4, 1, plik);
        fread(&PictureInfo.hdrSize, 4, 1, plik);
        fread(&PictureInfo.width, 4, 1, plik);
        fread(&PictureInfo.height, 4, 1, plik);
        fread(&PictureInfo.planes, 2, 1, plik);
        fread(&PictureInfo.depth, 2, 1, plik);
        fread(&PictureInfo.compression, 4, 1, plik);
        fread(&PictureInfo.bmpDataSize, 4, 1, plik);
        fread(&PictureInfo.hResolution, 4, 1, plik);
        fread(&PictureInfo.vResolution, 4, 1, plik);
        fread(&PictureInfo.nColors, 4, 1, plik);
        fread(&PictureInfo.nImportantColors, 4, 1, plik);

        /*przydzielanie pamieci dla tablicy kolorow*/
        tablica_kolorow = malloc(sizeof(kolory*) * PictureInfo.height);
        for(i=0;i<PictureInfo.height;i++)
        {
            tablica_kolorow[i]=(kolory*)malloc(sizeof(kolory) * PictureInfo.width);
        }

        /*dopelnienie do 4 bitow*/
        if((3*PictureInfo.width)%4)
        {
            padding=4-(PictureInfo.width*3)%4;
        }
        else
        {
            padding=0;
        }


        /*wczytanie bitmapy do tablicy kolorow*/
        fseek(plik, FileInfo.dataOffset, SEEK_SET);/*przejscie do kolorow w plik*/
        for(i = 0; i < PictureInfo.height; i++, fseek(plik, padding, SEEK_CUR))/*wczytywanie daych z pliku do tablicy_kolorow*/
        {
            for(j=0;j<PictureInfo.width;j++)
            {
                tablica_kolorow[i][j].B = fgetc(plik);
                tablica_kolorow[i][j].G = fgetc(plik);
                tablica_kolorow[i][j].R = fgetc(plik);
            }
        }


        /*operacje na nowym pliku*/
        if((plik2=fopen("wyjscie.bmp","wb")))
        {/*kopiowanie pliku*/
            fseek(plik, 0, SEEK_END);
            j = ftell(plik);/*przypisanie konca pliku zminnej*/
            fseek(plik, 0, SEEK_SET);/*powrot na poczatek pliku*/
            for(i=0;i<j;i++)
            {
                fputc(fgetc(plik),plik2);/*przepisanie zawartosci z pliku do nowego pliku2*/
            }
            fseek(plik2,FileInfo.dataOffset,SEEK_SET);/*przejscie do poczatku danych*/

            /*wybor obrowki danych*/
            switch(menu())
            {
                case 1:/*filtr medianowy*/
                {
                    int *tabB=malloc((2*szer+1)*(2*wys+1)*sizeof(int));/*przydzielnie pamieic dla tablic kolorow pomocniczej*/
                    int *tabR=malloc((2*szer+1)*(2*wys+1)*sizeof(int));
                    int *tabG=malloc((2*szer+1)*(2*wys+1)*sizeof(int));
                    for(i=0;i<PictureInfo.height;i++,fseek(plik2,padding,SEEK_CUR))
                    {
                        for(j=0;j<PictureInfo.width;j++)
                        {
                            n=0;
                            for(k=i-wys;k<i+wys;k++)
                            {
                                for(m=j-szer;m<j+szer;m++)
                                {
                                    if(k>=0&&k<PictureInfo.height&&m>=0&&m<PictureInfo.width)
                                    {
                                        tabB[n]=tablica_kolorow[k][m].B;/*wpisanie kolorow do tablicy pomocniczej*/
                                        tabR[n]=tablica_kolorow[k][m].R;
                                        tabG[n]=tablica_kolorow[k][m].G;
                                        n++;
                                    }
                                }
                            }
                            sortuj(tabB,0,n-1);/*sortowaie aby znalesc mediane*/
                            sortuj(tabG,0,n-1);
                            sortuj(tabR,0,n-1);
                            fputc((char)(mediana(tabB,n)), plik2);/*podmiana danych w pliku*/
                            fputc((char)(mediana(tabG,n)), plik2);
                            fputc((char)(mediana(tabR,n)), plik2);
                        }
                        printf("Wykoano %d na %d operacji\n",i,PictureInfo.height);/*licznik progresu*/
                    }
                    free(tabB);/*zwolnienie pamieci dla tablic kolorow pomocniczej*/
                    free(tabG);
                    free(tabR);
                    break;
                }
                case 2:/*filtr usredniajacy*/
                {
                    for(i=0;i<PictureInfo.height;i++,fseek(plik2,padding,SEEK_CUR))
                    {
                        for(j=0;j<PictureInfo.width;j++)
                        {
                            mianownik=0;
                            sumaR=sumaG=sumaB=0;
                            for(k=i-wys;k<i+wys;k++)
                            {
                                for(m=j-szer;m<j+szer;m++)
                                {
                                    if(k>=0&&k<PictureInfo.height&&m>=0&&m<PictureInfo.width)
                                    {
                                        mianownik++;
                                        sumaR+=((tablica_kolorow[k][m].R));/*sumowanie*/
                                        sumaG+=((tablica_kolorow[k][m].G));
                                        sumaB+=((tablica_kolorow[k][m].B));
                                    }
                                }
                            }
                            if(mianownik!=0)
                            {
                                fputc((char)(sumaB/mianownik), plik2);/*nadpisanie kolorow*/
                                fputc((char)(sumaG/mianownik), plik2);
                                fputc((char)(sumaR/mianownik), plik2);
                            }

                        }
                        printf("Wykoano %d na %d\n",i,PictureInfo.height);/*licznik progresu*/
                    }
                    break;
                }
                case 3:/*filtr minimalny*/
                {
                    int *tabB=malloc((2*szer+1)*(2*wys+1)*sizeof(int));/*przydzielenie pamieci dla tablic pomocniczej kolorow*/
                    int *tabR=malloc((2*szer+1)*(2*wys+1)*sizeof(int));
                    int *tabG=malloc((2*szer+1)*(2*wys+1)*sizeof(int));
                    for(i=0;i<PictureInfo.height;i++,fseek(plik2,padding,SEEK_CUR))
                    {
                        for(j=0;j<PictureInfo.width;j++)
                        {
                            n=0;
                            for(k=i-wys;k<i+wys;k++)
                            {
                                for(m=j-szer;m<j+szer;m++)
                                {
                                    if(k>=0&&k<PictureInfo.height&&m>=0&&m<PictureInfo.width)
                                    {
                                        tabB[n]=tablica_kolorow[k][m].B;/*wpisanie do tablicy kolorow*/
                                        tabR[n]=tablica_kolorow[k][m].R;
                                        tabG[n]=tablica_kolorow[k][m].G;
                                        n++;
                                    }
                                }
                            }
                            fputc((char)(mini(tabB,n)), plik2);/*nadpisaie kolorem o najmniejszej skladowej*/
                            fputc((char)(mini(tabG,n)), plik2);
                            fputc((char)(mini(tabR,n)), plik2);
                        }
                        printf("Wykoano %d na %d operacji\n",i,PictureInfo.height);
                    }
                    free(tabB);/*uwolnienie tablic pomocniczych kolorow*/
                    free(tabG);
                    free(tabR);
                    break;
                }
                case 4:/*filtr maksymalny*/
                {
                    int *tabB=malloc((2*szer+1)*(2*wys+1)*sizeof(int));
                    int *tabR=malloc((2*szer+1)*(2*wys+1)*sizeof(int));
                    int *tabG=malloc((2*szer+1)*(2*wys+1)*sizeof(int));
                    for(i=0;i<PictureInfo.height;i++,fseek(plik2,padding,SEEK_CUR))
                    {
                        for(j=0;j<PictureInfo.width;j++)
                        {
                            n=0;
                            for(k=i-wys;k<i+wys;k++)
                            {
                                for(m=j-szer;m<j+szer;m++)
                                {
                                    if(k>=0&&k<PictureInfo.height&&m>=0&&m<PictureInfo.width)
                                    {
                                        tabB[n]=tablica_kolorow[k][m].B;
                                        tabR[n]=tablica_kolorow[k][m].R;
                                        tabG[n]=tablica_kolorow[k][m].G;
                                        n++;
                                    }
                                }
                            }
                            fputc((char)(maksi(tabB,n)), plik2);
                            fputc((char)(maksi(tabG,n)), plik2);
                            fputc((char)(maksi(tabR,n)), plik2);
                        }
                        printf("Wykoano %d na %d operacji\n",i,PictureInfo.height);
                    }
                    free(tabB);
                    free(tabG);
                    free(tabR);
                    break;
                }
                case 5:/*filtr szarosci*/
                {
                    for(i=0;i<PictureInfo.height;i++,fseek(plik2,padding,SEEK_CUR))
                    {
                        for(j=0;j<PictureInfo.width;j++)
                        {
                            szary = (char)(0.229*tablica_kolorow[i][j].R + 0.587*tablica_kolorow[i][j].G + 0.114*tablica_kolorow[i][j].B);/*jest to rownowazne liczniu sredniej artmetyczniej*/
                            fputc(szary, plik2);
                            fputc(szary, plik2);
                            fputc(szary, plik2);
                        }
                    }
                    break;
                }
                case 6:/*negatyw*/
                {
                    for(i=0;i<PictureInfo.height;i++,fseek(plik2,padding,SEEK_CUR))
                        {
                        for(j=0;j<PictureInfo.width;j++)
                            {
                                fputc(255-tablica_kolorow[i][j].B,plik2);
                                fputc(255-tablica_kolorow[i][j].G,plik2);
                                fputc(255-tablica_kolorow[i][j].R,plik2);
                            }
                        }
                    break;
                }
            }
            fclose(plik2);
            printf("Pomyslnie dokonano obrobki zdjecia. Wynik w pliku wyjscie.bmp\n");
        }
        else
        {
            printf("ERROR: nie udalo sie stworzyc pliku\n");
        }
        for(i=0;i<PictureInfo.height;i++)/*uwalnianie tablic kolorow*/
        {
            free(tablica_kolorow[i]) ;
        }
        free(tablica_kolorow);
        fclose(plik);
    }
    else
    {
        printf("ERROR: blad podczas otwierania pliku\n");
    }
    return 0;
}
int menu()
{
    while(1)
    {
        printf("wybierz opcje obrobki zdjecia:\n[1] odszumianie zdjecia\n[2] konwersja na obraz w skali szarosci\n[3] generowanie negatywu\nWybieram: ");
        switch (poprawnosc_wyboru())
        {
        case 1:
            {
                    printf("podaj szerokosc maski: \n");
                    szer=(wielkosc()-1)/2;
                    printf("podaj wysokosc maski: \n");;
                    wys=(wielkosc()-1)/2;
                    printf("wybierz opcje odszumiania zdjecia:\n[1] filtr medianowy\n[2] filtr usredniajacy\n[3] filtr minimalny\n[4] filtr maksymalny\nWybieram: ");
                    while(1)
                    {
                         switch(poprawnosc_wyboru())
                        {
                        case 1:
                            {
                                return 1;
                            }
                        case 2:
                            {
                                return 2;
                            }
                        case 3:
                            {
                                return 3;
                            }
                        case 4:
                            {
                                return 4;
                            }
                        default:
                            {
                                printf("ERROR: zla opcja. sprobuj ponownie\n");
                            }
                        }
                    }

            }
        case 2:
            {
                return 5;
            }
        case 3:
            {
                return 6;
            }
        default:
            {
                printf("ERROR: zla opcja. sprobuj ponownie\n");
            }
        }
    }
}
int poprawnosc_wyboru()/*pobiera liczbe i sprawdza czy poprawnie wprowadzona*/
{
    int znak=0,c,blad=0,cyfry=0,liczba1=0;
    while(znak!=1)
    {
        blad=0,cyfry=0;
        while((c=getchar()))
        {
            if(!blad && isdigit(c)&&(c>='0')&&(c<='9'))
            {
                liczba1*=10;
                liczba1+=c-'0';
                cyfry++;

            }/* sprawdzanie czy znak nie blad i czy z zakresu cyfr*/
            else if(c=='\n')
            {
                znak=1;
                break;
            }
            else blad=1;
        }
        if(blad||liczba1==0)
        {
        printf("blad!\n");
        znak=0;
        liczba1=0;
        }
    }
return liczba1;
}
int wielkosc()/*przypisanie warosci dla maski*/
{
    int m=poprawnosc_wyboru();
    if(m%2==0)
    {
        m--;
    }
    return m;
}
void sortuj(int tab[],int lewy,int prawy)/*Quick sort*/
{
    int i=lewy,zamien;
    int j=prawy;
    int x=tab[(lewy+prawy)/2];
    do
    {
        while(tab[i]<x)
        {
            i++;
        }
        while(tab[j]>x)
        {
            j--;
        }
        if(i<=j)
        {
            zamien=tab[i];
            tab[i]=tab[j];
            tab[j]=zamien;
            i++;
            j--;
        }
    }while(i<=j);
    if(lewy<j)
    {
        sortuj(tab,lewy,j);
    }
    if(prawy>i)
    {
        sortuj(tab,i,prawy);
    }
}
int mediana(int tab[],int n)/*zwraca mediane z tablicy*/
{
    if(n%2==1)
    {
        return (tab[(n+1)/2]);
    }
    else
    {
        return ((tab[n/2]+tab[(n/2)+1])/2);
    }
}
int mini(int tab[],int n)/*zwraca minimalna liczbe z tablicy*/
{
    int wynik=tab[0],i;
    for(i=1;i<n;i++)
    {
        if(wynik>tab[i])
        {
            wynik=tab[i];
        }
    }
    return wynik;
}
int maksi(int tab[],int n)/*zwraca maksymalna liczbe z tablicy*/
{
    int wynik=tab[0],i;
    for(i=1;i<n;i++)
    {
        if(wynik<tab[i])
        {
            wynik=tab[i];
        }
    }
    return wynik;
}
