#include <string>
#include <unordered_set>
#include <set>
#include <utility>
#include <vector>
#include <iostream>
#include <climits>
#include "solver.h"
#include <functional>
#include <algorithm>

using namespace std;

int score = 0;
bool printTime = false;

/**
 * finds all squares that intersect with the given square and have
 * candidates remaining
 * 
 * @param x x position of the square
 * @param y y position of the square
*/
std::vector<std::pair<int, int>> Solver::connections(int x, int y, int max = INT_MAX) {
   // std::swap(x, y);
    std::vector<std::pair<int, int>> connections;

    for(int i = 0; i < 9; i++) {
        // iterate horizontally
        if(i != y && candidates[x][i].size() <= max && candidates[x][i].size() > 1) {
            connections.emplace_back(x, i);
        }

        // iterate vertically
        if(i != x && candidates[i][y].size() <= max && candidates[i][y].size() > 1) {
            connections.emplace_back(i, y);
        }

        // iterate cube
        int cx = 3 * (x / 3) + i % 3;
        int cy = 3 * (y / 3) + i / 3;
        if((cx != x && cy != y) && candidates[cx][cy].size() <= max && candidates[cx][cy].size() > 1) {
            connections.emplace_back(cx, cy);
        }
    }

    return connections;
}

std::vector<std::unordered_set<Solver::node, Solver::ColorNodeHash>> Solver::single_chains() {
    std::vector<std::unordered_set<node, ColorNodeHash>> nodes(9);

    /*
     * Strategy: Look through each square and find
     */
    // color graph first
    int chains = 0;
    std::vector<std::pair<int, int>> row[9][9], col[9][9], square[9][9];

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            int si = 3 * (i / 3) + j / 3;
            int sj = 3 * (i % 3) + j % 3;

            for (int m: candidates[si][sj]) {
                if (m != 0) {
                    row[si][m - 1].emplace_back(si, sj);
                    col[sj][m - 1].emplace_back(si, sj);
                    square[i][m - 1].emplace_back(si, sj);
                }
            }
        }
    }

    // dir 0 = row
    // dir 1 = col
    // dir 2 = square
    std::function<void(int, int, int, int, int)> eval = [&nodes, row, col, square, &chains, &eval](int i,
                                                                                                   int j,
                                                                                                   int num,
                                                                                                   int color,
                                                                                                   int dir) {
        if (dir != 0 && row[i][num].size() == 2) {
            for (auto p: row[i][num]) {
                if (p.second != j) {
                    node n = {p.first, p.second, num, color == 0 ? 1 : 0, chains};
                    if (nodes[num].count(n) == 0) {
                        nodes[num].insert(n);
                        eval(p.first, p.second, num, color == 0 ? 1 : 0, 0);
                    }
                }
            }
        }

        if (dir != 1 && col[j][num].size() == 2) {
            for (auto p: col[j][num]) {
                if (p.first != i) {
                    node n = {p.first, p.second, num, color == 0 ? 1 : 0, chains};
                    if (nodes[num].count(n) == 0) {
                        nodes[num].insert(n);
                        eval(p.first, p.second, num, color == 0 ? 1 : 0, 1);
                    }
                }
            }
        }

        int si = 3 * (i / 3) + j / 3;
        if (dir != 2 && square[si][num].size() == 2) {
            for (auto p: square[si][num]) {
                if (i != p.first && j != p.second) {
                    node n = {p.first, p.second, num, color == 0 ? 1 : 0, chains};
                    if (nodes[num].count(n) == 0) {
                        nodes[num].insert(n);
                        eval(p.first, p.second, num, color == 0 ? 1 : 0, 2);
                    }
                }
            }
        }
    };

    // create chains ~0.200ms (pretty slow)
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            for (int m: candidates[i][j]) {
                if (m != 0) {
                    Solver::node n = {i, j, m - 1, 0, chains};
                    if (nodes[m - 1].count(n) == 0) {
                        eval(i, j, m - 1, chains, -1);
                        chains += 1;
                    }
                }
            }
        }
    }

    return nodes;
}

