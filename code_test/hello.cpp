#include <iostream>
#include <cmath>
int main() {
    int n = 1000000000;
    double total = 0;
#pragma omp target teams distribute \
parallel for map(tofrom: total) map(to: n) reduction(+:total)
    for (int i = 0; i < n; ++i) {
        total += exp(sin(M_PI * (double) i/12345.6789));
    }
    std::cout << "total is " << total << '\n';
}
