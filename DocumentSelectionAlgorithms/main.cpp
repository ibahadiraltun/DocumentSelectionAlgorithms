//
//  main.cpp
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 24.12.2019.
//  Copyright © 2019 Bahadir Altun. All rights reserved.
//

#include "common.h"
#include "variables.h"

void run_algo1_for_query(int qid, string out_fpath);
void run_hedge_for_query(int qid, string out_fpath);

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

    for (int i = 0; i < queries.size(); i++) {
        cerr << "running algo for query -> " << queries[i] << " - " << id2qid[queries[i]] << endl;
        run_hedge_for_query(queries[i], out_fpath);
    }
    
    int _tmp;
    cout << "terminating... ";
    cin >> _tmp;

    return 0;
}
