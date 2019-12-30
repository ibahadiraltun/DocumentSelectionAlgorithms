//
//  algo1.h
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 24.12.2019.
//  Copyright © 2019 Bahadir Altun. All rights reserved.
//

#ifndef algo1_h
#define algo1_h

#include "common.h"

void run_algo1_for_query(int qid, FILE *ofile);
//extern void run_algo12_for_query(int qid, FILE *ofile);
//extern void run_algo13_for_query(int qid, FILE *ofile);
//extern void run_algo14_for_query(int qid, FILE *ofile);
extern long double get_ndcg(vector < long double > rel);

//extern const int DOC_SIZE;
//extern const int QID_SIZE;
//extern const int RUN_SIZE;
//
//extern const long double inf;
//
//extern string qrel_path;
//extern string runs_path;
//
//extern vector < string > all_docs;
//extern vector < string > all_runs;
//extern vector < int > queries;
//
//extern map < string, int > run2id;
//
//extern map < int, vector < string > > doc_list;
//extern map < pair < int, string >, int > rel_table;
//extern map < string, set < pair < int, string > > > runs;
//extern map < pair < string, int >, vector < int > > runs_vector;
//extern map < pair < string, int >, int > num_of_docs_by_run;
//extern map < pair < string, pair < int, string > >, int > ranks;
//extern map < string, long double > weights, probs;
//extern map < pair < string, string >, long double > loss;
//
//extern long double alpha;
//extern long double beta;

#endif /* algo1_h */
