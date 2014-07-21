#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <unordered_map>

using std::cout;
using std::endl;

// Template class implementation of a bidirectional graph (bigraph)
template<class NodeData = double, class EdgeData = double>
class Graph
{
    typedef unsigned int n_id; // Node id
    typedef std::tuple<n_id,n_id> e_id; // Edge id is stored as tuple (from node id, to node id)

    // A has function for the edge id tuple (used later in the unordered_map)
    struct key_hash : public std::unary_function<e_id, std::size_t>
    {
        std::size_t operator()(const e_id& id) const
        {
            return std::get<0>(id) ^ std::get<1>(id); // ^ std::get<2>(k);
        }
    };

    // Define node and edge structs
    struct node;
    struct edge;
    typedef std::shared_ptr<node> node_ptr;
    typedef std::shared_ptr<edge> edge_ptr;

    struct node
    {
        n_id id;
        std::vector<edge_ptr> edges_in, edges_out; // edges_in needed?
        NodeData data;

        // Constructors
        node(n_id id, NodeData data) : id(id), data(data) {}

        bool has_link_to(n_id other_node) const
        {
            for(const auto e : edges_out)
                if(e->to->id == other_node)
                    return true;
            return false;
        }

        bool has_link_from(n_id other_node) const
        {
            for(const auto e : edges_out)
                if(e->to->id == other_node)
                    return true;
            return false;
        }

        void remove_edge_to(n_id other_node)
        {
            edges_out.erase(std::remove_if(edges_out.begin(), edges_out.end(), [&other_node](edge_ptr e) { return e->to->id == other_node; }), edges_out.end());
        }

        void remove_edge_from(n_id other_node)
        {
            edges_in.erase(std::remove_if(edges_in.begin(), edges_in.end(), [&other_node](edge_ptr e) { return e->to->id == other_node; }), edges_in.end());
        }

        bool is_source_node() const
        {
            if(edges_in.empty())
                return true;
            return false;
        }

        bool is_sink_node() const
        {
            if(edges_out.empty())
                return true;
            return false;
        }
    };

    struct edge
    {
        e_id id;
        node_ptr from, to;
        EdgeData data;

        // Constructor here
        edge(node_ptr from, node_ptr to, EdgeData data)
            : id(e_id(from->id,to->id)), from(from), to(to), data(data) {}
    };

    // Print function
    friend std::ostream& operator<<(std::ostream& os, const Graph<NodeData,EdgeData> &graph)
    {
        for(const auto n : graph.nodes)
        {
            os << "Node " << n.first << "-> ";
            for(const auto e : n.second->edges_out)
                os << e->to->id << ", ";
                //os << e->to->id << " (" << e->data << "), ";
            os << endl;
        }
        return os;
    }

public:
    Graph() {}

    // List of nodes (id used as key)
    std::unordered_map<n_id, node_ptr> nodes;
    std::unordered_map<e_id, edge_ptr, key_hash> edges;
    // TODO: encapsulate nodes

    void add_node(n_id id)
    {
        // Assumes that NodeData() exists
        add_node(id, NodeData());
    }

    void add_node(n_id id, NodeData data)
    {
        if(!node_exists(id))
        {
            node_ptr n(new node(id,data));
            nodes.emplace(id, n);
        }
        else
        {
            cout << "Node " << id << " already exist!" << endl;
            return;
        }
    }

    void remove_node(n_id id)
    {
        if(node_exists(id))
        {
            // Remove all edges to and from node
            for(auto n : nodes)
            {
                //cout << n.first << endl;
                if(edge_exists(n.first, id))
                    remove_edge(n.first, id);
                if(edge_exists(id, n.first))
                    remove_edge(id, n.first);
            }

            // Remove node
            nodes.erase(id);
        }
        else
        {
            cout << "Cannot remove nonexistent node!" << endl;
        }
    }

    void add_edge(n_id from, n_id to)
    {
        // Assumes that EdgeData() exists
        add_edge(from, to, EdgeData());
    }

    void add_edge(n_id from, n_id to, EdgeData data)
    {
        if(edge_exists(from, to))
        {
            cout << "Edge already exist!" << endl;
        }
        else if(!node_exists(from) || !node_exists(to))
        {
            cout << "Nonexistent node(-s). Cannot add edge!" << endl;
        }
        else
        {
            edge_ptr e(new edge(nodes.at(from), nodes.at(to), data));
            edges.emplace(e->id,e);
            nodes.at(from)->edges_out.push_back(e);
            nodes.at(to)->edges_in.push_back(e);
        }
    }

    void remove_edge(n_id from, n_id to)
    {
        if(edge_exists(from, to))
        {
            edges.erase(e_id(from,to));
            nodes.at(from)->remove_edge_to(to);
            nodes.at(to)->remove_edge_from(from);
        }
        else
        {
            cout << "Cannot remove nonexistent edge!" << endl;
            return;
        }
    }

    bool node_exists(n_id node) const
    {
        return !(nodes.find(node) == nodes.end());
    }

    bool edge_exists(n_id from, n_id to) const
    {
        return !(edges.find(e_id(from,to)) == edges.end());
    }

};

#endif // GRAPH_H
