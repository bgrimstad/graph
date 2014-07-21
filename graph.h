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
    // Define node and edge structs
    struct node;
    struct edge;
    typedef std::shared_ptr<node> node_ptr;
    typedef std::shared_ptr<edge> edge_ptr;

    struct node
    {
        unsigned int id;
        std::vector<edge_ptr> edges_in, edges_out; // edges_in needed?
        NodeData data;

        // Constructors
        node(unsigned int id, NodeData data) : id(id), data(data) {}

        bool has_link_to(unsigned int other_node) const
        {
            for(const auto e : edges_out)
                if(e->to->id == other_node)
                    return true;
            return false;
        }

        bool has_link_from(unsigned int other_node) const
        {
            for(const auto e : edges_out)
                if(e->to->id == other_node)
                    return true;
            return false;
        }

        void remove_edge_to(unsigned int other_node)
        {
            edges_out.erase(std::remove_if(edges_out.begin(), edges_out.end(), [&other_node](edge_ptr e) { return e->to->id == other_node; }), edges_out.end());
        }

        void remove_edge_from(unsigned int other_node)
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
        node_ptr from, to;
        EdgeData data;

        // Constructor here
        edge(node_ptr from, node_ptr to, EdgeData data)
            : from(from), to(to), data(data) {}
    };

    // Print function
    friend std::ostream& operator<<(std::ostream& os, const Graph<NodeData,EdgeData> &graph)
    {
        for(const auto n : graph.nodes)
        {
            os << "Node " << n.first << ": ";
            for(const auto e : n.second->edges_out)
                os << e->to->id << " (" << e->data << "), ";
            os << endl;
        }
        return os;
    }

public:
    Graph() {}

    // List of nodes (id used as key)
    std::unordered_map<unsigned int, node_ptr> nodes;
    // TODO: encapsulate nodes

    void add_node(unsigned int id)
    {
        // Assumes that NodeData() exists
        add_node(id, NodeData());
    }

    void add_node(unsigned int id, NodeData data)
    {
        if(nodes.find(id) == nodes.end())
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

    void remove_node(unsigned int id)
    {
        if (nodes.find(id) != nodes.end())
        {
            // Remove all edges to and from node
            for(auto n : nodes)
            {
                //cout << n.first << endl;
                remove_edge(n.first, id);
                remove_edge(id, n.first);
            }

            // Remove node
            nodes.erase(id);
        }
    }

    void add_edge(unsigned int from, unsigned int to)
    {
        // Assumes that EdgeData() exists
        add_edge(from, to, EdgeData());
    }

    void add_edge(unsigned int from, unsigned int to, EdgeData data)
    {
        if(nodes.find(from) == nodes.end() || nodes.find(to) == nodes.end())
        {
            cout << "Nonexistent node(-s). Cannot add edge!" << endl;
            return;
        }
        else if(nodes.at(from)->has_link_to(to))
        {
            cout << "Edge already exist!" << endl;
            return;
        }
        else
        {
            edge_ptr e(new edge(nodes.at(from), nodes.at(to), data));
            nodes.at(from)->edges_out.push_back(e);
            nodes.at(to)->edges_in.push_back(e);
        }
    }

    void remove_edge(unsigned int from, unsigned int to)
    {
        if (nodes.find(from) == nodes.end() || nodes.find(to) == nodes.end())
        {
            cout << "Nonexistent nodes. Cannot add edge!" << endl;
            return;
        }
        else if(!nodes.at(from)->has_link_to(to))
        {
            cout << "Cannot remove nonexistent edge!" << endl;
            return;
        }
        else
        {
            nodes.at(from)->remove_edge_to(to);
            nodes.at(to)->remove_edge_from(from);
        }
    }

};

#endif // GRAPH_H
