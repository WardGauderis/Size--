//
// Created by ward on 12/28/19.
//

#ifndef SIZE_OLCA_H
#define SIZE_OLCA_H

#include <deque>
#include <filesystem>
#include "../util/variable.h"
#include "../util/production.h"
#include "../util/robin_hood.h"
#include "../util/settings.h"
#include "../pal/onlineHelper.h"

namespace algorithm::olca {

	class OLCA {
	public:
		OLCA();

		std::tuple<Settings, std::vector<Variable>, std::vector<Production>> run(pal::OnlineHelper helper);

	private:
		robin_hood::unordered_flat_map<Production, Variable> revDict;
		std::vector<Production> productions;
		std::vector<std::deque<Variable>> buffers;
		Settings settings;

		static bool isRepetitive(const std::deque<Variable>& w, size_t i);

		static bool isMinimal(const std::deque<Variable>& w, size_t i);

		static Variable lca(Variable x1, Variable x2);

		static bool isMaximal(const std::deque<Variable>& w, size_t i);

		static bool replacePair(const std::deque<Variable>& w, size_t i);

		Variable getVariable(Variable a, Variable b);

		void insertVariable(size_t index, Variable x);

	};

	std::tuple<Settings, std::vector<Variable>, std::vector<Production>>
	compress(const std::filesystem::path &input);

}


#endif //SIZE_OLCA_H
