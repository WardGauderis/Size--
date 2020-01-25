//============================================================================
// @name        : none.h
// @author      : Thomas Dooms
// @date        : 11/16/19
// @version     : 
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description : 
//============================================================================


#pragma once

#include <vector>
#include <algorithm>
#include <tuple>

#include "../util/variable.h"
#include "../util/production.h"
#include "../util/settings.h"
#include "../util/robin_hood.h"

template<uint8_t MAX>
class Counter {
public:
	void operator++() {
		if (count != MAX) ++count;
	}

	bool operator()() const {
		return count == MAX;
	}

private:
	uint8_t count = 0;
};

namespace algorithm::bisection {

	std::tuple<Settings, std::vector<Variable>, std::vector<Production>>
	compress(std::vector<Variable>&& variables, bool odd) {
		const auto settings = Settings();
		std::vector<Production> productions;
		productions.reserve(variables.size() / 2);

		std::for_each(variables.begin(), variables.end() - 1, [](auto& elem) { elem += 256; });
		if (not odd) variables.back() += 256;

		bool startReached = false;
		robin_hood::unordered_flat_map<Production, Variable> map;
		map.reserve(variables.size() / 4);

		uint32_t offset = 0;
		size_t size = variables.size();

		while (not startReached) {
			bool uneven = size % 2 != 0;

			for (size_t i = 0; i < size / 2; ++i) {
				const auto pair = map.emplace(Production{variables[2 * i], variables[2 * i + 1]},
				                              settings.offset(offset));
				if (pair.second) {
					++offset;
					productions.emplace_back(pair.first->first);
				}

				variables[i] = pair.first->second;
			}
			if (uneven) variables[(size / 2)] = variables[size - 1];

			size = (size / 2) + uneven;

			startReached = size == 1;
			map.clear();
		}

		variables.resize(size);

		return std::make_tuple(settings, std::move(variables), std::move(productions));
	}

}

namespace algorithm::bisectionPlusPlus {

	std::tuple<Settings, std::vector<Variable>, std::vector<Production>>
	compress(std::vector<Variable>&& variables, bool odd) {
		const auto settings = Settings();

		std::vector<Production> productions;
		robin_hood::unordered_flat_map<Production, Variable> map;
		robin_hood::unordered_flat_map<Production, Counter<4>> counters;

		std::for_each(variables.begin(), variables.end() - 1, [](auto& elem) { elem += 256; });
		if (not odd) variables.back() += 256;

		uint32_t offset = 0;
		uint32_t previousLevelBegin = 0;
		uint32_t levelBegin = settings.begin();
		size_t size = variables.size();

		productions.reserve(size / 256);
		map.reserve(size / 256);
		counters.reserve(size / 8);

		while (levelBegin != previousLevelBegin) {
			size_t index = 0;

			for (size_t i = 0; i < size / 2; ++i) {
				const auto& var1 = variables[2 * i];
				const auto& var2 = variables[2 * i + 1];

				if (var1 < previousLevelBegin || var2 < previousLevelBegin) continue;

				const auto pair = counters.emplace(Production{var1, var2}, Counter<4>());
				if (!pair.second) {
					++((*pair.first).second);
				}
			}

			for (size_t i = 0; i < size / 2; ++i) {
				const auto& var1 = variables[2 * i];
				const auto& var2 = variables[2 * i + 1];

				// enkel variabelen van vorige level die kunnen herhaald worden
				if (var1 < previousLevelBegin || var2 < previousLevelBegin || !counters[{var1, var2}]()) {
					variables[index++] = var1;
					variables[index++] = var2;
					continue;
				}

				const auto pair = map.emplace(Production{var1, var2}, settings.offset(offset));
				if (pair.second) {
					++offset;
					productions.emplace_back((*pair.first).first);
				}

				variables[index++] = pair.first->second;
			}
			if (size % 2 == 1) variables[index++] = variables[size - 1];

			if (counters.load_factor() > 0.25f) {
				counters.clear();
			}

			size = index;
			previousLevelBegin = levelBegin;
			levelBegin = settings.offset(offset);
		}

//		std::cout << "productions: " << variables.size() / 256.0 / productions.size() << std::endl;
//		std::cout << "map: " << variables.size() / 256.0 / map.size() << std::endl;
//		std::cout << "counters: " << variables.size() / 8.0 / counters.size() << std::endl;

		variables.resize(size);

		return std::make_tuple(settings, std::move(variables), std::move(productions));
	}

}