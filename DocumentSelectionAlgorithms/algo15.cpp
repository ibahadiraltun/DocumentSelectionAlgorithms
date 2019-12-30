//
//  algo15.cpp
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 28.12.2019.
//  Copyright © 2019 Bahadir Altun. All rights reserved.
//

/*
 Algorithm 1.5: Same with algo1, only difference is initial relevances are 0.5. See line 22.
*/

#include "algo15.h"

void run_algo15_for_query(int qid, FILE *ofile) {
    
    if (ofile == NULL) {
        printf("ERROR!!! %s cannot be opened\n", out_fpath.c_str());
        exit(0);
    }
    
    for (int i = 0; i < all_runs.size(); i++) {
        for (int j = 0; j < queries.size(); j++) {
            for (int k = 0; k < runs_vector[all_runs[i]][queries[j]].size(); k++) {
                runs_vector[all_runs[i]][queries[j]][k] = 0.5;
            }
        }
    }
    
    // probs.clear();
    // weights.clear();
    // loss.clear();
    
    memset(probs, 0, sizeof probs);
    memset(weights, 0, sizeof weights);
    memset(loss, 0, sizeof loss);
    memset(loss_run, 0, sizeof loss_run);
    
    // initialization with weights and probabilities
    for (int i = 0; i < all_runs.size(); i++) {
        int current_run = all_runs[i];
        weights[current_run] = 1.0;
        probs[current_run] = 1.0 / all_runs.size();
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
                loss[s][d] = log10((double) rmax / ranks[s][qid][d]) / 2.0;
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
    
    while (judgedDocs.size() < doc_list[qid].size()) {
        
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
        
        fprintf(ofile, "%d 0 %s %d\n", id2qid[qid], id2docid[dmax.first].c_str(), rel_dmax);
        // cerr << judgedDocs.size() << ' ' << id2qid[qid] << ' ' << id2docid[dmax.first] << " ----> " << dmax.second << ' ' << rel_dmax << endl;
        
        memset(loss_run, 0, sizeof loss_run);
        for (int i = 0; i < all_runs.size(); i++) {
            int s = all_runs[i];
            if (ranks[s][qid][dmax.first] != 0) {
                int index = ranks[s][qid][dmax.first] - 1;
                runs_vector[s][qid][index] = rel_dmax;
            } else {
                // nothing.
            }
            loss_run[s] = get_ndcg(runs_vector[s][qid]);
        }
        
        long double all_weights = 0.0;
        for (int i = 0; i < all_runs.size(); i++) {
            int s = all_runs[i];
            // weights[s] = weights[s] * pow(beta, loss_run[s] + alpha);
            // weights[s] = pow(beta, loss_run[s]);
            weights[s] = loss_run[s] + alpha;
            if (weights[s] == 0) {
                cerr << "weight of run " << s << " - " << judgedDocs.size() << " " << id2run[s] << " is === 0" << endl;
                // cerr << "THIS IS A SERIOUS ISSUE! FIX IT !!!" << endl;
            }
            // cerr << judgedDocs.size() << ' ' << id2run[s] << ' ' << weights[s] << endl;
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
        
        
    }
    
    // cout << judgedDocs.size() << ' ' << doc_list[qid].size() << endl;
    
}
