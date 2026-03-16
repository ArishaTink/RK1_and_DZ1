#include <iostream>
#include <boost/thread.hpp>
#include <atomic>
#include <string>
#include <queue>
#include <fstream>
#include <iterator>
#include <chrono>
using namespace std;

size_t countCharacters(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) return 0;
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return content.length();
}

int main() {
	boost::mutex queueMutex;
	std::atomic<int> proccessedFilesCounter = 0;
	std::vector<boost::thread> threads;
	std::queue<string> filesQueue;

	string path = "C:\\Users\\dixxa\\eclipse-workspace\\DZ1\\data\\";

	for (int i = 1; i <= 10; ++i) {
		string fname = "data" + to_string(i) + ".txt";
	    filesQueue.emplace(fname);
	}

	auto startTime = chrono::high_resolution_clock::now();

	for (int i = 0; i < 3; i++) {
		threads.push_back(boost::thread([&, thread_num = i + 1](){
			while (true) {
				string currentFile;
				{
					boost::mutex::scoped_lock lock(queueMutex);
					if (filesQueue.empty()) {
						break;
					}
					currentFile = filesQueue.front();
					filesQueue.pop();
				}

				std::ifstream in(path + currentFile);
				if (in.is_open()) {
					size_t symbols = countCharacters(path + currentFile);
					proccessedFilesCounter++;
					ofstream file(path + "processed_" + currentFile + ".txt");
					file << "Amount of symbols in " << currentFile << ": " << symbols;
					file.close();
					in.close();
				}
			}
		}));
	}

	for (auto& t : threads) {
	        t.join();
	    }

	auto endTime = chrono::high_resolution_clock::now();


	cout << "Всего обработано файлов: " << proccessedFilesCounter.load() << endl;
	cout << "Общее время обработки: " << chrono::duration<double>(endTime - startTime).count() << " секунд\n";
	return 0;
}
