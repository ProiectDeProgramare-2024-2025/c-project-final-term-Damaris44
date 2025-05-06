#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

typedef struct {
    char cardNumber[20];
    char cardHolder[50];
    char expiration[10];
    int views;
} Card;

#define MAX_CARDS 100
#define FILE_NAME "wallet.txt"

Card cards[MAX_CARDS];
int cardCount = 0;

void clearScreen() {
    system(CLEAR);
}

void pause() {
    printf("Press enter to continue...");
    while(getchar() != '\n');
    getchar();
}

int isValidCardNumber(char *num) {
    if (strlen(num) != 16) return 0;
    for (int i = 0; i < 16; i++) {
        if (!isdigit(num[i])) return 0;
    }
    return 1;
}

int isValidExpiration(char *exp) {
    if (strlen(exp) != 5 || exp[2] != '/') return 0;
    if (!isdigit(exp[0]) || !isdigit(exp[1]) || !isdigit(exp[3]) || !isdigit(exp[4])) return 0;

    int month = (exp[0] - '0') * 10 + (exp[1] - '0');
    int year = (exp[3] - '0') * 10 + (exp[4] - '0');

    if (month < 1 || month > 12) return 0;


    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int currentMonth = tm.tm_mon + 1;
    int currentYear = tm.tm_year % 100;


    if (year < currentYear || (year == currentYear && month < currentMonth)) return 0;

    return 1;
}

void saveCardsToFile() {
    FILE *file = fopen(FILE_NAME, "w");
    if (file == NULL) {
        printf(RED "[!] Error opening file for writing.\n" RESET);
        return;
    }
    for (int i = 0; i < cardCount; i++) {
        fprintf(file, "%s,%s,%s,%d\n",
                cards[i].cardNumber, cards[i].cardHolder,
                cards[i].expiration, cards[i].views);
    }
    fclose(file);
}

void loadCardsFromFile() {
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) return;
    cardCount = 0;
    while (fscanf(file, "%[^,],%[^,],%[^,],%d\n",
                  cards[cardCount].cardNumber,
                  cards[cardCount].cardHolder,
                  cards[cardCount].expiration,
                  &cards[cardCount].views) == 4) {
        cardCount++;
        if (cardCount >= MAX_CARDS) break;
    }
    fclose(file);
}

void addCard() {
    clearScreen();
    printf(CYAN "----- Add Card -----\n" RESET);

    if (cardCount < MAX_CARDS) {
        char temp[50];
        do {
            printf("Enter " YELLOW "card number (16 digits)" RESET ": ");
            scanf("%s", temp);
            if (!isValidCardNumber(temp)) {
                printf(RED "[!] Invalid card number. Please enter exactly 16 digits.\n" RESET);
            }
        } while (!isValidCardNumber(temp));
        strcpy(cards[cardCount].cardNumber, temp);

        printf("Enter " YELLOW "cardholder name" RESET ": ");
        scanf(" %[^\n]", cards[cardCount].cardHolder);

        do {
            printf("Enter " YELLOW "expiration date (MM/YY)" RESET ": ");
            scanf("%s", temp);
            if (!isValidExpiration(temp)) {
                printf(RED "[!] Invalid expiration date. Please ensure the format is MM/YY and the date is not in the past.\n" RESET);
            }
        } while (!isValidExpiration(temp));
        strcpy(cards[cardCount].expiration, temp);

        cards[cardCount].views = 0;
        cardCount++;
        saveCardsToFile();
        printf(GREEN "[+] Card added successfully!\n" RESET);
    } else {
        printf(RED "[!] Wallet is full! Cannot add more cards.\n" RESET);
    }
    pause();
}

void viewCards() {
    clearScreen();
    printf(CYAN "----- View Cards (sorted by least viewed) -----\n" RESET);

    if (cardCount == 0) {
        printf("No cards in your wallet.\n");
    } else {
        for (int i = 0; i < cardCount - 1; i++) {
            for (int j = 0; j < cardCount - i - 1; j++) {
                if (cards[j].views > cards[j+1].views) {
                    Card temp = cards[j];
                    cards[j] = cards[j+1];
                    cards[j+1] = temp;
                }
            }
        }

        for (int i = 0; i < cardCount; i++) {
            printf(YELLOW "Card %d:\n" RESET, i + 1);
            printf("  Number   : " GREEN "%s\n" RESET, cards[i].cardNumber);
            printf("  Holder   : " BLUE "%s\n" RESET, cards[i].cardHolder);
            printf("  Exp. Date: " CYAN "%s\n" RESET, cards[i].expiration);
            printf("  Views    : %d\n", cards[i].views);
            printf("-----------------------------\n");
            cards[i].views++;
        }
        saveCardsToFile();
    }
    pause();
}

void deleteCard() {
    clearScreen();
    printf(CYAN "----- Delete Card -----\n" RESET);

    if (cardCount == 0) {
        printf("No cards to delete.\n");
    } else {
        int del;
        for (int i = 0; i < cardCount; i++) {
            printf(YELLOW "%d. " RESET "%s - %s\n", i + 1, cards[i].cardNumber, cards[i].cardHolder);
        }
        printf("Enter the number of the card to delete: ");
        scanf("%d", &del);

        if (del < 1 || del > cardCount) {
            printf(RED "[!] Invalid selection.\n" RESET);
        } else {
            for (int i = del - 1; i < cardCount - 1; i++) {
                cards[i] = cards[i + 1];
            }
            cardCount--;
            saveCardsToFile();
            printf(GREEN "[+] Card deleted successfully.\n" RESET);
        }
    }
    pause();
}

void showMenu() {
    printf(CYAN "----- Electronic Wallet System -----\n" RESET);
    printf("1. " GREEN "Add Card\n" RESET);
    printf("2. " YELLOW "View Cards\n" RESET);
    printf("3. " RED "Delete Card\n" RESET);
    printf("0. Exit\n");
    printf("-----------------------------------\n");
    printf("Enter option: ");
}

int main() {
    loadCardsFromFile();

    int option;
    do {
        clearScreen();
        showMenu();
        if (scanf("%d", &option) != 1) {
            printf(RED "[!] Invalid input. Please enter a number.\n" RESET);
            while(getchar() != '\n');
            pause();
            continue;
        }

        switch(option) {
            case 1: addCard(); break;
            case 2: viewCards(); break;
            case 3: deleteCard(); break;
            case 0: printf(GREEN "Exiting...\n" RESET); break;
            default: printf(RED "[!] Invalid option. Try again.\n" RESET); pause(); break;
        }
    } while(option != 0);

    return 0;
}
