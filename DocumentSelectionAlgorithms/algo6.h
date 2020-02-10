//
//  algo6.h
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 18.01.2020.
//  Copyright © 2020 Bahadir Altun. All rights reserved.
//

#ifndef algo6_h
#define algo6_h

#include "common.h"

void run_algo6_for_query(int qid, FILE *ofile, int current_tot_queries, int judge_count);
//extern void run_algo12_for_query(int qid, FILE *ofile);
//extern void run_algo13_for_query(int qid, FILE *ofile);
//extern void run_algo14_for_query(int qid, FILE *ofile);
extern long double get_ndcg(vector < long double > rel);

#endif /* algo6_h */
