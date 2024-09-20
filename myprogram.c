#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CARDS 200
#define NUM_RUNS 10000  // Increase the number of runs for more significant time measurement

typedef struct {
    char name[65];
    int marketPrice;
} Card;

Card market[MAX_CARDS];
int marketSize = 0;

typedef struct {
    char name[65];
    int gertrudePrice;
} PriceList;

int findMarketPrice(const char* cardName) {
    for (int i = 0; i < marketSize; i++) {
        if (strcmp(cardName, market[i].name) == 0) {
            return market[i].marketPrice;
        }
    }
    return -1;
}

void bruteForceMaxProfit(PriceList priceList[], int I, int W, FILE *outputFile) {
    int maxProfit = 0, bestCount = 0;
    int bestSubset[MAX_CARDS];

    clock_t start = clock();

    for (int run = 0; run < NUM_RUNS; run++) {  // Repeat the brute force operation
        for (int subset = 0; subset < (1 << I); subset++) {
            int totalCost = 0, totalProfit = 0, cardCount = 0;
            int currentSubset[MAX_CARDS];

            for (int i = 0; i < I; i++) {
                if (subset & (1 << i)) {
                    currentSubset[cardCount++] = i;
                    totalCost += priceList[i].gertrudePrice;
                    int marketPrice = findMarketPrice(priceList[i].name);
                    if (marketPrice != -1) {
                        totalProfit += (marketPrice - priceList[i].gertrudePrice);
                    }
                }
            }

            if (totalCost <= W && totalProfit > maxProfit) {
                maxProfit = totalProfit;
                bestCount = cardCount;
                memcpy(bestSubset, currentSubset, cardCount * sizeof(int));
            }
        }
    }

    clock_t end = clock();
    double timeTaken = (double)(end - start) / CLOCKS_PER_SEC / NUM_RUNS;  // Average time per run

    fprintf(outputFile, "%d %d %d %.6f\n", I, maxProfit, bestCount, timeTaken);
    for (int i = 0; i < bestCount; i++) {
        fprintf(outputFile, "%s\n", priceList[bestSubset[i]].name);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <market-price-file> <price-list-file> <output-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *marketFile = fopen(argv[1], "r");
    FILE *priceFile = fopen(argv[2], "r");
    FILE *outputFile = fopen(argv[3], "w");

    if (!marketFile || !priceFile || !outputFile) {
        fprintf(stderr, "Error opening files.\n");
        return EXIT_FAILURE;
    }

    fscanf(marketFile, "%d", &marketSize);
    for (int i = 0; i < marketSize; i++) {
        fscanf(marketFile, "%s %d", market[i].name, &market[i].marketPrice);
    }

    int I, W;
    while (fscanf(priceFile, "%d %d", &I, &W) != EOF) {
        PriceList priceList[MAX_CARDS];
        for (int i = 0; i < I; i++) {
            fscanf(priceFile, "%s %d", priceList[i].name, &priceList[i].gertrudePrice);
            if (findMarketPrice(priceList[i].name) == -1) {
                fprintf(stderr, "Error: Card %s not found in market prices.\n", priceList[i].name);
                continue;
            }
        }

        bruteForceMaxProfit(priceList, I, W, outputFile);
    }

    fclose(marketFile);
    fclose(priceFile);
    fclose(outputFile);

    return EXIT_SUCCESS;
}
