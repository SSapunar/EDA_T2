#include "poscode.hpp"
#include "utils.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <cstdlib>

static inline void must(bool cond, const std::string& msg){
    if(!cond){ std::cerr << msg << "\n"; std::exit(1); }
}

static void bench_5x(const std::string& name,
                     const std::string& filepath,
                     size_t n,
                     void(*sorter)(Poscode*, size_t))
{
    std::vector<double> ts; ts.reserve(5);
    for(int rep=0; rep<5; ++rep){
        Poscode* A = readCodes(filepath, n);
        if(!A){
            std::cerr << "[WARN] No pude leer " << filepath << " (rep=" << rep << ")\n";
            return;
        }
        auto t0 = std::chrono::steady_clock::now();
        sorter(A, n);
        auto t1 = std::chrono::steady_clock::now();
        ts.push_back(std::chrono::duration<double>(t1 - t0).count());
        deleteCodes(A);
    }
    double mu=0, v=0; for(double x:ts) mu+=x; mu/=ts.size();
    for(double x:ts) v+=(x-mu)*(x-mu); v/=ts.size();
    std::cout << name << ": " << mu << " ± " << std::sqrt(v) << " s\n";
}

int main(){
    // desde cpp/build hacia la raíz
    struct Case { const char* name; const char* path; size_t n; };
    std::vector<Case> casos = {
        {"500K", "../../codes_500K.txt",  500000},
        {"1M",   "../../codes_1M.txt",   1000000},
        {"10M",  "../../codes_10M.txt", 10000000}
    };

    // 1) Mostrar los 10 primeros ordenados de CADA archivo (Radix)
    for(const auto& cs : casos){
        Poscode* data = readCodes(cs.path, cs.n);
        must(data != nullptr, std::string("No pude leer: ") + cs.path);
        radix_sort(data, cs.n);
        std::cout << "Primeros 10 (ordenados con Radix) de " << cs.name << ":\n";
        for(size_t i=0;i<10 && i<cs.n;++i){
            std::cout << data[i].getData() << "\n";
        }
        std::cout << "\n";
        deleteCodes(data);
    }

    // 2) Bench 5x pa' la pauta
    for(const auto& cs : casos){
        std::cout << "=== Bench 5x " << cs.name << " (" << cs.n << ", " << cs.path << ") ===\n";
        bench_5x("Radix", cs.path, cs.n, radix_sort);
        bench_5x("Quick", cs.path, cs.n, quick_sort);
        bench_5x("Merge", cs.path, cs.n, merge_sort);
        std::cout << "\n";
    }
    return 0;
}
