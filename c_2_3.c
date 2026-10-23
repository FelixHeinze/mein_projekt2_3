
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>./
#include <ctype.h>
#include <time.h>   /* time(), clock() und CLOCKS_PER_SEC */
#include <limits.h> 

/* Liest eine ganze Zahl sicher aus einer Zeile ein.
 * prompt: Text, der vor der Eingabe angezeigt wird.
 * result: Zeiger auf die Variable, in der die Zahl gespeichert werden soll.
 * Rueckgabewert 1 = Eingabe war gueltig, 0 = Eingabe ungueltig/EOF.
 * fgets() ist hier robuster als scanf(), weil wir die komplette Zeile pruefen. 
 
 kommentare ki generiert aus einfachen stichpunkten*/
int read_int(const char *prompt, int *result)
{
    char line[128], *end;
    long value;
    printf("%s", prompt);
    if (!fgets(line, sizeof line, stdin)) return 0;
    /* strtol wandelt den Anfang der Zeile in eine Zahl um. end zeigt danach
       auf das erste Zeichen, das nicht mehr zur Zahl gehoert. */
    value = strtol(line, &end, 10);
    /* Leerzeichen und Zeilenende nach der Zahl sind erlaubt. Andere Zeichen
       (z. B. "12abc") machen die Eingabe ungueltig. */
    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n') end++;
    if (end == line || *end != '\0' || value < INT_MIN || value > INT_MAX) {
        printf("Ungueltige Eingabe. Bitte eine ganze Zahl eingeben.\n");
        return 0;
    }
    *result = (int)value;
    return 1;
}
/* Verhindert, dass die Ausgabe sofort vom Menue ueberschrieben wird.
 * fgets wartet auf ENTER und konsumiert zugleich den Zeilenumbruch. */
void pause_menu(void) {
    char line[8];
    printf("\nENTER druecken, um zum Menue zurueckzukehren...");
    (void)fgets(line, sizeof line, stdin);
}

/* TEIL 2 / Aufgabe 1: gerade Zahlen von 20 bis 2 absteigend. */
/* for-Schleife: Startwert 20; solange n >= 2; nach jedem Durchlauf -2.
 * Dadurch werden ausschliesslich gerade Zahlen und zwar absteigend ausgegeben. */
void task1(void) {
    int n;
    printf("\nGerade Zahlen absteigend:\n");
    for (n = 20; n >= 2; n -= 2) printf("%d ", n);
    printf("\n");
}
/* Aufgabe 2: in ein zweites Array rueckwaerts kopieren und in-place drehen. */
/* Zwei Varianten zum Umdrehen eines Arrays:
 * 1) Rueckwaerts in ein zweites Array kopieren.
 * 2) Elemente des Originalarrays paarweise tauschen.
 * sizeof a / sizeof a[0] berechnet die Anzahl der Arrayelemente. */
void task2(void) {
    int a[] = {4, 8, 15, 16, 23, 42};
    int n = (int)(sizeof a / sizeof a[0]), b[6], i;
    printf("Original: ");
    for (i=0; i<n; i++) printf("%d ", a[i]);
    /* Das erste Element von b kommt vom letzten Element von a usw.
       n-1-i ist der rueckwaerts laufende Index. */
    for (i=0; i<n; i++) b[i] = a[n-1-i];
    printf("\nRueckwaerts in neuem Array: ");
    for (i=0; i<n; i++) printf("%d ", b[i]);
    /* In-place: nur die erste Haelfte durchlaufen, sonst wuerden die bereits
       getauschten Werte erneut vertauscht. temp bewahrt einen Wert beim Tausch. */
    for (i=0; i<n/2; i++) {
        int temp = a[i]; a[i] = a[n-1-i]; a[n-1-i] = temp;
    }
    printf("\nOriginalarray direkt umgedreht: ");
    for (i=0; i<n; i++) printf("%d ", a[i]);
    printf("\n");
}
/* Aufgabe 3: Zaehler fuer Werte 0 bis 9. */
/* f ist ein Zaehlerarray mit zehn Plaetzen: f[0] zaehlt Nullen,
 * f[1] Einsen usw. Alle Zaehler starten bei 0. */
