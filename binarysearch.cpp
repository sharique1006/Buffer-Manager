#include <iostream>
#include "file_manager.h"
#include "errors.h"
#include <cstring>

using namespace std;

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

int get_effective_last(int *arr){
	// gives last accessible
	int res = num_values_per_page - 1;
	while(arr[0]>arr[res]) res--;
	return res;
}

int find_first_occurence(int *arr,int target){
	int effective_last = get_effective_last(arr);

	int low = 0, high = effective_last, res = -1;
    while (low <= high) {
        // Normal Binary Search Logic
        int mid = (low + high) / 2;
        if (arr[mid] > target)
            high = mid - 1;
        else if (arr[mid] < target)
            low = mid + 1;
        else {
            res = mid;
            high = mid - 1;
        }
    }
    return res;
}

int find_last_occurence(int *arr,int target){
	int effective_last = get_effective_last(arr);

	int low = 0, high = effective_last, res = -1;
    while (low <= high) {
        // Normal Binary Search Logic
        int mid = (low + high) / 2;
        if (arr[mid] > target)
            high = mid - 1;
        else if (arr[mid] < target)
            low = mid + 1;
        else {
            res = mid;
            low = mid + 1;
        }
    }
    return res;
}

pair<int,int> find_first_page(int low, int high, int target){
	// page, page position
	pair<int,int> res = make_pair(-1,-1);

	while (low <= high) {
		// Normal Binary Search Logic
		int mid = (low + high) / 2;
		input_ph = input_fh.PageAt(mid);
		int *input_data = (int *)input_ph.GetData();
		input_fh.FlushPage(mid);

		int effective_last = get_effective_last(input_data);

		if (input_data[0] > target)
			high = mid - 1;
		else if (input_data[effective_last] < target)
			low = mid + 1;
		else {
			// split into finding first and last occurence
			// input_data[0] <= target <= input_data[effective_last]
			int first_occ = find_first_occurence(input_data, target);
			if(first_occ == -1){
				// input_data[0] < target < input_data[effective_last]
				res = make_pair(-1,-1);
				break;
			}else if(first_occ == 0){
				res = make_pair(mid,0);
				high = mid - 1;
			}else{
				res = make_pair(mid, first_occ);
				break;
			}
		}
	}
    return res;
}

pair<int,int> find_last_page(int low, int high, int target){
	// page, page position
	pair<int,int> res = make_pair(-1,-1);

	while (low <= high) {
		// Normal Binary Search Logic
		int mid = (low + high) / 2;
		input_ph = input_fh.PageAt(mid);
		int *input_data = (int *)input_ph.GetData();
		input_fh.FlushPage(mid);

		int effective_last = get_effective_last(input_data);

		if (input_data[0] > target)
			high = mid - 1;
		else if (input_data[effective_last] < target)
			low = mid + 1;
		else {
			// split into finding first and last occurence
			// input_data[0] <= target <= input_data[effective_last]
			int last_occ = find_last_occurence(input_data, target);
			if(last_occ == -1){
				// input_data[0] < target < input_data[effective_last]
				res = make_pair(-1,-1);
				break;
			}else if(last_occ == effective_last){
				res = make_pair(mid,effective_last);
				low = mid + 1;
			}else{
				res = make_pair(mid, last_occ);
				break;
			}
		}
	}
    return res;
}

int main(int argc, char *argv[]) {
	// printf("---------- Binary search ----------\n");
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
		
		int low = 0, high = num_pages - 1;
		pair<int,int> first_occ = make_pair(-1,-1), last_occ = make_pair(-1,-1);
		while (low <= high) {
			// Normal Binary Search Logic
			int mid = (low + high) / 2;
			input_ph = input_fh.PageAt(mid);
			int *input_data = (int *)input_ph.GetData();
			input_fh.FlushPage(mid);

			int effective_last = get_effective_last(input_data);

			if (input_data[0] > target)
				high = mid - 1;
			else if (input_data[effective_last] < target)
				low = mid + 1;
			else {
				// printf("Splitting at low %d and high %d and mid %d\n",low, high, mid);
				// split into finding first and last occurence
				first_occ = find_first_page(low, mid, target);
				if(first_occ.first!=-1) last_occ = find_last_page(mid, high, target);
				break;
			}
		}

		// printf("First occurence: At page %d and position %d\n",first_occ.first,first_occ.second);
		// printf("Last occurence: At page %d and position %d\n",last_occ.first,last_occ.second);

		int first_occ_page = first_occ.first;
		int first_occ_offset = first_occ.second;

		int last_occ_page = last_occ.first;
		int last_occ_offset = last_occ.second;

		if (first_occ_page != -1) {
			while(first_occ_page <= last_occ_page) {
				write_output(first_occ_page);
				write_output(first_occ_offset);
				first_occ_offset++;
				if (first_occ_offset == num_values_per_page) {
					first_occ_page++;
					first_occ_offset = 0;
				}
				if (first_occ_page == last_occ_page && first_occ_offset > last_occ_offset) {
					break;
				}		
			}
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