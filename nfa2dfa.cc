#include "nfa2dfa.h"

#include "char_type.h"
#include "test_utils.h"

auto print_dfa_node(std::set<std::size_t> const& set) -> auto& {
  std::cout << '{';
  for (
    auto [i, it] = std::make_tuple(std::size_t{}, set.begin());
    it != set.end();
    ++it, i++
  ) {
    std::cout << *it;
    if (i < set.size() - 1) {
      std::cout << ", ";
    }
  }
  return std::cout << '}';
}

template<typename char_t>
auto print_dfa_path(std::map<char_t, std::set<std::size_t>> const& map) {
  for (auto const [k, v]: map) {
    std::cout << k << "->";
    print_dfa_node(v) << std::endl;
  }
}

template<typename char_t>
auto print_dfa_path(std::map<char_t, std::size_t> const& map) {
  for (auto const [k, v]: map) {
    std::cout << k << "->" << v << std::endl;
  }
}

auto print_rows(auto const& dfa) {
  for (auto const& [k, v]: dfa) {
    print_dfa_node(k) << ':' << std::endl;
    print_dfa_path(v);
  }
}

auto print_reindexed(auto const& dfa) {
  for (std::size_t i = 0; i < dfa.matrix.size(); i++) {
    std::cout << i << ':' << std::endl;
    print_dfa_path(dfa.matrix[i]);
  }
}

int main() {
  {
    auto regex = std::make_unique<character>('a');
    auto nfa = regex->to_nfa();
    print_nfa(nfa);
    print_rows(nfa2dfa{nfa}());
  }

  std::cout << std::endl;

  {
    auto regex = std::make_unique<alternation>(
      std::make_unique<character>('a'),
      std::make_unique<character>('b')
    );
    auto nfa = regex->to_nfa();
    print_nfa(nfa);
    print_rows(nfa2dfa{nfa}());
  }

  std::cout << std::endl;

  {
    auto regex = std::make_unique<concatation>(
      std::make_unique<character>('a'),
      std::make_unique<character>('b')
    );
    auto nfa = regex->to_nfa();
    print_nfa(nfa);
    print_rows(nfa2dfa{nfa}());
  }

  std::cout << std::endl;

  {
    auto regex = std::make_unique<kleene>(
      std::make_unique<character>('a')
    );
    auto nfa = regex->to_nfa();
    print_nfa(nfa);
    auto dfa = nfa2dfa{nfa}();
    print_rows(dfa);
    auto reindexed = ::dfa_nodes<char_type>::reindexed(dfa);
    print_reindexed(reindexed);
  }
}
