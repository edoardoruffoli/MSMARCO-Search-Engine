#pragma once

#include <math.h> 

double TFIDF(unsigned int tf, unsigned int df, unsigned int N);

double BM25(unsigned int tf, unsigned int df, unsigned int doc_len, unsigned int avg_doc_len, unsigned int N);