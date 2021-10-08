//Sample file for students to get their code running

#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include <string.h>
#include <limits.h>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

int num_values_per_page = PAGE_CONTENT_SIZE/sizeof(int);

int LARGENUM;
int MAXINARRAY;
int NUMQUERIES;

FileManager fm;

string inpfilename, outfilename, queryfilename,inpfilename2;


void generate_rand_array(vector <int> &a,int mode=0){
    for(int i=0;i<a.size();i++){
        int filling = rand()%MAXINARRAY;
        if(mode)a[i] = filling+MAXINARRAY;
        else a[i]=filling;
    }
}

void write_to_file_array(int* a, int size, string filename){
    FileHandler fh = fm.CreateFile(filename.c_str());
    PageHandler ph;
    int* data;
    int page_filled = 0;
    int current_page = 0;
    for(int i=0;i<size;i++){
        if(page_filled == 0){
            ph = fh.NewPage();
            data = (int*)ph.GetData();
        }

        data[page_filled] = a[i];
        page_filled++;
        
        if(page_filled == num_values_per_page){
            page_filled = 0;
            fh.FlushPage(current_page);
            current_page++;
        }
    }
    while(page_filled!=num_values_per_page){
        data[page_filled] = INT_MIN;
        page_filled++;
    }
    fm.CloseFile(fh);
    // printf("Array written to file!\n");
}

void write_to_file_vector(vector<int> outputs, string filename){
    FileHandler fh = fm.CreateFile(filename.c_str());
    PageHandler ph;
    int* data;
    int page_filled = 0;
    int current_page = 0;
    for(int i=0;i<outputs.size();i++){
        if(page_filled == 0){
            ph = fh.NewPage();
            data = (int*)ph.GetData();
        }

        data[page_filled] = outputs[i];
        page_filled++;
        
        if(page_filled == num_values_per_page){
            page_filled = 0;
            fh.FlushPage(current_page);
            current_page++;
        }
    }
    while(page_filled!=num_values_per_page && outputs.size()>0){
        data[page_filled] = INT_MIN;
        page_filled++;
    }
    fm.CloseFile(fh);
    // printf("Vector written to file!\n");
}

void test_linear(bool sorted){
    vector <int> a(LARGENUM);

    generate_rand_array(a);
    if(sorted){
        // sort a
        sort(a.begin(), a.end());
    }

    // Generating test file
    ofstream outdata;
    int queries[NUMQUERIES];
    outdata.open((queryfilename).c_str());
    for(int i=0;i<NUMQUERIES;i++){
        int index_to_search = rand()%LARGENUM;
        int to_search = a[index_to_search] + rand()%2;
        queries[i] = to_search;
        outdata<<"SEARCH "<<to_search<<endl;
    }
    outdata.close();

    // while(a.size()>0) a.pop_back();

    write_to_file_vector(a, inpfilename);

    // Generating results
    vector<int> outputs;
    for(int i=0;i<NUMQUERIES;i++){
        int to_find = queries[i];
        for(int j=0;j<LARGENUM;j++){
            if(a[j]==to_find){
                int page_offset = j%num_values_per_page;
                int page_num = j/num_values_per_page;
                outputs.push_back(page_num);
                outputs.push_back(page_offset);
            }
        }
        outputs.push_back(-1);
        outputs.push_back(-1);
    }
    write_to_file_vector(outputs,outfilename);
}

void test_delete(){
    vector<int> a(LARGENUM, -1);
    //int a[LARGENUM];

    generate_rand_array(a);
    // sort a
    sort(a.begin(), a.end());

    // Generating test file
    ofstream outdata;
    int queries[NUMQUERIES];
    outdata.open((queryfilename).c_str());
    for(int i=0;i<NUMQUERIES;i++){
        int index_to_search = rand()%LARGENUM;
        int to_search = a[index_to_search];
        queries[i] = to_search;
        outdata<<"DELETE "<<to_search<<endl;
    }
    outdata.close();

    // while(a.size()>0) a.pop_back();
    write_to_file_vector(a, inpfilename);

    // Generating results
    int outputs[LARGENUM];
    for (int i = 0; i < LARGENUM; i++) {
        outputs[i] = 1;
    } 

    for(int i=0;i<NUMQUERIES;i++){
        int to_find = queries[i];
        for(int j=0;j<LARGENUM;j++){
            if(a[j]==to_find){
                outputs[j] = 0;
            }
        }
    }
    vector<int> results;
    for(int i = 0; i < LARGENUM; i++) {
        if(outputs[i]) {
            results.push_back(a[i]);
        }
    }
    write_to_file_vector(results,outfilename);
}

void test_join(bool sorted){
    vector<int> a(LARGENUM);
    int bsize = LARGENUM*(1 + rand() % 2);
    vector<int> b(bsize);


    generate_rand_array(a);
    generate_rand_array(b);

    if(sorted) {
        sort(b.begin(), b.end());
    }

    write_to_file_vector(a, inpfilename);
    write_to_file_vector(b, inpfilename2);

    vector<int> c;

    for(int i=0;i<LARGENUM;i++){
        for(int j=0;j<bsize;j++){
            if(a[i]==b[j]){
                c.push_back(a[i]);
            }
        }
    }
   
    write_to_file_vector(c,outfilename);
}

int main(int argc, char** argv) {
    srand(time(0));
    int test_what = stoi(argv[1]);
    LARGENUM = stoi(argv[5]);
    MAXINARRAY = stoi(argv[6]);
    NUMQUERIES = stoi(argv[7]);
    switch (test_what)
    {
    case 0:
        // Test linear search
        inpfilename = argv[2];
        outfilename = argv[4];
        queryfilename = argv[3];
        test_linear(false);
        break;
    case 1:
        // Test binary search
        inpfilename = argv[2];
        outfilename = argv[4];
        queryfilename = argv[3];
        test_linear(true);
        break;
    case 2:
        // Test delete
        inpfilename = argv[2];
        outfilename = argv[4];
        queryfilename = argv[3];
        test_delete();
        break;
    case 3:
        // Test join1
        inpfilename = argv[2];
        outfilename = argv[4];
        inpfilename2 = argv[3];
        test_join(false);
        break;
    case 4:
        // Test join2
        inpfilename = argv[2];
        outfilename = argv[4];
        inpfilename2 = argv[3];
        test_join(true);
        break;
    
    default:
        break;
    }
}
