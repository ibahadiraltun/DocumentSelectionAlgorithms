//
//  hedge.h
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 24.12.2019.
//  Copyright © 2019 Bahadir Altun. All rights reserved.
//

#ifndef hedge_h
#define hedge_h

#include "common.h"
#include "ndcg.h"

extern void get_runs(string runs_path);
extern void get_all_docs(string qrel_path);
extern void run_algo_for_query(int qid);
extern long double get_ndcg(vector < int > rel);

//extern string qrel_path;
//extern string runs_path;
//extern string out_fpath;
//
//extern int docid_tot;
//extern int qid_tot;
//
//extern vector < int > all_docs;
//extern vector < int > all_runs;
//extern vector < int > queries;
//
//extern int qid2id[QID_SIZE], id2qid[QID_SIZE];
//extern map < string, int > run2id, docid2id;
//extern map < int, string > id2run, id2docid;
//
//extern vector < int > doc_list[UQID_SIZE];
//extern int rel_table[UQID_SIZE][DOC_SIZE];
//extern set < pair < int, int > > runs[RUN_SIZE];
//extern vector < pair < int, int > > runs_vector[RUN_SIZE];
//extern int num_of_docs_by_run[RUN_SIZE][UQID_SIZE];
//extern map < int, int > ranks[RUN_SIZE][UQID_SIZE];
//
//extern long double weights[RUN_SIZE], probs[RUN_SIZE];
//extern long double loss[RUN_SIZE][DOC_SIZE], loss_run[RUN_SIZE];
//
////map < int, vector < string > > doc_list;
////map < pair < int, string >, int > rel_table;
////map < string, set < pair < int, string > > > runs;
////map < pair < string, int >, vector < int > > runs_vector;
////map < pair < string, int >, int > num_of_docs_by_run;
////map < pair < string, pair < int, string > >, int > ranks;
////map < string, long double > weights, probs;
////map < pair < string, string >, long double > loss;
//
//extern long double alpha = 0.1;
//extern long double beta = 0.1;

#endif /* hedge_h */
