//============================================================================
// @name        : timer.h
// @author      : Thomas Dooms
// @date        : 11/23/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================


#pragma once

#include <chrono>
#include <iostream>

namespace Utils {

	class Timer {
	public:
		static void start(std::string string = "") {
			str = std::move(string);
			last = std::chrono::system_clock::now();
			started = true;
		}

		template<typename T = std::chrono::milliseconds>
		static void end() {
			if (not started) throw std::runtime_error("clock not started");
			started = false;

			const auto now = std::chrono::system_clock::now();

			if (not str.empty()) std::cout << str << ": ";
			std::cout << std::chrono::duration_cast<T>(now - last).count();

			if constexpr (std::is_same_v<T, std::chrono::nanoseconds>) std::cout << " nanoseconds\n";
			else if constexpr (std::is_same_v<T, std::chrono::microseconds>) std::cout << " microseconds\n";
			else if constexpr (std::is_same_v<T, std::chrono::milliseconds>) std::cout << " milliseconds\n";
			else if constexpr (std::is_same_v<T, std::chrono::seconds>) std::cout << " seconds\n";
			else static_assert(std::is_same_v<T, void**>, "wrong template argument");
		}

	private:
		static inline std::chrono::time_point<std::chrono::system_clock> last;
		static inline std::string str;
		static inline bool started = false;
	};

}