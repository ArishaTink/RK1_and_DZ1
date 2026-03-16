#include <iostream>
using namespace std;
#include <boost/thread.hpp>
#include <thread>
#include <mutex>
#include <random>
#include <condition_variable>

class CountingSemaphore {
private:
	std::mutex mtx;
	std::condition_variable cv;
	int count;
public:
	explicit CountingSemaphore(int init_count) : count(init_count) {}

	void acquire() {
		 std::unique_lock<std::mutex> lock(mtx);
		 cv.wait(lock, [this]() { return count > 0; });
		 --count;
	 }

	 void release() {
		 std::unique_lock<std::mutex> lock(mtx);
		 ++count;
		 cv.notify_one();
	 }
};

CountingSemaphore sem(3);

int getRandomNumber(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(min, max);
    return dist(gen);
}

void worker(int id) {
	 sem.acquire();
	 std::cout << "Рабочий " << id << " работает" << endl;
	 int seconds = getRandomNumber(1,3);
	 std::this_thread::sleep_for(std::chrono::seconds(seconds));
	 std::cout << "Рабочий " << id << " завершил работу за " << seconds << " сек" << endl;
	 sem.release();
}
int main() {
	 std::thread threads[10];

	 for (int i = 0; i < 5; ++i) {
		 threads[i] = std::thread(worker, i + 1);
	 }

	 for (auto& t : threads) {
		 t.join();
	 }
	 return 0;
}


