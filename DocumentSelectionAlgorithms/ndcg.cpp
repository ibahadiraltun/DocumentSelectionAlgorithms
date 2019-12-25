//
//  ndcg.cpp
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 24.12.2019.
//  Copyright © 2019 Bahadir Altun. All rights reserved.
//

// NDCG implementation in C++

#include "ndcg.h"

long double get_ndcg(vector < int > rel) {
    
    // calculating DCG
    long double dcg = 0.0;
    // for (int i = 1; i <= rel.size(); i++) {
    //     dcg += (pow(2, rel[i - 1]) - 1) / log2(i + 1);
    // }
    for (int i = 1; i <= rel.size(); i++) {
        dcg += rel[i - 1] / log2(i + 1);
    }
    
    
    // calculating idcg(Ideal DCG)
    sort(rel.begin(), rel.end());
    reverse(rel.begin(), rel.end());
    
    
    long double idcg = 0.0;
    for (int i = 1; i <= rel.size(); i++) {
        idcg += rel[i - 1] / log2(i + 1);
    }
    
    if (idcg == 0) {
        return 0;
    }
    
    // nDCG = DCG / IDCG
    long double ndcg = dcg / idcg;
    return ndcg;
}

