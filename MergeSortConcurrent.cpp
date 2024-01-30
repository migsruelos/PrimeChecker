#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <string>

using namespace std;

#define CONTROL 8388608 //2^23

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

int main(){
    // TODO: Seed your randomizer
    using namespace std::chrono;
    long long time = time_point_cast<nanoseconds>(system_clock::now()).time_since_epoch().count();
    srand(time);

    // TODO: Get array size and thread count from user
    int upper_bound;
    int num_threads;

    std::cout << "Enter upper bound (default=2^23): ";
    std::string input;
    std::getline(std::cin, input);

    if (!input.empty()) {
        try {
        upper_bound = std::stoi(input);
        } catch (std::invalid_argument&) {
        std::cerr << "Invalid input. Using default CONTROL." << std::endl;
        upper_bound = CONTROL;
        }
    } else {
        upper_bound = CONTROL;  
    }

    std::cout << "Enter the number of threads (default=1): ";
    std::cin >> num_threads;

    // TODO: Generate a random array of given size
    vector<int> randArray;
    for(int i = 0; i < upper_bound; i++){
        randArray.push_back(rand() % upper_bound);
    }

    // TODO: Call the generate_intervals method to generate the merge sequence

    // TODO: Call merge on each interval in sequence

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
        if(r_ptr == (int)right.size() || left[l_ptr] <= right[r_ptr]) {
            array[i] = left[l_ptr];
            l_ptr++;
        } else {
            array[i] = right[r_ptr];
            r_ptr++;
        }
    }
}
