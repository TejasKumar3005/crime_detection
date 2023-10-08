#include <iostream>
#include <vector>
#include <sstream>
#include <cmath>

class CNFBuilder {
private:
    std::ostringstream cnf;
    int varCount;
    int clauseCount;

    int getNewVar() {
        return ++varCount;
    }

    void addClause(const std::vector<int>& literals) {
        for (int lit : literals) {
            cnf << lit << " ";
        }
        cnf << "0\n";
        clauseCount++;
    }

    // Add XOR clauses for A XOR B = S
    void addXORClauses(int A, int B, int S) {
        cnf << "c XOR clauses for " << A << " XOR " << B << " = " << S << "\n";
        cnf << A << " " << B << " " << -S << " 0\n";
        cnf << A << " " << -B << " " << S << " 0\n";
        cnf << -A << " " << B << " " << S << " 0\n";
        cnf << -A << " " << -B << " " << -S << " 0\n";
        clauseCount += 4;
    }

    // Add AND clauses for A AND B = C
    void addANDClauses(int A, int B, int C) {
        cnf << "c AND clauses for " << A << " AND " << B << " = " << C << "\n";
        cnf << -A << " " << -B << " " << C << " 0\n";
        cnf << A << " " << -C << " 0\n";
        cnf << B << " " << -C << " 0\n";
        clauseCount += 3;
    }

public:
    CNFBuilder(int n) : varCount(n), clauseCount(0) {}

    std::string buildCNF(int n, int k, const std::vector<int>& literals) {
        int bits = std::ceil(log2(k + 1));
        std::vector<std::vector<int>> counters(n, std::vector<int>(bits+1));
        std::vector<std::vector<int>> sums(n+1, std::vector<int>(bits));


        // Initialize counters with the first literal
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < bits+1; ++j) {
            if (j == 0) {
                counters[i][j] = literals[i];
            } else {
                counters[i][j] = getNewVar();
            }
        }
        }

        // initialize sums 
        for (int i = 0; i <= n; ++i)
        {
            for (int j = 0; j < bits; ++j) {
                
                sums[i][j] = getNewVar();
                if (i == 0){
                    addClause({ - sums[i][j]});
                }
            }
        }
        

        // Binary addition sums and counters
        for (int i = 1; i <= n; ++i) {
            // add counter[i-1] and sums[i-1] and store in sums[i] if need to do bit wise carry allot it to counter[i-1][j+1]
            for (int j = 0; j < bits; ++j) {
                addXORClauses(counters[i - 1][j], sums[i - 1][j], sums[i][j]);
                addANDClauses(counters[i - 1][j], sums[i - 1][j], counters[i - 1][j + 1]);
            }
        }

        // write clause for sum[n] as k in binary
        std::vector<int> kBinary(bits, 0);
        for (int i = 0; i < bits; ++i) {
            if (k & (1 << i)) {
                kBinary[i] = 1;
            }
        }
        for (int i = 0; i < bits; ++i) {
            if (kBinary[i] == 1) {
                addClause({sums[n][i]});
            } else {
                addClause({-sums[n][i]});
            }
        }

        // write clause for each counter[i][bits] should be 0
        for (int i = 0; i < n; ++i) {
            addClause({-counters[i][bits]});
        }


        // Return CNF in DIMACS format
        std::ostringstream output;
        output << "p cnf " << varCount << " " << clauseCount << "\n" << cnf.str();
        return output.str();
    }
};

int main() {
    int n, k;
    std::cout << "Enter n (number of literals): ";
    std::cin >> n;
    std::cout << "Enter k (number of true literals): ";
    std::cin >> k;
    
    std::vector<int> literals(n);
    // std::cout << "Enter the " << n << " literals: ";
    // for (int i = 0; i < n; ++i) {
    //     std::cin >> literals[i];
    // }

    // make literals 1 to n
    std::cout << std::endl;
    for (int i = 0; i < n; ++i) {
        literals[i] = i+1;
    }

    CNFBuilder builder(n);
    std::cout << builder.buildCNF(n, k, literals) << std::endl;

    return 0;
}