std::vector<pair<int, int>> Solver::get_pairs() {
    std::vector<pair<int, int>> pairs;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (candidates[i][j].size() == 2) pairs.emplace_back(i, j);
        }
    }

    return pairs;
}

std::vector<std::unordered_set<int>> util::remove_similar(const std::vector<std::unordered_set<int>> &v) {
    std::vector<std::unordered_set<int>> s = v;
    for (int i = 0; i < s.size(); i++) {
        for (int j = i + 1; j < s.size(); j++) {
            for (int m: v[i]) {
                if (v[j].count(m) > 0) {
                    s[i].erase(m);
                    s[j].erase(m);
                }
            }
        }
    }

    return s;
}

bool util::contains(
        const std::unordered_set<int> &u1,
        const std::unordered_set<int> &u2) {
    if (u2.size() > u1.size()) return false;

    int c = 0;

    for (int i: u1) if (u2.count(i) == 1) c++;

    return c == u2.size();
}

/**
 * returns the set of interesctions
*/
std::unordered_set<int> util::intersection(
        const std::vector<std::unordered_set<int>> &v) {

    std::vector<std::vector<int>> vn;
    for(std::unordered_set<int> u : v) {
        std::vector<int> vector;
        vector.reserve(u.size());
        for (auto it = u.begin(); it != u.end(); ) {
            vector.push_back(std::move(u.extract(it++).value()));
        }

        vn.push_back(vector);
    };
    
    return intersection(vn);
}

std::unordered_set<int> util::intersection(
        const std::vector<std::vector<int>> &v) {
    unordered_set<int> u;
    for (int i = 0; i < v.size(); i++) {
        std::vector<int> u1 = v[i];
        for (int j = i + 1; j < v.size(); j++) {
            std::vector<int> u2 = v[j];
            if (u1.size() < u2.size()) {
                for (int k: u1) {
                    if (std::count(u2.begin(), u2.end(), k) > 0) u.insert(k);
                }
            } else {
                for (int k: u2) {
                    if (std::count(u1.begin(), u1.end(), k) > 0) u.insert(k);
                }
            }
        }
    }
    return u;
}

/**
 * This function creates a single unordered set from a 
 * vector of unordered sets. 
 * 
 * @param v vector of unordered sets
 * @return a single unordered set that represents the 
 * union of the unordered sets in the parameter v
*/
std::unordered_set<int> util::c_union(
        const std::vector<std::unordered_set<int>> &v) {
    int size = 0;
    for (const std::unordered_set<int> &i: v) size += i.size() & INT_MAX;
    unordered_set<int> u;
    u.reserve(size);
    for (const std::unordered_set<int> &vu: v) {
        for (int i: vu) {
            u.insert(i);
        }
    }
    return u;
}

/**
 * find the differences between a vector of unordered sets
 * 
 * @param v
 * @return
 */
std::unordered_set<int> util::difference(
        const std::vector<std::unordered_set<int>> &v) {
    unordered_set<int> u = c_union(v);
    unordered_set<int> d = intersection(v);
    for (int i: d) u.erase(i);

    return u;
}

/**
 * sets the value of the square at a specific index and trims the candidates affected
 * 
 * @param i row of square
 * @param j column of the square
 * @param v value fo be assigned
 */
void Solver::set_square(int i, int j, int v) {
    board[i][j] = v;
    trim_cand(i, j);
}

void Solver::load_puzzle(const std::string &s) {
    if (s.length() != 81) {
        cout << "Error! The entered board does not have 81 values!";
        return;
    }

    for (int i = 0; i < 81; i++) {
        if (s.at(i) != '_' && s.at(i) != '0') set_square(i / 9, i % 9, s.at(i) - '0');
        //else if(s.at(i) == '_' && s.at(i) == '0') set_square(i / 9, i % 9, 0);
    }
}

void Solver::init_cand() {
    for (auto &candidate: candidates) {
        std::vector<std::unordered_set<int> > row;
        for (auto &j: candidate) {
            j = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        }
    }
}

/**
 * Remove all candidates that interact with a filled square on the board
 * specified by coordinates i, j
 * @param i row of square
 * @param j column of square
 */
