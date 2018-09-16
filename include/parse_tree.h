#ifndef PARSE_TREE_H
#define PARSE_TREE_H

#include "vector.h"
#include <utility>
#include <type_traits>
#include <algorithm>


template<typename T> class ParseTree {
public:

    class Node {
    private:

        friend class ParseTree;

        enum class StorageType : unsigned char {
            USERDATA,
            NONE
        };

        union Storage {
            T userdata;

            Storage() {}
            ~Storage() {}
        } userdata_storage;
        Vector<Node> children;
        Node* parent_node;
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


        static void swap_userdata(Storage& left, Storage& right) {
            if(left.storage_type == StorageType::USERDATA && right.storage_type == StorageType::USERDATA) {
                T tmp(std::move(left.userdata));
                left.userdata.~T();
                new (&left.userdata) T(std::move(right.userdata));
                right.userdata.~T();
                new (&right.userdata) T(std::move(tmp));
            }
            else if(left.storage_type == StorageType::USERDATA) {
                new (&right.userdata) T(std::move(left.userdata));
                left.userdata.~T();
            }
            else if(right.storage_type == StorageType::USERDATA) {
                new (&left.userdata) T(std::move(right.userdata));
                right.userdata.~T();
            }
        }

        static void swap(Node& left, Node& right) {
            using std::swap;

            swap_userdata(left.userdata_storage, right.userdata_storage);
            swap(left.children, right.children);
            swap(left.parent_node, right.parent_node);
            swap(left.storage_type, right.storage_type);
            swap(left.data_type, right.data_type);
        }


    public:

        Node(const Node& source) : userdata_storage(), children(source.children), parent_node(source.parent_node), storage_type(source.storage_type), data_type(source.data_type) {
            if(this->storage_type == Node::StorageType::USERDATA) {
                new (&(this->userdata_storage.userdata)) T(source.userdata_storage.userdata);
            }

            for(Node& child : this->children) child.parent_node = this;
        }


        Node(const Node&& source) : userdata_storage(), children(std::move(source.children)), parent_node(source.parent_node), storage_type(source.storage_type), data_type(source.data_type) {
            if(this->storage_type == Node::StorageType::USERDATA) {
                new (&(this->userdata_storage.userdata)) T(std::move(source.userdata_storage.userdata));
            }

            for(Node& child : this->children) child.parent_node = this;
        }


        Node& operator=(const Node& source) {
            Node tmp(source);
            Node* parent = this->parent_node;

            swap(*this, tmp);
            this->parent_node = parent;

            return *this;
        }


        Node& operator=(Node&& source) {
            Node tmp(source);
            Node* parent = this->parent_node;

            swap(*this, tmp);
            this->parent_node = parent;

            return *this;
        }


        ~Node() {
            this->destruct();
        }


        template<typename... Args> Node& create_child(Args&&... userdata_args) {
            this->children.push_back(Node(this, std::forward<Args>(userdata_args)...));
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
