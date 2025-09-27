#pragma once
#include <vector>
#include <string>
#include "poscode.hpp"

// --- Caballo arreador: declara to' lo que usamos pa' ordenar ---
class Sorters {
public:
    static void radix_sort(Poscode* A, size_t n);
    static void quick_sort(Poscode* A, size_t n);
    static void merge_sort(Poscode* A, size_t n);

private:
    // NOTA: sin const porque Poscode::getValue y getData no son const en tus headers
    static int  bucket_value(Poscode& pc, int p);
    static void counting_pass(Poscode* A, size_t n, int p, int M);
    static bool leq(Poscode& a, Poscode& b);

    // QuickSort (Lomuto)
    static int  q_partition(Poscode* A, int l, int r);
    static void qsort_rec(Poscode* A, int l, int r);

    // MergeSort
    static void merge(Poscode* A, int l, int m, int r, std::vector<Poscode>& tmp);
    static void msort_rec(Poscode* A, int l, int r, std::vector<Poscode>& tmp);
};
