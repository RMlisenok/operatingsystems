#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <random>

const int BUFFER_SIZE = 5; // Size of the buffer
const int NUM_PRODUCERS = 2; // Number of producers
const int NUM_CONSUMERS = 2; // Number of consumers
const int NUM_ITEMS = 10; // Number of items to produce and consume

// Structure for storing data
struct Data {
    int id; // Identifier for the data
    std::string message; // Message
};

// Buffer for storing data
Data buf[BUFFER_SIZE];
int count = 0; // Number of elements in the buffer
std::mutex mtx; // Mutex for synchronization
std::condition_variable cv_producer; // Condition variable for producers
std::condition_variable cv_consumer; // Condition variable for consumers

void Producer(int id) {
    for (int i = 0; i < NUM_ITEMS; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100)); // Simulating work

        Data data;
        data.id = i + 1; // Fill the identifier
        data.message = "Data from producer " + std::to_string(id); // Fill the message

        std::unique_lock<std::mutex> lock(mtx);
        cv_producer.wait(lock, [] { return count < BUFFER_SIZE; }); // Wait for space in the buffer

        // Produce element
        buf[count++] = data; // Add data to the buffer
        std::cout << "Producer " << id << " produced element: " << data.id << ", message: " << data.message << std::endl;

        // Notify consumers that an element has been added
        cv_consumer.notify_one();
    }
}

void Consumer(int id) {
    for (int i = 0; i < NUM_ITEMS; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 150)); // Simulating work

        std::unique_lock<std::mutex> lock(mtx);
        cv_consumer.wait(lock, [] { return count > 0; }); // Wait for the buffer to be non-empty

        // Consume element
        Data data = buf[--count]; // Extract data from the buffer
        std::cout << "Consumer " << id << " consumed element: " << data.id << ", message: " << data.message << std::endl;

        // Notify producers that an element has been extracted
        cv_producer.notify_one();
    }
}

int main() {
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    // Launch producers
    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        producers.emplace_back(Producer, i + 1);
    }

    // Launch consumers
    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        consumers.emplace_back(Consumer, i + 1);
    }

    // Wait for all threads to finish
    for (auto& producer : producers) {
        producer.join();
    }

    for (auto& consumer : consumers) {
        consumer.join();
    }

    return 0;
}
//Пусть имеется несколько производителей и несколько потребителей.