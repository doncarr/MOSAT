#ifndef RI_ADTLIB_H
#define RI_ADTLIB_H

#define ADTE_OUT_OF_MEMORY  "ADT Exception. Out of memory"
#define ADTE_LIST_IS_EMPTY  "ADT Exception. List is empty"
#define ADTE_STACK_IS_EMPTY "ADT Exception. Stack is empty"
#define ADTE_QUEUE_IS_EMPTY "ADT Exception. Queue is empty"
#define ADTE_NODE_NOT_FOUND "ADT Exception. Node not found"
#define ADTE_EOL_REACHED    "ADT Exception. End of list reached"
#define ADTE_OUT_OF_RANGE   "ADT Exception. Operation out of range"

namespace igrl
{
////////////////////////////////////////////////////////////////////////////////////////////////////
// Class name: LinkedList                                                                         //
// Description: An object of this class represents a linked list of elements of type LData. The   //
//              class interface includes methods for inserting and removing nodes, clearing the   //
//              list, among others. Besides, this class contains the declaration and definition   //
//              of two nested classes:                                                            //
//              LNode.- A private class (since it's for internal use only) whose objects represent//
//                      the nodes of the list.                                                    //
//              Iterator.- (Some sort of) Iterator to sequentially traverse and retrieve the nodes//
//                         of the list.                                                           //
// IMPORTANT: If LData is not a fundamental type, it MUST implement the equality operator since   //
//            some of the methods of LinkList rely on this. The expected function signature is:   //
//                                                                                                //
//            bool operator==(const <LData>&) const;  // <-- Change <LData> by a valid class name.//
//                                                                                                //
////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class LData> class LinkedList
  {
    class LNode
    {
      public:
      LData m_nData;                                                                                  // <-- The data of the node.
      LNode* m_pNextNode;                                                                             // <-- The pointer to the next node.
      LNode(const LData& nData): m_nData(nData), m_pNextNode(NULL) {}                                 // <-- The constructor.
      void Link(LNode& nextNode) { m_pNextNode = &nextNode; }                                         // <-- A method to link a node to another one.
    };
    public:
    class Iterator
    {
      friend class LinkedList<LData>;
      LNode* m_pCurrentNode;                                                                          // <-- A pointer to the "current" node.
      unsigned long m_cPosition;                                                                      // <-- The position the "current" node occupies in the list.
      public:
      Iterator(): m_pCurrentNode(NULL), m_cPosition(0) {}                                             // <-- The constructor.
      bool EndReached() const { return NULL == m_pCurrentNode; }                                      // <-- Tells if the iterator has reached the end of the list.
      bool operator==(const Iterator& rOp) const { return m_pCurrentNode == rOp.m_pCurrentNode; }     // <-- Equality operator
      bool operator!=(const Iterator& rOp) const { return !operator==(rOp); }                         // <-- Inequality operator
      LData& operator*() { return m_pCurrentNode->m_nData; }                                          // <-- Emulates the dereference operator, as if the iterator were a pointer to an LData variable
      LData* operator->() { return &m_pCurrentNode->m_nData; }                                        // <-- When LData is a class, this operator allows to access the members as if the iterator were
      Iterator& operator+=(unsigned long rOp) { while (0 < rOp--) operator++(); }                     // <-- Moves the iterator forward rOp positions.
      void operator++(int) { operator++(); }                                                          // <-- (Postfix version of ++).
      void operator++() { if (NULL == m_pCurrentNode) throw ADTE_EOL_REACHED;                         // <-- (Prefix version of ++) Moves the iterator forward one position.
        m_pCurrentNode = m_pCurrentNode->m_pNextNode; ++m_cPosition; }
    };
    LinkedList();                                                                                     // <-- All of the following members are described below, just before their implentations.
    LinkedList(const LinkedList<LData>& sList);
    ~LinkedList();
    void Clear();
    unsigned long Size() const;
    unsigned long Insert(const LData& nData);
    unsigned long Insert(const LData& nData, unsigned long nPosition);
    unsigned long Insert(const LData& nData, const Iterator& nPosition);
    unsigned long Remove(unsigned long nPosition);
    unsigned long Remove(const Iterator& nPosition);
    LData& Retrieve(unsigned long nPosition);
    const Iterator& NewIterator() const;
    Iterator Find(const LData& nData) const;
    Iterator Find(const LData& nData, Iterator sPosition) const;
    LinkedList<LData>& operator=(const LinkedList<LData>& rOp);
    private:
    Iterator m_bOfList;                                                                               // <-- An iterator always pointing to the first node of the list (used by NewIterator(...)).
    LNode* m_pFirstNode;                                                                              // <-- A pointer to the first node of the list
    LNode* m_pLastNode;                                                                               // <-- A pointer to the last node of the list
    unsigned long m_nOfNodes;                                                                         // <-- The number of nodes in the list.
  };

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method name: LinkedList::LinkedList                                                            //
// Description: Default constructor.                                                              //
////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class LData> LinkedList<LData>::LinkedList(): m_pFirstNode(NULL), m_pLastNode(NULL),
    m_nOfNodes(0)
  {                                                                                                   // <-- Absolutelly self-explanatory.
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method name: LinkedList::LinkedList(const LinkedList<LData>&)                                  //
// Description: Copy constructor.                                                                 //
// Parameters: sList[in].- The source list.                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class LData> LinkedList<LData>::LinkedList(const LinkedList<LData>& sList):
    m_pFirstNode(NULL), m_pLastNode(NULL), m_nOfNodes(0)
  {
    for (Iterator ss = sList.NewIterator(); !ss.EndReached(); ++ss)                                   // <-- Absolutelly self-explanatory.
      Insert(*ss);
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method name: LinkedList::~LinkedList()                                                         //
// Description: Destructor of LinkedList.                                                         //
////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class LData> LinkedList<LData>::~LinkedList()
  {
    Clear();                                                                                          // <-- Absolutelly self-explanatory.
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method name: LinkedList::Clear()                                                               //
// Description: This method removes all of the elements of the list.                              //
////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class LData> void LinkedList<LData>::Clear()
  {
    if (0 < m_nOfNodes)                                                                               // <-- If the list is alreay empty there is nothing to do.
    {
      LNode* pCurrentNode = m_pFirstNode;
      LNode* pNextNode = m_pFirstNode->m_pNextNode;
      for (unsigned long ss = 0; ss < m_nOfNodes; ++ss)                                               // <-- If the list is not empty, we traverse it from head to tail...
      {
        delete pCurrentNode;                                                                          //     and delete every node. Quite simple, isn't it?
        pCurrentNode = pNextNode;
        if (NULL != pCurrentNode)                                                                     // <-- On the last iteration pCurrentNode is nullified, hence this validation.
          pNextNode = pCurrentNode->m_pNextNode;
      }
      m_nOfNodes = 0;                                                                                 // <-- Finally, the nodes counter, as well as the internal iterator and the pointers to the first
      m_bOfList.m_pCurrentNode = m_pFirstNode = m_pLastNode = NULL;                                   //     and last nodes are updated accordingly.
    }
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method name: LinkedList::Size()                                                                //
// Description: This method returns the number of elements of the list.                           //
////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class LData> inline unsigned long LinkedList<LData>::Size() const
  {
    return m_nOfNodes;                                                                                // <-- Absolutelly self-explanatory.
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method name: LinkedList::Insert(const LData& nData)                                            //
// Description: This method inserts a node at the tail of the list and returns the updated size of//
//              of the list.                                                                      //
// Parameters: nData[in].- The data of the new node.                                              //
////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class LData> inline unsigned long LinkedList<LData>::Insert(const LData& nData)
  {
    return Insert(nData, m_nOfNodes);                                                                 // <-- Absolutelly self-explanatory.
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method name: LinkedList::Insert(const LData& nData, unsigned long nPosition)                   //                                                               //
// Description: This method inserts a node at the 0-based position nPosition and returns the      //
//              updated size of the list.                                                         //
// Parameters: nData[in].- The data of the new node.                                              //
//             nPosition[in].- The 0-based position of the node that is going to be inserted. If  //
//                             nPosition is out of range or there is not enough memory to create  //
//                             the new node, an ADT_EXCEPTION exception is thrown.                //
////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class LData> unsigned long LinkedList<LData>::Insert(const LData& nData,
    unsigned long nPosition)
  {
    if (m_nOfNodes < nPosition)                                                                       // <-- If nPostion is out of range...
      throw ADTE_OUT_OF_RANGE;                                                                        //     we let the caller know the insertion failed by throwing this exception.
    LNode* pNewNode = new LNode(nData);
    if (NULL == pNewNode)                                                                             // <-- If we run out of memory...
      throw ADTE_OUT_OF_MEMORY;                                                                       //     we let the caller by throwing a exception.
    if (0 == m_nOfNodes)                                                                              // <-- If the list is empty, the insertion of the first element is quite simple...
      m_bOfList.m_pCurrentNode = m_pFirstNode = m_pLastNode = pNewNode;                               //     The internal iterator, the pointers to the first and the last node must point to pNewNode.
    else                                                                                              // <-- If the list is not empty...
      if (0 == nPosition)                                                                             //     and the insertion is at the beginning of the list...
      {
        pNewNode->Link(*m_pFirstNode);                                                                //     pNewNode becomes the head of the list and it's linked to the former-head node.
        m_bOfList.m_pCurrentNode = m_pFirstNode = pNewNode;                                           //     The internal iterator and the pointer the first element must be updated too.
      }
      else if (nPosition == m_nOfNodes)                                                               // <-- If the list is not empty and the insertion is at tail of the list...
      {
        m_pLastNode->Link(*pNewNode);                                                                 //     the last node is linked to pNewNode, pNewNode becomes the tail of the list...
        m_pLastNode = pNewNode;                                                                       //     and m_pLastNode is updated accordingly.
      }
      else                                                                                            // <-- If the list is not empty and the insertion is at an intermediate position...
      {
        LNode* pCurrentNode = m_pFirstNode;
        LNode* pPreviousNode = NULL;
        for (unsigned long ss = 0; ss < nPosition; ++ss)                                              //     we locate the nodes at the target position and at the previous one.
        {
          pPreviousNode = pCurrentNode;
          pCurrentNode = pCurrentNode->m_pNextNode;
        }
        pPreviousNode->Link(*pNewNode);                                                               //     Now we make pNewNode occupy the target position by linking pPreviousNode to it and linking
        pNewNode->Link(*pCurrentNode);                                                                //     pNewNode to pCurrentNode.
      }
    return ++m_nOfNodes;                                                                              // <-- Finally, the node counter needs to be incremented.
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method name: LinkedList::Insert(const LData& nData, const Iterator& nPosition)                 //
// Description: This method inserts a node at the 0-based position nPosition                      //
// Parameters: nData[in].- The data of the new node.                                              //
//             nPosition[in].- An iterator pointing to the position at which the new node is going//
//                             to be inserted.                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class LData> inline unsigned long LinkedList<LData>::Insert(const LData& nData,
    const Iterator& nPosition)
  {
    return Insert(nPosition.m_cPosition);                                                             // <-- Absolutelly self-explanatory.
  }
////////////////////////////////////////////////////////////////////////////////////////////////////
// Method name: LinkedList::Remove(unsigned long nPosition)                                       //
// Description: This method removes a node from the list.                                         //
// Parameters: nPosition[in].- The 0-based position of the node to remove. If nPosition is out of //
//                             range, an ADT_EXCEPTION exception is thrown.                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class LData> unsigned long LinkedList<LData>::Remove(unsigned long nPosition)
  {
    if (nPosition >= m_nOfNodes)                                                                      // <-- If nPosition is out of range...
      throw ADTE_OUT_OF_RANGE;                                                                        //     we let the caller know by throwing this exception.
    LNode* pPreviousNode = NULL;
    LNode* pCurrentNode = m_pFirstNode;
    if (0 == nPosition)                                                                               // <-- When the target node is the head of the list, the removal consist symply on...
    {
      m_bOfList.m_pCurrentNode = m_pFirstNode = m_pFirstNode->m_pNextNode;                            //     making the second node (if any) the head of the list,...
      delete pCurrentNode;                                                                            //     deleting the head node,...
      if (1 == m_nOfNodes)                                                                            //     and, if we are removing the last node of the list,...
        m_pLastNode = NULL;                                                                           //     nullifing m_pLastNode.
    }
    else                                                                                              // <-- When the target node isn't the head of the list, the removal requires a little more effort.
    {
      for (unsigned long ss = 0; ss < nPosition; ++ss)                                                //     Firstly, we have to locate the target node....
      {
        pPreviousNode = pCurrentNode;
        pCurrentNode = pCurrentNode->m_pNextNode;
      }
      pPreviousNode->Link(*pCurrentNode->m_pNextNode);                                                //     Once it's been located, a link beetween its previous node and its next node is established.
      if (pCurrentNode == m_pLastNode)                                                                //     Besides, if it turns out that the target node is the tail of the list, the previous node
        m_pLastNode = pPreviousNode;                                                                  //     must become the tail so m_pLastNode must me directed to it and,...
      delete pCurrentNode;                                                                            //     of course, the target node must be deleted.
    }
    return --m_nOfNodes;                                                                              // <-- Finally, the node counter needs to be decremented.
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method name: LinkedList::Remove(const Iterator& nPosition)                                     //
// Description: This method removes a node from the list.                                         //
// Parameters: nPosition[in].- An iterator pointing to the node to remove.                        //
////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class LData> inline unsigned long
    LinkedList<LData>::Remove(const Iterator& nPosition)
  {
    return Remove(nPosition.m_cPosition);                                                             // <-- Absolutelly self-explanatory.
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method name: LinkedList::Retrieve(LData& nData, unsigned long nPosition)                       //
// Description: This method retrieves the data of the element at the nPosition position.          //
// Parameters: nPosition[in].- The 0-based position of the element from which the data is to be   //
//                             retrieved. If the list is empty or nPosition is out of range, an   //
//                             ADT_EXCEPTION exception is thrown.                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class LData> LData& LinkedList<LData>::Retrieve(unsigned long nPosition)
  {
    if (0 == m_nOfNodes)                                                                              // <-- If the list is empty,...
      throw ADTE_LIST_IS_EMPTY;                                                                       //     we let the caller know by throwing this exception.
    if (m_nOfNodes <= nPosition)                                                                      // <-- If nPosition is out of range,...
      throw ADTE_OUT_OF_RANGE;                                                                        //     we throw another exception.
    LNode* pCurrentNode = m_pFirstNode;
    for (unsigned long ss = 0; ss < nPosition; ++ss)                                                  // <-- If nPosition is a valid position...
      pCurrentNode = pCurrentNode->m_pNextNode;                                                       //     we locate the corresponding node and...
    return pCurrentNode->m_nData;                                                                     //     return its data.
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method name: LinkedList::NewIterator()                                                         //
// Description: This method returns an Iterator pointing to the first node of the list or a "NULL"//
//              Iterator if the list is empty.                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class LData> inline const typename LinkedList<LData>::Iterator&
    LinkedList<LData>::NewIterator() const
  {
    return m_bOfList;                                                                                 // <-- Absolutelly self-explanatory.
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method name: LinkedList::Find(const LData& nData)                                              //
// Description: This method looks for a node whose data is equal to nData through all list until  //
//              a matching node is found or the end of the list is reached. If the list is empty  //
//              or the node is not found, an ADT_EXCEPTION exception is thrown.                   //
// Parameters: nData[in].- The value this method looks for in the nodes.                          //
////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class LData> inline typename LinkedList<LData>::Iterator
    LinkedList<LData>::Find(const LData& nData) const
  {
    return Find(nData, m_bOfList);                                                                    // <-- Absolutelly self-explanatory.
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method name: LinkedList::Find(const LData& nData, const Iterator& sPosition)                   //
// Description: This method looks for a node whose data is equal to nData starting from sPosition //
//              until a matching node is found or the end of the list is reached. If the list is  //
//              empty or the node is not found, an ADT_EXCEPTION exception is thrown. If the node //
//              is found, an iterator pointing to the matching node is returned.                  //
// Parameters: nData[in].- The value this method looks for in the nodes.                          //
//             sPosition[in] .- A reference to an iterator poiting to the node at which the search//
//             begins.                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class LData> typename LinkedList<LData>::Iterator
    LinkedList<LData>::Find(const LData& nData, Iterator sPosition) const
  {
    if (0 == m_nOfNodes)                                                                              // <-- If the list is empty...
      throw ADTE_LIST_IS_EMPTY;                                                                       //     we let the caller know by throwing this exception.
    for(; !sPosition.EndReached(); ++sPosition)                                                       // <-- The list is traversed from sPosition until a matching node is found or the end of the list
      if (*sPosition == nData)                                                                        //     is reached.
        return sPosition;
    throw ADTE_NODE_NOT_FOUND;                                                                        // <-- If the node was not found, a different exception is thrown, to let the caller distinguish
  }                                                                                                   //     between both cases.

////////////////////////////////////////////////////////////////////////////////////////////////////
// Method name: LinkedList::operator=(const LinkedList& rOp)                                      //
// Description: The assigment operator.                                                           //
// Parameter: The source list.                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class LData> LinkedList<LData>&
    LinkedList<LData>::operator=(const LinkedList& rOp)
  {
    if (this != &rOp)                                                                                 // <-- First we MUST check if this is a self assignment. If it is, there is nothing to do.
    {
      Clear();                                                                                        // <-- Now, any existing node in the target list is removed.
      for (Iterator ss = rOp.NewIterator(); !ss.EndReached(); ++ss)                                   // <-- And the rest is self-explanatory; isn't it?
        Insert(*ss);
    }
    return *this;
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class name: Stack                                                                              //
// Description: A very simple class to represent a stack data structure.                          //
////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class SData> class Stack
  {
    LinkedList<SData> m_sItems;                                                                       // <-- A linked list in which the stack items are stored.
    public:
    void Clear();
    const SData& Top() const;
    unsigned long Pop();
    unsigned long Size();
    unsigned long Push(const SData& sItem);
  };

  template <class SData> inline void Stack<SData>::Clear()
  {
    m_sItems.Clear();                                                                                 // <-- Absolutelly self-explanatory.
  }

  template <class SData> inline const SData& Stack<SData>::Top() const
  {
    if (0 == m_sItems.Size())                                                                         // <-- If the stack is empty there is nothing to do but...
      throw ADTE_STACK_IS_EMPTY;                                                                      //     throwing this exception.
    return m_sItems.Retrieve(m_sItems.Size() - 1);                                                    // <-- The last node of the list is retrieved and returned.
  }

  template <class SData> inline unsigned long Stack<SData>::Pop()
  {
    if (0 == m_sItems.Size())                                                                         // <-- If the stack is empty there is nothing to do but...
      throw ADTE_STACK_IS_EMPTY;                                                                      //     throwing this exception.
    return m_sItems.Remove(m_sItems.Size() - 1);                                                      // <-- The last node of the list is removed.
  }

  template <class SData> inline unsigned long Stack<SData>::Size()
  {
    return m_sItems.Size();                                                                           // <-- Absolutelly self-explanatory.
  }

  template <class SData> inline unsigned long Stack<SData>::Push(const SData& sItem)
  {
    return m_sItems.Insert(sItem);                                                                    // <-- As simple as inserting a node at the end of the list. If we run out of memory and the new
  }                                                                                                   //     sItem can't be pushed, Insert throws an ADTE_OUT_OF_MEMORY exception.

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class name: Queue                                                                              //
// Description: A very simple class to represent a queue data structure.                          //
////////////////////////////////////////////////////////////////////////////////////////////////////

  template <class QData> class Queue
  {
    LinkedList<QData> m_qItems;                                                                       // <-- A linked list in which the queue items are stored.
    public:
    void Clear();
    const QData& Front() const;
    unsigned long Size();
    unsigned long Dequeue();
    unsigned long Enqueue(const QData& qItem);
  };

  template <class QData> inline void Queue<QData>::Clear()
  {
    m_qItems.Clear();                                                                                 // <-- Absolutelly self-explanatory.
  }

  template <class QData> inline const QData& Queue<QData>::Front() const
  {
    if (0 == m_qItems.Size())                                                                         // <-- If the queue is empty there is nothing to do but...
      throw ADTE_QUEUE_IS_EMPTY;                                                                      //     throwing this exception.
    return m_qItems.Retrieve(0);                                                                      // <-- The first node of the list is returned.
  }

  template <class QData> inline unsigned long Queue<QData>::Size()
  {
    return m_qItems.Size();                                                                           // <-- Absolutelly self-explanatory.
  }

  template <class QData> inline unsigned long Queue<QData>::Dequeue()
  {
    if (0 == m_qItems.Size())                                                                         // <-- If the queue is empty there is nothing to do but...
      throw ADTE_QUEUE_IS_EMPTY;                                                                      //     throwing this exception.
    return m_qItems.Remove(0);                                                                        // <-- The first node of the list is removed.
  }

  template <class QData> inline unsigned long Queue<QData>::Enqueue(const QData& qItem)
  {
    return m_qItems.Insert(qItem);                                                                    // <-- As simple as inserting a node at the end of the list. If we run out of memory and the new
  }                                                                                                   //     qItem can't be enqueued, Insert throws an ADTE_OUT_OF_MEMORY exception.
}

#endif
