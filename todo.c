

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>      // printf, fprintf, fgets, fopen, fclose, fdopen
#include <stdlib.h>     // strtol, mkstemp, free
#include <string.h>     // strlen, strchr, memmove
#include <unistd.h>     // getopt, close, unlink
#include <errno.h>      // errno
#include <limits.h>     // LONG_MAX

#define DATEINAME "todo.txt"
#define MAX_LINE 1024   // Maximalgröße einer Zeile inklusive '\n' und '\0'

/* Welche Aktion soll ausgeführt werden? */
typedef enum {
    AKTION_KEINE,
    AKTION_HILFE,
    AKTION_LISTE,
    AKTION_HINZUFUEGEN,
    AKTION_BEARBEITEN,
    AKTION_LOESCHEN,
    AKTION_ERLEDIGEN,
    AKTION_OEFFNEN
} Aktion;

/* Filter, der nur beim Auflisten verwendet werden darf. */
typedef enum {
    FILTER_ALLE,
    FILTER_OFFEN,
    FILTER_ERLEDIGT
} Filter;

/* Gibt die korrekte Aufrufsyntax und alle Optionen aus. */
static void hilfe_anzeigen(const char *programmname)
{
    printf("usage: %s [-a text] [-e text item-no] [-d item-no]\n"
           "          [-c item-no] [-u item-no] [-l] [-U] [-C] [-h]\n"
           "options:\n"
           "  -a text          Neue Aufgabe hinzufügen\n"
           "  -e text nr       Text von Aufgabe nr ändern\n"
           "  -d nr            Aufgabe nr löschen\n"
           "  -c nr            Aufgabe nr als erledigt markieren\n"
           "  -u nr            Aufgabe nr wieder öffnen\n"
           "  -l               Alle Aufgaben auflisten\n"
           "  -U               Nur offene Aufgaben zeigen (benötigt -l)\n"
           "  -C               Nur erledigte Aufgaben zeigen (benötigt -l)\n"
           "  -h               Diese Hilfe anzeigen\n",
           programmname);
}

/*
 * Wandelt eine Zeichenkette in eine positive Aufgabennummer um.
 * strtol() wird statt atoi() benutzt, weil strtol() Fehler erkennen kann.
 * Rückgabe: 1 bei Erfolg, 0 bei ungültiger Eingabe.
 */
static int nummer_lesen(const char *text, long *nummer)
{
    char *ende;
    long wert;

    if (text == NULL || *text == '\0') {
        return 0;
    }

    errno = 0;
    wert = strtol(text, &ende, 10);

    /* Prüfen: Zahl vorhanden, keine Zeichen dahinter, kein Überlauf, > 0. */
    if (errno != 0 || *ende != '\0' || ende == text || wert <= 0) {
        return 0;
    }
    if (wert > LONG_MAX) { /* Zusätzliche Dokumentation der Bereichsprüfung. */
        return 0;
    }

    *nummer = wert;
    return 1;
}

/*
 * Liest die nächste gültige Aufgabe.
 * Leere Zeilen werden übersprungen.
 * Eine nichtleere Zeile muss mit 'o' oder 'e' beginnen.
 * Zu lange Zeilen werden als Fehler behandelt.
 * Rückgabe: 1 = Aufgabe gelesen, 0 = Dateiende, -1 = ungültige Datei/Lesefehler.
 */
static int naechste_aufgabe(FILE *datei, char zeile[MAX_LINE], long *zeilennummer)
{
    while (fgets(zeile, MAX_LINE, datei) != NULL) {
        size_t laenge = strlen(zeile);

        /*
         * Wenn der Puffer voll ist und kein Zeilenumbruch enthalten ist,
         * passt die komplette Zeile nicht hinein. Auch dann, wenn das
         * Dateiende direkt danach kommt, ist sie laut Aufgabenstellung zu lang.
         */
        if (laenge == MAX_LINE - 1 && strchr(zeile, '\n') == NULL) {
            fprintf(stderr, "Fehler: Eine Zeile ist länger als %d Zeichen.\n",
                    MAX_LINE - 1);
            return -1;
        }

        /* Eine Zeile mit nur '\n' zählt nicht als Aufgabe. */
        if (laenge == 1 && zeile[0] == '\n') {
            continue;
        }
        if (laenge == 0) {
            continue;
        }

        /* Das Statuszeichen am Zeilenanfang muss gültig sein. */
        if (zeile[0] != 'o' && zeile[0] != 'e') {
            fprintf(stderr,
                    "Fehler: Ungültiges Statuszeichen am Anfang einer Datenzeile.\n");
            return -1;
        }

        ++(*zeilennummer);
        return 1;
    }

    if (ferror(datei)) {
        perror("Fehler beim Lesen der To-Do-Datei");
        return -1;
    }
    return 0;
}

