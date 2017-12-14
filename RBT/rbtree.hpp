////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Реализация классов красно-черного дерева
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      01.05.2017
///            This is a part of the course "Algorithms and Data Structures" 
///            provided by  the School of Software Engineering of the Faculty 
///            of Computer Science at the Higher School of Economics.
///
/// "Реализация" (шаблонов) методов, описанных в файле rbtree.h
///
////////////////////////////////////////////////////////////////////////////////

#include <stdexcept>        // std::invalid_argument


namespace xi {


//==============================================================================
// class RBTree::node
//==============================================================================

template <typename Element, typename Compar >
RBTree<Element, Compar>::Node::~Node()
{

}



template <typename Element, typename Compar>
typename RBTree<Element, Compar>::Node* RBTree<Element, Compar>::Node::setLeft(Node* lf)
{
    // предупреждаем повторное присвоение
    if (_left == lf)
        return nullptr;

    // если новый левый — действительный элемент
    if (lf)
    {
        // если у него был родитель
        if (lf->_parent)
        {
            // ищем у родителя, кем был этот элемент, и вместо него ставим бублик
            if (lf->_parent->_left == lf)
                lf->_parent->_left = nullptr;
            else                                    // доп. не проверяем, что он был правым, иначе нарушение целостности
                lf->_parent->_right = nullptr;      
        }

        // задаем нового родителя
        lf->_parent = this;
    }

    // если у текущего уже был один левый — отменяем его родительскую связь и вернем его
    Node* prevLeft = _left;
    _left = lf;

    if (prevLeft)
        prevLeft->_parent = nullptr;

    return prevLeft;
}


template <typename Element, typename Compar>
typename RBTree<Element, Compar>::Node* RBTree<Element, Compar>::Node::setRight(Node* rg)
{
    // предупреждаем повторное присвоение
    if (_right == rg)
        return nullptr;

    // если новый правый — действительный элемент
    if (rg)
    {
        // если у него был родитель
        if (rg->_parent)
        {
            // ищем у родителя, кем был этот элемент, и вместо него ставим бублик
            if (rg->_parent->_left == rg)
                rg->_parent->_left = nullptr;
            else                                    // доп. не проверяем, что он был правым, иначе нарушение целостности
                rg->_parent->_right = nullptr;
        }

        // задаем нового родителя
        rg->_parent = this;
    }

    // если у текущего уже был один левый — отменяем его родительскую связь и вернем его
    Node* prevRight = _right;
    _right = rg;

    if (prevRight)
        prevRight->_parent = nullptr;

    return prevRight;
}


//==============================================================================
// class RBTree
//==============================================================================

template <typename Element, typename Compar >
RBTree<Element, Compar>::RBTree()
{
    NIL = new Node();
    _root = NIL;
    _dumper = nullptr;
}

template <typename Element, typename Compar >
RBTree<Element, Compar>::~RBTree()
{
    // грохаем пока что всех через корень
    clear(_root);
    delete NIL;
}

template <typename Element, typename Compar >
void RBTree<Element, Compar>::clear(Node* root)
{
    if (root->_left != NIL)
    {
        clear(root->_left);
        delete root->_left;
    }
       
    if (root->_right != NIL)
    {
        clear(root->_right);
        delete root->_right;
    }
        
}


template <typename Element, typename Compar >
void RBTree<Element, Compar>::deleteNode(Node* nd)
{
    // если переданный узел не существует, просто ничего не делаем, т.к. в вызывающем проверок нет
    if (nd == nullptr)
        return;

    // потомков убьет в деструкторе
    delete nd;
}


template <typename Element, typename Compar >
void RBTree<Element, Compar>::insert(const Element& key)
{
   // вставляем как в обычное дерево
    Node* newNode = insertNewBstEl(key);

    // отладочное событие
    if (_dumper)
        _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_BST_INS, this, newNode);

    rebalance(newNode);

    // отладочное событие
    if (_dumper)
        _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_INSERT, this, newNode);

}




template <typename Element, typename Compar>
const typename RBTree<Element, Compar>::Node* RBTree<Element, Compar>::find(const Element& key)
{
    Node* startNode = _root;
    while (startNode != NIL && (_compar(key, startNode->_key) || _compar(startNode->_key,key)))
    {
        if (_compar(key, startNode->_key))
            startNode = startNode->_left;
        else
            startNode = startNode->_right;
    }
    return startNode;
}

