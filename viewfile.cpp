#include <iostream>
#include "file_manager.h"
#include "errors.h"
#include <cstring>
#include <fstream>

using namespace std;

int main(int argc, char *argv[]) {
    char *file = argv[1];
    FileManager fm;
    FileHandler fh = fm.OpenFile(file);
	PageHandler ph;

	int num_pages = fh.LastPage().GetPageNum();
	fh.FlushPage(num_pages);
	num_pages++;
	int num_values_per_page = PAGE_CONTENT_SIZE/sizeof(int);
	cout << "Number of Pages = " << num_pages << endl;
	cout << "Page Content Size = " << PAGE_CONTENT_SIZE << endl;
	cout << "Number of Integers on a page = " << num_values_per_page << endl << endl;

	for(int i = 0; i < num_pages; i++) {
		cout << "Page Number " << i << endl;
		ph = fh.PageAt(i);
		int *data = (int *)ph.GetData();
		for (int j = 0; j < num_values_per_page; j++) {
			cout << data[j] << " ";
		}
		cout << endl;
		fh.UnpinPage(i);
	}
	fm.CloseFile(fh);
}