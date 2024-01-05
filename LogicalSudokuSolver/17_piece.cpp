//
// Created by Adam Smithey on 5/7/23.
//

#include <iostream>
#include <vector>
#include <cmath>

std::vector<std::vector<int> > r = {{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0}};

/*
 * 0 0 0 0 0 8 4 0 0
7 0 0 0 0 0 0 0 0
0 6 3 0 0 0 0 0 0
0 0 0 0 0 0 0 0 3
3 0 0 0 0 7 0 0 0
0 0 0 3 0 0 0 0 9
0 0 0 6 0 0 0 0 1
2 9 0 0 0 0 0 0 0
0 0 0 1 0 0 0 9 6

 */
std::vector<std::vector<int> > grid = {{0,0,0,0,0,8,4,0,0},{7,0,0,0,0,0,0,0,0},{0,6,3,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,3},{3,0,0,0,0,7,0,0,0},{0,0,0,3,0,0,0,0,9},{0,0,0,6,0,0,0,0,1},{2,9,0,0,0,0,0,0,0},{0,0,0,1,0,0,0,9,6}};
std::vector<std::vector<int>> temp;

bool valid(int i, int j, int k) {
    for(int l = 0; l < 9; l++) {
        if(grid[i][l] == k) return false;
        if(grid[l][j] == k) return false;
        if(grid[floor(l / 3) + 3 * floor(i / 3)][l % 3 + 3 * floor(j / 3)] == k) return false;
    }
    return true;
}

bool check() {
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            //console.log(i + " " + j);
            if(grid[i][j] != 0) continue;
            for(int k = 1; k <= 10; k++) {
                if(temp[i][j] == k) continue;
                int l = (k == 10) ? temp[i][j] : k;
                // specifically exclude the correct value
                if(valid(i, j, l)) {
                    grid[i][j] = l;
                    if(check()) {
                        return true;
                    } else grid[i][j] = 0;
                }
            }
            return false;
        }
    }
    return true;
}

bool solve() {
    // iterate over graph
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            if(grid[i][j] == 0) {
                int k = rand() % 9 + 1;
                std::vector<int> kList;
                while(kList.size() != 9) {
                    if(std::count(kList.begin(), kList.end(), k) > 0){
                        k = rand() % 9 + 1;
                        continue;
                    }
                    if(valid(i, j, k)) {
                        grid[i][j] = k;
                        if(solve()) {
                            return true;
                        }
                        else grid[i][j] = 0;
                    }

                    kList.push_back(k);
                    k = rand() % 9 + 1;
                }
                return false;
            }
        }
    }
    return true;

}

bool complete() {
    // iterate over graph
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            if(grid[i][j] == 0) {
                for(int k = 1; k <= 9; k++) {
                    if(valid(i, j, k)) {
                        grid[i][j] = k;
                        if(solve()) {
                            return true;
                        }
                        else grid[i][j] = 0;
                    }
                }
                return false;
            }
        }
    }
    return true;

}

void printBoard(const std::vector<std::vector<int>>& board) {
    for(const std::vector<int>& v : board) {
        std::string s;
        for(int i : v) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }
}

bool test(const std::vector<std::vector<int>>& board) {
    for(const std::vector<int>& v : board) {
        for(int i : v) {
            if(i == 0) return false;
        }
    }
    return true;
}

void create() {
    std::srand (std::time (nullptr));
    std::vector<std::vector<int>> t;
    t = grid;
    solve();
    temp = grid;
    grid = t;
    bool c = false;

    for(int j = 0; j < 1000; j++) {
        grid = r;
        for(int i = 0; i < 17; i++) {
            int x = rand() % 9;
            int y = rand() % 9;
            int k = rand() % 9 + 1;

            if(grid[x][y] == 0 && valid(x, y, k)) {
                grid[x][y] = k;
                continue;
            }

            i--;
        }

        std::cout << std::endl;
//
        printBoard(grid);
//        t = grid;
        std::cout << std::endl;
        complete();
//
        printBoard(grid);


        //std::cout << solve();
//        if(test(grid)) {
//            std::cout << std::endl;
//
//            printBoard(grid);
//            temp = grid;
//            grid = t;
//
//            check();
//            if(temp == grid) {
//                grid = r;
//                c = true;
//            } else grid = r;
//        } else {
//            grid = r;
//            std::cout << std::endl;
//
//            printBoard(grid);
//
//        }
    }

    printBoard(grid);
    grid = temp;
}

int main() {
    auto start = std::chrono::system_clock::now();
    printBoard(grid);
    std::cout << std::endl;
    solve();
    printBoard(grid);
    //create();

    // Some computation here
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s"
              << std::endl;


    return 0;
}
