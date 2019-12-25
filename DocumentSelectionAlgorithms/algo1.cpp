////
////  algo1.cpp
////  DocumentSelectionAlgorithms
////
////  Created by Bahadir Altun on 24.12.2019.
////  Copyright © 2019 Bahadir Altun. All rights reserved.
////
//
///*
// Algorithm 1: Hedge method using nDCG as a loss parameter.
//*/
//
//#include "algo1.h"
//
//void run_algo1_for_query(int qid, string out_fpath) {
//
//    FILE *ofile = fopen(out_fpath.c_str(), "a");
//
//    if (ofile == NULL) {
//        printf("ERROR!!! %s cannot be opened", out_fpath.c_str());
//        exit(0);
//    }
//
//    probs.clear();
//    weights.clear();
//    loss.clear();
//
//    // initialization with weights and probabilities
//    for (int i = 0; i < all_runs.size(); i++) {
//        string current_run = all_runs[i];
//        weights[current_run] = 1.0;
//        probs[current_run] = 1.0 / all_runs.size();
//    }
//
//    set < string > judgedDocs;
//    vector < string > judgedDocs_ordered;
//
//    int rmax = (int) doc_list[qid].size(); // In TREC, it is typically the union of the top 1000 docs retrieved
//    //////
//
//
//    long double ln_rmax = log(rmax);
//    vector < long double > suff_ln(rmax + 9, 0);
//    assert(rmax <= DOC_SIZE); // aborts if DOC_SIZE > rmax
//    for (int i = 1; i <= rmax; i++) {
//        suff_ln[i] = suff_ln[i - 1] + log(i);
//    }
//
//    //    rmax/j + rmax/(j + 1) + rmax/(j + 2) ... m
//    //
//    //    ln(rmaxˆm / (j*(j + 1)...rmax))
//    //
//    //    ln(rmaxˆn) - ln(j*(j + 1)...rmax)
//    //    ln(rmaxˆn) - (lnj + ln(j+1) + ... ln(rmax))
//
//    cout << rmax << endl;
//
//    for (int i = 0; i < all_runs.size(); i++) {
//        for (int j = 0; j < doc_list[qid].size(); j++) {
//            string s = all_runs[i]; // current run
//            string d = doc_list[qid][j]; // current document
//            if (runs[s].find(make_pair(qid, d)) != runs[s].end()) {
//                loss[make_pair(s, d)] = log10(rmax / ranks[make_pair(s, make_pair(qid, d))]) / 2.0;
//            } else {
//                // this doc was not retrieved current run
//                int ts = num_of_docs_by_run[make_pair(s, qid)];
//                long double numerator = ln_rmax * (rmax - ts);
//                long double denominator = suff_ln[rmax] - suff_ln[ts];
//                loss[make_pair(s, d)] = (numerator - denominator) / (2 * (rmax - ts));
//            }
//            // cout << loss[make_pair(s, d)] << endl;
//        }
//    }
//
//
//    while (judgedDocs.size() < rmax) {
//        // cout << "HEREEEE " << endl;
//
//        bool is_full = true;
//        pair < string, long double > dmax;
//        dmax.second = -inf;
//        for (int i = 0; i < doc_list[qid].size(); i++) {
//            string cdoc = doc_list[qid][i];
//            long double weighted_mix_loss = 0.0;
//            if (judgedDocs.find(cdoc) == judgedDocs.end()) {
//                is_full = false;
//                for (int j = 0; j < all_runs.size(); j++) {
//                    string crun = all_runs[j];
//                    weighted_mix_loss += probs[crun] * loss[make_pair(crun, cdoc)];
//                }
//                if (weighted_mix_loss > dmax.second or dmax.second == -inf) {
//                    dmax = make_pair(cdoc, weighted_mix_loss);
//                }
//            } else {
//                // this doc was already judged
//                continue;
//            }
//        }
//
//        if (is_full) {
//            // judged all possible documents
//            break;
//        }
//
//        // we have dmax as the maximum weighted document
//
//        cout << is_full << ' ' << dmax.first << " ----> " << dmax.second << endl;
//
//        judgedDocs.insert(dmax.first);
//        judgedDocs_ordered.push_back(dmax.first);
//        int rel_dmax = rel_table[make_pair(qid, dmax.first)];
//        // rel_dmax = (rel_dmax > 0);
//
//        // fprintf(ofile, "%d 0 %s %d\n", qid, dmax.first.c_str(), rel_dmax);
//
//        // cout << "ASDASD" << endl;
//
//        map < string, long double > loss_run;
//        for (int i = 0; i < all_runs.size(); i++) {
//            string s = all_runs[i];
//            if (ranks[make_pair(s, make_pair(qid, dmax.first))] != 0) {
//                int index = ranks[make_pair(s, make_pair(qid, dmax.first))] - 1;
//            //    cout << index << endl;
//                runs_vector[make_pair(s, qid)][index] = rel_dmax;
//            } else {
//                // nothing.
//            }
//            loss_run[s] = get_ndcg(runs_vector[make_pair(s, qid)]);
//        }
//
//        long double all_weights = 0.0;
//        for (int i = 0; i < all_runs.size(); i++) {
//            string s = all_runs[i];
//            weights[s] = weights[s] * pow(beta, loss_run[s]);
//            all_weights = all_weights + weights[s];
//        }
//
//        for (int i = 0; i < all_runs.size(); i++) {
//            string s = all_runs[i];
//            probs[s] = weights[s] / all_weights;
//        }
//    }
//
//    cerr << judgedDocs.size() << ' ' << doc_list[qid].size() << endl;
//
//    fclose(ofile);
//
//}
