#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>

typedef __uint128_t uint128;

#define MAX_DIGITS 39

uint128 pows[10][MAX_DIGITS + 1];
uint128 minPow, maxPow;
int N;
int digitsMultiSet[10];
int testMultiSet[10];
int mod9_pows[10];

// Function to print __int128 as a decimal string
void print_uint128(uint128 num) {
    if (num == 0) {
        printf("0");
        return;
    }
    char buffer[50] = {0};
    int index = 49;
    while (num > 0) {
        buffer[--index] = (num % 10) + '0';
        num /= 10;
    }
    printf("%s", &buffer[index]);
}

// Function to precompute pows[d][j] = d^j
void genPows(int maxN) {
    for (int d = 0; d < 10; d++) {
        pows[d][0] = 1;
        for (int j = 1; j <= maxN; j++) {
            pows[d][j] = pows[d][j - 1] * d;
        }
    }
}

// Function to compute minPow = 10^(N-1) and maxPow = 10^N
void prepareLimits(int currentN) {
    minPow = 1;
    for (int i = 0; i < currentN - 1; i++) minPow *= 10;
    maxPow = minPow * 10;
}

// Function to extract digit frequencies from a __int128 number
void getFrequency(uint128 candidate, int freq[10]) {
    memset(freq, 0, sizeof(int) * 10);
    while (candidate > 0) {
        freq[candidate % 10]++;
        candidate /= 10;
    }
}

// Function to check if two frequency arrays match
int frequenciesEqual(int *a, int *b) {
    for (int i = 0; i < 10; i++) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

// Function to verify if the candidate is an Armstrong number
int check(uint128 candidate) {
    if (candidate >= maxPow || candidate < minPow) return 0;
    getFrequency(candidate, testMultiSet);
    return frequenciesEqual(testMultiSet, digitsMultiSet);
}

void search(int digit, int unused, uint128 pow, int sum_candidate_mod9, int sum_digits_mod9) {
    if (pow >= maxPow) return;

    // Prune if even the maximum possible remaining sum can't reach minPow
    if (digit > 0) {
        uint128 max_remaining = (uint128)unused * pows[digit][N];
        if (pow + max_remaining < minPow) {
            return;
        }
    }

    if (digit == -1) {
        // Check congruence modulo 9
        if (sum_candidate_mod9 != sum_digits_mod9) return;
        if (check(pow)) {
            print_uint128(pow);
            printf("\n");
        }
        return;
    }

    if (digit == 0) {
        digitsMultiSet[digit] = unused;
        search(digit - 1, 0, pow, sum_candidate_mod9, (sum_digits_mod9 + unused * 0) % 9);
    } else {
        uint128 bi = pow;
        for (int i = 0; i <= unused; i++) {
            digitsMultiSet[digit] = i;
            int new_sum_candidate = (sum_candidate_mod9 + i * mod9_pows[digit]) % 9;
            int new_sum_digits = (sum_digits_mod9 + i * digit) % 9;
            search(digit - 1, unused - i, bi, new_sum_candidate, new_sum_digits);
            if (i != unused) bi += pows[digit][N];
        }
    }
}

// Function to generate Armstrong numbers up to maxN digits
void generate(int maxN) {
    genPows(maxN);
    for (N = 1; N <= maxN; N++) {
        prepareLimits(N);
        // Precompute mod9_pows for current N
        for (int d = 0; d < 10; d++) {
            mod9_pows[d] = (int)(pows[d][N] % 9);
        }
        memset(digitsMultiSet, 0, sizeof(digitsMultiSet));
        search(9, N, 0, 0, 0);
    }
}

int main() {
    int maxDigits = 27;
    clock_t start = clock();
    generate(maxDigits);
    clock_t finish = clock();
    double elapsedSeconds = (double)(finish - start) / CLOCKS_PER_SEC;
    printf("Execution time: %.2f seconds\n", elapsedSeconds);
    return 0;
}