template <typename Element, typename Compar >
typename RBTree<Element, Compar>::Node* 
        RBTree<Element, Compar>::insertNewBstEl(const Element& key)
{
    Node* insNode = new Node(key);
    insNode->_color = Color::RED;
    Node* searchNode = _root;
    Node* parentNode = NIL;
    while (searchNode != NIL)
    {
        parentNode = searchNode;
        if (_compar(key, searchNode->_key))
            searchNode = searchNode->_left;
        else if (!_compar(key, searchNode->_key))
            searchNode = searchNode->_right;
    }
    insNode->_parent = parentNode;
    if (parentNode == NIL)
        _root = insNode;
    else if (_compar(key, parentNode->_key))
        parentNode->_left = insNode;
    else
        parentNode->_right = insNode;
    insNode->_left = NIL;
    insNode->_right = NIL;
    return insNode;
}


template <typename Element, typename Compar >
typename RBTree<Element, Compar>::Node* 
    RBTree<Element, Compar>::rebalanceDUG(Node* nd)
{
    
    //родитель, дедушка и дядя
    Node* dad = nd->_parent;
    Node* grandpa = dad->_parent;
    Node* uncle = (dad == grandpa->_left)?grandpa->_right:grandpa->_left; // для левого случая нужен правый дядя и наоборот.

    // если дядя такой же красный, как сам нод и его папа...
    if (uncle->isRed())
    {
        // перекрашиваем "родственников"
        uncle->_color = Color::BLACK;
        dad->_color = Color::BLACK;
        grandpa->_color = Color::RED;
        
        // отладочное событие
        if (_dumper)
            _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_RECOLOR1, this, nd);

        // теперь чередование цветов "узел-папа-дедушка-дядя" — К-Ч-К-Ч, но надо разобраться, что там
        // с дедушкой и его предками, поэтому продолжим с дедушкой
        nd = grandpa;
    }

    // дядя черный
    // смотрим, является ли узел "правильно-правым" у папочки
    else if (!uncle->isRed())                                        // для левого случая нужен правый узел, поэтом отрицание
    {                                               // CASE2 в действии
        if (dad == grandpa->_left)
        {
            if (nd == nd->_parent->_right)
            {
                nd = dad;
                rotLeft(nd);
            }
            nd->_parent->_color = Color::BLACK;
            nd->_parent->_parent->_color = Color::RED;
            nd->_parent->_right->_color = Color::BLACK;
            rotRight(nd->_parent->_parent);
            if (_dumper)
                _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_RECOLOR3D, this, nd);
        }
        else
        {
            if (nd == dad->_parent->_left)
            {
                nd = dad;
                rotRight(nd);
            }
            nd->_parent->_color = Color::BLACK;
            nd->_parent->_parent->_color = Color::RED;
            nd->_parent->_left->_color = Color::BLACK;
            rotLeft(nd->_parent->_parent);
            if (_dumper)
                _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_RECOLOR3G, this, nd);
        }
    }


    return nd;
}


template <typename Element, typename Compar >
void RBTree<Element, Compar>::rebalance(Node* nd)
{


    // пока папа — цвета пионерского галстука, действуем
    while (nd->_parent->_color == Color::RED)
    {
        rebalanceDUG(nd);
    } 

    _root->_color = Color::BLACK;

}



template <typename Element, typename Compar>
void RBTree<Element, Compar>::rotLeft(typename RBTree<Element, Compar>::Node* nd)
{
    // правый потомок, который станет после левого поворота "выше"
    Node* r = nd->_right;  
    if (!r)
        throw std::invalid_argument("Can't rotate left since the right child is nil");
    if (nd->_parent == NIL)
        _root = r;
    else
    {
        if (nd == nd->_parent->_left)
            nd->_parent->_left = r;
        else
            nd->_parent->_right = r;
    }
    r->_parent = nd->_parent;
    nd->_right = r->_left;
    r->_left->_parent = nd;
    r->_left = nd;
    nd->_parent = r;
    // отладочное событие
    if (_dumper)
        _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_LROT, this, nd);
}



template <typename Element, typename Compar>
void RBTree<Element, Compar>::rotRight(typename RBTree<Element, Compar>::Node* nd)
{
    Node* l = nd->_left;
    if (!l)
        throw std::invalid_argument("Can't rotate right since the left child is nil");
    if (nd->_parent == NIL)
        _root = l;
    else
    {
        if (nd == nd->_parent->_left)
            nd->_parent->_left = l;
        else
            nd->_parent->_right = l;
    }
    l->_parent = nd->_parent;
    nd->_left = l->_right;
    l->_right->_parent = nd;
    l->_right = nd;
    nd->_parent = l;
    if (_dumper)
        _dumper->rbTreeEvent(IRBTreeDumper<Element, Compar>::DE_AFTER_RROT, this, nd);
}


} // namespace xi

