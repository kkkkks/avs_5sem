#include <iostream>
using namespace std;

__int8 A[] = { 4, 3, 1, 13, 2, 9, 11, 5 };
__int8 B[] = { 8, 21, 6, 19, 10, -2, -5, 12 };
__int8 C[] = { -7, 23, -9, -3, 0, 17, -6, 7 };
__int16 D[] = { 29, -20, -1, -8, 25, 27, -2, 0 };

void result() {
    __int16 F[8];
    __asm {
        movq mm0, [A]
        movq mm1, [B]
        psubsb mm0, mm1 
        movq mm3, mm0 // mm3 = mm0 = a - b

        pcmpgtb mm2, mm3
        punpcklbw mm0, mm2 // low bytes
        punpckhbw mm3, mm2 // high bytes
        movq mm6, mm0 // mm6 = a - b lb
        movq mm7, mm3 // mm7 = a - b hb

        movq mm0, [C]
        movq mm1, [C]
        pxor mm2, mm2
        pcmpgtb mm2, mm0
        punpcklbw mm0, mm2 // low bytes
        punpckhbw mm1, mm2 // high bytes
        movq mm4, mm0 // mm4 = c (lb)
        movq mm5, mm1 // mm5 = c (hb)

        movq mm0, [D] // high bytes
        movq mm1, [D + 8] // low bytes, mm0 mm1 = d
        
        paddsw mm0, mm4
        paddsw mm1, mm5 // mm0 mm1 = c + d
        pmullw mm6, mm0
        pmullw mm7, mm1 // mm6 mm7 = (a - b) * (c + d)

        movq [F], mm6
        movq [F + 8], mm7
    }
   
    for (int i = 0; i < 8; i++) {      
        cout << "True answer:\t " << (A[i] - B[i]) * (C[i] + D[i]) << "\nMy answer:\t " << F[i] << "\n\n";
    }
}

int main() {
    result();
    return 0;
}