/* Schreibt eine Zeile vollständig in die Ausgabedatei. */
static int zeile_schreiben(FILE *datei, const char *zeile)
{
    if (fputs(zeile, datei) == EOF) {
        perror("Fehler beim Schreiben der temporären Datei");
        return 0;
    }
    return 1;
}

/*
 * Listet die Aufgaben auf. Es wird keine temporäre Datei benötigt,
 * weil diese Aktion die gespeicherten Daten nicht verändert.
 */
static int liste_anzeigen(Filter filter)
{
    FILE *datei = fopen(DATEINAME, "r");
    char zeile[MAX_LINE];
    long nummer = 0;
    int status;

    if (datei == NULL) {
        if (errno == ENOENT) {
            /* Eine noch nicht angelegte Liste ist einfach leer. */
            printf("Die To-Do-Liste ist noch leer.\n");
            return 0;
        }
        perror("To-Do-Datei kann nicht geöffnet werden");
        return 1;
    }

    while ((status = naechste_aufgabe(datei, zeile, &nummer)) == 1) {
        int erledigt = (zeile[0] == 'e');

        /* Je nach Filter offene oder erledigte Aufgaben überspringen. */
        if (filter == FILTER_OFFEN && erledigt) {
            continue;
        }
        if (filter == FILTER_ERLEDIGT && !erledigt) {
            continue;
        }

        /* Nummer und Status ausgeben; der Text beginnt nach dem Statuszeichen. */
        printf("%ld. [%c] %s", nummer, erledigt ? 'x' : ' ', zeile + 1);

        /* Auch die letzte Datei-Zeile ohne '\n' sauber abschließen. */
        if (strchr(zeile, '\n') == NULL) {
            putchar('\n');
        }
    }

    if (fclose(datei) != 0) {
        perror("Fehler beim Schließen der To-Do-Datei");
        return 1;
    }
    return status == -1 ? 1 : 0;
}

/*
 * Führt eine Änderung an der Liste durch.
 * Die vorhandene Datei wird nie direkt überschrieben: Stattdessen wird eine
 * temporäre Datei im selben Verzeichnis erstellt. Erst wenn Lesen und Schreiben
 * erfolgreich waren, ersetzt rename() die alte Datei. So bleibt die alte Liste
 * bei einem Fehler während der Bearbeitung normalerweise erhalten.
 */
