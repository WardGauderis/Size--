//============================================================================
// @name        : repair.h
// @author      : Thomas Dooms
// @date        : 11/19/19
// @version     :
// @copyright   : BA1 Informatica - Thomas Dooms - University of Antwerp
// @description :
//============================================================================

namespace algorithm::repair
{
    using Pair = uint64_t;

    namespace dot
    {
        bool less(const std::pair<uint64_t, uint32_t>& lhs, const std::pair<uint64_t, uint32_t>& rhs) noexcept
        {
            return lhs.second < rhs.second;
        }
        bool greater(const std::pair<uint64_t, uint32_t>& lhs, const std::pair<uint64_t, uint32_t>& rhs) noexcept
        {
            return lhs.second > rhs.second;
        }

        class queue
        {
        public:
            using iterator = std::vector<std::pair<Pair, uint32_t>>::iterator;
            using reverse_iterator = std::vector<std::pair<Pair, uint32_t>>::reverse_iterator;
            using pointer  = std::vector<std::pair<Pair, uint32_t>>::pointer;

            // I haven't yet found a case where 200 is a bad guess.
            explicit queue(size_t amount) : low(low_index), high()
            {
                high.reserve(amount / 1000);

                std::fill(increase_cache.begin(), increase_cache.end(), nullptr);
                std::fill(decrease_cache.begin(), decrease_cache.end(), nullptr);
            }

            void insert(Pair pair)
            {
                low[1].emplace(pair, false);
            }

            [[nodiscard]] bool increase(Pair pair, uint32_t num)
            {
                if(high_index != 0 and num > high[high_index - 1].second) return false;

                if(num >= low.size())
                {
                    const auto cache_iter = std::find_if(increase_cache.begin(), increase_cache.end(), [&](auto ptr){ return ptr != nullptr and ptr->first == pair; });

                    iterator iter;
                    iterator upper;

                    if(cache_iter != increase_cache.end() and (*cache_iter)->first == pair)
                    {
                        iter = iterator(*cache_iter);
                        upper = std::find_if(std::reverse_iterator(iter), high.rend(), [&](auto elem){ return elem.second != num; }).base();

                        (*cache_iter) = &*upper;
                    }
                    else
                    {
                        upper = std::lower_bound(high.begin() + high_index, high.end(), std::make_pair(pair, num), greater);
                        iter = std::find_if(upper, high.end(), [&](const auto& elem){ return elem.first == pair; });

                        increase_cache[increase_index] = &*upper;
                        increase_index = (increase_index + 1) % increase_size;
                    }

                    iter->second++;
                    std::swap(*iter, *upper);
                }
                else
                {
                    // update lowest index to highest inserted value
                    low_index = std::max(low_index, static_cast<size_t>(num + 1));
                    // we actually dont need to erase, so we dont
                    low[num].erase(pair);
                    if (num + 1 < low.size())
                    {
                        low[num + 1].emplace(pair, false);
                    }
                    else
                    {
                        const auto cap = high.capacity();
                        high.emplace_back(pair, num + 1);
                        if(cap != high.capacity())
                        {
                            //reset caches
                            increase_index = 0;
                            decrease_index = 0;
                            std::fill(increase_cache.begin(), increase_cache.end(), nullptr);
                            std::fill(increase_cache.begin(), increase_cache.end(), nullptr);
                        }
                    }
                }
                return true;
            }

