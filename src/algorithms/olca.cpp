//
// Created by ward on 12/28/19.
//

#include <cmath>
#include "olca.h"

algorithm::olca::OLCA::OLCA() {
	revDict.reserve(256);
	productions.reserve(256);
}

std::tuple<Settings, std::vector<Variable>, std::vector<Production>>
algorithm::olca::OLCA::run(pal::OnlineHelper helper) {

	return std::tuple<Settings, std::vector<Variable>, std::vector<Production>>();
}

bool algorithm::olca::OLCA::isRepetitive(const std::deque<Variable>& w, const size_t i) {
	return w[i] == w[i + 1];
}

bool algorithm::olca::OLCA::isMinimal(const std::deque<Variable>& w, const size_t i) {
	return w[i] < w[i - 1] && w[i] < w[i + 1];
}

Variable algorithm::olca::OLCA::lca(Variable x1, Variable x2) {
	Variable n1, n2, x;
	n1 = (x1 << 1) - 1;
	n2 = (x2 << 1) - 1;
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

bool algorithm::olca::OLCA::replacePair(const std::deque<Variable>& w, const size_t i) {
	if (isRepetitive(w, i)) return true;
	else if (isRepetitive(w, i + 1)) return false;
	else if (isRepetitive(w, i + 2)) return true;
	else if (isMinimal(w, i) || isMaximal(w, i)) return true;
	else if (isMinimal(w, i + 1) || isMaximal(w, i + 1)) return false;
	return true;
}

void algorithm::olca::OLCA::insertVariable(size_t index, Variable x) {
	if (buffers.size() <= index) {
		buffers.resize(index + 1);
		buffers[index].push_back(UINT32_MAX);
	}
	auto& q = buffers[index];

	q.push_back(x);
	if (q.size() < 5) return;
	if (replacePair(q, 1)) {
		q.pop_front();
		const auto y1 = q.front();
		q.pop_front();
		const auto y2 = q.front();
		const auto z = getVariable(y1, y2);
	}
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

std::tuple<Settings, std::vector<Variable>, std::vector<Production>>
algorithm::olca::compress(const std::filesystem::path& input) {
	pal::OnlineHelper helper(input, "ok");
	OLCA olca;
	return olca.run(helper);
}
