#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>

#define LIMIT 10000000

std::vector<int> primes;
std::mutex primes_mutex;

/*
This function checks if an integer n is prime.

Parameters:
n : int - integer to check

Returns true if n is prime, and false otherwise.
*/
bool check_prime(const int &n) {
	
  for (int i = 2; i * i <= n; i++) {
    if (n % i == 0) {
      return false;
    }
  }
  
  return true;
}

/*
This function implements mutual exclusion for the `primes` list.

Parameters:
start : int - start of the range (inclusive)
end : int - end of the range (inclusive)
*/
void get_primes(int start, int end) {
	
  for (int current_num = start; current_num <= end; current_num++) {
    bool is_prime = check_prime(current_num);

    if (is_prime) {
      {
        std::lock_guard<std::mutex> lock(primes_mutex);
        primes.push_back(current_num);
      }
    }
  }
  
}


int main() {
  int upper_bound;
  int num_threads;

  // user input 
  std::cout << "Enter the upper bound of integers to check: ";
  std::cin >> upper_bound;

  std::cout << "Enter the number of threads (default=1): ";
  std::cin >> num_threads;

  // start timer
  auto start_time = std::chrono::high_resolution_clock::now();

  // split the range of integers across the specified number of threads
  std::vector<std::thread> threads;
  int range_size = upper_bound / num_threads;
  
  for (int i = 0; i < num_threads; i++) {
    int start = i * range_size + 2;
    int end = (i == num_threads - 1) ? upper_bound : (i + 1) * range_size;
    threads.emplace_back(get_primes, start, end);
  }

  // join threads
  for (auto &thread : threads) {
    thread.join();
  }

  // stop timer
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

  // print output
  std::cout << "Total Time Taken: " << duration << " ms" << std::endl;
  std::cout << primes.size() << " primes were found." << std::endl;

  return 0;
}