static int liste_aendern(Aktion aktion, const char *neuer_text, long zielnummer)
{
    char tempname[] = "./todo.tmp.XXXXXX";
    char zeile[MAX_LINE];
    long nummer = 0;
    int temp_fd = -1;
    FILE *eingabe = NULL;
    FILE *ausgabe = NULL;
    int status = 0;
    int ziel_gefunden = 0;
    int eingabe_existiert = 1;

    /* Temporärdatei sicher und exklusiv im aktuellen Verzeichnis anlegen. */
    temp_fd = mkstemp(tempname);
    if (temp_fd == -1) {
        perror("Temporäre Datei kann nicht angelegt werden");
        return 1;
    }

    ausgabe = fdopen(temp_fd, "w");
    if (ausgabe == NULL) {
        perror("Temporäre Datei kann nicht geöffnet werden");
        close(temp_fd);
        unlink(tempname);
        return 1;
    }

    eingabe = fopen(DATEINAME, "r");
    if (eingabe == NULL) {
        if (errno == ENOENT && aktion == AKTION_HINZUFUEGEN) {
            /* Beim ersten Hinzufügen darf todo.txt noch nicht existieren. */
            eingabe_existiert = 0;
        } else {
            perror("To-Do-Datei kann nicht geöffnet werden");
            fclose(ausgabe);
            unlink(tempname);
            return 1;
        }
    }

    if (eingabe_existiert) {
        while ((status = naechste_aufgabe(eingabe, zeile, &nummer)) == 1) {
            if (nummer == zielnummer && aktion != AKTION_HINZUFUEGEN) {
                ziel_gefunden = 1;
            }

            /* Löschen: Die Zielzeile wird absichtlich nicht kopiert. */
            if (aktion == AKTION_LOESCHEN && nummer == zielnummer) {
                continue;
            }

            /* Text ändern: Statuszeichen bleibt erhalten, nur der Text ändert sich. */
            if (aktion == AKTION_BEARBEITEN && nummer == zielnummer) {
                if (fprintf(ausgabe, "%c%s\n", zeile[0], neuer_text) < 0) {
                    perror("Fehler beim Schreiben");
                    status = -1;
                    break;
                }
                continue;
            }

            /* Erledigen/Öffnen: Nur das erste Statuszeichen wird ersetzt. */
            if ((aktion == AKTION_ERLEDIGEN || aktion == AKTION_OEFFNEN) &&
                nummer == zielnummer) {
                zeile[0] = (aktion == AKTION_ERLEDIGEN) ? 'e' : 'o';
            }

            /* Alle nicht gelöschten bzw. nicht ersetzten Zeilen unverändert kopieren. */
            if (!zeile_schreiben(ausgabe, zeile)) {
                status = -1;
                break;
            }
        }

        if (fclose(eingabe) != 0) {
            perror("Fehler beim Schließen der To-Do-Datei");
            status = -1;
        }
        eingabe = NULL;
    }

    /* Fehler beim Lesen oder beim Verarbeiten der Eingabedatei? */
    if (status == -1) {
        fclose(ausgabe);
        unlink(tempname);
        return 1;
    }

    /* Bei einer Änderung muss die angegebene Nummer tatsächlich existieren. */
    if (aktion != AKTION_HINZUFUEGEN && !ziel_gefunden) {
        fprintf(stderr, "Fehler: Aufgabe %ld existiert nicht.\n", zielnummer);
        fclose(ausgabe);
        unlink(tempname);
        return 1;
    }

    /* Neue Aufgabe am Ende hinzufügen; Aufgaben sind anfangs immer offen. */
    if (aktion == AKTION_HINZUFUEGEN) {
        if (neuer_text == NULL || strchr(neuer_text, '\n') != NULL ||
            strchr(neuer_text, '\r') != NULL) {
            fprintf(stderr, "Fehler: Der Aufgabentext darf keinen Zeilenumbruch enthalten.\n");
            fclose(ausgabe);
            unlink(tempname);
            return 1;
        }
        if (fprintf(ausgabe, "o%s\n", neuer_text) < 0) {
            perror("Fehler beim Anhängen der neuen Aufgabe");
            fclose(ausgabe);
            unlink(tempname);
            return 1;
        }
    }

    /* Erst alle gepufferten Daten schreiben und die Datei schließen. */
    if (fflush(ausgabe) != 0) {
        perror("Fehler beim Schreiben der temporären Datei");
        fclose(ausgabe);
        unlink(tempname);
        return 1;
    }
    if (fclose(ausgabe) != 0) {
        perror("Fehler beim Schließen der temporären Datei");
        unlink(tempname);
        return 1;
    }

    /* rename() ersetzt die alte Datei erst nach erfolgreichem Erstellen. */
    if (rename(tempname, DATEINAME) != 0) {
        perror("Die neue To-Do-Datei kann nicht übernommen werden");
        unlink(tempname);
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    Aktion aktion = AKTION_KEINE;
    Filter filter = FILTER_ALLE;
    const char *text_argument = NULL;
    const char *nummer_argument = NULL;
    long zielnummer = 0;
    int option;
    int hat_filter = 0;
    int exit_code;

    /*
     * getopt() zerlegt die Optionen aus argv.
     * Doppelpunkt nach a/e/d/c/u bedeutet: Diese Option benötigt Argumente.
     * getopt() akzeptiert auch zusammengefasste Optionen wie -lU.
     */
    opterr = 0; /* Eigene, einheitliche Fehlermeldung statt getopt-Standardtext. */
    while ((option = getopt(argc, argv, "ha:e:d:c:u:lUC")) != -1) {
        Aktion neue_aktion = AKTION_KEINE;

        switch (option) {
            case 'h':
                neue_aktion = AKTION_HILFE;
                break;
            case 'a':
                neue_aktion = AKTION_HINZUFUEGEN;
                text_argument = optarg;
                break;
            case 'e':
                neue_aktion = AKTION_BEARBEITEN;
                text_argument = optarg;
                break;
            case 'd': neue_aktion = AKTION_LOESCHEN; nummer_argument = optarg; break;
            case 'c': neue_aktion = AKTION_ERLEDIGEN; nummer_argument = optarg; break;
            case 'u': neue_aktion = AKTION_OEFFNEN; nummer_argument = optarg; break;
            case 'l': neue_aktion = AKTION_LISTE; break;
            case 'U':
                if (hat_filter && filter != FILTER_OFFEN) {
                    fprintf(stderr, "Fehler: -U und -C können nicht kombiniert werden.\n");
                    return 1;
                }
                filter = FILTER_OFFEN;
                hat_filter = 1;
                break;
            case 'C':
                if (hat_filter && filter != FILTER_ERLEDIGT) {
                    fprintf(stderr, "Fehler: -U und -C können nicht kombiniert werden.\n");
                    return 1;
                }
                filter = FILTER_ERLEDIGT;
                hat_filter = 1;
                break;
            default:
                fprintf(stderr, "Fehler: Unbekannte Option oder fehlendes Argument.\n");
                hilfe_anzeigen(argv[0]);
                return 1;
        }

        /* Genau eine Hauptaktion ist erlaubt; -U/-C sind nur Listenfilter. */
        if (neue_aktion != AKTION_KEINE) {
            if (aktion != AKTION_KEINE) {
                fprintf(stderr, "Fehler: Bitte genau eine Hauptaktion angeben.\n");
                hilfe_anzeigen(argv[0]);
                return 1;
            }
            aktion = neue_aktion;
        }

    }

    /* Nach den Optionen dürfen keine zusätzlichen Argumente übrig bleiben. */
    if (optind < argc) {
        if (aktion == AKTION_BEARBEITEN && nummer_argument == NULL &&
            optind + 1 == argc) {
            /* Unterstützt auch -e "Text" 3: die Nummer steht hinter dem Text. */
            nummer_argument = argv[optind];
            ++optind;
        }
        if (optind < argc) {
            fprintf(stderr, "Fehler: Überflüssige Argumente.\n");
            hilfe_anzeigen(argv[0]);
            return 1;
        }
    }

    if (aktion == AKTION_KEINE) {
        fprintf(stderr, "Fehler: Es wurde keine Aktion angegeben.\n");
        hilfe_anzeigen(argv[0]);
        return 1;
    }

    if (hat_filter && aktion != AKTION_LISTE) {
        fprintf(stderr, "Fehler: -U und -C dürfen nur zusammen mit -l verwendet werden.\n");
        return 1;
    }

    /* Für Aktionen, die eine Nummer erwarten, diese jetzt sicher auswerten. */
    if (aktion == AKTION_BEARBEITEN || aktion == AKTION_LOESCHEN ||
        aktion == AKTION_ERLEDIGEN || aktion == AKTION_OEFFNEN) {
        if (!nummer_lesen(nummer_argument, &zielnummer)) {
            fprintf(stderr, "Fehler: Es wird eine positive Aufgabennummer benötigt.\n");
            hilfe_anzeigen(argv[0]);
            return 1;
        }
    }

    /* Die Hilfe benötigt keine Datei und wird sofort ausgegeben. */
    if (aktion == AKTION_HILFE) {
        hilfe_anzeigen(argv[0]);
        return 0;
    }

    /* Aktion ausführen und Fehlercode an die Kommandozeile zurückgeben. */
    switch (aktion) {
        case AKTION_LISTE:
            exit_code = liste_anzeigen(filter);
            break;
        case AKTION_HINZUFUEGEN:
            if (text_argument == NULL || *text_argument == '\0') {
                fprintf(stderr, "Fehler: Bei -a muss ein nichtleerer Text angegeben werden.\n");
                return 1;
            }
            exit_code = liste_aendern(aktion, text_argument, 0);
            break;
        case AKTION_BEARBEITEN:
            if (text_argument == NULL || *text_argument == '\0') {
                fprintf(stderr, "Fehler: Bei -e muss ein nichtleerer neuer Text angegeben werden.\n");
                return 1;
            }
            exit_code = liste_aendern(aktion, text_argument, zielnummer);
            break;
        case AKTION_LOESCHEN:
        case AKTION_ERLEDIGEN:
        case AKTION_OEFFNEN:
            exit_code = liste_aendern(aktion, NULL, zielnummer);
            break;
        default:
            fprintf(stderr, "Fehler: Interne Aktionsauswahl ungültig.\n");
            return 1;
    }

    return exit_code;
}
