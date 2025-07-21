#include <windows.h>
#include <string>
#include <iostream>
#include <stack>
#include <vector>
#include <ranges>
#include <print>
#include <random>
#include <numeric>

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


std::vector<std::string> splitString(std::string& s, const char delimter = ' ') {
	std::vector<std::string> strs{};
	auto parts = s | std::views::split(delimter);

	for (const auto&& part : parts) {
		strs.emplace_back(part.begin(), part.end());
	}

	return strs;
}

std::vector<std::string> NextElements(std::vector<std::string>* in, std::size_t N /* amount of elements to get*/) {

		std::vector<std::string> next{};

		if (in->size() < N) {
			N = in->size();
		}

		next.reserve(N);

		for (int i = 0; i < N; ++i) {
			next.push_back(std::move(in->at(i)));
		}

		in->erase(in->begin(), in->begin() + N);

		return next;
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

int main() {
	std::string input = "Hello, World My name is OneCheetah and I am coding in c++ being we thought java was very STINKY and we hate it so we are using this bumass language int the worst IDE on gods green earth!!!!!!!!!!!!";
	std::vector<std::string> strs = splitString(input);
	constexpr int WPM = 30;
	constexpr int SPREAD_TIME_MS = 250;

	Sleep(2000);
	while (!strs.empty()) {
		std::vector<std::string> nextElements = NextElements(&strs, WPM);

		const int timeBetweenInputsMs = (60 / CumStrSize(nextElements)) * 1000;

		for (std::string const& str : nextElements) {
			for (char c : str) {
				//std::println("{} {}", str, timeBetweenInputsMs);
				Sleep(randomIntgerSpread(timeBetweenInputsMs, SPREAD_TIME_MS));
				sendSmartInput(c);
			}
			Sleep(randomIntgerSpread(timeBetweenInputsMs, 100));
			sendSmartInput(VK_SPACE);
		}
	}

	return 0;
}
