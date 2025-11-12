#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool equalPathsHelper(Node* node, int& height) {
    if (!node) {               
        height = -1;
        return true;
    }

    int hl, hr;
    bool okL = equalPathsHelper(node->left,  hl);
    bool okR = equalPathsHelper(node->right, hr);

    
    height = 1 + (hl > hr ? hl : hr);

    if (!okL || !okR) return false;

    
    if (node->left != nullptr && node->right != nullptr) {
        return hl == hr;
    }
    return true;
}

bool equalPaths(Node * root)
{
    // Add your code below
     int h;
    return equalPathsHelper(root, h);
}

