#include "utils.hpp"
#include <iostream>
#include <fstream>

#include "sorters.hpp"      // <-- NUEVO: declaramos la clase con los sorters

#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cctype>

// ================== WRAPPERS PÚBLICOS (API ya dada) ==================

void quick_sort(Poscode *A, size_t n){
    // --- QuickSort: rapidito, sin tanta vuelta ---
    Sorters::quick_sort(A, n);
}

void merge_sort(Poscode *A, size_t n){
    // --- MergeSort: ordenadito, como desfile dieciochero ---
    Sorters::merge_sort(A, n);
}

void radix_sort(Poscode *A, size_t n){
    // --- Radix como manda el PDF ---
    // letras: última (p=6) y penúltima (p=5) con M=26
    // dígitos: p=4..1 con M=10, de derecha a izquierda
    Sorters::radix_sort(A, n);
}

// ================== E/S DE ARCHIVOS (ya existía) ==================

Poscode *readCodes(const std::string &strfile, size_t n){
    Poscode *codes = new Poscode[n];

    std::ifstream inputFile(strfile);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open the file!" << std::endl;
        return nullptr;
    }

    std::string line;
    for(size_t i = 0; i < n; i++){
        std::getline(inputFile, line);
        codes[i] = Poscode(line);
    }

    inputFile.close();
    return codes;
}

void deleteCodes(Poscode *codes){
    if (codes != nullptr) {
        delete[] codes;
    }
}

int Sorters::bucket_value(Poscode& pc, int p){   // <--- sin const
    int i = p - 1;
    char c = pc.getValue((size_t)i);
    if(i <= 3){
        if(c < '0' || c > '9') throw std::runtime_error("Aquí iba un dígito poh compadre");
        return c - '0';
    }else{
        if(c >= 'a' && c <= 'z') c = char(c - 'a' + 'A');
        if(c < 'A' || c > 'Z') throw std::runtime_error("Aquí iba letra A..Z, no me haga trampa");
        return c - 'A';
    }
}

// --- Counting estable por columna: guardamos puros ÍNDICES, no el dato ---
void Sorters::counting_pass(Poscode *A, size_t n, int p, int M){
    std::vector<std::vector<size_t>> buckets((size_t)M);
    for(size_t i = 0; i < n; ++i){
        int b = bucket_value(A[i], p);  // <--- pasa A[i] por referencia no-const
        if(b < 0 || b >= M) throw std::runtime_error("Baldecito fuera de rango, ah");
        buckets[(size_t)b].push_back(i);
    }
    std::vector<Poscode> tmp; tmp.reserve(n);
    for(int b = 0; b < M; ++b){
        for(size_t idx : buckets[(size_t)b]) tmp.push_back(A[idx]);
    }
    for(size_t i = 0; i < n; ++i) A[i] = tmp[i];
}

// --- Comparador lexicográfico pa' Quick/Merge ---
bool Sorters::leq(Poscode& a, Poscode& b){       // <--- sin const
    return a.getData() <= b.getData();
}

// --- QuickSort (Lomuto), al hueso ---
int Sorters::q_partition(Poscode *A, int l, int r){
    Poscode pivot = A[r];
    int i = l;
    for(int j = l; j < r; ++j){
        if(leq(A[j], pivot)){
            std::swap(A[i], A[j]);
            ++i;
        }
    }
    std::swap(A[i], A[r]);
    return i;
}
void Sorters::qsort_rec(Poscode *A, int l, int r){
    if(l >= r) return;
    int p = q_partition(A, l, r);
    qsort_rec(A, l, p - 1);
    qsort_rec(A, p + 1, r);
}

// --- MergeSort de siempre, firme como apero bueno ---
void Sorters::merge(Poscode *A, int l, int m, int r, std::vector<Poscode>& tmp){
    int i = l, j = m + 1, k = l;
    while(i <= m && j <= r){
        if(leq(A[i], A[j])) tmp[k++] = A[i++];
        else                tmp[k++] = A[j++];
    }
    while(i <= m) tmp[k++] = A[i++];
    while(j <= r) tmp[k++] = A[j++];
    for(int t = l; t <= r; ++t) A[t] = std::move(tmp[t]);
}
void Sorters::msort_rec(Poscode *A, int l, int r, std::vector<Poscode>& tmp){
    if(l >= r) return;
    int m = l + (r - l)/2;
    msort_rec(A, l, m, tmp);
    msort_rec(A, m + 1, r, tmp);
    merge(A, l, m, r, tmp);
}

// --- Públicas: las que vas a llamar desde afuera (o vía wrappers) ---
void Sorters::quick_sort(Poscode *A, size_t n){
    if(!A || n == 0) return;
    qsort_rec(A, 0, (int)n - 1);
}
void Sorters::merge_sort(Poscode *A, size_t n){
    if(!A || n == 0) return;
    std::vector<Poscode> tmp(n);
    msort_rec(A, 0, (int)n - 1, tmp);
}
void Sorters::radix_sort(Poscode *A, size_t n){
    if(!A || n == 0) return;
    // letras: última (p=6) y penúltima (p=5) con M=26
    counting_pass(A, n, /*p=*/6, /*M=*/26);
    counting_pass(A, n, /*p=*/5, /*M=*/26);
    // dígitos: p=4..1 con M=10, de derecha a izquierda
    counting_pass(A, n, /*p=*/4, /*M=*/10);
    counting_pass(A, n, /*p=*/3, /*M=*/10);
    counting_pass(A, n, /*p=*/2, /*M=*/10);
    counting_pass(A, n, /*p=*/1, /*M=*/10);
}

// ================== FIN IMPLEMENTACIÓN DE Sorters ==================
