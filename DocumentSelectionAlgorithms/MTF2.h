//
//  MTF2.h
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 4.04.2020.
//  Copyright © 2020 Bahadir Altun. All rights reserved.
//

#ifndef MTF2_h
#define MTF2_h

#include "common.h"

void run_MTF2_for_query(FILE *ofile, int judge_count);
//extern void run_algo12_for_query(int qid, FILE *ofile);
//extern void run_algo13_for_query(int qid, FILE *ofile);
//extern void run_algo14_for_query(int qid, FILE *ofile);
extern long double get_ndcg(vector < long double > rel);

#endif /* MTF2_h */
