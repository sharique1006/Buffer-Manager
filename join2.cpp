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

int get_effective_last(int *arr) {
	// gives last accessible
	int res = num_values_per_page - 1;
	while (arr[0] > arr[res]) res--;
	return res;
}

int find_first_occurence(int *arr, int target) {
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

int find_last_occurence(int *arr, int target) {
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

pair<int, int> find_first_page(int low, int high, int target) {
	// page, page position
	pair<int, int> res = make_pair(-1, -1);

	while (low <= high) {
		// Normal Binary Search Logic
		int mid = (low + high) / 2;
		input_ph2 = input_fh2.PageAt(mid);
		int *input_data = (int *)input_ph2.GetData();
		input_fh2.FlushPage(mid);

		int effective_last = get_effective_last(input_data);

		if (input_data[0] > target)
			high = mid - 1;
		else if (input_data[effective_last] < target)
			low = mid + 1;
		else {
			// split into finding first and last occurence
			// input_data[0] <= target <= input_data[effective_last]
			int first_occ = find_first_occurence(input_data, target);
			if (first_occ == -1) {
				// input_data[0] < target < input_data[effective_last]
				res = make_pair(-1, -1);
				break;
			} else if (first_occ == 0) {
				res = make_pair(mid, 0);
				high = mid - 1;
			} else {
				res = make_pair(mid, first_occ);
				break;
			}
		}
	}
	return res;
}

pair<int, int> find_last_page(int low, int high, int target) {
	// page, page position
	pair<int, int> res = make_pair(-1, -1);

	while (low <= high) {
		// Normal Binary Search Logic
		int mid = (low + high) / 2;
		input_ph2 = input_fh2.PageAt(mid);
		int *input_data = (int *)input_ph2.GetData();
		input_fh2.FlushPage(mid);

		int effective_last = get_effective_last(input_data);

		if (input_data[0] > target)
			high = mid - 1;
		else if (input_data[effective_last] < target)
			low = mid + 1;
		else {
			// split into finding first and last occurence
			// input_data[0] <= target <= input_data[effective_last]
			int last_occ = find_last_occurence(input_data, target);
			if (last_occ == -1) {
				// input_data[0] < target < input_data[effective_last]
				res = make_pair(-1, -1);
				break;
			} else if (last_occ == effective_last) {
				res = make_pair(mid, effective_last);
				low = mid + 1;
			} else {
				res = make_pair(mid, last_occ);
				break;
			}
		}
	}
	return res;
}

int main(int argc, char *argv[]) {
	// printf("---------- Join2 ----------\n");
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

	for (int i = 0; i < num_pages1; i++) {
		input_ph1 = input_fh1.PageAt(i);
		int *input_data1 = (int *)input_ph1.GetData();
		for (int k = 0; k < num_values_per_page; k++) {
			if (i == num_pages2 - 1 && input_data1[k] == INT_MIN) {
					break;
			}
			//binary search input_data1[k] in R2
			int target = input_data1[k];
			int low = 0, high = num_pages2 - 1;
			pair<int, int> first_occ = make_pair(-1, -1), last_occ = make_pair(-1, -1);
			while (low <= high) {
				// Normal Binary Search Logic
				int mid = (low + high) / 2;
				input_ph2 = input_fh2.PageAt(mid);
				int *input_data = (int *)input_ph2.GetData();
				input_fh2.FlushPage(mid);

				int effective_last = get_effective_last(input_data);

				if (input_data[0] > target)
					high = mid - 1;
				else if (input_data[effective_last] < target)
					low = mid + 1;
				else {
					// printf("Splitting at low %d and high %d and mid %d\n",low, high, mid);
					// split into finding first and last occurence
					first_occ = find_first_page(low, mid, target);
					if (first_occ.first != -1) last_occ = find_last_page(mid, high, target);
					break;
				}
			}

			// printf("First occurence of %d: At page %d and position %d\n", target, first_occ.first, first_occ.second);
			// printf("Last occurence of %d: At page %d and position %d\n", target, last_occ.first, last_occ.second);

			// input_data1[k] repeated for
			if (first_occ.first != -1) {
				// cout<<first_occ.first<<endl;
				int num = (last_occ.first - first_occ.first) * num_values_per_page + last_occ.second - first_occ.second + 1;
				// printf("%d\n", num );
				for (int ip = 0; ip < num; ip++) {
					write_output(target);
				}
			}

			
		}
		if ((i + 1) % (n - 2) == 0 && i != 0) {
			int l = i;
			for (int z = 0; z < n - 2; z++) {
				input_fh1.UnpinPage(l--);
			}
		}
	}

	while (num_values_on_curr_page != 0) {
		write_output(INT_MIN);
	}
	fm.CloseFile(output_fh);
	fm.CloseFile(input_fh1);
	fm.CloseFile(input_fh2);


	return 0;
}