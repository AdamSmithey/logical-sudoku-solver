#include <iostream>
#include <vector>
#include <algorithm>
#include "solver.h"

using namespace std;

int main() {
   auto start = std::chrono::system_clock::now();

   Solver solver("093004560060003140004608309981345000347286951652070483406002890000400010029800034");

   auto end = std::chrono::system_clock::now();

   std::chrono::duration<double> elapsed_seconds = end - start;
   std::cout << "elapsed time: " << (elapsed_seconds.count() * 1000) << "ms"
             << std::endl;

}
