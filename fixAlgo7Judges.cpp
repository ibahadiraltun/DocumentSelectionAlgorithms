//
//  fixAlgo7Judges.cpp
//  DocumentSelectionAlgorithms
//
//  Created by Bahadir Altun on 8.03.2020.
//  Copyright © 2020 Bahadir Altun. All rights reserved.
//

// This code is separate from other algorithms. It fixes the output of Algo7 and sorts it by qid.

#include <algorithm>
#include <iostream>
#include <iterator>
#include <fstream>
#include <cassert>
#include <climits>
#include <cstdlib>
#include <dirent.h>
#include <cstring>
#include <iomanip>
#include <numeric>
#include <bitset>
#include <cstdio>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <queue>
#include <stack>
#include <map>
#include <unordered_map>
#include <set>

using namespace std;


const int NofQueries = 50;

map < string, vector < string > > lines;

int main() {
    
    for (int judge_count = 50; judge_count <= 500; judge_count += 50) {
        string fpath = "/Users/ibahadiraltun/Desktop/InformationRetrieval/DocumentSelectionAlgorithms/4_tmp_judges"
                        + to_string(judge_count);
        ifstream infile;
        infile.open(fpath.c_str());
        cerr << fpath << endl;
        int x = 0;
        cin >> x;
        lines.clear();
        while (true) {
            int qid, tmp, rel;
            string doc;
            infile >> qid >> tmp >> doc >> rel;
            if (infile.eof()) break;
            lines[to_string(qid)].push_back(to_string(qid) + " " + to_string(tmp) + " " + doc + " " + to_string(rel));
            string tmpp = to_string(qid) + " " + to_string(tmp) + " " + doc + " " + to_string(rel);
            cerr << tmpp << endl;
        }
        infile.close();
        cerr << "Reading finish..." << endl;
        cin >> x;
        string out_fpath =
                "/Users/ibahadiraltun/Desktop/InformationRetrieval/DocumentSelectionAlgorithms/new_qrels/trec-8/4_algo7_"
                + to_string(judge_count);
        FILE *ofile = fopen(out_fpath.c_str(), "w");
        map < string, vector < string > > :: iterator it;
        for (it = lines.begin(); it != lines.end(); it++) {
            string qid = it -> first;
            vector < string > v = it -> second;
            for (int i = 0; i < v.size(); i++) {
                cerr << v[i] << endl;
                fprintf(ofile, "%s\n", v[i].c_str());
            }
        }
        fclose(ofile);
        
    }
    
    
    
    return 0;
}
