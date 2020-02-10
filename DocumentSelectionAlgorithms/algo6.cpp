//
//  algo6.cpp
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 18.01.2020.
//  Copyright © 2020 Bahadir Altun. All rights reserved.
//

#include "algo6.h"

// Algorithm 6: Hedge + alpha coefficent described in doc.

/*
 - SERIOUS BUG!!!
 When we mutate runs_vector(see line 135), it is being already calculated for later queries. Since we are operating every query for each judge_count(50, 100, ..., 150)
 it is being meaningless for next queries.
 
 To fix it:
 Use local vector and do not change global runs_vector.
 copy_runs_vector() is a function to copy global runs_vector.
 
 */

extern vector < long double > copy_runs_vector(int run, int qid);

void run_algo6_for_query(int qid, FILE *ofile, int current_tot_queries, int judge_count) {
    
    //    FILE *ofile = fopen(out_fpath.c_str(), "a");
    
    if (ofile == NULL) {
        printf("ERROR!!! %s cannot be opened\n", out_fpath.c_str());
        exit(0);
    }
    
    // probs.clear();
    // weights.clear();
    // loss.clear();
    
    memset(probs, 0, sizeof probs);
    memset(weights, 0, sizeof weights);
    memset(loss, 0, sizeof loss);
    memset(loss_run, 0, sizeof loss_run);
    
    long double cur_weights[RUN_SIZE] = {};
    long double prev_avg_weights[RUN_SIZE] = {};
    long double prev_all_weights = 0;
    for (int i = 0; i < all_runs.size(); i++) {
        prev_avg_weights[all_runs[i]] = glob_ndcg[all_runs[i]] / (current_tot_queries + 1);
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
        probs[current_run] = prev_avg_weights[current_run] / prev_all_weights;
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
    
    cerr << rmax << " unique docs" << endl;
    cerr << (int) doc_list[qid].size() << " pooled unique docs" << endl;
    
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
    
    cerr << "alpha ----------> " << doc_list[qid].size() << ' ' << alpha << endl;
    
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
        rel_dmax = (rel_dmax > 0);
        
        // updating alpha and beta coefs.
        
        for (int i = 0; i < all_runs.size(); i++) {
            int s = all_runs[i];
            if (runs[s][qid].find(dmax.first) != runs[s][qid].end()) {
                beta += 1.0 / ranks[s][qid][dmax.first];
                alpha -= 2.0 / ranks[s][qid][dmax.first];
            }
        }
        
        fprintf(ofile, "%d 0 %s %d\n", id2qid[qid], id2docid[dmax.first].c_str(), rel_dmax);
        // cerr << judgedDocs.size() << ' ' << id2qid[qid] << ' ' << id2docid[dmax.first] << " ----> " << dmax.second << endl;
        
        // unordered_map < string, long double > loss_run;
        // long double loss_run[RUN_SIZE] = {};
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
        
        long double all_weights = 0.0;
        for (int i = 0; i < all_runs.size(); i++) {
            int s = all_runs[i];
            // cout << weights[s] << " ";
            long double tmp = weights[s];
            cur_weights[s] = cur_weights[s] * pow(0.1, loss_run[s]);
            weights[s] = alpha * prev_avg_weights[s] + beta * cur_weights[s];
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
        }
        
//        for (int i = 0; i < all_runs.size(); i++) {
//            int s = all_runs[i];
//            if (ranks[s][qid][dmax.first] != 0) {
//                int index = ranks[s][qid][dmax.first] - 1;
//                loc_runs_vector[s][qid][index] = rel_dmax;
//            } else {
//                // nothing.
//            }
//        }
        
    }
//
    for (int i = 0; i < all_runs.size(); i++) {
        int s = all_runs[i];
        glob_ndcg[s] += cur_weights[s];
    }
    
    // cout << judgedDocs.size() << ' ' << doc_list[qid].size() << endl;
    
}

