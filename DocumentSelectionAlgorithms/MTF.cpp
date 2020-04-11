//
//  MTF.cpp
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 2.04.2020.
//  Copyright © 2020 Bahadir Altun. All rights reserved.
//

#pragma comment(linker, "/STACK:6000000000")
#pragma comment(linker, "/HEAP:6000000000")

#include "MTF.h"
#include <thread>
#include <sys/resource.h>
#include <mutex>

// MTF: Real time MoveToFront algorithm for document selection using threads.

// MTF Algorithm:
/*
    ------
 
        1- Each run has a weight called weights[s].
        2- Initially, all runs have weights[i] = run.size() which equals to number of docs returned by current run.
        3- We randomly select run s among weights[s] values are max.
        4- We judge each doc in s until we recieved non-relevant doc.
        5- After recieving non-relevant doc, we decrease weights[s] by 1 and switch to next maximum run.
 
    ------
*/

extern vector < long double > copy_runs_vector(int run, int qid);

mutex mtx;
long double glob_scores[RUN_SIZE][UQID_SIZE];

class TOPIC_MTF {
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
    
    TOPIC_MTF() { }
    TOPIC_MTF(int _qid,
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
        
        for (int i = 0; i < all_runs.size(); i++) {
            int s = all_runs[i];
            cur_weights[s] = POOL_SIZE;
        }
        
        int last_index[RUN_SIZE] = {};
        set < int > judgedDocs;
        while (judgedDocs.size() < judge_count && judgedDocs.size() < doc_list[qid].size()) {
            int mx_run = -1;
            for (int i = 0; i < all_runs.size(); i++) {
                int s = all_runs[i];
                if (cur_weights[s] == -1) continue;
                if (mx_run == -1 or cur_weights[s] > cur_weights[mx_run]) {
                    mx_run = s;
                }
            }
            // mx_run keeps the random s such that cur_weights[s] is max among all runs.
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
                    
                    mtx.lock();
                    fprintf(ofile, "%d 0 %s %d\n", id2qid[qid], id2docid[cur_doc].c_str(), rel_doc);
                    cerr << judge_count << ' ' << judgedDocs.size() << ' ' << id2qid[qid] << ' ' << id2docid[cur_doc] << " ----> " << cur_weights[mx_run] << " " << cur_doc << endl;
                    this_thread::sleep_for(chrono::milliseconds((int) rand() % 5));
                    mtx.unlock();

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
            
        }
        
    }
    
};

void thread_task_MTF(TOPIC_MTF& o) {
    o.get_docs();
}

void run_MTF_for_query(FILE *ofile, int judge_count) {
    
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
    
    thread t_qids[RUN_SIZE];
    TOPIC_MTF topics[UQID_SIZE];
    
    for (int i = 0; i < queries.size(); i++) {
        int qid = queries[i];
        topics[i] = TOPIC_MTF(qid, ref(ofile), judge_count);
    }
    
    for (int i = 0; i < queries.size(); i++) {
        int qid = queries[i];
        t_qids[qid] = thread(thread_task_MTF, ref(topics[i]));
    }
    
    cerr << "DONE threading..." << endl;
    
    for (int i = 0; i < queries.size(); i++) {
        t_qids[queries[i]].join();
    }
    
    cout << judge_count << " is finished" << endl;
    
}


