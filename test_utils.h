#pragma once

#include <iostream>
#include "nfa.h"

template <typename char_t>
void print_nfa_node(std::size_t idx, typename nfa<char_t>::node const& node) {
  if (node.adjacent_idx1 != node.adjacent_idx_null) {
    std::cout << idx << ", " << node.edge1 << "->" << node.adjacent_idx1 << std::endl;
  } 
  if (node.adjacent_idx2 != node.adjacent_idx_null) {
    std::cout << idx << ", " << node.edge2 << "->" << node.adjacent_idx2 << std::endl;
  } 
}

template <typename char_t>
void print_nfa(nfa<char_t> const &nfa) {
  std::cout << "nfa node count: " << nfa.nodes.size() << std::endl; 
  for (std::size_t i = 0; i < nfa.nodes.size(); i++) {
    print_nfa_node<char_t>(i, nfa.nodes[i]);
  }
}

using nfa_t = nfa<char_type>;
using reg_expr = nfa_t::reg_expr;
using character = nfa_t::character;
using alternation = nfa_t::alternation;
using concatation = nfa_t::concatation;
using kleene = nfa_t::kleene;
