//
//  MTF.h
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 2.04.2020.
//  Copyright © 2020 Bahadir Altun. All rights reserved.
//

#ifndef MTF_h
#define MTF_h

#include "common.h"

void run_MTF_for_query(FILE *ofile, int judge_count);
//extern void run_algo12_for_query(int qid, FILE *ofile);
//extern void run_algo13_for_query(int qid, FILE *ofile);
//extern void run_algo14_for_query(int qid, FILE *ofile);
extern long double get_ndcg(vector < long double > rel);

#endif /* MTF_h */
