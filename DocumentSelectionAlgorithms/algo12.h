//
//  algo12.h
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 28.12.2019.
//  Copyright © 2019 Bahadir Altun. All rights reserved.
//

#ifndef algo12_h
#define algo12_h

#include "common.h"

void run_algo12_for_query(int qid, FILE *ofile);
//extern void run_algo12_for_query(int qid, FILE *ofile);
//extern void run_algo13_for_query(int qid, FILE *ofile);
//extern void run_algo14_for_query(int qid, FILE *ofile);
extern long double get_ndcg(vector < int > rel);

#endif /* algo12_h */
