#include <math.h> 

inline double TFIDF(unsigned int tf, unsigned int df, unsigned int N);

inline double BM25(unsigned int tf, unsigned int df, unsigned int doc_len, unsigned int avg_doc_len, unsigned int N);