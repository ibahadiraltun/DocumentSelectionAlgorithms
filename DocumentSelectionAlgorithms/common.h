//
//  common.h
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 24.12.2019.
//  Copyright © 2019 Bahadir Altun. All rights reserved.
//

#ifndef common_h
#define common_h

#include <algorithm>
#include <iostream>
#include <iterator>
#include <fstream>
#include <cassert>
#include <climits>
#include <cstdlib>
#include <dirent.h>
#include <cstring>
#include <iomanip>
#include <numeric>
#include <bitset>
#include <cstdio>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <queue>
#include <stack>
#include <map>
#include <unordered_map>
#include <set>

using namespace std;

//extern const int DOC_SIZE;
//extern const int QID_SIZE;
//extern const int UQID_SIZE;
//extern const int RUN_SIZE;

#define DOC_SIZE (400009)
#define QID_SIZE (510)
#define UQID_SIZE (55)
#define RUN_SIZE (100)
#define JUDG_SIZE (1000)
#define POOL_SIZE (100)

extern const long double inf;

extern string qrel_path;
extern string runs_path;
extern string out_fpath;

extern int tot_run;
extern int tot_qid;
extern int tot_docid;

extern vector < int > all_docs;
extern vector < int > all_runs;
extern vector < int > queries;

extern int qid2id[QID_SIZE], id2qid[QID_SIZE];
extern map < string, int > run2id, docid2id;
extern map < int, string > id2run, id2docid;

extern vector < int > doc_list[UQID_SIZE], doc_list_all[UQID_SIZE];
extern int rel_table[UQID_SIZE][DOC_SIZE];
extern set < int > runs[RUN_SIZE][UQID_SIZE];
extern vector < int > runs_vector_docs[RUN_SIZE][UQID_SIZE];
extern vector < long double > runs_vector[RUN_SIZE][UQID_SIZE];
extern int num_of_docs_by_run[RUN_SIZE][UQID_SIZE];
extern unordered_map < int, int > ranks[RUN_SIZE][UQID_SIZE];

extern long double weights[RUN_SIZE], probs[RUN_SIZE];
extern long double loss[RUN_SIZE][DOC_SIZE], loss_run[RUN_SIZE];
extern long double glob_ndcg[RUN_SIZE];

//map < int, vector < string > > doc_list;
//map < pair < int, string >, int > rel_table;
//map < string, set < pair < int, string > > > runs;
//map < pair < string, int >, vector < int > > runs_vector;
//map < pair < string, int >, int > num_of_docs_by_run;
//map < pair < string, pair < int, string > >, int > ranks;
//map < string, long double > weights, probs;
//map < pair < string, string >, long double > loss;

extern long double alpha;
extern long double beta;

#endif /* common_h */
