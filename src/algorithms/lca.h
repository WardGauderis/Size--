//
// Created by ward on 12/28/19.
//

#ifndef SIZE_LCA_H
#define SIZE_LCA_H

namespace algorithm::lca {

	inline bool isRepetitive(const std::vector<Variable>& w, const size_t i) {
		return w[i] == w[i + 1];
	}

	inline bool isMinimal(const std::vector<Variable>& w, const size_t i) {
		return w[i] < w[i - 1] && w[i] < w[i + 1];
	}

	inline Variable lca(Variable x1, Variable x2) {
		Variable n1, n2, x;
		n1 = (x1 << 1u) - 1;
		n2 = (x2 << 1u) - 1;
		x = n1 ^ n2;
		return std::floor(std::log2(x));
	}

	inline bool isMaximal(const std::vector<Variable>& w, const size_t i) {
		if ((w[i - 1] > w[i] || w[i] > w[i + 1] || w[i + 1] > w[i + 2]) &&
		    (w[i - 1] < w[i] || w[i] < w[i + 1] || w[i + 1] < w[i + 2]))
			return false;
		auto tmp = lca(w[i], w[i + 1]);
		return tmp > lca(w[i - 1], w[i]) && tmp > lca(w[i + 1], w[i + 2]);
	}

	inline bool isPair(const std::vector<Variable>& w, size_t i) {
		if (isRepetitive(w, i)) return true;
		else if (isRepetitive(w, i + 1)) return false;
		else if (isRepetitive(w, i + 2)) return true;
		else if (isMinimal(w, i) || isMaximal(w, i)) return true;
		else return (!isMinimal(w, i + 1) && !isMaximal(w, i + 1));
	}

	Variable
	getVariable(std::vector<Production>& productions, const Settings& settings,
	            robin_hood::unordered_flat_map<Production, Variable>& revDict, Variable a, Variable b) {
		static uint32_t offset = 0;
		const auto pair = revDict.emplace(Production{a, b}, settings.offset(offset));
		if (pair.second) {
			++offset;
			productions.emplace_back(pair.first->first);
		}
		return pair.first->second;
	}

	std::tuple<Settings, std::vector<Variable>, std::vector<Production>>
	compress(std::vector<Variable>&& variables) {
		const Settings settings(Settings::Flags::lca_encoding);

		std::vector<Production> productions;
		robin_hood::unordered_flat_map<Production, Variable> revDict;

		while (variables.size() != 1) {
			size_t oldPos = 0;
			size_t newPos = 0;
			while (oldPos < variables.size() - 2) {
				if (isPair(variables, oldPos)) {
					variables[newPos] = getVariable(productions, settings, revDict, variables[oldPos], variables[oldPos + 1]);
					oldPos += 2;
				} else {
					variables[newPos] = variables[oldPos];
					variables[++newPos] = getVariable(productions, settings, revDict, variables[oldPos + 1],
					                                  variables[oldPos + 2]);
					oldPos += 3;
				}
				++newPos;
			}

			if(oldPos == variables.size() - 2){
				variables[newPos++] = getVariable(productions, settings, revDict, variables[oldPos], variables[oldPos + 1]);
			} else if (oldPos == variables.size() - 1){
				variables[newPos++] = variables[oldPos];
			}

			variables.resize(newPos);
		}

		return std::make_tuple(settings, std::move(variables), std::move(productions));
	}

}

#endif //SIZE_LCA_H