void task3(void) {
    int a[] = {1,3,2,4,0,9,1,1,4}, f[10] = {0}, i;
    /* Fuer jeden Wert a[i] wird der passende Zaehler um eins erhoeht.
       Das Beispielarray enthaelt ausschliesslich Werte von 0 bis 9. */
    for (i=0; i<(int)(sizeof a/sizeof a[0]); i++) f[a[i]]++;
    printf("Haeufigkeiten:\n");
    for (i=0; i<10; i++) printf("%d: %d-mal\n", i, f[i]);
}
/* Aufgabe 4: Zeilen-, Spalten- und Diagonalsummen einer 4x4-Matrix. */
/* Eine Matrix ist ein zweidimensionales Array. m[r][c] bedeutet Zeile r,
 * Spalte c. In C beginnen beide Indizes bei 0. */
void task4(void) {
    int m[4][4]={{2,3,5,7},{11,13,17,19},{23,29,31,37},{41,43,47,53}};
    int r,c,s,d1=0,d2=0;
    /* Zeilensumme: c laeuft innerhalb einer festen Zeile r. */
    for(r=0;r<4;r++){ s=0; for(c=0;c<4;c++) s+=m[r][c]; printf("Zeile %d: %d\n",r+1,s); }
    /* Spaltensumme: r laeuft innerhalb einer festen Spalte c. */
    for(c=0;c<4;c++){ s=0; for(r=0;r<4;r++) s+=m[r][c]; printf("Spalte %d: %d\n",c+1,s); }
    /* Hauptdiagonale: Zeilen- und Spaltenindex sind gleich.
       Gegendiagonale: die Spalte laeuft von 3 nach 0. */
    for(r=0;r<4;r++){ d1+=m[r][r]; d2+=m[r][3-r]; }
    printf("Hauptdiagonale: %d\nGegendiagonale: %d\n",d1,d2);
}
/* Aufgabe 5: Minimum, Maximum und Mittelwert. */
/* Minimum und Maximum werden mit dem ersten Arraywert initialisiert.
 * So funktionieren auch Arrays, deren Werte alle negativ sind. */
void task5(void) {
    int a[]={9,23,-1,0,3,16,2,1,-2,4,8}, n=11, i, min=a[0], max=a[0], sum=0;
    /* Jeder Wert wird mit dem bisher kleinsten/groessten verglichen und
       zugleich zur Gesamtsumme addiert. Cast auf double erzwingt Dezimaldivision. */
    for(i=0;i<n;i++){ if(a[i]<min)min=a[i]; if(a[i]>max)max=a[i]; sum+=a[i]; }
    printf("Minimum: %d\nMaximum: %d\nDurchschnitt: %.2f\n",min,max,(double)sum/n);
}
/* Aufgabe 6: Dreieck mit 1, 3, 5, 7, 9 Rauten. */
/* Aussen-Schleife erzeugt die fuenf Zeilen. Die erste innere Schleife setzt
 * fuehrende Leerzeichen; die zweite druckt 1, 3, 5, 7 bzw. 9 #-Zeichen. */
void task6(void) {
    int r,c;
    for(r=1;r<=5;r++){
        for(c=0;c<5-r;c++) putchar(' ');
        for(c=0;c<2*r-1;c++) putchar('#');
        putchar('\n');
    }
}
#define W 79
#define H 24
/* Gibt das Spielfeld zeilenweise aus. H und W sind Konstanten fuer Hoehe
 * und Breite; jedes Feldzeichen wird direkt ausgegeben. */
void print_grid(char g[H][W]) {
    int y,x; for(y=0;y<H;y++){for(x=0;x<W;x++)putchar(g[y][x]);putchar('\n');}
}
/* Wartet ungefaehr milliseconds Millisekunden, ohne Windows-Header zu benoetigen.
   Diese einfache Variante belegt waehrend der Pause Rechenzeit. */
/* Wartet ungefaehr milliseconds Millisekunden, ohne Windows-Header zu benoetigen.
 * clock() misst CPU-Zeit. Diese einfache Busy-Wait-Loesung ist portabel, belegt
 * waehrend der Pause aber Rechenzeit und ist nicht fuer Echtzeit geeignet. */
void wait_ms(unsigned int milliseconds) {
    clock_t start = clock();
    clock_t ticks = (clock_t)((double)milliseconds * CLOCKS_PER_SEC / 1000.0);
    while (clock() - start < ticks) { /* warten */ }
}
/* Aufgabe 7: Game of Life. Mit STRG+C kann die Dauerschleife beendet werden. */
/* Conway's Game of Life: '@' ist lebendig, ein Leerzeichen ist tot.
 * cur enthaelt die aktuelle Generation, next die berechnete Folgegeneration.
 * Beide Arrays sind wichtig: Alle neuen Zustaende werden aus derselben alten
 * Generation berechnet, bevor cur aktualisiert wird. */
