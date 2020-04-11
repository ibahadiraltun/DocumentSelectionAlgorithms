//
//  algo7.cpp
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 28.02.2020.
//  Copyright © 2020 Bahadir Altun. All rights reserved.
//
//
#pragma comment(linker, "/STACK:6000000000")
#pragma comment(linker, "/HEAP:6000000000")

#include "algo7.h"
#include <thread>
#include <sys/resource.h>
#include <mutex>

// Algorithm 7: Algo6 + using threads for real time simulation.

extern vector < long double > copy_runs_vector(int run, int qid);

extern mutex mtx;
extern long double glob_scores[RUN_SIZE][UQID_SIZE];

class TOPIC {
public:
    long double cur_weights[RUN_SIZE];
    long double prev_avg_weights[RUN_SIZE];
    long double prev_all_weights;
    long double weights[RUN_SIZE];
    long double probs[RUN_SIZE];
    // long double loss[RUN_SIZE][100];
    unordered_map < long double, long double > loss[RUN_SIZE];
    long double loss_run[RUN_SIZE];
    
    int qid;
    FILE *ofile;
    int judge_count;
    
    TOPIC() { }
    TOPIC(int _qid,
          FILE *_ofile,
          int _judge_count) {
        // glob_scores = _glob_scores;
        qid = _qid;
        ofile = _ofile;
        judge_count = _judge_count;
    }
    
