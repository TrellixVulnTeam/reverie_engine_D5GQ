#pragma once

// Project
#include "fortress/containers/GContainerExtensions.h"
#include "fortress/system/memory/GPointerTypes.h"

namespace rev {


/// @class DagNode
/// @brief  A node of a Directed Acyclic Graph (DAG)
/// @details A directed acyclic graph is more generalized notion of a tree structure, except that in a DAG, a child may
///     have more than one parent, as opposed to in a tree, where each child may only have at most one parent.
/// @note To achieve functionality without memory leaks, all DAG nodes are cached in a static map.  
///     To completely delete a DAG node, they must be manually erased from this static map using the 
///     EraseFromNodeVec method.
/// @todo Use unique pointers instead
template<typename T>
class DagNode: public std::enable_shared_from_this<DagNode<T>>
{
public:
    typedef std::vector<std::shared_ptr<T>> NodeVector;

    /// @name Static Methods
    /// @{

    /// @brief Create a DAG node
    template<typename DagType, typename ...Args>
    static std::shared_ptr<DagType> CreateDagNode(const Args&... args) {
        static_assert(std::is_base_of_v<DagNode<T>, DagType>, "Type must derive from DagNode");

        // Create DAG node and add to node map
        std::shared_ptr<DagType> myDag = prot_make_shared<DagType>(args...);
        myDag->addToNodeMap();
        return myDag;
    }

    /// @brief Erases a node with the given ID from the static list
    static void EraseFromNodeVec(Uint32_t id) {
        // Clear from static vector
        s_nodes[id] = nullptr;

        s_erasedNodes.push_back(id);
    }

    /// @brief Returns map of DAG nodes 
    static std::vector<std::shared_ptr<T>>& DagNodes() { return s_nodes; }

    /// @brief Clear all Dag nodes of this type
    static void Clear() {
        s_nodes.clear();
        s_erasedNodes.clear();
    }

    /// @}

    /// @name Destructor
    /// @{

    virtual ~DagNode() {
    }

    /// @}

    /// @name Public Methods
    /// @{

    /// @brief Return the unique integer ID given an array buffer
    void get3ChannelId(Uint32_t* colorArray) {
        Get3ChannelId(id(), colorArray);
    }

    /// @brief The unique integer ID of the DAG Node
    Uint32_t id() const { return m_uniqueID; }

    bool hasChild(Uint32_t id, Uint32_t& outIndex) {
        bool found = false;
        Uint32_t numChildren = (Uint32_t)m_children.size();
        for (Uint32_t i = 0; i < numChildren; i++) {
            if (m_children[i]->id() == id) {
                outIndex = i;
                return true;
            }
        }
        return found;
    }

    /// @brief Add a parent to the given node
    void addParent(const std::shared_ptr<T>& parent) {
        // Add parent to this node
        addParentPrivate(parent);

        // Add this node as a child of the parent node
        parent->addChildPrivate(sharedPtr());
    }

    /// @brief Add a child to the given node
    void addChild(const std::shared_ptr<T>& child) {
        // Add child to this node
        addChildPrivate(child);

        // Add this node as a parent of the child node
        child->addParentPrivate(sharedPtr());
    }

    /// @brief Remove a parent from the given node
    void removeParent(Uint32_t parentId) {
        if (std::shared_ptr<T> parent = m_parents.at(parentId).lock()) {
            parent->removeChildPrivate(m_uniqueID);
        }
        removeParentPrivate(parentId);
    }

    /// @brief Remove a child from the given node
    void removeChild(DagNode* child)
    {
        child->removeParentPrivate(m_uniqueID);
        removeChildPrivate(child->id());
    }
    void removeChild(const std::shared_ptr<DagNode>& child) {
        child->removeParentPrivate(m_uniqueID);
        removeChildPrivate(child->id());
    }

    /// @brief Outputs a string containing the hierarchy description of this node
    GString hierarchyDescription(int level = 0) {
        GString margin = QStringLiteral("\t").repeated(level);
        GString tabbed = margin + "\t";
        GString description = margin + GString::FromNumber(m_uniqueID);

        for (const std::shared_ptr<DagNode>& child : m_children) {
            if (child->hasChildren()) {
                description += "\n" + child->hierarchyDescription(level + 1);
            }
            else {
                description += "\n" + tabbed + GString::FromNumber(child->id());
            }
        }

        return description;
    }

    /// @}

    /// @name Properties
    /// @{

    std::vector<std::shared_ptr<T>>& children() { return m_children; }
    const std::vector<std::shared_ptr<T>>& children() const { return m_children; }

    /// @brief Whether or not the node has children
    inline bool hasChildren() const { return numChildren() != 0; }

