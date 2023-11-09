#include "char_type.h"
#include "nfa2dfa.h"
#include <memory>
#include <span>

#include "test_utils.h"

template <typename char_t=char_type>
struct dfa: dfa_nodes<char_t> {
  std::size_t current_state{0};

  bool accept_one(char_t ch) {
    auto it = this->matrix[current_state].find(ch);
    if (it == this->matrix[current_state].end()) {
      return false;
    }
    current_state = it->second;
    return true;
  }

  bool accept(std::span<const char_t> str) {
    for (auto ch: str) {
      auto successful = accept_one(ch);
      if (!successful) {
        return false;
      }
    }
    return this->acceptance.contains(current_state);
  }
};


int main() {
  // auto regex = std::make_unique<kleene>(
    // std::make_unique<character>('a')
  // );

  // \([ab]*\)
  auto regex = std::make_unique<concatation>(
    std::make_unique<character>('('),
    std::make_unique<concatation>(
      std::make_unique<kleene>(
        std::make_unique<alternation>(
          std::make_unique<character>('a'),
          std::make_unique<character>('b')
        )
      ),
      std::make_unique<character>(')')
    )
  );

  auto nfa = regex->to_nfa();
  auto dfa_raw = nfa2dfa{nfa}();
  auto reindexed = ::dfa_nodes<char_type>::reindexed(dfa_raw);
  auto dfa = ::dfa{reindexed};

  auto& str = "(aaabaaba)";
  std::cout << dfa.accept(std::span{reinterpret_cast<char_type const*>(str), std::size(str) - 1}) << std::endl;
}
