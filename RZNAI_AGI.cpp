// RZNAI_AGI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "RZNAI_AGI.hpp"

#ifndef __RZNAI_AGI_CPP__
#define __RZNAI_AGI_CPP__

using namespace std;

unsigned __int64* simp_vector_create (__int64 init_sz) {

    unsigned __int64* ret = new unsigned __int64[init_sz];
    return ret;

}

unsigned __int64 simp_vector_read(unsigned __int64* v, __int64 vtop, __int64 vcap, __int64 loc) {

    if (loc > vtop)
        return 0;

    return v[loc];
}

void simp_vector_append(unsigned __int64** v, __int64 * vtop, __int64 * vcap, unsigned __int64 data) {

    *vtop = *vtop + 1;

    if (*vtop == *vcap)
        *v[*vtop] = data;
    else {
        unsigned __int64* newv = new unsigned __int64[*vcap * 2];
        for (__int64 i = 0; i < *vcap * 2; i++)
            newv[i] = 0;
        for (__int64 i = 0; i < *vcap; i++)
            newv[i] = *v[i];
        *vcap *= 2;
        delete[] * v;
        *v = newv;
    }

}

unsigned __int64 * simp_stack_create(__int64 * tos) {

    *tos = 0;
    return simp_vector_create(16);

}

unsigned __int64 simp_stack_pop(unsigned __int64* s, __int64* tos, __int64 vtop, __int64 vcap) {

    if (*tos == 0)
        return 0;
    else {
        *tos = *tos - 1;
        return simp_vector_read (s, vtop, vcap, *tos + 1);
    }
}

void simp_stack_push(unsigned __int64** s, __int64* tos, __int64* vcap, __int64 data) {

    simp_vector_append(s, tos, vcap, data);
    *tos = *tos + 1;

}


//https ://www.geeksforgeeks.org/dsa/2-satisfiability-2-sat-problem/

// C++ implementation to find if the given
// expression is satisfiable using the
// Kosaraju's Algorithm

// data structures used to implement Kosaraju's
// Algorithm. Please refer
// https://www.geeksforgeeks.org/strongly-connected-components/

// adds edges to form the original graph
void addEdges(int a, int b)
{
    adj[a].push_back(b);
}

// add edges to form the inverse graph
void addEdgesInverse(int a, int b)
{
    adjInv[b].push_back(a);
}

// for STEP 1 of Kosaraju's Algorithm
void dfsFirst(int u)
{
    if (visited[u])
        return;

    visited[u] = 1;

    for (int i = 0; i < adj[u].size(); i++)
        dfsFirst(adj[u][i]);

    s.push(u);
}

// for STEP 2 of Kosaraju's Algorithm
void dfsSecond(int u)
{
    if (visitedInv[u])
        return;

    visitedInv[u] = 1;

    for (int i = 0; i < adjInv[u].size(); i++)
        dfsSecond(adjInv[u][i]);

    scc[u] = counter;
}

// function to check 2-Satisfiability
void is2Satisfiable(int n, int m, int a[], int b[])
{
    // adding edges to the graph
    for (int i = 0; i < m; i++)
    {
        // variable x is mapped to x
        // variable -x is mapped to n+x = n-(-x)

        // for a[i] or b[i], addEdges -a[i] -> b[i]
        // AND -b[i] -> a[i]
        if (a[i] > 0 && b[i] > 0)
        {
            addEdges(a[i] + n, b[i]);
            addEdgesInverse(a[i] + n, b[i]);
            addEdges(b[i] + n, a[i]);
            addEdgesInverse(b[i] + n, a[i]);
        }

        else if (a[i] > 0 && b[i] < 0)
        {
            addEdges(a[i] + n, n - b[i]);
            addEdgesInverse(a[i] + n, n - b[i]);
            addEdges(-b[i], a[i]);
            addEdgesInverse(-b[i], a[i]);
        }

        else if (a[i] < 0 && b[i]>0)
        {
            addEdges(-a[i], b[i]);
            addEdgesInverse(-a[i], b[i]);
            addEdges(b[i] + n, n - a[i]);
            addEdgesInverse(b[i] + n, n - a[i]);
        }

        else
        {
            addEdges(-a[i], n - b[i]);
            addEdgesInverse(-a[i], n - b[i]);
            addEdges(-b[i], n - a[i]);
            addEdgesInverse(-b[i], n - a[i]);
        }
    }

    // STEP 1 of Kosaraju's Algorithm which
    // traverses the original graph
    for (int i = 1; i <= 2 * n; i++)
        if (!visited[i])
            dfsFirst(i);

    // STEP 2 of Kosaraju's Algorithm which
    // traverses the inverse graph. After this,
    // array scc[] stores the corresponding value
    while (!s.empty())
    {
        int n = s.top();
        s.pop();

        if (!visitedInv[n])
        {
            dfsSecond(n);
            counter++;
        }
    }

    for (int i = 1; i <= n; i++)
    {
        // for any 2 variable x and -x lie in
        // same SCC
        if (scc[i] == scc[i + n])
        {
            // not satisfiable
            return;
        }
    }

    // no such variables x and -x exist which lie
    // in same SCC
    // 
    // satisfiable

    return;
}

Dict_Entry** create_dict (__int64 prime_sz) {

    Dict_Entry** ret = new Dict_Entry * [prime_sz];

    for (__int64 i = 0; i < prime_sz; i++) {
        ret[i] = new Dict_Entry ();
        ret[i]->next = NULL;
        ret[i]->state_action = 0;
        ret[i]->vectored_state = 0;
    }

    return ret;

}

void create_dict_entry(Dict_Entry** d, __int64 prime_sz, unsigned __int64 sa, unsigned __int32 vs) {

    Dict_Entry* p = d [sa % prime_sz];

    while (p->next != NULL && p->state_action < sa)
        p = p->next;

    if (p->state_action == sa)
        p->vectored_state = vs;
    else {
        Dict_Entry* temp = p->next;
        p->next = new Dict_Entry();
        p = p->next;
        p->state_action = sa;
        p->vectored_state = vs;
        p->next = temp;
    }

}

void remove_dict_entry (Dict_Entry** d, __int64 prime_sz, unsigned __int64 sa) {

    Dict_Entry* p = d[sa % prime_sz];

    if (p->next == NULL)
        return;

    while (p->next->next != NULL && p->next->state_action != sa)
        p = p->next;

    if (p->next->state_action == sa) {
        Dict_Entry* dump = p->next;
        p->next = p->next->next;
        delete dump;
    }

}

void instantiate() {

}

//  Driver function to test above functions
int main()
{
    // n is the number of variables
    // 2n is the total number of nodes
    // m is the number of clauses
    int n = 5, m = 7;

    // each clause is of the form a or b
    // for m clauses, we have a[m], b[m]
    // representing a[i] or b[i]

    // Note:
    // 1 <= x <= N for an uncomplemented variable x
    // -N <= x <= -1 for a complemented variable x
    // -x is the complement of a variable x

    // The CNF being handled is:
    // '+' implies 'OR' and '*' implies 'AND'
    // (x1+x2)*(x2’+x3)*(x1’+x2’)*(x3+x4)*(x3’+x5)*
    // (x4’+x5’)*(x3’+x4)
    int a[] = { 1, -2, -1, 3, -3, -4, -3 };
    int b[] = { 2, 3, -2, 4, 5, -5, 4 };

    // We have considered the same example for which
    // Implication Graph was made
    is2Satisfiable(n, m, a, b);

    return 0;
}

#endif

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
