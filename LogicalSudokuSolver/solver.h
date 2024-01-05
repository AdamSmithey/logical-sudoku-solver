#include <vector>
#include <string>
#include <unordered_set>
#include <utility>

#ifndef SOLVER_H
#define SOLVER_H

class Solver
{
    int board[9][9] = {{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0}};

    std::unordered_set<int> candidates[9][9];

    std::vector<std::pair<int, int>> checked_pairs;

    // create color node
    public: struct node {
        int x, y, val, color, chain;
        bool end = false;

        bool operator==(const node& n) const {
            return (this->x == n.x && this->y == n.y && this->val == n.val);
        }
    };

    // create hash for that node
    class ColorNodeHash {
    public:
        // create unique 3 digit hash for each element
        size_t operator()(const node& t) const {
            return t.y*10 + t.x*100 + t.val*10000;
        }
    };

    std::vector<std::pair<int, int>> connections(int x, int y, int max);
    std::vector<std::unordered_set<node, ColorNodeHash>> single_chains();
    std::vector<std::pair<int, int>> get_pairs();



    void set_square(int i, int j, int v);
    void init_cand();
    void trim_cand(int i, int j);
    void trim_cand(int i, int j, int v, std::vector<std::pair<int, int>> e);

    // basic
    bool remove_singles(); // finished
    bool hidden_singles(); // finished
    bool naked_pairs_and_triples(); // finished
    bool hidden_pairs_and_triples();
    bool pointing_pairs_and_triples();
    bool box_line_reduction();

    // tough
    bool x_wing();
    bool simple_coloring();
    bool y_wing();

    public:
        void load_puzzle(const std::string& s);

        void solve();
        explicit Solver(const std::string& s);

};

#endif

#ifndef UTIL_H
#define UTIL_H

class util {

    public:
        static bool contains(const std::unordered_set<int>& u1, const std::unordered_set<int>& u2);
        static std::unordered_set<int> intersection(const std::vector<std::unordered_set<int>>& v);
        static std::unordered_set<int> intersection(const std::vector<std::vector<int>>& v);
        static std::unordered_set<int> c_union(const std::vector<std::unordered_set<int>>& v);
        static std::unordered_set<int> difference(const std::vector<std::unordered_set<int>> &v);
        static std::vector<std::unordered_set<int>> remove_similar(const std::vector<std::unordered_set<int>> &v);
};

#endif
