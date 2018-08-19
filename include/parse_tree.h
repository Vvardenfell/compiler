#ifndef PARSE_TREE_H
#define PARSE_TREE_H

#include "vector.h"

template<typename T> class ParseTree {
public:

    class Node {
    private:

        friend class Vector<Node>;
        friend class ParseTree;

        enum class StorageType : unsigned char {
            USERDATA,
            NONE
        };

        union Storage {
            T userdata;

            Storage() {}
            ~Storage() {}
            Storage(const Storage&) {}
        } userdata_storage;
        Vector<Node> children;
        Node* const parent_node;
        Node::StorageType storage_type;
        FundamentalType data_type;


        Node() : userdata_storage(), children(), parent_node(this), storage_type(Node::StorageType::NONE), data_type(FundamentalType::NONE) {}


        template<typename... Args> Node(Node* parent, Args&&... userdata_args)
                                                            : userdata_storage(), children(), parent_node(parent), storage_type(Node::StorageType::USERDATA), data_type(FundamentalType::NONE) {
            new (&(this->userdata_storage.userdata)) T(std::forward<Args>(userdata_args)...);
        }


        template<typename U = T> typename std::enable_if<std::is_trivially_destructible<U>::value>::type destruct() {}
        template<typename U = T> typename std::enable_if<!std::is_trivially_destructible<U>::value>::type destruct() {
            if(this->storage_type == Node::StorageType::USERDATA) {
                try {
                    this->userdata_storage.userdata.~U();
                }
                catch(...) {}
            }
        }


    public:

        Node(const Node& source) : userdata_storage(), children(source.children), parent_node(source.parent_node), storage_type(source.storage_type), data_type(source.data_type) {
            if(this->storage_type == Node::StorageType::USERDATA) {
                new (&(this->userdata_storage.userdata)) T(source.userdata_storage.userdata);
            }
        }


        ~Node() {
            this->destruct();
        }


        template<typename... Args> Node& create_child(Args&&... userdata_args) {
            this->children.emplace_back(this, std::forward<Args>(userdata_args)...);
            return *(this->children.end() - 1);
        }


        Node& parent() {
            return *this->parent_node;
        }


        const Node& parent() const {
            return *this->parent_node;
        }


        Node& child(std::size_t index) {
            return this->children[index];
        }


        const Node& child(std::size_t index) const {
            return this->children[index];
        }


        std::size_t child_count() const {
            return this->children.size();
        }


        bool has_userdata() const {
            return this->storage_type == Node::StorageType::USERDATA;
        }


        T& userdata() {
            return this->userdata_storage.userdata;
        }


        const T& userdata() const {
            return this->userdata_storage.userdata;
        }


        FundamentalType get_data_type() const {
            return this->data_type;
        }


        void set_data_type(FundamentalType data_type) {
            this->data_type = data_type;
        }
    };


private:

    Node root_node;

public:

    ParseTree() : root_node() {}


    Node& root() {
        return this->root_node;
    }


    const Node& root() const {
        return this->root_node;
    }
};

#endif // PARSE_TREE_H
