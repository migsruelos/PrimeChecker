#include <iostream>
#include <utility>
#include <vector>
#include <algorithm> // std::shuffle
#include <random>    // std::mt19937 and std::random_device
#include <chrono>    // timer
#include <thread>    // std::thread

using namespace std;

typedef pair<int, int> ii;

const unsigned int seed_value = 42; // Task 1: Seed value defined as a constant

vector<ii> generate_intervals(int start, int end);

void merge(vector<int> &array, int s, int e);

// Task 6: Concurrent version of merge sort
void merge_concurrent(vector<int> &array, const vector<ii> &intervals);

int main() {
    // Task 1: Instantiate and seed a randomizer/random number generator
    mt19937 rng(seed_value);

    // Task 2: User input: array size and thread count
    int array_size, thread_count;
    cout << "Enter array size: ";
    cin >> array_size;
    cout << "Enter thread count: ";
    cin >> thread_count;

    // Task 3: Generate a random array of given size and shuffle
    vector<int> random_array(array_size);
    iota(random_array.begin(), random_array.end(), 1); // Filling array with integers 1 to N
    shuffle(random_array.begin(), random_array.end(), rng);

    // Task 4: Use the generate_intervals method to generate the merge sequence
    vector<ii> intervals = generate_intervals(0, array_size - 1);

    // Task 5: Merge on each interval in sequence (single-thread) and time the execution
    auto start_time = chrono::high_resolution_clock::now(); // Timer start
    for (const auto &interval : intervals) {
        merge(random_array, interval.first, interval.second);
    }
    auto end_time = chrono::high_resolution_clock::now(); // Timer end

    // Calculate and display execution time
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    cout << "Single-threaded execution time: " << duration.count() << " microseconds" << endl;

    // Task 6: Implement the concurrent version of merge sort
    start_time = chrono::high_resolution_clock::now(); // Timer start for concurrent version
    merge_concurrent(random_array, intervals);
    end_time = chrono::high_resolution_clock::now(); // Timer end for concurrent version

    // Calculate and display execution time for concurrent version
    duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    cout << "Concurrent execution time: " << duration.count() << " microseconds" << endl;

    // Task 7: Implement a sanity check to verify that the array is sorted
    if (is_sorted(random_array.begin(), random_array.end())) {
        cout << "Array is sorted!" << endl;
    } else {
        cout << "Array is not sorted!" << endl;
    }

    return 0;
}

vector<ii> generate_intervals(int start, int end) {
    vector<ii> frontier;
    frontier.push_back(ii(start, end));
    int i = 0;
    while (i < (int)frontier.size()) {
        int s = frontier[i].first;
        int e = frontier[i].second;

        i++;

        // if base case
        if (s == e) {
            continue;
        }

        // compute midpoint
        int m = s + (e - s) / 2;

        // add prerequisite intervals
        frontier.push_back(ii(m + 1, e));
        frontier.push_back(ii(s, m));
    }

    vector<ii> retval;
    for (int i = (int)frontier.size() - 1; i >= 0; i--) {
        retval.push_back(frontier[i]);
    }
    return retval;
}

void merge(vector<int> &array, int s, int e) {
    int m = s + (e - s) / 2;
    vector<int> left;
    vector<int> right;
    for (int i = s; i <= e; i++) {
        if (i <= m) {
            left.push_back(array[i]);
        } else {
            right.push_back(array[i]);
        }
    }
    int l_ptr = 0, r_ptr = 0;

    for (int i = s; i <= e; i++) {
        // no more elements on left half
        if (l_ptr == (int)left.size()) {
            array[i] = right[r_ptr];
            r_ptr++;

        // no more elements on right half or left element comes first
        } else if (r_ptr == (int)right.size() || left[l_ptr] <= right[r_ptr]) {
            array[i] = left[l_ptr];
            l_ptr++;
        } else {
            array[i] = right[r_ptr];
            r_ptr++;
        }
    }
}

// Task 6: Concurrent version of merge sort
void merge_concurrent(vector<int> &array, const vector<ii> &intervals) {
    vector<thread> threads;

    for (const auto &interval : intervals) {
        threads.emplace_back([&array, interval]() {
            merge(array, interval.first, interval.second);
        });
    }

    for (auto &thread : threads) {
        thread.join();
    }
}
