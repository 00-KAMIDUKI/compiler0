#include "test_utils.h"

int main() {
  {
    auto regex = std::make_unique<character>('a');
    regex->to_nfa();
    print_nfa(regex->to_nfa());
  }

  std::cout << std::endl;

  {
    auto regex = std::make_unique<alternation>(
      std::make_unique<character>('a'),
      std::make_unique<character>('b')
    );
    print_nfa(regex->to_nfa());
  }

  std::cout << std::endl;

  {
    auto regex1 = std::make_unique<concatation>(
      std::make_unique<character>('a'),
      std::make_unique<character>('b')
    );
    print_nfa(regex1->to_nfa());
  }

  std::cout << std::endl;

  {
    auto regex1 = std::make_unique<kleene>(
      std::make_unique<character>('a')
    );
    print_nfa(regex1->to_nfa());
  }
}
