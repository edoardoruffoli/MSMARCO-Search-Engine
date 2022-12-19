#include "MSMARCO-Search-Engine/scoring_functions.hpp"

double TFIDF(unsigned int tf, unsigned int df, unsigned int N) {
    return (1.0 + log10(tf))*log10((double)N/df);
}

double BM25(unsigned int tf, unsigned int df, unsigned int doc_len, unsigned int avg_doc_len, unsigned int N) {
    double k1 = 0.82;
    double b = 0.68;
    return tf * log10((double)N / df) / (k1 * ((1 - b) + b * ((double)doc_len / avg_doc_len)) + tf);
}