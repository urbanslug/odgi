#ifndef dgraph_node_hpp
#define dgraph_node_hpp

#include <iostream>
#include <cstdint>
#include <string>
#include <handlegraph/util.hpp>
#include <vector>
#include <cstring>
#include "varint.hpp"

namespace odgi {

using namespace handlegraph;
using nid_t = handlegraph::nid_t;
const uint8_t EDGE_RECORD_LENGTH = 2;
const uint8_t PATH_RECORD_LENGTH = 5;

/// A node object with the sequence, its edge lists, and paths
class node_t {
    std::vector<uint8_t> bytes{ 5, 0, 0, 0, 0 };
public:
    void clear(void) {
        bytes.clear();
        bytes = { 5, 0, 0, 0, 0 };
    }
    struct layout_t {
        uint64_t data[5] = { 0, 0, 0, 0, 0 };
        inline const uint64_t layout_bytes(void) const { return data[0]; }
        inline const uint64_t seq_start(void) const { return data[0]; }
        inline const uint64_t seq_bytes(void) const { return data[1]; }
        inline const uint64_t edge_start(void) const { return layout_bytes()+seq_bytes(); }
        inline const uint64_t edge_count(void) const { return data[2]; }
        inline const uint64_t edge_bytes(void) const { return data[3]; }
        inline const uint64_t path_start(void) const { return layout_bytes()+seq_bytes()+edge_bytes(); }
        inline const uint64_t path_count(void) const { return data[4]; }
        inline void set_layout_bytes(const uint64_t& i) { data[0] = i; }
        inline void set_seq_bytes(const uint64_t& i) { data[1] = i; }
        inline void set_edge_count(const uint64_t& i) { data[2] = i; }
        inline void set_edge_bytes(const uint64_t& i) { data[3] = i; }
        inline void set_path_count(const uint64_t& i) { data[4] = i; }
    };
    struct step_t {
        uint64_t data[5] = { 0, 0, 0, 0, 0 }; // PATH_RECORD_LENGTH
        step_t(void) { }
        step_t(const uint64_t& id,
               const bool& rev,
               const uint64_t& prev_id,
               const uint64_t& prev_rank,
               const uint64_t& next_id,
               const uint64_t& next_rank) {
            data[0] = pack_step(id, rev);
            data[1] = prev_id;
            data[2] = prev_rank;
            data[3] = next_id;
            data[4] = next_rank;
        }
        inline const uint64_t path_id(void) const { return step_path_id(data[0]); }
        inline const bool is_rev(void) const { return step_is_rev(data[0]); }
        inline const uint64_t prev_id(void) const { return data[1]; }
        inline const uint64_t prev_rank(void) const { return data[2]; }
        inline const uint64_t next_id(void) const { return data[3]; }
        inline const uint64_t next_rank(void) const { return data[4]; }
        inline void set_path_step(const uint64_t& i) { data[0] = i; }
        inline void set_path_id(const uint64_t& i) { data[0] = pack_step(i, is_rev()); }
        inline void set_is_rev(const bool& b) { data[0] = pack_step(path_id(), b); }
        inline void set_prev_id(const uint64_t& i) { data[1] = i; }
        inline void set_prev_rank(const uint64_t& i) { data[2] = i; }
        inline void set_next_id(const uint64_t& i) { data[3] = i; }
        inline void set_next_rank(const uint64_t& i) { data[4] = i; }
    };
    layout_t get_layout(void) const;
    layout_t set_layout(layout_t layout);
    uint64_t sequence_size(void) const;
    const std::string sequence(void) const;
    void set_sequence(const std::string& seq);
    uint64_t edge_count_offset(void) const;
    uint64_t edge_count(void) const;
    uint64_t edge_offset(void) const;
    std::vector<uint64_t> edges(void) const;
    void add_edge(const uint64_t& relative_id, const uint64_t& edge_type);
    void remove_edge(const uint64_t& rank);
    void set_edge_count(const uint64_t& count, const layout_t& layout);
    void add_path_step(const uint64_t& path_id, const bool& is_rev,
                       const uint64_t& prev_id, const uint64_t& prev_rank,
                       const uint64_t& next_id, const uint64_t& next_rank);
    void add_path_step(const step_t& step);
    void set_path_step(const uint64_t& rank, const uint64_t& path_id, const bool& is_rev,
                       const uint64_t& prev_id, const uint64_t& prev_rank,
                       const uint64_t& next_id, const uint64_t& next_rank);
    void set_path_step(const uint64_t& rank, const step_t& step);
    void flip_paths(const uint64_t& start_marker, const uint64_t& end_marker);
    const std::vector<node_t::step_t> get_path_steps(void) const;
    const step_t get_path_step(const uint64_t& rank) const;
    void remove_path_step(const uint64_t& rank);
    void clear_path_steps(void);
    uint64_t path_count(void) const;
    uint64_t serialize(std::ostream& out) const;
    void load(std::istream& in);
    void display(void) const;

    // path step helpers
    inline static uint64_t pack_step(const uint64_t& path_id, const bool& is_rev) {
        assert(path_id < (0x1ULL << 63));
        return ((path_id << 1) | (is_rev ? 1 : 0));
    }
    inline static uint64_t step_path_id(const uint64_t& step) {
        return step >> 1;
    }
    inline static bool step_is_rev(const uint64_t& step) {
        return step & 1;
    }

};

}

#endif