            [[nodiscard]] bool decrease(Pair pair, uint32_t num)
            {
                if(high_index != 0 and num > high[high_index - 1].second) return false;
                if(num == 0) return false;

                if(num >= low.size())
                {
                    const auto cache_iter = std::find_if(decrease_cache.begin(), decrease_cache.end(), [&](auto ptr){ return ptr != nullptr and ptr->first == pair; });

                    iterator iter;
                    iterator lower;

                    if(cache_iter != decrease_cache.end() and (*cache_iter)->first == pair)
                    {
                        iter = iterator(*cache_iter);
                        lower = std::find_if(iter, high.end(), [&](auto elem){ return elem.second != num; }) - 1;

                        (*cache_iter) = &*lower;
                    }
                    else
                    {
                        lower = std::lower_bound(high.rbegin(), high.rend(), std::make_pair(pair, num), less).base() - 1;
                        iter = std::find_if(reverse_iterator(lower)-1, high.rend(), [&](const auto& elem){ return elem.first == pair; }).base() - 1;

                        decrease_cache[decrease_index] = &*lower;
                        decrease_index = (decrease_index + 1) % decrease_size;
                    }

                    iter->second--;
                    std::swap(*iter, *lower);

                    if(num == low.size())
                    {
                        high.pop_back();
                        low.back().emplace(pair, false);
                    }
                }
                else
                {
                    // we actually dont need to erase, so we dont
                    low[num].erase(pair);
                    low[num - 1].emplace(pair, false);
                }
                return true;
            }

            std::pair<Pair, uint32_t> pop() noexcept
            {
                if(high_index < high.size()) return high[high_index++];
                for(auto i = low_index; i != 0; i--)
                {
                    if(low[i - 1].empty()) continue;
                    const auto copy = low[i - 1].begin()->first;
                    low[i - 1].erase( low[i - 1].begin() );
                    //update low index to lowest found
                    low_index = i;
                    return std::make_pair(copy, i - 1);
                }
                // has never happened before.
                return std::make_pair(0,0);
            }

            void clear_unused(size_t min_occurrences)
            {
                for(size_t i = 0; i < min_occurrences; i++)
                {
                    // reset map
                    low[i] = robin_hood::unordered_map<Pair, bool>();
                }
            }

        private:
            size_t high_index = 0;
            size_t low_index = 200;

            std::vector<robin_hood::unordered_map<Pair, bool>> low;
            std::vector<std::pair<Pair, uint32_t>> high;

            // this seems to be a good guess
            constexpr static inline size_t increase_size = 8;
            constexpr static inline size_t decrease_size = 8;

            uint8_t increase_index = 0;
            std::array<pointer, increase_size> increase_cache {};

            uint8_t decrease_index = 0;
            std::array<pointer, decrease_size> decrease_cache {};
        };

        using map = robin_hood::unordered_map<Pair, std::pair<std::vector<Variable>, uint32_t>>;

        class maxmap
        {
        public:
            explicit maxmap(size_t amount, size_t min_occurrences, double clear_factor, Mode mode) : queue(amount), min_occurrences(min_occurrences), clear_factor(clear_factor)
            {
                // very scientific
                if(mode == Mode::memory_efficient) map = dot::map(amount / 20);
                else if(mode == Mode::average) map = dot::map(amount / 12);
                else if(mode == Mode::fast) map = dot::map(amount / 7);
                else throw;
            }

            template<typename Func>
            void construct(const std::vector<Variable>& string, Func&& compose)
            {
                Pair last_pair = std::numeric_limits<Pair>::max();
                for(size_t i = 0; i < string.size() - 1; i++)
                {
                    const auto pair = compose(string[i], string[i + 1]);

                    // skip when we have a string of the same chars
                    if(pair == last_pair)
                    {
                        last_pair = std::numeric_limits<Pair>::max();
                        continue;
                    }
                    else last_pair = pair;
                    emplace(pair, static_cast<uint32_t>(i));
                }
            }

            void emplace(Pair pair, uint32_t index)
            {
                const auto iter = map.find(pair);

                if(iter == map.end())
                {
                    unused_count++;
                    map.emplace(pair, std::make_pair(std::vector<uint32_t>{index}, 1));
                    queue.insert(pair);
                }
                else
                {
                    iter->second.first.push_back(index);
                    const auto increased = queue.increase(iter->first, iter->second.second);
                    if(increased) iter->second.second += 1;
                }
            }

            auto pop() noexcept
            {
                // find element in map
                const auto top = queue.pop();
                const auto iter = map.find(top.first);

                // move the element from the map
                const auto result = *std::make_move_iterator(iter);
                map.erase(iter);
                return result;
            }

