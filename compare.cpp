#include <iostream>
#include "file_manager.h"
#include "errors.h"
#include <cstring>
#include <fstream>
#include <map>
#include <limits.h>

using namespace std;

int m[1000001];

void compare(char *file, int mode) {

	FileManager fm;
	FileHandler fh = fm.OpenFile(file);
	PageHandler ph;

	int num_pages = fh.LastPage().GetPageNum() + 1;
	int num_values_per_page = PAGE_CONTENT_SIZE / sizeof(int);
	// cout << "Number of Pages = " << num_pages << endl;
	// cout << "Page Content Size = " << PAGE_CONTENT_SIZE << endl;
	// cout << "Number of Integers on a page = " << num_values_per_page << endl << endl;

	for (int i = 0; i < num_pages; i++) {
		// cout << "Page Number " << i << endl;
		ph = fh.PageAt(i);
		int *data = (int *)ph.GetData();
		for (int j = 0; j < num_values_per_page; j++) {
			// cout << data[j] << " ";
			if (data[j] < 1000000 && data[j] >= 0) {
				if (mode)m[data[j]]++;
				else m[data[j]]--;
			}else if(data[j]==INT_MIN && i!=num_pages-1){
				cout<<"INVALID FORMAT"<<endl;
				exit(1);
			}else if(data[j]==INT_MIN){
				if (mode)m[1000000]++;
				else m[1000000]--;
			}

		}
		// cout << endl;
		fh.UnpinPage(i);
	}
	fm.CloseFile(fh);
}


int main(int argc, char *argv[]) {

	for(int i=0;i<1000001;i++) m[i]=0;

	compare(argv[1], 0);
	compare(argv[2], 1);

	for (int i = 0; i < 1000001; i++) {
		if(m[i]!=0){
			cout<<"NOT MATCHED"<<endl;
			return 0;
		}
	}
	cout<<"MATCH"<<endl;
	return 0;
}
