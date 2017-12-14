////////////////////////////////////////////////////////////////////////////////
// Module Name:  main.cpp
// Authors:      Sergey Shershakov
// Version:      0.1.0
// Date:         01.05.2017
//
// This is a part of the course "Algorithms and Data Structures" 
// provided by  the School of Software Engineering of the Faculty 
// of Computer Science at the Higher School of Economics.
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "rbtree.h"
#include <ctime>


using namespace std;


void simplestTest()
{
    using namespace xi;
    srand(time(0));
    // просто создаем объект дерева
    RBTree<int> tree1;
    int a[15]{ 10,9,20,-7,4,1,0,5,-2,12,6,15,-21,25,8 };
    /*for (int i = 0; i < 200; i++)
        a[i] = i;
    for (int i = 0; i < 200; i++)
    {
        int j = rand() % 200;
        int t = a[i];
        a[i] = a[j];
        a[j] = t;
    }*/
    for (int i : a)
        tree1.insert(i);
    const RBTree<int>::Node* nd = tree1.find(20);
    cout << nd->getKey() << endl;
    cout << 1 << endl;
}





int main()
{
    cout << "Hello, World!" << endl;

    simplestTest();


    return 0;
}