            void decrease(Pair pair, Pair curr) noexcept
            {
                if(pair == curr) return;
                const auto iter = map.find(pair);
                if(iter == map.end()) return; // means we cleared the unused

                const auto decreased = queue.decrease(iter->first, iter->second.second);
                if(decreased)
                {
                    if(iter->second.second == min_occurrences) unused_count--;
                    iter->second.second -= 1;
                }
            }

            void try_clear_unused()
            {
                if(double(unused_count) / double(map.size()) < 1 - clear_factor) return;
                unused_count = 0;

                for(auto iter = map.begin(); iter != map.end(); ++iter)
                {
                    if(iter->second.second < min_occurrences) iter = map.erase(iter);
                    if(iter == map.end()) break;
                }
                queue.clear_unused(min_occurrences);
            }

            auto empty() const noexcept
            {
                return map.empty();
            }

        private:
            dot::map map;
            dot::queue queue;

            size_t min_occurrences;
            double clear_factor;

            size_t unused_count = 0;
        };
    }



    std::tuple<Settings, std::vector<Variable>, std::vector<Production>> compress(std::vector<Variable>&& string, Mode mode)
    {
        // all the lambda functions
        constexpr auto compose   = [](Variable lhs, Variable rhs) -> Pair { return (static_cast<Pair>(lhs) << 32u) + rhs; };
        constexpr auto decompose = [](Pair elem) { return std::array{static_cast<Variable>(elem >> 32u), static_cast<Variable>(elem)}; };
        constexpr auto not_tombstone = [](const auto elem){ return elem != Settings::end(); };

        constexpr double clear_factor = 0.80;                   // if 1/5 is unused, run the clear
        constexpr size_t min_occurrences = 8;                   // magic stuff
        constexpr size_t production_guess = 100;                // safe guess, doesn't matter much
        constexpr size_t min_memory_efficient_size = 50000000;  // 50MB

        if(mode == Mode::none_specified) mode = Mode::memory_efficient;
        if(mode == Mode::fast and string.size() > min_memory_efficient_size) mode = Mode::memory_efficient;

        Settings settings;
        dot::maxmap map(string.size(), min_occurrences, clear_factor, mode);
        std::vector<Production> productions;

        productions.reserve(string.size() / production_guess);

        size_t production_counter = 0;

        map.construct(string, compose);

        while(true)
        {
            if(map.empty()) break;

            const auto [key, value] = map.pop();
            if(value.second <= min_occurrences) break;

            const auto pair = decompose(key);

            // determine new value for production
            uint32_t new_value;
            if(Settings::is_reserved_rule(pair[0], pair[1]))
            {
                new_value = Settings::convert_to_reserved(pair[0], pair[1]);
            }
            else
            {
                new_value = settings.offset(production_counter);
                productions.emplace_back(pair);
                production_counter++;
            }

            // loop over all occurrences
            for(auto index : value.first)
            {
                const auto curr = string.begin() + index;

                // means we already changed the value to another production
                if(*curr != pair[0]) continue;

                const auto second = std::find_if(curr + 1, string.end(), not_tombstone);

                // means we already changed the value to another production
                if(second == string.end() or *second != pair[1]) continue;

                const auto next = std::find_if(second + 1, string.end(), not_tombstone);
                const auto prev = std::find_if(std::reverse_iterator(curr), string.rend(), not_tombstone);

                if(next != string.end()) map.decrease(compose(*second, *next), key);
                if(prev != string.rend()) map.decrease(compose(*prev, *curr), key);

                *curr = new_value;
                if(second != string.end()) *second = Settings::end();

                if(next != string.end() ) map.emplace(compose(*curr, *next), index);
                if(prev != string.rend()) map.emplace(compose(*prev, *curr), prev.base() - string.begin() - 1);
            }

            if(mode != Mode::fast) map.try_clear_unused();
        }

        // move away tombstones
        const auto end = std::copy_if(string.begin(), string.end(), string.begin(), not_tombstone);
        string.resize(end - string.begin());

        return std::make_tuple(settings, std::move(string), std::move(productions));
    }
}