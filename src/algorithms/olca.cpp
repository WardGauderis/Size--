//
// Created by ward on 12/28/19.
//

#include <cmath>
#include "olca.h"

algorithm::olca::OLCA::OLCA(OnlineReader& reader) {
	revDict.reserve(reader.getSize() / 256);
	productions.reserve(reader.getSize() / 256);
}

std::tuple<Settings, std::vector<Variable>, std::vector<Production>>
algorithm::olca::OLCA::run(OnlineReader& reader, bool verbose) {
	auto size = reader.getSize();
	if(verbose) std::cout << size << " bytes total" << std::endl;
	for (size_t i = 0; i < size; ++i) {
		insertVariable(0, reader.readVariable());
		if (verbose) {
			printf("\r");
			printf("%ld bytes -> %zu rules", i+1, productions.size());
			fflush(stdout);
		}
	}
	if(verbose) std::cout << std::endl;
	for (size_t i = 0; i < buffers.size(); ++i) {
		cleanupVariable(i);
	}

	return std::make_tuple(settings, std::vector<Variable>{buffers.back()[1]}, std::move(productions));
}

bool algorithm::olca::OLCA::isRepetitive(const std::deque<Variable>& w, const size_t i) {
	return w[i] == w[i + 1];
}

bool algorithm::olca::OLCA::isMinimal(const std::deque<Variable>& w, const size_t i) {
	return w[i] < w[i - 1] && w[i] < w[i + 1];
}

Variable algorithm::olca::OLCA::lca(Variable x1, Variable x2) {
	Variable n1, n2, x;
	n1 = (x1 << 1u) - 1;
	n2 = (x2 << 1u) - 1;
	x = n1 ^ n2;
	return std::floor(std::log2(x));
}

bool algorithm::olca::OLCA::isMaximal(const std::deque<Variable>& w, const size_t i) {
	if ((w[i - 1] > w[i] || w[i] > w[i + 1] || w[i + 1] > w[i + 2]) &&
	    (w[i - 1] < w[i] || w[i] < w[i + 1] || w[i + 1] < w[i + 2]))
		return false;
	auto tmp = lca(w[i], w[i + 1]);
	return tmp > lca(w[i - 1], w[i]) && tmp > lca(w[i + 1], w[i + 2]);
}

bool algorithm::olca::OLCA::isPair(const std::deque<Variable>& w, size_t i) {
	if (isRepetitive(w, i)) return true;
	else if (isRepetitive(w, i + 1)) return false;
	else if (isRepetitive(w, i + 2)) return true;
	else if (isMinimal(w, i) || isMaximal(w, i)) return true;
	else return (!isMinimal(w, i + 1) && !isMaximal(w, i + 1));
}

void algorithm::olca::OLCA::insertVariable(size_t index, Variable x) {
	if (buffers.size() <= index) {
		buffers.resize(index + 1);
		buffers[index].push_back(UINT32_MAX);
	}

	buffers[index].push_back(x);
	if (buffers[index].size() < 5) return;

	if (!isPair(buffers[index], 1)) {
		buffers[index].pop_front();
		insertVariable(index + 1, buffers[index].front());
	}

	buffers[index].pop_front();
	const auto y1 = buffers[index].front();
	buffers[index].pop_front();
	const auto y2 = buffers[index].front();
	const auto z = getVariable(y1, y2);
	insertVariable(index + 1, z);
}

Variable algorithm::olca::OLCA::getVariable(Variable a, Variable b) {
	if (Settings::is_reserved_rule(a, b)) return Settings::convert_to_reserved(a, b);


	static uint32_t offset = 0;
	const auto pair = revDict.emplace(Production{a, b}, settings.offset(offset));
	if (pair.second) {
		++offset;
		productions.emplace_back(pair.first->first);
	}
	return pair.first->second;
}

void algorithm::olca::OLCA::cleanupVariable(size_t index) {
	if (buffers[index].size() == 2) {
		if (index == buffers.size() - 1) start = buffers[index][1];
		else insertVariable(index + 1, buffers[index][1]);
	} else if (buffers[index].size() == 3) insertVariable(index + 1, getVariable(buffers[index][1], buffers[index][2]));
	else if (buffers[index].size() == 4) {
		if (isPair(buffers[index], 1)) {
			insertVariable(index + 1, getVariable(buffers[index][1], buffers[index][2]));
			insertVariable(index + 1, buffers[index][3]);
		} else {
			insertVariable(index + 1, buffers[index][1]);
			insertVariable(index + 1, getVariable(buffers[index][2], buffers[index][3]));
		}
	}
}

std::tuple<Settings, std::vector<Variable>, std::vector<Production>>
algorithm::olca::compress(OnlineReader& reader, bool verbose) {
	OLCA olca(reader);
	return olca.run(reader, verbose);
}
