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

void prune_file(pair<int,int> first_occ, pair<int,int> last_occ){
	// fm.PrintBuffer();

	// Need as many page handlers as there are pages open consecutively
	if(first_occ.first==-1) return;
	int add_to_page = first_occ.first;
	int add_to_ptr = first_occ.second;
	PageHandler ph_to = input_fh.PageAt(add_to_page);
	int *ph_to_data = (int *)ph_to.GetData();
	
	int add_from_page = last_occ.first;
	int add_from_ptr = last_occ.second + 1;
	PageHandler ph_from = input_fh.PageAt(add_from_page);
	int *ph_from_data = (int *)ph_from.GetData();

	while(1){
		// fm.PrintBuffer();
		if(add_from_ptr == num_values_per_page) {
			if (add_to_page != add_from_page) input_fh.FlushPage(add_from_page);
			add_from_page = add_from_page + 1;
			if (add_from_page == num_pages) break;
			add_from_ptr = 0;
			ph_from = input_fh.PageAt(add_from_page);
			ph_from_data = (int *)ph_from.GetData();
		}

		if(ph_from_data[add_from_ptr]==INT_MIN){
			if (add_to_page != add_from_page) input_fh.FlushPage(add_from_page);
			break;
		}

		if(add_to_ptr == num_values_per_page){
			add_to_page = add_to_page + 1;
			add_to_ptr = 0;
			ph_to = input_fh.PageAt(add_to_page);
			ph_to_data = (int *)ph_to.GetData();
		}

		//printf("Copying to page %d offset %d from page %d offset %d\n", add_to_page, add_to_ptr, add_from_page, add_from_ptr);
		
		ph_to_data[add_to_ptr] = ph_from_data[add_from_ptr];

		add_to_ptr++;
		add_from_ptr++;

		if(add_to_ptr == num_values_per_page) {
			input_fh.MarkDirty(add_to_page);
			input_fh.FlushPage(add_to_page);
		}
	}
	if(add_to_ptr!=0){
		for(int i=add_to_ptr;i<num_values_per_page;i++){
			ph_to_data[i] = INT_MIN;
		}	
	}
	
	input_fh.MarkDirty(add_to_page);
	input_fh.FlushPage(add_to_page);

	// printf("Alright till here\n");
	// fm.PrintBuffer();

	// Delete add_to_page only when add_to_ptr is 0
	for(int i=num_pages-1;i>add_to_page-(add_to_ptr==0);i--){
		input_fh.DisposePage(i);
		input_fh.FlushPage(i);
		num_pages--;
	}

}

void is_file_empty(){
	PageHandler ph_dummy = input_fh.FirstPage();
	int* data = (int*)ph_dummy.GetData();
	if(data[0]==INT_MIN) input_fh.DisposePage(0);
	num_pages--;
}

int main(int argc, char *argv[]) {
	// printf("---------- Deletion ----------\n");
	char *input_file = argv[1];
	char *query_file = argv[2];

	/* Get Input Attr */
	input_fh = fm.OpenFile(input_file);
	// TODO think optimization
	num_pages = input_fh.LastPage().GetPageNum();
	// printf("++++++++++++NUM PAGES = %d++++++++++++\n",num_pages);
	if(num_pages==-1){
		fm.CloseFile(input_fh);
		return 0;
	}
	input_fh.FlushPage(num_pages);
	num_pages++;

	num_values_per_page = PAGE_CONTENT_SIZE/sizeof(int);
	// cout << "Number of Pages = " << num_pages << endl;
	// cout << "Page Content Size = " << PAGE_CONTENT_SIZE << endl;
	// cout << "Number of Integers on a page = " << num_values_per_page << endl << endl;

	/* Query File */
	ifstream infile;
	infile.open(query_file);
	string query;
	int target;
	int query_number = 1;

	while(infile>>query>>target) {
		//cout << "Query Number " << query_number << " DELETE " << target << endl;
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
				// split into finding first and last occurence
				first_occ = find_first_page(low, mid, target);
				if(first_occ.first!=-1) last_occ = find_last_page(mid, high, target);
				break;
			}
		}

		// printf("First occurence: At page %d and position %d\n",first_occ.first,first_occ.second);
		// printf("Last occurence: At page %d and position %d\n",last_occ.first,last_occ.second);

		prune_file(first_occ,last_occ);
	}

	is_file_empty();

	// Not to worry about this as done by CloseFile implicitly
	// output_fh.FlushPages();

	infile.close();
	fm.CloseFile(input_fh);

	return 0;
}