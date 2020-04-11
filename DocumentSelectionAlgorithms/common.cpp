//
//  common.cpp
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 24.12.2019.
//  Copyright © 2019 Bahadir Altun. All rights reserved.
//

#include "common.h"

void get_runs(string runs_path) {
    
    int asd = 0;
    
    DIR *d;
    struct dirent *dir;
    d = opendir(runs_path.c_str());
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            // printf("%s\n", dir -> d_name);
            char *tmp = strstr(dir -> d_name, "input.");
            if (tmp == NULL) {
                // this is not a run file
                continue;
            }
            
            string current_run = string(tmp + 6);
            run2id[current_run] = ++tot_run; id2run[tot_run] = current_run;
            int run_id = tot_run;
            all_runs.push_back(run_id);
            cout << current_run << " has id -> " << run_id << endl;
            
            ifstream file;
            file.open(runs_path + "/" + dir -> d_name);
            
            string _docid, tmp2, tmp4;
            int _qid, _rank;
            long double _score;
            int qid_id, docid_id;
    
            vector < pair < pair < int, long double >, pair < int, int > > > raw_in;
            
            if (file.is_open()) {
                int cnt[QID_SIZE] = {};
                while (file >> _qid >> tmp2 >> _docid >> _rank >> _score >> tmp4) {
                    // cout << _qid << " " << tmp2 << " " << _docid << " " << _rank << " " << _score << " " << tmp4 << endl;
                    cnt[_qid]++;
                    if (cnt[_qid] > JUDG_SIZE) continue;
                    qid_id = qid2id[_qid];
                    docid_id = docid2id[_docid];
                    if (docid_id == 0) {
                        docid2id[_docid] = ++tot_docid;
                        id2docid[tot_docid] = _docid;
                        docid_id = tot_docid;
                    }
                    if (cnt[_qid] <= POOL_SIZE) {
                        doc_list[qid_id].push_back(docid_id);
                    }
                    runs[run_id][qid_id].insert(docid_id);
                    runs_vector[run_id][qid_id].push_back(0.0); // keeps rel values
                    runs_vector_docs[run_id][qid_id].push_back(docid_id); // keeps doc ids.
                    int tmp = ++num_of_docs_by_run[run_id][qid_id];
                    doc_list_all[qid_id].push_back(docid_id);
                    all_docs.push_back(docid_id);
                    raw_in.push_back(make_pair(make_pair(qid_id, _score), make_pair(docid_id, _rank)));
                    // ranks[make_pair(current_run, make_pair(_qid, _docid))] = num_of_docs_by_run[make_pair(current_run, _qid)];
                }
                
                sort(raw_in.begin(), raw_in.end());
                reverse(raw_in.begin(), raw_in.end());
                
                int tot = 0;
                int tot2 = 0;
                for (int i = 0; i < raw_in.size(); i++) {
                    _score = raw_in[i].first.second;
                    qid_id = raw_in[i].first.first;
                    docid_id = raw_in[i].second.first;
                    _rank = raw_in[i].second.second;
                    if (i == 0 or qid_id != raw_in[i - 1].first.first) {
                        tot = 1;
                        tot2 = 0;
//                    } else if (i == 0 or _score != raw_in[i - 1].first.second) {
//                        tot++;
//                        tot2 = 0;
                    } else {
                        tot++;
                    }
                    // ranks[make_pair(current_run, make_pair(_qid, _docid))] = tot;
                    ranks[run_id][qid_id][docid_id] = tot;
                }
                
                raw_in.clear();
                
            } else {
                puts("ERROR! Run file not found");
                exit(0);
            }
            
            file.close();
            
            asd = asd + 1;
            // if (asd == 3) break;
        }
        
        cout << "Total Runs -> " << (int) all_runs.size() << endl;
        
        sort(all_docs.begin(), all_docs.end());
        vector < int > :: iterator it = unique(all_docs.begin(), all_docs.end());
        all_docs.resize(distance(all_docs.begin(), it));
        
        for (int i = 0; i < queries.size(); i++) {
            int qid = queries[i];
            // cout << qid << ' ' << doc_list[qid].size() << endl;
//            sort(doc_list[qid].begin(), doc_list[qid].end());
//            vector < int > :: iterator it = unique(doc_list[qid].begin(), doc_list[qid].end());
//            doc_list[qid].resize(distance(doc_list[qid].begin(), it));
//
            map < int, int > used_doc, used_doc_all;
            vector < int > tmp;
            for (int i = 0; i < doc_list[qid].size(); i++) {
                if (used_doc[doc_list[qid][i]]) {
                    continue;
                }
                used_doc[doc_list[qid][i]] = 1;
                tmp.push_back(doc_list[qid][i]);
            }
            doc_list[qid].clear();
            for (int i = 0; i < tmp.size(); i++) {
                doc_list[qid].push_back(tmp[i]);
            }
            tmp.clear();
            
//            sort(doc_list_all[qid].begin(), doc_list_all[qid].end());
//            it = unique(doc_list_all[qid].begin(), doc_list_all[qid].end());
//            doc_list_all[qid].resize(distance(doc_list_all[qid].begin(), it));
            
            for (int i = 0; i < doc_list_all[qid].size(); i++) {
                if (used_doc_all[doc_list_all[qid][i]]) {
                    continue;
                }
                used_doc_all[doc_list_all[qid][i]] = 1;
                tmp.push_back(doc_list_all[qid][i]);
            }
            doc_list_all[qid].clear();
            for (int i = 0; i < tmp.size(); i++) {
                doc_list_all[qid].push_back(tmp[i]);
            }
            tmp.clear();

            
            cerr << "Total Unique Docs for Query " << qid << " - " << id2qid[qid] << " is -> " << (int) doc_list_all[qid].size() << endl;
            cerr << "Total Pooled Unique Docs for Query " << qid << " - " << id2qid[qid] << " is -> " << (int) doc_list[qid].size() << endl;
            cerr << " -------- " << endl;
        }
        
        cerr << "Total Docs(All Queries) -> " << (int) all_docs.size() << endl;
        
        closedir(d);
    } else {
        printf("ERROR! Runs not found");
        exit(0);
    }
    
}

void get_all_docs(string qrel_path) {
    
    ifstream file;
    file.open(qrel_path);
    
    string _docid, tmp;
    int _qid, _rel;
    
    if (file.is_open()) {
        while (file >> _qid >> tmp >> _docid >> _rel) {
            // cout << _qid << ' ' << tmp << ' ' << _docid << ' ' << _rel << endl;
            // rel_table[make_pair(_qid, _docid)] = _rel;
            if (queries.empty() or id2qid[queries.back()] != _qid) {
                qid2id[_qid] = ++tot_qid; id2qid[tot_qid] = _qid;
                queries.push_back(tot_qid);
                // tot_doic = 0;
            }
            if (docid2id[_docid] == 0) {
                // if it is unset
                docid2id[_docid] = ++tot_docid; id2docid[tot_docid] = _docid;
            }
            rel_table[qid2id[_qid]][docid2id[_docid]] = _rel;
            // doc_list[_qid].push_back(_docid);
            // all_docs.push_back(_docid);
        }
    } else {
        puts("ERROR! Qrel file not found");
        exit(0);
    }
    
    cerr << "Total Queries -> " << (int) queries.size() << endl;
    
    file.close();
}