    /// @brief Whether or not the node has children
    inline bool hasParents() const { return numParents() != 0; }

    /// @brief Number of parent DAG nodes
    inline Uint32_t numParents() const { return (Uint32_t)m_parents.size(); }

    /// @brief Number of child DAG nodes
    inline Uint32_t numChildren() const { return (Uint32_t)m_children.size(); }

    /// @}


protected:

    /// @name Friends
    /// @{

    /// @brief Commands that have access to DAG node private methods
    friend class AddSceneObjectCommand;

    /// @}

    /// @name Constructors/
    /// @{

    DagNode(){ 
    }

    /// @}

    NodeVector parents() {

        // Generate vector of shared pointers to valid parents
        auto parents = std::vector<std::shared_ptr<T>>();
        parents.reserve(m_parents.size());
        for (auto& parentPair : m_parents) {
            if (std::shared_ptr<T> parent = parentPair.second.lock()) {
                parents.push_back(parent);
            }
        }

        // Update map of parents if some are invalid
        if (m_parents.size() != parents.size()) {
            m_parents.clear();
            for (const std::shared_ptr<T>& parent : parents) {
                Map::Emplace(m_parents, parent->id(), parent);
            }
        }

        return parents;
    }

    /// @brief returns a shared pointer to this object from the static node map
    const std::shared_ptr<T>& sharedPtr() {
        return s_nodes.at(m_uniqueID);
    }

    /// @brief Adds to static list of nodes
    void addToNodeMap() {
        if (!s_erasedNodes.size()) {
            m_uniqueID = (Uint32_t)s_nodes.size();
            s_nodes.push_back(std::static_pointer_cast<T>(shared_from_this()));
        }
        else {
            m_uniqueID = s_erasedNodes.back();
            s_erasedNodes.pop_back();
            s_nodes[m_uniqueID] = std::static_pointer_cast<T>(shared_from_this());
        }
    }
    
    /// @brief Child DAG nodes of this node
    NodeVector m_children;
    
    /// @brief Parent DAG nodes of this node
    tsl::robin_map<Uint32_t, std::weak_ptr<T>> m_parents;

private:

    virtual void onAddChild(const std::shared_ptr<T>& child) = 0;
    virtual void onAddParent(const std::shared_ptr<T>& parent) = 0;
    virtual void onRemoveChild(const std::shared_ptr<T>& child) = 0;
    virtual void onRemoveParent(const std::shared_ptr<T>& parent) = 0;

    /// @brief Adds child to the given node
    /// @Detailed Does not add to any other structures or alter other nodes
    void addChildPrivate(const std::shared_ptr<T>& child) {
        Uint32_t childIndex;
        if (hasChild(child->id(), childIndex)) {
            throw std::range_error("Error, node is already a child of the current node");
        }
        m_children.push_back(child);

        onAddChild(child);
    }

    /// @brief Adds parent to the given node
    /// @Detailed Does not add to any other structures or alter other nodes
    void addParentPrivate(const std::shared_ptr<T>& parent) {
        if (m_parents.find(parent->id()) != m_parents.end()) {
            throw std::range_error("Error, node is already a parent of the current node");
        }
        Map::Emplace(m_parents, parent->id(), parent);

        onAddParent(parent);
    }

    /// @brief Remove child from the given node
    /// @Detailed Does not alter other nodes
    void removeChildPrivate(Uint32_t childID) {
        Uint32_t childIndex;
        if (!hasChild(childID, childIndex)) {
            throw std::range_error("Error, no child with the given ID was found for removal");
        }
        onRemoveChild(m_children[childIndex]);
        m_children.erase(m_children.begin() + childIndex);
    }

    /// @brief Remove parent from the given node
    /// @Detailed Does not alter other nodes
    void removeParentPrivate(Uint32_t parentID) {
        if (!Map::HasKey(m_parents, parentID)) {
            throw std::range_error("Error, no parent with the given ID was found for removal");
        }
        std::shared_ptr<T> parent = m_parents[parentID].lock();
        onRemoveParent(parent);
        m_parents.erase(parentID);
    }

    /// @brief map of all DAG nodes
    static std::vector<std::shared_ptr<T>> s_nodes;
    static std::vector<Uint32_t> s_erasedNodes;

    /// @brief Unique integer ID for the DAG node
    Uint32_t m_uniqueID{std::numeric_limits<Uint32_t>::max()};

};

template<typename T>
std::vector<std::shared_ptr<T>> DagNode<T>::s_nodes = {};

template<typename T>
std::vector<Uint32_t> DagNode<T>::s_erasedNodes = {};



} // end rev namespace
