//
//  MTF2.cpp
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 4.04.2020.
//  Copyright © 2020 Bahadir Altun. All rights reserved.
//

#pragma comment(linker, "/STACK:6000000000")
#pragma comment(linker, "/HEAP:6000000000")

#include "MTF2.h"
#include <thread>
#include <sys/resource.h>
#include <mutex>

// MTF: Real time MoveToFront algorithm for document selection using threads.

// MTF 2 Algorithm:
/*
 ------
 
 1- Each run has a weight called weights[s].
 2- Initially, all runs have weights[i] = run.size() which equals to number of docs returned by current run.
 3- We select run s among weights[s] * beta + avg_weights[s] * alpha values are max.
 4- We judge each doc in s until we recieved non-relevant doc.
 5- After recieving non-relevant doc, we decrease weights[s] by 1 and switch to next maximum run.
 
 ------
 */

/*
    MTF2 -> a = 0.4 b = 0.6
    MTF2_new -> a = 0.3 b = 0.7
    MTF2_new1 -> a = 0.5 b = 0.5
    MTF2_new2 -> a = 0.2 b = 0.8
*/

extern vector < long double > copy_runs_vector(int run, int qid);

extern mutex mtx;
extern long double glob_scores[RUN_SIZE][UQID_SIZE];

class TOPIC_MTF2 {
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
    
    long double alpha = 0.3;
    long double beta = 0.7;
    
    TOPIC_MTF2() { }
    TOPIC_MTF2(int _qid,
               FILE *_ofile,
               int _judge_count) {
        // glob_scores = _glob_scores;
        qid = _qid;
        ofile = _ofile;
        judge_count = _judge_count;
    }
    
    long double get_avg(int s) {
        int invalids = 0;
        long double sum = 0;
        for (int i = 0; i < queries.size(); i++) {
            if (queries[i] == qid) continue;
            if (glob_scores[s][queries[i]] == -1) {
                invalids++;
                continue;
            }
            sum = sum + glob_scores[s][queries[i]];
        }
        return sum / (long double) (queries.size() - (invalids + 1));
    }
    
    void get_docs() {
        if (ofile == NULL) {
            printf("ERROR!!! %s cannot be opened\n", out_fpath.c_str());
            exit(0);
        }
        
        for (int i = 0; i < all_runs.size(); i++) {
            int s = all_runs[i];
            cur_weights[s] = POOL_SIZE;
        }
        
        int last_index[RUN_SIZE] = {};
        set < int > judgedDocs;
        while (judgedDocs.size() < judge_count && judgedDocs.size() < doc_list[qid].size()) {
            int mx_run = -1;
            long double mx_avg = -1;
            for (int i = 0; i < all_runs.size(); i++) {
                int s = all_runs[i];
                if (cur_weights[s] == -1) continue;
//                if (mx_run == -1 or cur_weights[s] > cur_weights[mx_run]) {
//                    mx_run = s;
//                    mx_avg = this -> get_avg(s);
//                } else if (cur_weights[s] == cur_weights[mx_run]) {
//                    long double s_avg = this -> get_avg(s);
//                    if (s_avg >= mx_avg) {
//                        mx_run = s;
//                        mx_avg = s_avg;
//                    }
//                }
                
                long double s_avg = this -> get_avg(s);
                long double act_weight = this -> alpha * s_avg + this -> beta * cur_weights[s];
                
                if (act_weight > mx_avg) {
                    mx_avg = act_weight;
                    mx_run = s;
                }
            }
            
            if (mx_run == -1) {
                // all runs have been exhausted.
                break;
            }
            
            // judging front index of s until it exceeds current runs' size.
            while (judgedDocs.size() < judge_count && last_index[mx_run] < POOL_SIZE && last_index[mx_run] < runs_vector_docs[mx_run][qid].size()) {
                int cur_index = last_index[mx_run];
                int cur_doc = runs_vector_docs[mx_run][qid][cur_index];
                int rel_doc = (rel_table[qid][cur_doc] > 0);
                last_index[mx_run]++;
                
                if (judgedDocs.find(cur_doc) == judgedDocs.end()) {
                    judgedDocs.insert(cur_doc);
                    
                    mtx.lock(); {
                        fprintf(ofile, "%d 0 %s %d\n", id2qid[qid], id2docid[cur_doc].c_str(), rel_doc);
                        cerr << judge_count << ' ' << judgedDocs.size() << ' ' << id2qid[qid] << ' ' << id2docid[cur_doc] << " ----> " << cur_weights[mx_run] << " " << cur_doc << endl;
                        this_thread::sleep_for(chrono::milliseconds((int) rand() % 5));
                    } mtx.unlock();
                    
                }
                
                if (!rel_doc) {
                    // judged non-relevant doc.
                    // we should switch to next maximum run.
                    cur_weights[mx_run]--;
                    break;
                }
            }
            
            if (last_index[mx_run] >= POOL_SIZE
                || last_index[mx_run] >= runs_vector_docs[mx_run][qid].size()) {
                cur_weights[mx_run] = -1;
            }
            
            mtx.lock(); {
                glob_scores[mx_run][qid] = cur_weights[mx_run];
                this_thread::sleep_for(chrono::milliseconds((int) rand() % 5));
            } mtx.unlock();
            
        }
        
    }
    
};

void thread_task_MTF2(TOPIC_MTF2& o) {
    o.get_docs();
}

void run_MTF2_for_query(FILE *ofile, int judge_count) {
    
    // memset(glob_scores, 0, sizeof glob_scores);
    for (int i = 0; i < all_runs.size(); i++) {
        for (int j = 0; j < queries.size(); j++) {
            glob_scores[all_runs[i]][queries[j]] = POOL_SIZE;
        }
    }
    
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
    
    thread t_qids[RUN_SIZE];
    TOPIC_MTF2 topics[UQID_SIZE];
    
    for (int i = 0; i < queries.size(); i++) {
        int qid = queries[i];
        topics[i] = TOPIC_MTF2(qid, ref(ofile), judge_count);
    }
    
    for (int i = 0; i < queries.size(); i++) {
        int qid = queries[i];
        t_qids[qid] = thread(thread_task_MTF2, ref(topics[i]));
    }
    
    cerr << "DONE threading..." << endl;
    
    for (int i = 0; i < queries.size(); i++) {
        t_qids[queries[i]].join();
    }
    
    cout << judge_count << " is finished" << endl;
    
}
