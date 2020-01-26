//
// Created by ward on 12/28/19.
//

#ifndef SIZE_OLCA_H
#define SIZE_OLCA_H

#include <deque>
#include "../util/production.h"
#include "../util/robin_hood.h"
#include "../util/settings.h"
#include "../pal/onlineReader.h"

namespace algorithm::olca {

	class OLCA {
	public:
		OLCA(OnlineReader& reader);

		std::tuple<Settings, std::vector<Variable>, std::vector<Production>> run(OnlineReader& reader);

	private:
		robin_hood::unordered_flat_map<Production, Variable> revDict;
		std::vector<Production> productions;
		std::vector<std::deque<Variable>> buffers;
		Settings settings;
		Variable start = UINT32_MAX;

		static inline bool isRepetitive(const std::deque<Variable>& w, size_t i);

		static inline bool isMinimal(const std::deque<Variable>& w, size_t i);

		static inline Variable lca(Variable x1, Variable x2);

		static inline bool isMaximal(const std::deque<Variable>& w, size_t i);

		static inline bool isPair(const std::deque<Variable>& w, size_t i);

		Variable getVariable(Variable a, Variable b);

		void insertVariable(size_t index, Variable x);

		void cleanupVariable(size_t index);

	};

	std::tuple<Settings, std::vector<Variable>, std::vector<Production>>
	compress(OnlineReader& reader);

}


#endif //SIZE_OLCA_H
