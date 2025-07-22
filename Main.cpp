#include <windows.h>
#include <string>
#include <iostream>
#include <stack>
#include <vector>
#include <ranges>
#include <print>
#include <random>
#include <numeric>
#include <fstream>

// (inclusive) 
int randomInteger(int min, int max) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist((((0) > (min)) ? (0) : (min)), max);
	return dist(gen);
}

inline int randomIntgerSpread(int num, int spread) {
	return randomInteger(num - spread, num + spread);
}

void sendKey(const WORD vkCode, const bool keyDown) {
	INPUT input = { 0 };
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = vkCode;
	input.ki.dwFlags = keyDown ? 0 : KEYEVENTF_KEYUP;

	SendInput(1, &input, sizeof(INPUT));
}

void sendSmartInput(const char c) {
	const SHORT vkResult = VkKeyScan(c);

	if (vkResult == -1) {
		std::cerr << "Cannot map character: " << c << "\n";
		return;
	}

	const BYTE vkCode = LOBYTE(vkResult);
	const BYTE shiftState = HIBYTE(vkResult);

	if (shiftState & 1) {
		sendKey(VK_SHIFT, true);
	}

	sendKey(vkCode, true);
	Sleep(randomInteger(50, 100));
	sendKey(vkCode, false);

	if (shiftState & 1) {
		sendKey(VK_SHIFT, false);
	}
}

std::vector<std::string> NextElements(std::ifstream* file, std::size_t N /* amount of elements to get*/) {
		if (!file->is_open()) {
			return std::vector<std::string>{};
		}

		std::vector<std::string> elements{};
		elements.reserve(N);

		
		std::string word;
		while (elements.size() < N && *file >> word) {
			elements.emplace_back(std::move(word));
		}

		return elements;
}

int CumStrSize(std::vector<std::string> vec) {
	return std::accumulate(
		vec.begin(),
		vec.end(),
		size_t{ 0 },
		[](size_t sum, const std::string& str) {
			return sum + str.size();
		}
	);
}

void KeyboardDeleteAll() {
	sendKey(VK_CONTROL, true);
	sendKey('A', true);
	Sleep(50);
	sendKey('A', false);
	sendKey(VK_CONTROL, false);

	Sleep(25);

	sendKey(VK_BACK, true);  
	Sleep(50);
	sendKey(VK_BACK, false);   
}

void Countdown(int n) {
	for (int i = n; i > 0; --i) {
		std::println("{}", i);
		Sleep(1000L);
	} 

}

int main(int argc, char* argv[]) {
	if (argc <= 1) {
		std::println("Please provide the file path as an argument");
		Sleep(1000L);
		return 1;
	}
	std::ifstream file(argv[1]);
	if (!file.is_open()) {
		std::cerr << "Error opening file!" << std::endl;
		file.close();
		Sleep(1000L);
		return 1;
	}

	constexpr int WPM = 25;
	constexpr int SPREAD_TIME_MS = 250;

	Countdown(5);
	while (true) {
		std::vector<std::string> nextElements = NextElements(&file, WPM);

		const int timeBetweenInputsMs = static_cast<int>((60.0 / CumStrSize(nextElements)) * 1000.0);

		for (std::string const& str : nextElements) {
			for (char c : str) {
				Sleep(randomIntgerSpread(timeBetweenInputsMs, SPREAD_TIME_MS));
				sendSmartInput(c);
			}
			Sleep(randomInteger(50, 100));
			sendSmartInput(VK_SPACE);
		}

		if (file.eof()) {
			file.clear();
			file.seekg(0, std::ios::beg);
			std::println("Reached EOF! Restarting...");
			KeyboardDeleteAll();
		}
	}

	file.close();
	return 0;
}
