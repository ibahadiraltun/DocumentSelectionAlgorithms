//
//  variables.h
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 24.12.2019.
//  Copyright © 2019 Bahadir Altun. All rights reserved.
//

#ifndef variables_h
#define variables_h

extern void get_runs(string runs_path);
extern void get_all_docs(string qrel_path);
extern void run_hedge_for_query(int qid, FILE *ofile);
extern void run_algo1_for_query(int qid, FILE *ofile);
extern void run_algo12_for_query(int qid, FILE *ofile);
extern void run_algo13_for_query(int qid, FILE *ofile);
extern void run_algo14_for_query(int qid, FILE *ofile);
extern void run_algo15_for_query(int qid, FILE *ofile);
extern void run_algo16_for_query(int qid, FILE *ofile);
extern void run_algo2_for_query(int qid, FILE *ofile);
extern void run_algo4_for_query(int qid, FILE *ofile, int current_tot_queries, int judge_count);
extern void run_algo5_for_query(int qid, FILE *ofile, int current_tot_queries, int judge_count);
extern void run_algo6_for_query(int qid, FILE *ofile, int current_tot_queries, int judge_count);
extern void run_algo7_for_query(FILE *ofile, int judge_count);
extern long double get_ndcg(vector < long double > rel);


//
//const int DOC_SIZE = 2e5 + 9;
//const int QID_SIZE = 5e2 + 9;
//const int UQID_SIZE = 50 + 9;
//const int RUN_SIZE = 1e2 + 9;

#define DOC_SIZE (400009)
#define QID_SIZE (510)
#define UQID_SIZE (55)
#define RUN_SIZE (100)
#define JUDG_SIZE (1000)
#define POOL_SIZE (100)

const long double inf = 1e8 + 8;

string qrel_path;
string runs_path;
string out_fpath;

int tot_run;
int tot_qid;
int tot_docid;

vector < int > all_docs;
vector < int > all_runs;
vector < int > queries;

int qid2id[QID_SIZE], id2qid[QID_SIZE];
map < string, int > run2id, docid2id;
map < int, string > id2run, id2docid;

vector < int > doc_list[UQID_SIZE], doc_list_all[UQID_SIZE];
int rel_table[UQID_SIZE][DOC_SIZE];
set < int > runs[RUN_SIZE][UQID_SIZE];
vector < int > runs_vector_docs[RUN_SIZE][UQID_SIZE];
vector < long double > runs_vector[RUN_SIZE][UQID_SIZE];
int num_of_docs_by_run[RUN_SIZE][UQID_SIZE];
unordered_map < int, int > ranks[RUN_SIZE][UQID_SIZE];

long double weights[RUN_SIZE], probs[RUN_SIZE];
long double loss[RUN_SIZE][DOC_SIZE], loss_run[RUN_SIZE];
long double glob_ndcg[RUN_SIZE];

//map < int, vector < string > > doc_list;
//map < pair < int, string >, int > rel_table;
//map < string, set < pair < int, string > > > runs;
//map < pair < string, int >, vector < int > > runs_vector;
//map < pair < string, int >, int > num_of_docs_by_run;
//map < pair < string, pair < int, string > >, int > ranks;
//map < string, long double > weights, probs;
//map < pair < string, string >, long double > loss;

long double alpha = 0.0001;
long double beta = 0.1;

#endif /* variables_h */
