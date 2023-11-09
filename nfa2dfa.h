#pragma once

#include <map>
#include <tuple>
#include "nfa.h"

template <typename char_t=char_type>
struct dfa_nodes {
  std::vector<std::map<char_t, std::size_t>> matrix;
  std::set<std::size_t> acceptance;

  static auto reindexed(std::map<std::set<std::size_t>, std::map<char_t, std::set<std::size_t>>> const& raw_dfa) {
    auto set_to_idx = std::map<std::set<std::size_t>, std::size_t>{};
    for (
      auto [idx, it] = std::make_tuple(std::size_t{}, raw_dfa.begin());
      it != raw_dfa.end();
      idx++, ++it
    ) {
      set_to_idx.emplace(it->first, idx);
    }
    dfa_nodes res{};
    for (auto const& [k, v]: raw_dfa) {
      auto new_v = std::map<char_t, std::size_t>{};
      for (auto const& [ch, out]: v) {
        new_v.emplace(ch, set_to_idx[out]);
      }
      res.matrix.emplace_back(std::move(new_v));
    }
    for (auto const& [k, v]: set_to_idx) {
      if (k.contains(1)) { // final state
        res.acceptance.emplace(v);
      } 
    }
    return res;
  }
};

template <typename char_t=char_type>
struct nfa2dfa {
  using nfa_t = ::nfa<char_t>;
  using nfa_node = typename nfa_t::node;
  using dfa_node_t = std::set<std::size_t>;
  using row_value_t = std::map<char_t, dfa_node_t>;

  constexpr static auto adjacent_idx_null = nfa_node::adjacent_idx_null;

  nfa_t nfa;

  auto operator()() const {
    std::map<dfa_node_t, row_value_t> rows;
    auto res = std::set{nfa.initial_idx};
    nfa.nodes[nfa.initial_idx].get_epsilon_closure(nfa, res);
    insert(res, rows);
    return rows;
  }

  auto epsilon_closure(std::set<std::size_t> const& move) const {
    auto res = move;
    for (auto nfa_node_idx: move) {
      auto& nfa_node = nfa.nodes[nfa_node_idx];
      nfa_node.get_epsilon_closure(nfa, res);
    }
    return res;
  }

  auto insert(dfa_node_t const& dfa_node, std::map<dfa_node_t, row_value_t>& rows) const -> void {
    row_value_t res;
    for (auto nfa_node_idx: dfa_node) {
      auto& nfa_node = nfa.nodes[nfa_node_idx];
      nfa_node.move(nfa, res);
    }

    for (auto& [ch, move]: res) {
      move = epsilon_closure(move);
    }

    auto [it, flag] = rows.emplace(dfa_node, std::move(res));

    auto& [k, move_vec] = *it;
    for (auto const& [ch, move]: move_vec) {
      if (move.empty()) {
        break;
      }
      if (!rows.contains(move)) {
        insert(move, rows);
      }
    }
  }
};


