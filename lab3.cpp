#include <iostream>
#include <stdio.h>
#include <omp.h>
#include <ctime>
#include <vector>
using namespace std;
long n = 20000;

void lab3(vector<int> A, vector<int> B, vector<int> answer1, vector<int> answer2) {

    long i, j;

    clock_t start1 = clock();
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (j - i > -1) {
                answer1[i] += A[j - i] * B[j];
            }
        }
    }
    clock_t end1 = clock();
    cout << "Without OMP: " << (end1 - start1) / (CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

    clock_t start2 = clock();
    #pragma omp parallel  for private(i,j)
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (j - i > -1) {
                answer2[i] += A[j - i] * B[j];
            }
        }
    }
    clock_t end2 = clock();
    cout << "With OMP: " << (end2 - start2) / (CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

    for (int i = 0; i < n; i++) {
        if (answer1[i] != answer2[i])
            cout << "Answer is NOT right";
    }
}

int main()
{
    vector<int> A, B;
    vector<int> answer1, answer2;

    for (long i = 0; i < n; i++) {
        A.push_back(rand());
        B.push_back(rand());
        answer1.push_back(0);
        answer2.push_back(0);
    }

    lab3(A, B, answer1, answer2);
    return 0;
}