void task7(void) {
    char cur[H][W], next[H][W];
    int x,y,dx,dy,n,gen;
    /* time(NULL) liefert einen zeitabhaengigen Startwert fuer Zufallszahlen.
       srand() sollte normalerweise nur einmal zu Programmbeginn aufgerufen werden. */
    srand((unsigned)time(NULL));
    /* rand()%100 liefert 0 bis 99. Werte unter 30 ergeben etwa 30 % lebendige
       Felder; die restlichen Felder werden als Leerzeichen angelegt. */
    for(y=0;y<H;y++)for(x=0;x<W;x++)cur[y][x]=(rand()%100<30)?'@':' ';
    printf("Game of Life startet. Mit STRG+C kann die Simulation beendet werden.\n");
    for(gen=0;;gen++){
        /* ANSI-Escape-Sequenz loescht viele moderne Terminalfenster. */
        printf("\033[2J\033[H"); printf("Generation %d (STRG+C zum Beenden)\n",gen); print_grid(cur);
        /* Fuer jedes Feld werden die acht Nachbarpositionen untersucht.
           dx und dy nehmen jeweils -1, 0 oder 1 an. */
        for(y=0;y<H;y++)for(x=0;x<W;x++){
            n=0;
            for(dy=-1;dy<=1;dy++)for(dx=-1;dx<=1;dx++){
                int nx=x+dx, ny=y+dy;
                /* Die Position (0,0) waere das Feld selbst und ist kein Nachbar. */
                if(dx==0&&dy==0)continue;
                /* Grenzen pruefen, bevor auf das Array zugegriffen wird.
                   Nur lebendige Nachbarfelder erhoehen n. */
                if(nx>=0&&nx<W&&ny>=0&&ny<H&&cur[ny][nx]=='@')n++;
            }
            /* Lebensregeln: Lebendige Zelle bleibt bei 2 oder 3 Nachbarn lebendig;
               tote Zelle wird bei genau 3 lebendig. Sonst stirbt/bleibt sie tot. */
            if(cur[y][x]=='@') next[y][x]=(n==2||n==3)?'@':' ';
            else next[y][x]=(n==3)?'@':' ';
        }
        /* Erst nachdem alle Felder berechnet wurden, wird next nach cur kopiert. */
        for(y=0;y<H;y++)for(x=0;x<W;x++)cur[y][x]=next[y][x];
        wait_ms(200);
    }
}

/* FUNKTIONEN / STRINGS Aufgabe 1: drei kleine Funktionen. */
/* Gibt den groesseren von zwei Werten zurueck. Der ?: Operator ist eine
 * Kurzform fuer if/else: Bedingung ? Wert_wenn_wahr : Wert_wenn_falsch. */
int maximum(int a,int b){ return a>b?a:b; }
/* Der Restoperator % liefert den Divisionsrest. Gerade Zahlen haben Rest 0
 * bei Division durch 2. Rueckgabe 1 bedeutet wahr, 0 bedeutet falsch. */
int ist_gerade(int x){ return x%2==0; }
/* Addiert 1 + 2 + ... + n. Bei n <= 0 wird die Schleife nicht ausgefuehrt,
 * daher ist das Ergebnis 0. */
int summe_bis(int n){ int i,s=0; for(i=1;i<=n;i++)s+=i; return s; }
void task8(void) {
    printf("maximum(3,7)=%d; maximum(-4,2)=%d; maximum(9,9)=%d\n",maximum(3,7),maximum(-4,2),maximum(9,9));
    printf("ist_gerade(10)=%d; ist_gerade(7)=%d; ist_gerade(0)=%d\n",ist_gerade(10),ist_gerade(7),ist_gerade(0));
    printf("summe_bis(5)=%d; summe_bis(1)=%d; summe_bis(-3)=%d\n",summe_bis(5),summe_bis(1),summe_bis(-3));
}
/* Aufgabe 2: Fakultät und Potenz fuer kleine Werte; -1 signalisiert Ueberlauf. */
/* n! = 1 * 2 * ... * n; per Definition ist 0! = 1.
 * Negative Eingaben liefern hier 0 als Fehlerindikator. -1 bedeutet Ueberlauf. */