void Solver::trim_cand(int i, int j) {
    candidates[i][j].clear();
    candidates[i][j].insert(0);

    for (int k = 0; k < 9; k++) {
        candidates[i][k].erase(board[i][j]);
        candidates[k][j].erase(board[i][j]);
        candidates[k / 3 + 3 * (i / 3)][k % 3 + 3 * (j / 3)].erase(board[i][j]);
    }
}

/**
 * Will remove candidates of a specific number relative to a specific square, specified by coordinates i, j
 * @param i row of square
 * @param j column of square
 * @param v value to trim
 * @param e squares to exclude
 */
void Solver::trim_cand(int i, int j, int v, std::vector<std::pair<int, int>> e) {
    for (int k = 0; k < 9; k++) {
        if (std::find(e.begin(), e.end(), std::make_pair(i, k)) != e.end()) candidates[i][k].erase(v);
        if (std::find(e.begin(), e.end(), std::make_pair(k, j)) != e.end()) candidates[k][j].erase(v);
        if (std::find(e.begin(), e.end(), std::make_pair(k / 3 + 3 * (i / 3), k % 3 + 3 * (j / 3))) !=
            e.end())
            candidates[k / 3 + 3 * (i / 3)][k % 3 + 3 * (j / 3)].erase(v);
    }
}

// finished
bool Solver::remove_singles() {
    auto start = std::chrono::system_clock::now();
    bool check = false;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (candidates[i][j].size() == 1 && board[i][j] == 0) {
                check = true;
                set_square(i, j, *candidates[i][j].begin());
            }
        }
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    if(printTime && check)
        std::cout << "remove singles - successful: " << (check ? "true" : "false")
                  << " - elapsed time: " << elapsed_seconds.count() * 1000 << "s"
                  << std::endl;

    return check;
}

/**
 * find and remove hidden singles
 * @return
 */
