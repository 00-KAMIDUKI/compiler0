#pragma once

#include <vector>
#include <memory>
#include <map>
#include <set>
#include "char_type.h"


template <typename char_t=char_type>
struct nfa {
  constexpr static auto epsilon = char_t::epsilon;
  struct node {
    static constexpr auto adjacent_idx_null = static_cast<std::size_t>(-1);

    std::size_t adjacent_idx1{adjacent_idx_null};
    std::size_t adjacent_idx2{adjacent_idx_null};
    char_t edge1{};
    char_t edge2{};

    // will always modify edge2 if both edge have values
    void add_edge(char_t edge, std::size_t node_idx) {
      if (adjacent_idx1 == adjacent_idx_null) {
        adjacent_idx1 = node_idx;
        edge1 = edge;
      } else {
        adjacent_idx2 = node_idx;
        edge2 = edge;
      }
    }

    // assuming not null
    void move1(nfa const& nfa, std::map<char_t, std::set<std::size_t>>& res) const {
      if (edge1 != epsilon) {
        auto [it, inserted] = res.try_emplace(edge1);
        auto& [k, set] = *it;
        set.emplace(adjacent_idx1);
        return;
      }
      auto move = nfa.nodes[adjacent_idx1];
      if (move.adjacent_idx1 != adjacent_idx_null) {
        move.move1(nfa, res);
      }
      if (move.adjacent_idx2 != adjacent_idx_null) {
        move.move2(nfa, res);
      }
    }

    void move2(nfa const& nfa, std::map<char_t, std::set<std::size_t>>& res) const {
      if (edge2 != epsilon) {
        auto [it, inserted] = res.try_emplace(edge2);
        auto& [k, set] = *it;
        set.emplace(adjacent_idx1);
        return;
      }
      auto move = nfa.nodes[adjacent_idx2];
      if (move.adjacent_idx1 != adjacent_idx_null) {
        move.move1(nfa, res);
      }
      if (move.adjacent_idx2 != adjacent_idx_null) {
        move.move2(nfa, res);
      }
    }

    auto move(nfa const& nfa, std::map<char_t, std::set<std::size_t>>& res) const {
      if (adjacent_idx1 != adjacent_idx_null) {
        move1(nfa, res);
      }
      if (adjacent_idx2 != adjacent_idx_null) {
        move2(nfa, res);
      }
    }

    auto move(nfa const& nfa) const {
      std::map<char_t, std::set<std::size_t>> res;
      if (adjacent_idx1 != adjacent_idx_null) {
        move1(nfa, res);
      }
      if (adjacent_idx2 != adjacent_idx_null) {
        move2(nfa, res);
      }
      return res;
    }

    auto get_epsilon_closure(nfa const& nfa, std::set<std::size_t>& res) const -> void {
      if (adjacent_idx1 != adjacent_idx_null && edge1 == epsilon) {
        res.emplace(adjacent_idx1);
        nfa.nodes[adjacent_idx1].get_epsilon_closure(nfa, res);
      }
      if (adjacent_idx2 != adjacent_idx_null && edge2 == epsilon) {
        res.emplace(adjacent_idx2);
        nfa.nodes[adjacent_idx2].get_epsilon_closure(nfa, res);
      }
    }
  };

  std::vector<node> nodes;
  constexpr static std::size_t initial_idx{0};
  constexpr static std::size_t final_idx{1}; 

  nfa() {
    nodes.emplace_back();
    nodes.emplace_back();
  }

  bool operator==(nfa const&) const = default;

  struct context {
    private:
      struct nfa& nfa;

    public:
      std::size_t initial_idx;
      std::size_t final_idx;

      context(struct nfa& nfa, std::size_t initial_idx, std::size_t final_idx) 
        : nfa{nfa}, initial_idx{initial_idx}, final_idx{final_idx} {}

      explicit context(struct nfa& nfa)
        : nfa{nfa}, initial_idx{nfa.initial_idx}, final_idx{nfa.final_idx} {}

      // reference will be invalidated when new nodes added.
      auto initial() const -> node& {
        return nfa.nodes[initial_idx];
      }

      // reference will be invalidated when new nodes added.
      auto final() const -> node const& {
        return nfa.nodes[final_idx];
      }

      auto add_node() const -> std::size_t {
        nfa.nodes.emplace_back();
        return nfa.nodes.size() - 1;
      }

      auto nodes() const -> std::vector<node>& {
        return nfa.nodes;
      }

      auto switch_to(std::size_t new_initial_idx, std::size_t new_final_idx) const {
        return context{this->nfa, new_initial_idx, new_final_idx};
      }
  };

  struct reg_expr {
    auto to_nfa() const -> nfa {
      struct nfa nfa{};
      this->to_nfa(context{nfa});
      return nfa;
    }
    virtual void to_nfa(context const& context) const = 0;
    virtual ~reg_expr() = default;
  };

  struct character: reg_expr {
    char_t ch;
    explicit character(char_t ch): ch{ch} {}
    void to_nfa(context const& context) const override {
      context.initial().add_edge(ch, context.final_idx);
    }
    using reg_expr::to_nfa;
  };

  struct alternation: reg_expr {
    std::unique_ptr<reg_expr> r;
    std::unique_ptr<reg_expr> s;

    alternation(std::unique_ptr<reg_expr>&& r, std::unique_ptr<reg_expr>&& s)
      : r{std::move(r)}, s{std::move(s)} {}

    void to_nfa(context const& context) const override {
      auto r_initial = context.add_node();
      auto r_final = context.add_node();
      auto s_initial = context.add_node();
      auto s_final = context.add_node();
      context.initial().add_edge(epsilon, r_initial);
      context.initial().add_edge(epsilon, s_initial);
      context.nodes()[r_final].add_edge(epsilon, context.final_idx);
      context.nodes()[s_final].add_edge(epsilon, context.final_idx);
      
      r->to_nfa(context.switch_to(r_initial, r_final));
      s->to_nfa(context.switch_to(s_initial, s_final));
    }

    using reg_expr::to_nfa;
  };

  struct concatation: reg_expr {
    std::unique_ptr<reg_expr> r;
    std::unique_ptr<reg_expr> s;

    concatation(std::unique_ptr<reg_expr>&& r, std::unique_ptr<reg_expr>&& s)
      : r{std::move(r)}, s{std::move(s)} {}

    void to_nfa(context const& context) const override {
      auto r_final = context.add_node();
      auto s_initial = context.add_node();
      context.nodes()[r_final].add_edge(epsilon, s_initial);
      
      r->to_nfa(context.switch_to(context.initial_idx, r_final));
      s->to_nfa(context.switch_to(s_initial, context.final_idx));
    }

    using reg_expr::to_nfa;
  };

  struct kleene: reg_expr {
    std::unique_ptr<reg_expr> r;

    kleene(std::unique_ptr<reg_expr>&& r): r{std::move(r)} {}

    void to_nfa(context const& context) const override {
      auto r_initial = context.add_node();
      auto r_final = context.add_node();
      context.initial().add_edge(epsilon, r_initial);
      context.initial().add_edge(epsilon, context.final_idx);
      context.nodes()[r_final].add_edge(epsilon, r_initial);
      context.nodes()[r_final].add_edge(epsilon, context.final_idx);
      r->to_nfa(context.switch_to(r_initial, r_final));
    }

    using reg_expr::to_nfa;
  };
};