int fakultaet(int n) {
    int i,r=1; if(n<0)return 0;
    /* Vor der Multiplikation pruefen wir, ob r*i groesser als INT_MAX wuerde.
       r > INT_MAX/i ist eine sichere Pruefung ohne den Ueberlauf erst auszuloesen. */
    for(i=2;i<=n;i++){if(r>INT_MAX/i)return -1;r*=i;} return r;
}
/* Berechnet basis hoch exponent durch wiederholte Multiplikation.
 * Negative Exponenten werden in dieser ganzzahligen Funktion nicht unterstuetzt.
 * long long dient als Zwischenwert, damit ein Ueberlauf frueh erkannt wird. */
int potenz(int basis,int exponent) {
    int i,r=1; if(exponent<0)return 0;
    for(i=0;i<exponent;i++){
        long long tmp=(long long)r*basis;
        if(tmp>INT_MAX||tmp<INT_MIN)return -1;
        r=(int)tmp;
    } return r;
}
void task9(void) {
    int n; for(n=0;n<=6;n++)printf("%d! = %d\n",n,fakultaet(n));
    printf("potenz(2,10) = %d\n",potenz(2,10));
}
/* Aufgabe 3: Kommandozeilenargumente sind Strings; atoi wandelt sie in int um. */
/* Summiert anzahl Textargumente, nachdem atoi() sie in ganze Zahlen umwandelt.
 * Hinweis: atoi() erkennt ungueltige Zahlen nicht verlaesslich; fuer echte
 * Benutzereingaben waere strtol() mit Fehlerpruefung besser. */
int summiere(int anzahl,char *werte[]) { int i,s=0; for(i=0;i<anzahl;i++)s+=atoi(werte[i]); return s; }
/* argc ist die Anzahl der Kommandozeilenargumente; argv ist das Array der
 * Argumentstrings. argv[0] ist ueblicherweise der Programmname, deshalb beginnt
 * die eigentliche Zahlenliste bei argv[1]. */
void task10(int argc,char *argv[]) {
    char *demo[]={"4","10","-3"}; int i,m,s;
    if(argc>1){
        s=summiere(argc-1,&argv[1]); m=atoi(argv[1]);
        for(i=2;i<argc;i++)m=maximum(m,atoi(argv[i]));
        printf("Summe: %d\nMaximum: %d\n",s,m);
    } else {
        s=summiere(3,demo); m=maximum(maximum(4,10),-3);
        printf("Keine Argumente gesetzt; Beispiel 4 10 -3\nSumme: %d\nMaximum: %d\n",s,m);
        printf("Visual Studio: Argumente in Projekteigenschaften > Debuggen eintragen.\n");
    }
}
/* Aufgabe 4: Stringlaenge ohne strlen(). */
/* Zaehlt Zeichen bis zum String-Endzeichen '\0'. size_t ist der passende
 * vorzeichenlose Typ fuer Groessen und Laengen. Der Originalstring wird nicht geaendert. */
size_t meine_strlen(const char *s){ size_t n=0; while(s[n]!='\0')n++; return n; }
void task11(void) {
    const char t[]="Programmieren in C";
    printf("meine_strlen: %zu\nstrlen:       %zu\n",meine_strlen(t),strlen(t));
}
/* Aufgabe 5: bestimmtes Zeichen und Vokale zaehlen. */
/* Geht Zeichen fuer Zeichen durch den String. *s ist das aktuelle Zeichen;
 * s++ verschiebt den Zeiger auf das naechste Zeichen. */
int zaehle_zeichen(const char *s,char c){ int n=0; while(*s){if(*s==c)n++;s++;}return n; }
/* Zaehlt die Vokale a, e, i, o, u ohne Beachtung der Gross-/Kleinschreibung.
 * Um tolower() sicher zu verwenden, wird das Zeichen als unsigned char uebergeben. */
int zaehle_vokale(const char *s){
    int n=0; while(*s){int c=tolower((unsigned char)*s);if(c=='a'||c=='e'||c=='i'||c=='o'||c=='u')n++;s++;}return n;
}
void task12(void) {
    const char t[]="Programmieren in C";
    printf("'r': %d-mal; Vokale: %d\n",zaehle_zeichen(t,'r'),zaehle_vokale(t));
}
/* Aufgabe 6: Zeichen im veränderbaren Array in Grossbuchstaben umwandeln. */
/* Wandelt den String direkt im Speicher um. Das funktioniert nur mit einem
 * veraenderbaren Array wie char text[]; ein Stringliteral sollte nicht veraendert werden. */
