#include <iostream>
#include "file_manager.h"
#include "errors.h"
#include <cstring>
#include <cmath>
#include <limits.h>

using namespace std;

int n = BUFFER_SIZE;

/* File Manager */
FileManager fm;

int num_values_per_page;

/* Input File1 Handler */
FileHandler input_fh1;
PageHandler input_ph1;
int num_pages1;

/* Input File2 Handler */
FileHandler input_fh2;
PageHandler input_ph2;
int num_pages2;

/* Output File Handler */
FileHandler output_fh;
PageHandler output_ph;
int curr_page;
int num_values_on_curr_page;
int* data;

void write_output(int val) {
	if (num_values_on_curr_page == 0) {
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
	// printf("---------- Join1 ----------\n");
	char *input_file1 = argv[1];
	char *input_file2 = argv[2];
	char *output_file = argv[3];


	/* Get Input Attr */
	input_fh1 = fm.OpenFile(input_file1);
	num_pages1 = input_fh1.LastPage().GetPageNum();
	input_fh1.FlushPage(num_pages1);
	num_pages1++;

	/* Get Input Attr */
	input_fh2 = fm.OpenFile(input_file2);
	num_pages2 = input_fh2.LastPage().GetPageNum();
	input_fh2.FlushPage(num_pages2);
	num_pages2++;

	num_values_per_page = PAGE_CONTENT_SIZE / sizeof(int);
	// cout << "Number of Pages = " << num_pages1 << " " << num_pages2 << endl;
	// cout << "Page Content Size = " << PAGE_CONTENT_SIZE << endl;
	// cout << "Number of Integers on a page = " << num_values_per_page << endl << endl;

	/* Output Attr */
	output_fh = fm.CreateFile(output_file);
	curr_page = -1;
	num_values_on_curr_page = 0;

	for (int i = 0; i < num_pages1 * ceil((double)num_pages2 / (n - 2)); i++) {
		int page_R1 = i % num_pages1;
		
		if (page_R1 == 0 && i != 0) {
			for (int j = (i / num_pages1 - 1) * (n - 2); j < n - 2 + (i / num_pages1 - 1) * (n - 2) && j < num_pages2; j++) {
				input_fh2.UnpinPage(j);
			}
		}
		input_ph1 = input_fh1.PageAt(page_R1);
		int *input_data1 = (int *)input_ph1.GetData();
		for (int j = 0; j < n - 2; j++) {
			int page_R2 =  j+(i / num_pages1)*(n-2);
			if(page_R2 >= num_pages2){
				break;
			}
			// cout << i % num_pages1  << " " << j+(i / num_pages1)*(n-2) << endl;
			
			input_ph2 = input_fh2.PageAt(page_R2);
			int *input_data2 = (int *)input_ph2.GetData();
			// fm.PrintBuffer();

			for (int k = 0; k < num_values_per_page; k++) {
				if (page_R1 == num_pages1 - 1 && input_data1[k] == INT_MIN) {
					break;
				}
				for (int l = 0; l < num_values_per_page; l++) {
					if (page_R2 == num_pages2 - 1 && input_data2[l] == INT_MIN) {
						break;
					}
					if (input_data1[k] == input_data2[l]) {
						write_output(input_data1[k]);
					}

				}
				
			}


		}
		input_fh1.UnpinPage(page_R1);
	}

	while (num_values_on_curr_page != 0) {
		write_output(INT_MIN);
	}
	// Not to worry about this as done by CloseFile implicitly
	// output_fh.FlushPages();
	fm.CloseFile(output_fh);
	fm.CloseFile(input_fh1);
	fm.CloseFile(input_fh2);

	return 0;
}