// finished
bool Solver::hidden_singles() {
    auto start = std::chrono::system_clock::now();

    bool check = false;
    // check rows, columns, and squares for matches
    for (int i = 0; i < 9; i++) {
        std::vector<std::vector<std::pair<int, int>>> v1(9); // rows
        std::vector<std::vector<std::pair<int, int>>> v2(9); // columns
        std::vector<std::vector<std::pair<int, int>>> v3(9); // squares
        for (int j = 0; j < 9; j++) {
            // look in rows and add elements
            for (int k: candidates[i][j]) {
                if (k != 0) v1[k - 1].emplace_back(i, j);
            }

            // look in columns and add elements
            for (int k: candidates[j][i]) {
                if (k != 0) v2[k - 1].emplace_back(j, i);
            }

            // look in squares and add elements
            for (int k: candidates[j % 3 + 3 * (i / 3)][j / 3 + 3 * (i % 3)]) {
                if (k != 0) v3[k - 1].emplace_back(j % 3 + 3 * (i / 3), j / 3 + 3 * (i % 3));
            }
        }

        for (int j = 0; j < 9; j++) {
            if (v1[j].size() == 1) {
                check = true;
                set_square(v1[j][0].first, v1[j][0].second, j + 1);
            }
            if (v2[j].size() == 1) {
                check = true;
                set_square(v2[j][0].first, v2[j][0].second, j + 1);
            }
            if (v3[j].size() == 1) {
                check = true;
                set_square(v3[j][0].first, v3[j][0].second, j + 1);
            }
        }

        remove_singles();
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    if(printTime && check)
        std::cout << "hidden singles - successful: " << (check ? "true" : "false")
                  << " - elapsed time: " << elapsed_seconds.count() * 1000 << "s"
                  << std::endl;

    return check;
}

/**
 * find all naked pairs and adjust candidates accordingly
 * WAY to fast on false, make slower
 * @return true of this returned a value
 */
// (finished) (ADD COMMENTS later)
bool Solver::naked_pairs_and_triples() {
    auto start = std::chrono::system_clock::now();

    bool check = false;
    std::vector<std::pair<int, int>> v;
    auto test = [check](auto rule, std::vector<std::unordered_set<int>> v) {
        for (int j = 0; j < v.size(); j++) {
            for (int k = j + 1; k < v.size(); k++) {
                std::unordered_set<int> un = util::c_union({v[j], v[k]});

                if (un.size() == 2) {
                    for (int m: un) for (int l = 0; l < 9; l++) rule(l, m, un);
                } else if (un.size() == 3) {
                    for (int l = k + 1; l < v.size(); l++) {
                        // if these three make a triplet, then square must be included in the union of all elements
                        if (util::contains(un, v[l])) {
                            for (int n = 0; n < 9; n++) {
                                auto a = un.begin();

                                // why? because I want to
                                rule(n, a.operator*(), un);
                                a.operator++();
                                rule(n, a.operator*(), un);
                                a.operator++();
                                rule(n, a.operator*(), un);
                                if(check) return;
                            }
                        }
                    }
                }
            }
        }
    };

    // add all pairs to vector v
    for (int i = 0; i < 9; i++) {
        std::vector<std::unordered_set<int>> v1; // rows
        std::vector<std::unordered_set<int>> v2; // columns
        std::vector<std::unordered_set<int>> v3; // squares
        for (int j = 0; j < 9; j++) {
            if (candidates[i][j].size() == 2 || candidates[i][j].size() == 3) v1.emplace_back(candidates[i][j]);
            if (candidates[j][i].size() == 2 || candidates[j][i].size() == 3) v2.emplace_back(candidates[j][i]);
            if (candidates[j % 3 + 3 * (i / 3)][j / 3 + 3 * (i % 3)].size() == 2 ||
                candidates[j % 3 + 3 * (i / 3)][j / 3 + 3 * (i % 3)].size() == 3)
                v3.emplace_back(candidates[j % 3 + 3 * (i / 3)][j / 3 + 3 * (i % 3)]);
        }

        auto v1_rule = [this, &check, &i](int l, int m, const std::unordered_set<int> &u) {
            if (candidates[i][l].size() == 1) return;
            if (!util::contains(u, candidates[i][l]) && candidates[i][l].erase(m)) check = true;
        };

        auto v2_rule = [this, &check, &i](int l, int m, const unordered_set<int> &u) {
            if (candidates[l][i].size() == 1) return;
            if (!util::contains(u, candidates[l][i]) && candidates[l][i].erase(m)) check = true;
        };

        auto v3_rule = [this, &check, &i](int l, int m, const unordered_set<int> &u) {
            if (candidates[(l % 3) + 3 * (i / 3)][(l / 3) + 3 * (i % 3)].size() == 1) return;
            if (!util::contains(u, candidates[(l % 3) + 3 * (i / 3)][(l / 3) + 3 * (i % 3)])
                && candidates[(l % 3) + 3 * (i / 3)][(l / 3) + 3 * (i % 3)].erase(m))
                check = true;
        };

        if (v1.size() > 1) test(v1_rule, v1);
        if (v2.size() > 1) test(v2_rule, v2);
        if (v3.size() > 1) test(v3_rule, v3);
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    if(printTime && check)
        std::cout << "naked pairs - successful: " << (check ? "true" : "false")
                  << " - elapsed time: " << elapsed_seconds.count() * 1000 << "s"
                  << std::endl;

    return check;
}

bool Solver::hidden_pairs_and_triples() {
    auto start = std::chrono::system_clock::now();

    bool check = false;
    auto test = [](auto rule, std::vector<int> *v) {
        for (int i = 0; i < 9; i++) {
            for (int j = i + 1; j < 9; j++) {
                if (v[i] == v[j] && v[i].size() == 2) {
                    // remove all candidates that are not the pair from the square
                    for (int m = 0; m < 9; m++) {
                        if (m != i && m != j) {
                            rule(v[i][0], m + 1);
                            rule(v[i][1], m + 1);
                        }
                    }
                }
            }
        }
    };

    // count the frequency of each candidate in a row, column, and square
    for (int i = 0; i < 9; i++) {
        std::vector<int> v1[9]; // rows
        std::vector<int> v2[9]; // columns
        std::vector<int> v3[9]; // squares
        for (int j = 0; j < 9; j++) {
            // look in rows and add elements
            for (int k: candidates[i][j]) {
                if (k != 0) v1[k - 1].emplace_back(j);
            }

            // look in columns and add elements
            for (int k: candidates[j][i]) {
                if (k != 0) v2[k - 1].emplace_back(j);
            }

            // look in squares and add elements
            for (int k: candidates[j % 3 + 3 * (i / 3)][j / 3 + 3 * (i % 3)]) {
                if (k != 0) v3[k - 1].emplace_back(j);
            }
        }

        auto v1_rule = [this, &check, &i](int l, int m) {
            if (candidates[i][l].erase(m)) check = true;
        };

        auto v2_rule = [this, &check, &i](int l, int m) {
            if (candidates[l][i].erase(m)) check = true;

        };

        auto v3_rule = [this, &check, &i](int l, int m) {
            if (candidates[(l % 3) + 3 * (i / 3)][(l / 3) + 3 * (i % 3)].erase(m)) check = true;
        };

        test(v1_rule, v1);
        test(v2_rule, v2);
        test(v3_rule, v3);

    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    if(printTime && check)
        std::cout << "hidden pairs - successful: " << (check ? "true" : "false")
                  << " - elapsed time: " << elapsed_seconds.count() * 1000 << "s"
                  << std::endl;

    return check;
}

/**
 *
 * @return
 */
// finished

bool Solver::pointing_pairs_and_triples() {
    auto start = std::chrono::system_clock::now();

    bool check = false;

    // iterate through all squares
    for (int i = 0; i < 9; i++) {
        std::vector<std::unordered_set<int>> v1(3); // rows
        std::vector<std::unordered_set<int>> v2(3); // columns

        // iterate through all areas in squares
        for (int j = 0; j < 9; j++) {
            int si = 3 * (i / 3) + j / 3;
            int sj = 3 * (i % 3) + j % 3;

            // add to rows
            if (*candidates[si][sj].begin() != 0) {
                v1[j / 3] = util::c_union({v1[j / 3], candidates[si][sj]});
                v2[j % 3] = util::c_union({v2[j % 3], candidates[si][sj]});
            }
        }

        // check for rows
        for (int k: util::difference({v1[0], v1[1], v1[2]})) {
            if (v1[0].count(k) > 0) {
                for (int j = 0; j < 9; j++) {
                    if ((j < 3 * (i % 3) || j >= 3 * (i % 3) + 3) && candidates[3 * (i / 3)][j].erase(k)) check = true;
                }
            } else if (v1[1].count(k) > 0) {
                for (int j = 0; j < 9; j++) {
                    if ((j < 3 * (i % 3) || j >= 3 * (i % 3) + 3) && candidates[3 * (i / 3) + 1][j].erase(k))
                        check = true;
                }
            } else if (v1[2].count(k) > 0) {
                for (int j = 0; j < 9; j++) {
                    if ((j < 3 * (i % 3) || j >= 3 * (i % 3) + 3) && candidates[3 * (i / 3) + 2][j].erase(k))
                        check = true;
                }
            }
        }

        // column
        for (int k: util::difference({v2[0], v2[1], v2[2]})) {
            if (v2[0].count(k) > 0) {
                for (int j = 0; j < 9; j++) {
                    if ((j < 3 * (i / 3) || j >= 3 * (i / 3) + 3) && candidates[j][3 * (i % 3)].erase(k)) check = true;
                }
            } else if (v2[1].count(k) > 0) {
                for (int j = 0; j < 9; j++) {
                    if ((j < 3 * (i / 3) || j >= 3 * (i / 3) + 3) && candidates[j][3 * (i % 3) + 1].erase(k))
                        check = true;
                }
            } else if (v2[2].count(k) > 0) {
                for (int j = 0; j < 9; j++) {
                    if ((j < 3 * (i / 3) || j >= 3 * (i / 3) + 3) && candidates[j][3 * (i % 3) + 2].erase(k))
                        check = true;
                }
            }
        }
    }

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    if(printTime && check)
        std::cout << "pointing pairs - successful: " << (check ? "true" : "false")
                  << " - elapsed time: " << elapsed_seconds.count() * 1000 << "s"
                  << std::endl;

    return check;
}

// finished
bool Solver::box_line_reduction() {
    bool check = false;
    auto start = std::chrono::system_clock::now();

    // check all the lines and columns and look for box line reduction
    /* -- I need to make more strategy outlines --
     * Strategy: iterate through each row and column, and separate these lines
     * and columns by squares. Then, remove all similarities from the parsed
     * lines and columns. Then, if there is a number left over in a square parsing,
     * remove that number from the rest of the square, ignoring that LINE.
     */
    for (int i = 0; i < 9; i++) {
        std::vector<std::unordered_set<int>> row(3);
        std::vector<std::unordered_set<int>> col(3);

        // iterate through the row and column, splitting these rows and columns into 3 seperate sections,
        // with each section representing the intersections between the lines and the three squares that
        // runs through.
        for (int j = 0; j < 9; j++) {
            if (candidates[i][j].size() > 1) row[j / 3] = util::c_union({row[j / 3], candidates[i][j]});
            if (candidates[j][i].size() > 1) col[j / 3] = util::c_union({col[j / 3], candidates[j][i]});
        }

        // Derive unique candidates and retain their position.
        // All leftover numbers are to be used in the box line reduction.
        row = util::remove_similar(row);
        col = util::remove_similar(col);

        // iterate through each row and column in the square.
        for (int j = 0; j < 3; j++) {
            for (int m: row[j]) {
                // iterate through the square and remove m from all slots
                for (int k = 0; k < 9; k++) {
                    int si = k / 3 + (3 * (i / 3));
                    int sj = k % 3 + (3 * j);
                    if (si != i && candidates[si][sj].erase(m)) check = true;
                }
            }

            for (int m: col[j]) {
                // iterate through the square and remove m from all slots
                for (int k = 0; k < 9; k++) {
                    int si = k / 3 + (3 * (i / 3));
                    int sj = k % 3 + (3 * j);
                    if (si != i && candidates[sj][si].erase(m)) check = true;
                }
            }
        }
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    if(printTime && check)
        std::cout << "box line reduction - successful: " << (check ? "true" : "false")
                  << " - elapsed time: " << elapsed_seconds.count() * 1000 << "s"
                  << std::endl;

    return check;
}

/**
 *
 * @return
 */
// finished
bool Solver::x_wing() {
    auto start = std::chrono::system_clock::now();
    bool check = false;

    std::vector<int> rows[9][9], cols[9][9];

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            // row
            for (int m: candidates[i][j]) {
                if (m == 0) continue;
                rows[i][m - 1].emplace_back(j);
            }

            for (int m: candidates[j][i]) {
                if (m == 0) continue;
                cols[i][m - 1].emplace_back(j);
            }
        }
    }

    auto erase = [this](int i, int j, int k) { return candidates[i][j].erase(k); };

    auto eval = [&check, erase](int id, int i, int j, std::vector<int> v[9][9]) {
        if (v[i][j].size() == 2) {
            // iterate through the rest of the rows again
            for (int k = i + 1; k < 9; k++) {
                // iterate through the columns

                if (v[k][j].size() == 2 && v[k][j] == v[i][j]) {
                    int m = v[i][j][0];
                    int n = v[i][j][1];
                    for (int l = 0; l < 9; l++) {
                        if (l != i && l != k) {
                            if (id == 0) {
                                if (erase(l, m, j + 1)) check = true;
                                if (erase(l, n, j + 1)) check = true;
                            } else {
                                if (erase(m, l, j + 1)) check = true;
                                if (erase(n, l, j + 1)) check = true;
                            }
                        }
                    }
                }
            }
        }
    };

    // loop through each number
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            eval(0, i, j, rows); // rows
            eval(1, i, j, cols); // cols
        }
    }


    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    if(printTime && check)
        std::cout << "x wing - successful: " << (check ? "true" : "false")
                  << " - elapsed time: " << elapsed_seconds.count() * 1000 << "s"
                  << std::endl;

    return check;
}