void zu_grossbuchstaben(char *s){ while(*s){*s=(char)toupper((unsigned char)*s);s++;} }
void task13(void){ char t[]="Hallo Welt"; zu_grossbuchstaben(t); printf("%s\n",t); }
/* Aufgabe 7: Passwort maximal dreimal abfragen. */
/* fgets() liest auch Leerzeichen ein und begrenzt die Anzahl der Zeichen.
 * strcmp() liefert 0, wenn zwei Strings exakt gleich sind. Das Passwort hier
 * ist ein festes Lehrbeispiel, keine sichere Speicherung fuer echte Anwendungen. */
void task14(void) {
    const char pw[]="geheim123"; char input[100]; int i,ok=0;
    for(i=1;i<=3;i++){
        size_t n; printf("Passwort (Versuch %d/3): ",i);
        if(!fgets(input,sizeof input,stdin))return;
        /* fgets speichert normalerweise den Zeilenumbruch mit. Entfernen wir
           ihn, damit strcmp() nur das eingegebene Passwort vergleicht. */
        n=strlen(input); if(n>0&&input[n-1]=='\n')input[n-1]='\0';
        if(strcmp(input,pw)==0){puts("Zugang gewaehrt.");ok=1;break;}
        puts("Falsches Passwort.");
    }
    if(!ok)puts("Zugang gesperrt.");
}
/* Aufgabe 8: maximal groesse-1 Zeichen kopieren und immer mit Nullbyte beenden. */
/* Kopiert hoechstens groesse-1 Zeichen und setzt danach immer '\0'.
 * Bei groesse == 0 darf ziel nicht beschrieben werden. Die Funktion verhindert
 * dadurch, dass das Zielarray durch diese Kopie ueberlaeuft. */
void sicher_kopieren(char *ziel,size_t groesse,const char *quelle) {
    size_t i=0; if(groesse==0)return;
    while(i<groesse-1&&quelle[i]!='\0'){ziel[i]=quelle[i];i++;}
    ziel[i]='\0';
}
void task15(void) {
    char a[8],b[4],c[1];
    sicher_kopieren(a,sizeof a,"hello"); sicher_kopieren(b,sizeof b,"hello"); sicher_kopieren(c,sizeof c,"hello");
    printf("Groesse 8: [%s]\nGroesse 4: [%s]\nGroesse 1: [%s]\n",a,b,c);
}

/* Programmeinstieg. argc/argv werden an Aufgabe 10 weitergereicht, damit dort
 * optional eingegebene Kommandozeilenargumente verarbeitet werden koennen. */
int main(int argc,char *argv[]) {
    int choice;
    /* do-while zeigt das Menue mindestens einmal an und wiederholt es,
       solange choice nicht 0 ist. */
    do {
        puts("\n==============================================");
        puts(" C-LERNPROGRAMM: TEIL 2 + FUNKTIONEN/STRINGS");
        puts("==============================================");
        puts(" 1 Gerade Zahlen absteigend");
        puts(" 2 Array umkehren (zwei Varianten)");
        puts(" 3 Haeufigkeit der Zahlen 0 bis 9");
        puts(" 4 Matrix: Zeilen-, Spalten-, Diagonalsummen");
        puts(" 5 Minimum, Maximum, Durchschnitt");
        puts(" 6 Dreieck aus #");
        puts(" 7 Conway's Game of Life (STRG+C beendet)");
        puts(" 8 Kleine Funktionen");
        puts(" 9 Fakultaet und Potenz");
        puts("10 Summe der Kommandozeilenargumente");
        puts("11 Eigene Stringlaenge");
        puts("12 Zeichen und Vokale zaehlen");
        puts("13 Grossbuchstaben");
        puts("14 Passwortabfrage");
        puts("15 Sicheres Kopieren");
        puts(" 0 Programm beenden");
        if(!read_int("Auswahl: ",&choice)){pause_menu();continue;}
        /* switch waehlt die passende Aufgabenfunktion. break verhindert,
           dass danach auch der naechste case ausgefuehrt wird. */
        switch(choice){
            case 1:task1();break; case 2:task2();break; case 3:task3();break;
            case 4:task4();break; case 5:task5();break; case 6:task6();break;
            case 7:task7();break; case 8:task8();break; case 9:task9();break;
            case 10:task10(argc,argv);break; case 11:task11();break; case 12:task12();break;
            case 13:task13();break; case 14:task14();break; case 15:task15();break;
            case 0:puts("Programm wird beendet.");break;
            default:puts("Ungueltige Auswahl.");break;
        }
        if(choice!=0)pause_menu();
    } while(choice!=0);
    return 0;
}
