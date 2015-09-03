#include <iostream>
#include <cstring>
#include <vector>

const int ANSWER_DIVIDER = 1000000007,
        MAX_CHAR_VALUE = 255,
        FT_SIZE = 1001,
        EMPTY_VALUE = -1,
        k = 26;

struct bohr_vertex_t {
    size_t next_vertex[k], auto_move[k];
    size_t suff_link, suff_flink, par;
    bool is_terminal;
    char symbol;
};

std::vector<bohr_vertex_t> bohr;

bohr_vertex_t bohr_create_vertex(size_t p, char c) {
    bohr_vertex_t v;
    memset(&v.next_vertex, MAX_CHAR_VALUE, sizeof(v.next_vertex));
    memset(&v.auto_move, MAX_CHAR_VALUE, sizeof(v.auto_move));
    v.is_terminal = false;
    v.suff_link = EMPTY_VALUE;
    v.suff_flink = EMPTY_VALUE;
    v.par = p;
    v.symbol = c;

    return v;
}

void bohr_init() {
    bohr.push_back(bohr_create_vertex(0, '$'));
}

void bohr_append_word(const std::string& s) {
    size_t num = 0;
    for (size_t i = 0; i < s.length(); ++i) {
        char ch = s[i] - 'a';
        if (bohr[num].next_vertex[ch] == EMPTY_VALUE) {
            bohr.push_back(bohr_create_vertex(num, ch));
            bohr[num].next_vertex[ch] = bohr.size() - 1;
        }
        num = bohr[num].next_vertex[ch];
    }
    bohr[num].is_terminal = true;
}

size_t bohr_auto_move(size_t v, char ch);

size_t bohr_suff_link(size_t v) {
    if (bohr[v].suff_link == EMPTY_VALUE) {
        bohr[v].suff_link = (v == 0 || bohr[v].par == 0) ? 0 :
                            bohr_auto_move(bohr_suff_link(bohr[v].par), bohr[v].symbol);
    }
    return bohr[v].suff_link;
}

size_t bohr_auto_move(size_t v, char ch) {
    if (bohr[v].auto_move[ch] == EMPTY_VALUE) {
        if (bohr[v].next_vertex[ch] != EMPTY_VALUE) {
            bohr[v].auto_move[ch] = bohr[v].next_vertex[ch];
        } else {
            bohr[v].auto_move[ch] = (v == 0) ? 0 :
                                    bohr_auto_move(bohr_suff_link(v), ch);
        }
    }
    return bohr[v].auto_move[ch];
}

size_t bohr_suff_flink(size_t v) {
    if (bohr[v].suff_flink == EMPTY_VALUE) {
        size_t u = bohr_suff_link(v);
        if (u == 0) {
            bohr[v].suff_flink = 0;
        } else {
            bohr[v].suff_flink = (bohr[u].is_terminal) ? u : bohr_suff_flink(u);
        }
    }
    return bohr[v].suff_flink;
}

bool bohr_is_terminal(size_t v) {
    for (size_t u = v; u != 0; u = bohr_suff_flink(u)) {
        if (bohr[u].is_terminal) {
            return true;
        }
    }
    return false;
}

int64_t forbidden(
        size_t n, size_t l, size_t v,
        int64_t forbidden_table[FT_SIZE][FT_SIZE], int64_t &term_count
) {
    int64_t sum = 0;

    if(forbidden_table[n][v] == EMPTY_VALUE) {
        for (size_t i = 0; i < l; i++) {
            size_t u = bohr_auto_move(v, i);
            int64_t forbidden_value = forbidden_table[n-1][u];

            if(forbidden_value == EMPTY_VALUE && bohr_is_terminal(u)) {
                int64_t new_term_count = 1;
                for(size_t i = 1; i < n; i++) {
                    new_term_count *= l;
                    new_term_count %= ANSWER_DIVIDER;
                }
                forbidden_table[n-1][u] = new_term_count;
                term_count += new_term_count;
                sum += new_term_count;
            } else if (forbidden_value == EMPTY_VALUE) {
                forbidden_table[n-1][u] = forbidden(n - 1, l, u, forbidden_table, term_count);
                sum += forbidden_table[n-1][u];
            } else {
                term_count += forbidden_value;
                sum += forbidden_value;
            }
        }
    } else {
        term_count += forbidden_table[n][v];
    }
    sum %= ANSWER_DIVIDER;
    return sum;
}


int64_t get_answer(size_t n, size_t l) {
    int64_t forbidden_table[FT_SIZE][FT_SIZE];
    int64_t term_count = 0;

    memset(forbidden_table, EMPTY_VALUE, FT_SIZE * FT_SIZE * sizeof(int64_t));
    size_t bohr_size = bohr.size();

    for (size_t i = 0; i <= bohr_size; ++i) {
        forbidden_table[0][i] = bohr_is_terminal(i) ? 1 : 0;
    }
    forbidden(n, l, 0, forbidden_table, term_count);
    l %= ANSWER_DIVIDER;

    int64_t p = 1;
    for(size_t i = 1; i <= n; i++) {
        p *= l;
        p %= ANSWER_DIVIDER;
    }

    if (term_count > p)
        p += ANSWER_DIVIDER;
    term_count %= ANSWER_DIVIDER;

    return (p - term_count) % ANSWER_DIVIDER;
}

int main(int argc, const char * argv[]) {
    size_t n, k, l;
    std::cin >> n >> k >> l;

    bohr_init();
    std::string w = "";

    for (size_t i = 0; i < k; ++i) {
        std::cin >> w;
        bohr_append_word(w);
    }
    std::cout << get_answer(n, l) << std::endl;

    return 0;
}