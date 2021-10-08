#include <iostream>
#include "file_manager.h"
#include "errors.h"
#include <limits.h>
#include <fstream>

using namespace std;

/* File Manager */
FileManager fm;

/* Input File Handler */
FileHandler input_fh;
PageHandler input_ph;
int num_pages;
int num_values_per_page;

/* Output File Handler */
FileHandler output_fh;
PageHandler output_ph;
int curr_page;
int num_values_on_curr_page;
int* data;

void write_output(int val) {
	if(num_values_on_curr_page == 0){
		output_ph = output_fh.NewPage();
		curr_page++;
		data = (int *)output_ph.GetData();
	}

	data[num_values_on_curr_page] = val;
	num_values_on_curr_page++;

	if (num_values_on_curr_page >= num_values_per_page) {
		output_fh.FlushPage(curr_page);
		num_values_on_curr_page = 0;
	}
	
}

int main(int argc, char *argv[]) {
	// printf("---------- Linear Search ----------\n");
	char *input_file = argv[1];
	char *query_file = argv[2];
	char *output_file = argv[3];

	/* Get Input Attr */
	input_fh = fm.OpenFile(input_file);
	// TODO think optimization
	num_pages = input_fh.LastPage().GetPageNum();
	input_fh.FlushPage(num_pages);
	num_pages++;

	num_values_per_page = PAGE_CONTENT_SIZE/sizeof(int);
	// cout << "Number of Pages = " << num_pages << endl;
	// cout << "Page Content Size = " << PAGE_CONTENT_SIZE << endl;
	// cout << "Number of Integers on a page = " << num_values_per_page << endl << endl;

	/* Output Attr */
	output_fh = fm.CreateFile(output_file);
	curr_page = -1;
	num_values_on_curr_page = 0;

	/* Query File */
	ifstream infile;
	infile.open(query_file);
	string query;
	int target;
	int query_number = 1;

	while(infile>>query>>target) {
		// cout << "Query Number " << query_number << " Search for " << target << endl;
		query_number++;

		for(int i = 0; i < num_pages; i++){
			input_ph = input_fh.PageAt(i);
			int *input_data = (int *)input_ph.GetData();
			/* Iterating over data in ith page */
			// TODO optimisation binary search over a page
			for (int j = 0; j < num_values_per_page; j++) {
				if(i==num_pages-1 && input_data[j]==INT_MIN){
					break;
				}
				if (target == input_data[j]) {
					write_output(i);
					write_output(j);
					// cout << "Target " << target << " found on page number " << i << " at offset " << j << endl;
				}
			}
			input_fh.FlushPage(i);
		}
		write_output(-1);
		write_output(-1);
		
		// cout << endl;
	}
	while (num_values_on_curr_page != 0) {
		write_output(INT_MIN);
	}
	// Not to worry about this as done by CloseFile implicitly
	// output_fh.FlushPages();

	infile.close();
	fm.CloseFile(output_fh);
	fm.CloseFile(input_fh);
	return 0;
}