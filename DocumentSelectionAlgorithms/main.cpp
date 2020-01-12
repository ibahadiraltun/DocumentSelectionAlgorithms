//
//  main.cpp
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 24.12.2019.
//  Copyright © 2019 Bahadir Altun. All rights reserved.
//

#include "common.h"
#include "variables.h"

int main(int argc, char *argv[]) {

    // Fast I/O
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    for (int i = 1; i < argc; i++) {

        if (strcmp(argv[i], "-q") == 0) {
            qrel_path = argv[i + 1];
            i = i + 1;
        } else if (strcmp(argv[i], "-r") == 0) {
            runs_path = argv[i + 1];
            i = i + 1;
        } else if (strcmp(argv[i], "-o") == 0) {
            out_fpath = argv[i + 1];
            i = i + 1;
        }

    }

    get_all_docs(qrel_path);
    get_runs(runs_path);
    
    for (int judge_count = 50; judge_count <= 500; judge_count += 50) {
        // we shoul reset ndcg scores for every judge, but not for every query.
        memset(glob_ndcg, 0, sizeof glob_ndcg);
        string out_fpath_judge = out_fpath + to_string(judge_count);
        FILE *ofile = fopen(out_fpath_judge.c_str(), "w");
        for (int i = 0; i < queries.size(); i++) {
            cerr << "running algo for query -> " << queries[i] << " - " << id2qid[queries[i]] << " with judge count -> " << judge_count << endl;
            run_algo4_for_query(queries[i], ofile, i, judge_count);
        }
        fclose(ofile);
    }
    
    int _tmp;
    cout << "terminating... ";
    cin >> _tmp;

    return 0;
}
