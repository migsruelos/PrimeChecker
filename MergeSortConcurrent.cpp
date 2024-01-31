#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <string>
#include <thread>
#include <random>
#include <algorithm>


using namespace std;
using namespace std::chrono;

#define CONTROL 8388608 //2^23
#define RAND_SEED 1

typedef pair<int,int> ii;

/*
This function generates all the intervals for merge sort iteratively, given the 
range of indices to sort. Algorithm runs in O(n).

Parameters:
start : int - start of range
end : int - end of range (inclusive)

Returns a list of integer pairs indicating the ranges for merge sort.
*/
vector<ii> generate_intervals(int start, int end);

/*
This function performs the merge operation of merge sort.

Parameters:
array : vector<int> - array to sort
s     : int         - start index of merge
e     : int         - end index (inclusive) of merge
*/
void merge(vector<int> &array, int s, int e);
void merge_concurrent(vector<int> &array, const vector<ii> &intervals, int thread_count);

int main(){
    // TODO: Seed your randomizer
    srand(RAND_SEED);

    // TODO: Get array size and thread count from user
    int array_size;
    int thread_count;

    std::cout << "Enter array size (default=2^23): ";
    std::string input;
    std::getline(std::cin, input);

    if (!input.empty()) {
        try {
        array_size = std::stoi(input);
        } catch (std::invalid_argument&) {
        std::cerr << "Invalid input. Using default CONTROL." << std::endl;
        array_size = CONTROL;
        }
    } else {
        array_size = CONTROL;  
    }

    std::cout << "Enter the number of threads: ";
    std::cin >> thread_count;

    // TODO: Generate a random array of given size
    vector<int> randArray;
    for(int i = 0; i < array_size; i++){ //Fill array from 1 to N
        randArray.push_back(i+1);
    }
    int temp, randIndex;
    for(int i = 0; i < array_size; i++){ //Shuffle
        temp = randArray[i]; //Current to temp
        randIndex = rand() % array_size; //Get random index
        randArray[i] = randArray[randIndex]; //Set current to random index value
        randArray[randIndex] = temp; //Set random index to current in temp
    }

	// TODO: Call the generate_intervals method to generate the merge sequence
	vector<ii> intervals = generate_intervals(0, array_size - 1);

	// TODO: Call merge on each interval in sequence
	auto start = high_resolution_clock::now();
    for (auto interval : intervals) {
        merge(randArray, interval.first, interval.second);
    }
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);
    std::cout << "Single-threaded merge sort time: " << duration.count() << " ms" << endl;
    
    // Call the concurrent merge sort function
    auto start_concurrent = high_resolution_clock::now();
    merge_concurrent(randArray, intervals, thread_count);
    auto stop_concurrent = high_resolution_clock::now();

    auto duration_concurrent = duration_cast<microseconds>(stop_concurrent - start_concurrent);
    std::cout << "Concurrent merge sort time: " << duration_concurrent.count() << " ms" << endl;
    
    // Sanity check
    bool isSorted = is_sorted(randArray.begin(), randArray.end());
    if (isSorted) {
        std::cout << "Array is sorted.\n";
    } else {
        std::cout << "Array is not sorted!\n";
    }

    // Once you get the single-threaded version to work, it's time to implement 
    // the concurrent version. Good luck :)
}

vector<ii> generate_intervals(int start, int end) {
    vector<ii> frontier;
    frontier.push_back(ii(start,end));
    int i = 0;
    while(i < (int)frontier.size()){
        int s = frontier[i].first;
        int e = frontier[i].second;

        i++;

        // if base case
        if(s == e){
            continue;
        }

        // compute midpoint
        int m = s + (e - s) / 2;

        // add prerequisite intervals
        frontier.push_back(ii(m + 1,e));
        frontier.push_back(ii(s,m));
    }

    vector<ii> retval;
    for(int i = (int)frontier.size() - 1; i >= 0; i--) {
        retval.push_back(frontier[i]);
    }
    return retval;
}

void merge(vector<int> &array, int s, int e) {
    int m = s + (e - s) / 2;
    vector<int> left;
    vector<int> right;
    for(int i = s; i <= e; i++) {
        if(i <= m) {
            left.push_back(array[i]);
        } else {
            right.push_back(array[i]);
        }
    }
    int l_ptr = 0, r_ptr = 0;

    for(int i = s; i <= e; i++) {
        // no more elements on left half
        if(l_ptr == (int)left.size()) {
            array[i] = right[r_ptr];
            r_ptr++;

        // no more elements on right half or left element comes first
        } else if(r_ptr == (int)right.size() || left[l_ptr] <= right[r_ptr]) {
            array[i] = left[l_ptr];
            l_ptr++;
        } else {
            array[i] = right[r_ptr];
            r_ptr++;
        }
    }
}

void merge_concurrent(vector<int> &array, const vector<ii> &intervals, int thread_count) {
    vector<thread> threads;

    int intervals_per_thread = intervals.size() / thread_count;
    int start = 0;

    for (int i = 0; i < thread_count; i++) {
        int end = start + intervals_per_thread - 1;
        if (i == thread_count - 1) {
            
            end = intervals.size() - 1;
        }

        threads.emplace_back([&, start, end]() {
            for (int j = start; j <= end; j++) {
                merge(array, intervals[j].first, intervals[j].second);
            }
        });

        start = end + 1;
    }

    for (auto &thread : threads) {
        thread.join();
    }
}