    void get_docs() {
        if (ofile == NULL) {
            printf("ERROR!!! %s cannot be opened\n", out_fpath.c_str());
            exit(0);
        }
//
//        mtx.lock();
//        fprintf(ofile, "HELLO %d\n", qid);
//        cerr << "HELLO " << qid << endl;
//        this_thread::sleep_for(chrono::milliseconds(20));
//        mtx.unlock();



        for (int i = 0; i < all_runs.size(); i++) {
            long double tmp = 0;
            for (int j = 0; j < queries.size(); j++) {
                int cur_qid = queries[j];
                if (cur_qid == qid) {
                    continue;
                }
                tmp += glob_scores[all_runs[i]][cur_qid];
            }
            tmp = tmp / (long double) (queries.size() - 1);
            prev_avg_weights[all_runs[i]] = tmp;
        }


        for (int i = 0; i < all_runs.size(); i++) {
            // prev_avg_weights[all_runs[i]] = glob_ndcg[all_runs[i]] / (current_tot_queries + 1);
            cur_weights[all_runs[i]] = 1.0;
            prev_all_weights = prev_all_weights + prev_avg_weights[all_runs[i]];
        }
        vector < long double > loc_runs_vector[RUN_SIZE][UQID_SIZE];
        for (int i = 0; i < all_runs.size(); i++) {
            int current_run = all_runs[i];
            loc_runs_vector[current_run][qid] = copy_runs_vector(current_run, qid);
        }

        // initialization with weights and probabilities
        for (int i = 0; i < all_runs.size(); i++) {
            int current_run = all_runs[i];
            // weights[current_run] = 1.0;
            if (prev_all_weights != 0) {
                probs[current_run] = prev_avg_weights[current_run] / prev_all_weights;
            } else {
                probs[current_run] = 1.0 / all_runs.size();
            }
            // probs[current_run] = prev_avg_weights[current_run] / prev_all_weights;
            // probs[current_run] = 1.0 / ((long double) all_runs.size());
            //        weights[current_run] = (current_tot_queries == 0) ? 1.0 : glob_ndcg[current_run] / current_tot_queries;
            //        probs[current_run] = weights[current_run] / all_runs.size();
        }

        set < int > judgedDocs;
        vector < int > judgedDocs_ordered;

        int rmax = (int) doc_list_all[qid].size(); // In TREC, it is typically the union of the top 1000 docs retrieved
        long double ln_rmax = log(rmax);
        vector < long double > suff_ln(rmax + 9, 0);
        assert(rmax <= DOC_SIZE); // aborts if DOC_SIZE > rmax
        for (int i = 1; i <= rmax; i++) {
            suff_ln[i] = suff_ln[i - 1] + log(i);
        }

        // cerr << qid << ' ' << rmax << " unique docs" << endl;
        // cerr << (int) doc_list[qid].size() << " pooled unique docs" << endl;

        for (int i = 0; i < all_runs.size(); i++) {
            for (int j = 0; j < doc_list[qid].size(); j++) {
                int s = all_runs[i]; // current run
                int d = doc_list[qid][j]; // current document
                if (runs[s][qid].find(d) != runs[s][qid].end()) {
                    // this doc was retrieved by current run
                    loss[s][d] = log10(rmax / ranks[s][qid][d]) / 2.0;
                    // loss[make_pair(s, d)] = log10(rmax / ranks[make_pair(s, make_pair(qid, d))]) / 2.0;
                } else {
                    // this doc was not retrieved current run
                    int ts = num_of_docs_by_run[s][qid];
                    long double numerator = ln_rmax * (rmax - ts);
                    long double denominator = suff_ln[rmax] - suff_ln[ts];
                    loss[s][d] = (numerator - denominator) / (2 * (rmax - ts));
                    // loss[make_pair(s, d)] = (numerator - denominator) / (2 * (rmax - ts));
                }
                // cout << loss[s][d] << endl;
            }
        }

        long double alpha = 0.0;
        long double beta = 0.0;
        // alpha & beta are different from global alpha & beta.
        // they are stay this function's scope and they are not global.

        for (int i = 0; i < 1000; i++) {
            alpha += 1.0 / (i + 1);
        }

        // cerr << "alpha ----------> " << doc_list[qid].size() << ' ' << alpha << endl;

        while (judgedDocs.size() < judge_count && judgedDocs.size() < doc_list[qid].size()) {

            bool is_full = true;
            pair < int, long double > dmax;
            dmax.second = -inf;
            for (int i = 0; i < doc_list[qid].size(); i++) {
                int cdoc = doc_list[qid][i];
                long double weighted_mix_loss = 0.0;
                if (judgedDocs.find(cdoc) == judgedDocs.end()) {
                    is_full = false;
                    for (int j = 0; j < all_runs.size(); j++) {
                        int crun = all_runs[j];
                        weighted_mix_loss += probs[crun] * loss[crun][cdoc];
                    }
                    if (weighted_mix_loss > dmax.second or dmax.second == -inf) {
                        dmax = make_pair(cdoc, weighted_mix_loss);
                        // weighted_mix_loss = dmax.second;
                    }
                    if (dmax.second == -inf) {
                        // is_full = true;
                        // break;
                    }
                } else {
                    // this doc was already judged
                    continue;
                }
            }

            if (is_full) {
                // judged all possible documents
                break;
            }

            // we have dmax as the maximum weighted document

            // cout << is_full << ' ' << dmax.first << " ----> " << dmax.second << endl;

            judgedDocs.insert(dmax.first);
            judgedDocs_ordered.push_back(dmax.first);
            int rel_dmax = rel_table[qid][dmax.first];
            // rel_dmax = (rel_dmax > 0);

            // updating alpha and beta coefs.

            for (int i = 0; i < all_runs.size(); i++) {
                int s = all_runs[i];
                if (runs[s][qid].find(dmax.first) != runs[s][qid].end()) {
                    beta += 2.0 / ranks[s][qid][dmax.first];
                    alpha -= 1.0 / ranks[s][qid][dmax.first];
                }
            }

            mtx.lock();
            fprintf(ofile, "%d 0 %s %d\n", id2qid[qid], id2docid[dmax.first].c_str(), rel_dmax);
            cerr << judge_count << ' ' << judgedDocs.size() << ' ' << id2qid[qid] << ' ' << id2docid[dmax.first] << " ----> " << dmax.second << " " << dmax.first << endl;
            this_thread::sleep_for(chrono::milliseconds((int) rand() % 5));
            mtx.unlock();

//            // unordered_map < string, long double > loss_run;
//            long double loss_run[RUN_SIZE] = {};
            memset(loss_run, 0, sizeof loss_run);
            for (int i = 0; i < all_runs.size(); i++) {
                int s = all_runs[i];
                int cur_rank;
                if (runs[s][qid].find(dmax.first) != runs[s][qid].end()) {
                    loss_run[s] = ((rel_dmax ? -1 : 1) * log((double) rmax / ranks[s][qid][dmax.first])) / 2.0;
                } else {
                    // this doc was not retrieved by s
                    int ts = num_of_docs_by_run[s][qid];
                    long double numerator = ln_rmax * (rmax - ts);
                    long double denominator = suff_ln[rmax] - suff_ln[ts];
                    loss_run[s] = (rel_dmax ? -1 : 1) * (numerator - denominator) / (2 * (rmax - ts));
                }
                // cout << loss_run[s] << endl;
            }
            
//            memset(loss_run, 0, sizeof loss_run);
//            for (int i = 0; i < all_runs.size(); i++) {
//                int s = all_runs[i];
//                if (ranks[s][qid][dmax.first] != 0) {
//                    int index = ranks[s][qid][dmax.first] - 1;
//                    loc_runs_vector[s][qid][index] = rel_dmax;
//                } else {
//                    // nothing.
//                }
//                loss_run[s] = get_ndcg(loc_runs_vector[s][qid]);
//            }


            long double all_weights = 0.0;
            for (int i = 0; i < all_runs.size(); i++) {
                int s = all_runs[i];
                // cout << weights[s] << " ";
                long double tmp = weights[s];
                cur_weights[s] = cur_weights[s] * pow(0.1, loss_run[s]);
                // weights[s] = cur_weights[s];
                weights[s] = alpha * prev_avg_weights[s] + beta * cur_weights[s];
                // weights[s] = loss_run[s] + 0.0001;
                long double tmp2 = weights[s];
                if (weights[s] == 0) {
                    cerr << "weight of run " << s << " - " << judgedDocs.size() << " " << id2run[s] << " is === 0" << endl;
                    // cerr << "THIS IS A SERIOUS ISSUE! FIX IT !!!" << endl;
                }
                // cerr << id2qid[qid] << " - " << id2run[s] << " " << weights[s] << endl;
                all_weights = all_weights + weights[s];
            }

            assert(all_weights != 0);

            for (int i = 0; i < all_runs.size(); i++) {
                int s = all_runs[i];
                probs[s] = weights[s] / all_weights;
                if (probs[s] == 0) {
                    cerr << "prob ====== 0 =============> " << judgedDocs.size() << " " << weights[s] << ' ' << all_weights << ' ' << loss_run[s] << endl;
                    // cerr << "THIS IS A SERIOUS ISSUE! FIX IT !!!" << endl;
                    // exit(0);
                }
                mtx.lock();
                glob_scores[s][qid] = cur_weights[s];
                // this_thread::sleep_for(chrono::milliseconds(20));
                mtx.unlock();
            }

            // updating average weights
            for (int i = 0; i < all_runs.size(); i++) {
                long double tmp = 0;
                for (int j = 0; j < queries.size(); j++) {
                    int cur_qid = queries[j];
                    if (cur_qid == qid) {
                        continue;
                    }
                    tmp += glob_scores[all_runs[i]][cur_qid];
                }
                tmp = tmp / (long double) (queries.size() - 1);
                prev_avg_weights[all_runs[i]] = tmp;
            }


        }

//
//        if (ofile == NULL) {
//            printf("ERROR!!! %s cannot be opened\n", out_fpath.c_str());
//            exit(0);
//        }
//
//        // probs.clear();
//        // weights.clear();
//        // loss.clear();
//
//        memset(probs, 0, sizeof probs);
//        memset(weights, 0, sizeof weights);
//        memset(loss, 0, sizeof loss);
//        memset(loss_run, 0, sizeof loss_run);
//
//        set < int > judgedDocs;
//        vector < int > judgedDocs_ordered;
//
//        // initialization with weights and probabilities
//        for (int i = 0; i < all_runs.size(); i++) {
//            int current_run = all_runs[i];
//            weights[current_run] = 1.0;
//            probs[current_run] = 1.0 / all_runs.size();
//        }
//
//        vector < long double > loc_runs_vector[RUN_SIZE][UQID_SIZE];
//        for (int i = 0; i < all_runs.size(); i++) {
//            int current_run = all_runs[i];
//            loc_runs_vector[current_run][qid] = copy_runs_vector(current_run, qid);
//        }
//
//
//        int rmax = (int) doc_list_all[qid].size(); // In TREC, it is typically the union of the top 1000 docs retrieved
//        long double ln_rmax = log(rmax);
//        vector < long double > suff_ln(rmax + 9, 0);
//        assert(rmax <= DOC_SIZE); // aborts if DOC_SIZE > rmax
//        for (int i = 1; i <= rmax; i++) {
//            suff_ln[i] = suff_ln[i - 1] + log(i);
//        }
//
//        mtx.lock();
//        cerr << rmax << " unique docs" << endl;
//        cerr << (int) doc_list[qid].size() << " pooled unique docs" << endl;
//        mtx.unlock();
//
//        for (int i = 0; i < all_runs.size(); i++) {
//            for (int j = 0; j < doc_list[qid].size(); j++) {
//                int s = all_runs[i]; // current run
//                int d = doc_list[qid][j]; // current document
//
//                if (runs[s][qid].find(d) != runs[s][qid].end()) {
//                    // this doc was retrieved by current run
//                    loss[s][d] = log10((double) rmax / ranks[s][qid][d]) / 2.0;
//                    // loss[make_pair(s, d)] = log10(rmax / ranks[make_pair(s, make_pair(qid, d))]) / 2.0;
//                } else {
//                    // this doc was not retrieved current run
//                    int ts = num_of_docs_by_run[s][qid];
//                    long double numerator = ln_rmax * (rmax - ts);
//                    long double denominator = suff_ln[rmax] - suff_ln[ts];
//                    loss[s][d] = (numerator - denominator) / (2 * (rmax - ts));
//                    // loss[make_pair(s, d)] = (numerator - denominator) / (2 * (rmax - ts));
//                }
//            }
//        }
//
//        while (judgedDocs.size() < doc_list[qid].size()) {
//
//            bool is_full = true;
//            pair < int, long double > dmax;
//            dmax.second = -inf;
//            for (int i = 0; i < doc_list[qid].size(); i++) {
//                int cdoc = doc_list[qid][i];
//                long double weighted_mix_loss = 0.0;
//                if (judgedDocs.find(cdoc) == judgedDocs.end()) {
//                    is_full = false;
//                    for (int j = 0; j < all_runs.size(); j++) {
//                        int crun = all_runs[j];
//                        weighted_mix_loss += probs[crun] * loss[crun][cdoc];
//                    }
//                    if (weighted_mix_loss > dmax.second or dmax.second == -inf) {
//                        dmax = make_pair(cdoc, weighted_mix_loss);
//                        // weighted_mix_loss = dmax.second;
//                    }
//                    if (dmax.second == -inf) {
//                        // is_full = true;
//                        // break;
//                    }
//                } else {
//                    // this doc was already judged
//                    continue;
//                }
//            }
//
//            if (is_full) {
//                // judged all possible documents
//                break;
//            }
//
//            // we have dmax as the maximum weighted document
//
//            judgedDocs.insert(dmax.first);
//            judgedDocs_ordered.push_back(dmax.first);
//            int rel_dmax = rel_table[qid][dmax.first];
//            // rel_dmax = (rel_dmax > 0);
//
//            mtx.lock();
//            fprintf(ofile, "%d 0 %s %d\n", id2qid[qid], id2docid[dmax.first].c_str(), rel_dmax);
//            cerr << judge_count << ' ' << judgedDocs.size() << ' ' << id2qid[qid] << ' ' << id2docid[dmax.first] << " ----> " << dmax.second << " " << dmax.first << endl;
//            this_thread::sleep_for(chrono::milliseconds((int) rand() % 5));
//            mtx.unlock();
//
//
////            fprintf(ofile, "%d 0 %s %d\n", id2qid[qid], id2docid[dmax.first].c_str(), rel_dmax);
////            cerr << judgedDocs.size() << ' ' << id2qid[qid] << ' ' << id2docid[dmax.first] << " ----> " << dmax.second << ' ' << rel_dmax << endl;
//
//            memset(loss_run, 0, sizeof loss_run);
//            for (int i = 0; i < all_runs.size(); i++) {
//                int s = all_runs[i];
//                if (ranks[s][qid][dmax.first] != 0) {
//                    int index = ranks[s][qid][dmax.first] - 1;
//                    loc_runs_vector[s][qid][index] = rel_dmax;
//                } else {
//                    // nothing.
//                }
//                loss_run[s] = get_ndcg(loc_runs_vector[s][qid]);
//            }
//
//            long double all_weights = 0.0;
//            for (int i = 0; i < all_runs.size(); i++) {
//                int s = all_runs[i];
//                // weights[s] = weights[s] * pow(beta, loss_run[s] + alpha);
//                // weights[s] = pow(beta, loss_run[s]);
//                weights[s] = loss_run[s] + alpha;
//                if (weights[s] == 0) {
//                    cerr << "weight of run " << s << " - " << judgedDocs.size() << " " << id2run[s] << " is === 0" << endl;
//                    // cerr << "THIS IS A SERIOUS ISSUE! FIX IT !!!" << endl;
//                }
//                // cerr << judgedDocs.size() << ' ' << id2run[s] << ' ' << weights[s] << endl;
//                all_weights = all_weights + weights[s];
//            }
//
//            assert(all_weights != 0);
//
//            for (int i = 0; i < all_runs.size(); i++) {
//                int s = all_runs[i];
//                probs[s] = weights[s] / all_weights;
//                if (probs[s] == 0) {
//                    cerr << "prob ====== 0 =============> " << judgedDocs.size() << " " << weights[s] << ' ' << all_weights << ' ' << loss_run[s] << endl;
//                    // cerr << "THIS IS A SERIOUS ISSUE! FIX IT !!!" << endl;
//                    // exit(0);
//                }
//            }
//
//
//        }

        
        
        
    }
    
};