/**
 * Simple coloring colors the graph in a simple manner.
 * @return
 */
// finished
bool Solver::simple_coloring() {
    auto start = std::chrono::system_clock::now();
    bool check = false;

    std::vector<std::unordered_set<Solver::node, Solver::ColorNodeHash>> nodes = single_chains();

    // iterate through numbers 1 - 9
    for (int i = 0; i < 9; i++) {

        /*
         * "u" is the set of all nodes with the value "i"
         * This is done for simplicity's sake.
         */
        auto u = nodes[i];

        // rule 2 (run rule 2 before rule 4)
        /* Method:
         * compare all elements of u
         */
        unordered_set<int> used;
        for (auto j = u.begin(); j != u.end(); ++j) {

            // create a temp iterator of j
            auto temp = j;

            for (auto k = ++temp; k != u.end(); ++k) {

                if (j->chain == k->chain && j->color == k->color) {
                    if (j->x == k->x || j->y == k->y) {
                        for (const auto &m: u) {
                            if (m.chain == j->chain && m.color == j->color) {
                                candidates[m.x][m.y].erase(m.val + 1);
                                check = true;
                            }
                        }
                    }
                }
            }
        }

        // rule 4
        /* Method:
         * We will compare each element in each chain, and we check the intersection points.
         * If this intersection point contains the value of the two nodes, we will remove that
         * value.
         */
        for (auto j = u.begin(); j != u.end(); ++j) {

            /*
             * create a temp iterator of j
             */
            auto temp = j;

            // iterate through u again to compare all elements in u
            for (auto k = ++temp; k != u.end(); ++k) {

                /*
                 * Check for the conditions of rule 2. If the chain is the same and the colors
                 * differ, the requirements are satisfied. We proceed to execute the rule.
                 */
                if (j->chain == k->chain && j->color != k->color) {

                    /*
                     * Check if the two nodes are in the same row. If so, iterate through that row
                     * and remove any candidate of value "i" that is not in either of the nodes.
                     */
                    if (j->x == k->x) {
                        for (int l = 0; l < 9; l++) {
                            if (u.count({j->x, l, i}) == 0
                                && candidates[j->x][l].erase(i + 1)) {
                                check = true;
                            }
                        }
                    }

                        /*
                         * Next, check if the two nodes are in the same col. If so, iterate through that
                         * col and remove any candidate of value "i" that is not in either of the nodes.
                         */
                    else if (j->y == k->y) {
                        for (int l = 0; l < 9; l++) {
                            if (u.count({l, j->y, i}) == 0
                                && candidates[l][j->y].erase(i + 1)) {
                                check = true;
                            }
                        }
                    }

                        /*
                         * If the nodes are not in similar rows, columns, or squares, check the two
                         * intersection values of the nodes and, if that node is not in "u", remove the
                         * candidates of value "i".
                         */
                    else {
                        // check (x1, y2) intersection
                        if (u.count({.x = j->x, .y = k->y, .val = i}) == 0
                            && candidates[j->x][k->y].erase(i + 1)) {
                            check = true;
                        }

                        // check (x2, y1) intersection
                        if (u.count({.x = k->x, .y = j->y, .val = i}) == 0
                            && candidates[k->x][j->y].erase(i + 1)) {
                            check = true;
                        }
                    }
                }
            }
        }
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    if(printTime && check)
        std::cout << "simple coloring - successful: " << (check ? "true" : "false")
                  << " - elapsed time: " << elapsed_seconds.count() * 1000 << "s"
                  << std::endl;
    return check;
}

/**
 * Finished, need to make faster, **will come back to this
*/
bool Solver::y_wing() {
    auto start = std::chrono::system_clock::now();
    bool check = false;

    // find pairs
    std::vector<std::pair<int, int>> pairs = get_pairs();

    // check all pairs in  "pairs"
    for(pair<int, int> p : pairs) { 
        // cout << "P: " << p.first << " " << p.second << endl;
        std::vector<std::pair<int, int>> con = connections(p.first, p.second, 2);

        // for(auto&[a,b] : con) cout << a << " " << b << endl;

        bool b = false;
        // compare all connections
        for(int j = 0; j < con.size(); j++) {
            int ai = con.at(j).first, aj = con.at(j).second;
            if(candidates[ai][aj] == candidates[p.first][p.second]) continue;

            unordered_set<int> i1 = util::intersection({candidates[p.first][p.second], candidates[ai][aj]});
            
            // make sure the connection has one valid intersection with parent square
            if(i1.size() != 1) continue;

            // cout << "FIRST: " << ai << " " << aj << endl;
            // loop through all other connections to make pairs
            for(int k = j+1; k < con.size(); k++) {
                int bi = con.at(k).first, bj = con.at(k).second;
                if(candidates[bi][bj] == candidates[p.first][p.second]) continue;

                unordered_set<int> i2 = util::intersection({candidates[p.first][p.second], candidates[bi][bj]});

                // make sure connection has one valid intersection with parent 
                // that differs from the other connection
                if(i2.size() != 1 || *i1.begin() == *i2.begin()) continue;

                // cout << "SECOND: " << bi << " " << bj << endl;

                unordered_set<int> s1 = candidates[ai][aj];
                unordered_set<int> s2 = candidates[bi][bj];

                /*
                 * If these two cells have a single number in common that does not
                 * intersect with parent, we can continue on with the process.
                */
                std::unordered_set<int> i3 = util::intersection({s1, s2});
                if(i3.size() == 1 && s1.size() > 1 && s2.size() > 1) {
                    std::vector<pair<int, int>> c1 = connections(ai, aj);
                    std::vector<pair<int, int>> c2 = connections(bi, bj);


                    if(std::find(c1.begin(), c1.end(), con.at(k)) != c1.end()) continue;

                    for(int n = 0; n < c1.size(); n++) {
                        for(int m = 0; m < c2.size(); m++) {
                            //cout << "(" << c1[n].first << ", " << c2[n].second << "), (" << c2[m].first << ", " << c2[m].second << ")" << endl;
                            // ensure these pairs overlap and these pairs are not equal to
                            // the original square.
                            if(c1[n] == c2[m] && c1[n] != p && c1[n] != con.at(j) && c1[n] != con.at(k)) {
                               // cout << c1[n].first << " " << c1[n].second << " " << *i3.begin() << endl;
                                check = check || candidates[c1[n].first][c1[n].second].erase(*i3.begin()) != 0;
                                if(check) goto endf;
                            }   
                        }
                    }
                }
            }
        }
    }      

    endf:

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    if(printTime && check)
        std::cout << "y wing - successful: " << (check ? "true" : "false")
                  << " - elapsed time: " << elapsed_seconds.count() * 1000 << "s"
                  << std::endl;
    return check;
}



void Solver::solve() {
    printTime = true;
    while (true) {
        if (remove_singles()) {
            score += 3;
            continue;
        }
        if (hidden_singles()) {
            score += 3;
            continue;
        }
        if (naked_pairs_and_triples()) {
            score += 6;
            continue;
        }
        if (hidden_pairs_and_triples()) {
            score += 6;
            continue;
        }
        if (pointing_pairs_and_triples()) {
            score += 19;
            continue;
        }
        if (box_line_reduction()) {
            score += 23;
            continue;
        }
        if (x_wing()) {
            score += 50;
            continue;
        }
        if (simple_coloring()) {
            score += 50;
            continue;
        }
        if (y_wing()) {
            score += 50;
            continue;
        } 
        break;
    }
}


Solver::Solver(const std::string &s) {

    init_cand();
    load_puzzle(s);

    solve();

    cout << endl;

    // for (auto &candidate: candidates) {
    //     for (auto &j: candidate) {
    //         int c = 9;
    //         for (int k : j) {
    //             cout << k;
    //             c--;
    //         }
    //         for (int i = 0; i < c; i++) cout << " ";
    //         cout << " ";
    //     }
    //     cout << endl;
    // }

    for (auto &i: board) {
        for (int j : i) {
            cout << (j != 0 ? std::to_string(j) : "_") << "  ";
        }
        cout << endl;
    }

    cout << "Score: " << score << endl;
}



