//
//  algo7.h
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 28.02.2020.
//  Copyright © 2020 Bahadir Altun. All rights reserved.
//

#ifndef algo7_h
#define algo7_h

#include "common.h"

void run_algo7_for_query(FILE *ofile, int current_tot_queries, int judge_count);
//extern void run_algo12_for_query(int qid, FILE *ofile);
//extern void run_algo13_for_query(int qid, FILE *ofile);
//extern void run_algo14_for_query(int qid, FILE *ofile);
extern long double get_ndcg(vector < long double > rel);

#endif /* algo7_h */