void thread_task(TOPIC& o) {
    o.get_docs();
}

void run_algo7_for_query(FILE *ofile, int judge_count) {
    
    memset(glob_scores, 0, sizeof glob_scores);
    
    srand(time(NULL));
    
    const rlim_t kStackSize = 1 * 1024 * 1024 * 1024;   // min stack size = 1 GB
    struct rlimit rl;
    int result;

    result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0) {
        if (rl.rlim_cur < kStackSize) {
            rl.rlim_cur = kStackSize;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result != 0) {
                fprintf(stderr, "setrlimit returned result = %d\n", result);
            }
        }
    }

    
    // cerr << "HERE 111 " << endl;
    
    //    long double glob_scores[RUN_SIZE][QID_SIZE] = {};
//    long double **glob_scores;
//    glob_scores = new long double *[RUN_SIZE];
//    for(int i = 0; i < RUN_SIZE; i++)
//        glob_scores[i] = new long double[UQID_SIZE];

    // cerr << "PASS " << endl;
    
//    for (int i = 0; i < RUN_SIZE; i++)
//        for (int j = 0; j < QID_SIZE; j++)
//            glob_scores[i][j] = 0.0;
    
    thread t_qids[RUN_SIZE];
    // cerr << "HERE 222 " << endl;
    
    TOPIC topics[UQID_SIZE];
    for (int i = 0; i < queries.size(); i++) {
        int qid = queries[i];
        topics[i] = TOPIC(qid, ref(ofile), judge_count);
        
    }
    
    for (int i = 0; i < queries.size(); i++) {
        int qid = queries[i];
        t_qids[qid] = thread(thread_task, ref(topics[i]));
//        cerr << "HERE 444 " << endl;
    }
    
    cerr << "DONE threading..." << endl;
    
    for (int i = 0; i < queries.size(); i++) {
        t_qids[queries[i]].join();
    }
    
    cout << judge_count << " is finished" << endl;
    
}


