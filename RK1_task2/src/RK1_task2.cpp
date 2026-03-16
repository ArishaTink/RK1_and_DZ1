#include <iostream>
using namespace std;
#include <thread>
#include <vector>
#include <queue>
#include <semaphore>
#include <random>
#include <boost/thread.hpp>

std::binary_semaphore detail_access(1);
std::binary_semaphore storage_access(1);

struct Detail{
	bool processed;
	int id;

	Detail(bool a, int b) : processed(a), id(b) {}
};

int getRandomNumber(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(min, max);
    return dist(gen);
}

int main() {
	vector<std::thread> threads;
	vector<Detail> storage;
	std::queue<Detail> details;

	for (int  i = 0; i < 10; ++i) {
		details.emplace(false, i+1);
	}

	for (int i = 0; i < 3; ++i) {
		threads.push_back(std::thread([&, thread_num = i + 1](){

			while (true) {
				detail_access.acquire();
				if (details.empty()) {
					detail_access.release();
					break;
				}
				Detail detail = details.front();
				details.pop();
				detail_access.release();

				detail.processed = true;
				int seconds = getRandomNumber(1,2);
				std::this_thread::sleep_for(std::chrono::seconds(seconds));


				storage_access.acquire();
				storage.push_back(detail);
				cout << "Машина " << thread_num << " обработала деталь " << detail.id << " за " << seconds << " сек" << endl;
				storage_access.release();

			}
		}));
	}

	for (auto& t : threads) {
		t.join();
	}

	return 0;
}
