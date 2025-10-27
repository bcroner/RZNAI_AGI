// RZNAI_AGI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "RZNAI_AGI.hpp"

#ifndef __RZNAI_AGI_CPP__
#define __RZNAI_AGI_CPP__

using namespace std;

__int32* simp_vector_create (__int32 init_sz) {

    __int32* ret = new __int32[init_sz];
    return ret;

}

__int32 simp_vector_read(__int32* v, __int32 vtop, __int32 vcap, __int32 loc) {

    if (loc > vtop)
        return 0;

    return v[loc];
}

void simp_vector_append(__int32** v, __int32 * vtop, __int32 * vcap, __int32 data) {

    *vtop = *vtop + 1;

    if (*vtop < *vcap)
        *v[*vtop] = data;
    else {
        __int32* newv = new __int32[*vcap * 2];
        for (__int64 i = 0; i < *vcap * 2; i++)
            newv[i] = 0;
        for (__int64 i = 0; i < *vcap; i++)
            newv[i] = *v[i];
        *vcap *= 2;
        delete[] * v;
        *v = newv;
        *v[*vtop] = data;
    }

}

__int32 * simp_stack_create(__int32 * vtop) {

    *vtop = -1;
    return simp_vector_create(16);

}

__int32 simp_stack_pop(__int32* s, __int32 * vtop, __int32 vcap) {

    if (*vtop == -1)
        return 0;
    else {
        *vtop = *vtop - 1;
        return simp_vector_read (s, *vtop, vcap, *vtop + 1);
    }
}

void simp_stack_push(__int32** s, __int32* vtop, __int32* vcap, __int32 data) {

    simp_vector_append(s, vtop, vcap, data);
    *vtop = *vtop + 1;

}

//https://www.geeksforgeeks.org/dsa/2-satisfiability-2-sat-problem/

// C++ implementation to find if the given
// expression is satisfiable using the
// Kosaraju's Algorithm

// data structures used to implement Kosaraju's
// Algorithm. Please refer
// https://www.geeksforgeeks.org/strongly-connected-components/

// adds edges to form the original graph
void addEdges(AGI_Sys * stm, __int32 a, __int32 b)
{
    simp_stack_push ( & (stm->adj[a]), & (stm->adj_top), & (stm->adj_cap), b);
}

// add edges to form the inverse graph
void addEdgesInverse(AGI_Sys* stm, __int32 a, __int32 b)
{
    simp_stack_push ( & (stm->adjInv[b]), & (stm->adjInv_top), & (stm->adjInv_cap), a);
}

// for STEP 1 of Kosaraju's Algorithm
void dfsFirst(AGI_Sys* stm, __int32 u)
{
    if (stm->visited[u])
        return;

    stm->visited[u] = 1;

    for (int i = 0; i < stm->adj_sz[u]; i++)
        dfsFirst(stm, stm->adj[u][i]);

    simp_stack_push(stm->Stack, & (stm->Stack_top), & (stm->Stack_cap), u);
}

// for STEP 2 of Kosaraju's Algorithm
void dfsSecond(AGI_Sys* stm, __int32 u)
{
    if (stm->visitedInv[u])
        return;

    stm->visitedInv[u] = 1;

    for (int i = 0; i < stm->adjInv_sz[u]; i++)
        dfsSecond(stm, stm->adjInv[u][i]);

    stm->scc[u] = stm->counter;
}

// function to check 2-Satisfiability
void is2Satisfiable(AGI_Sys* stm, __int32 n, __int32 m, __int32 a[], __int32 b[])
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
            addEdges(stm, a[i] + n, b[i]);
            addEdgesInverse(stm, a[i] + n, b[i]);
            addEdges(stm, b[i] + n, a[i]);
            addEdgesInverse(stm, b[i] + n, a[i]);
        }

        else if (a[i] > 0 && b[i] < 0)
        {
            addEdges(stm, a[i] + n, n - b[i]);
            addEdgesInverse(stm, a[i] + n, n - b[i]);
            addEdges(stm, -b[i], a[i]);
            addEdgesInverse(stm, -b[i], a[i]);
        }

        else if (a[i] < 0 && b[i]>0)
        {
            addEdges(stm, -a[i], b[i]);
            addEdgesInverse(stm, -a[i], b[i]);
            addEdges(stm, b[i] + n, n - a[i]);
            addEdgesInverse(stm, b[i] + n, n - a[i]);
        }

        else
        {
            addEdges(stm, -a[i], n - b[i]);
            addEdgesInverse(stm, -a[i], n - b[i]);
            addEdges(stm, -b[i], n - a[i]);
            addEdgesInverse(stm, -b[i], n - a[i]);
        }
    }

    // STEP 1 of Kosaraju's Algorithm which
    // traverses the original graph
    for (int i = 1; i <= 2 * n; i++)
        if (!stm->visited[i])
            dfsFirst(stm, i);

    // STEP 2 of Kosaraju's Algorithm which
    // traverses the inverse graph. After this,
    // array scc[] stores the corresponding value
    while (stm->Stack_top != -1)
    {
        int n = *stm->Stack[stm->Stack_top];
        simp_stack_pop(*stm->Stack, & (stm->Stack_top), stm->Stack_cap);

        if (!stm->visitedInv[n])
        {
            dfsSecond(stm, n);
            stm->counter++;
        }
    }

    for (int i = 1; i <= n; i++)
    {
        // for any 2 variable x and -x lie in
        // same SCC
        if (stm->scc[i] == stm->scc[i + n])
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
        ret[i]->next = 0;
        ret[i]->init_state = 0;
        ret[i]->action_out = 0;
        ret[i]->vect_state = 0;
    }

    return ret;

}

void create_dict_entry(Dict_Entry** d, __int64 prime_sz, __int32 is, __int32 ao, __int32 vs) {

    Dict_Entry* p = d [is % prime_sz];

    while (p->next != 0 && ( p->init_state < is || (p->init_state == is && p->action_out < ao)))
        p = p->next;

    if (p->init_state == is && p->action_out == ao)
        p->vect_state = vs;
    else {
        Dict_Entry* temp = p->next;
        p->next = new Dict_Entry();
        p = p->next;
        p->init_state = is;
        p->action_out = ao;
        p->vect_state = vs;
        p->next = temp;
    }

}

void remove_dict_entry (Dict_Entry** d, __int64 prime_sz, __int32 is, __int32 ao) {

    Dict_Entry* p = d[is % prime_sz];

    if (p->next == 0)
        return;

    while (p->next->next != 0 && (p->next->init_state == is || p->next->action_out == ao))
        p = p->next;

    if (p->next->init_state == is && p->next->action_out == ao) {
        Dict_Entry* dump = p->next;
        p->next = p->next->next;
        delete dump;
    }

}

AGI_Sys * instantiate() {

    AGI_Sys * ret = new AGI_Sys();

    ret->in_sz = 16;
    ret->out_sz = 4;
    ret->In_Q_ct = 7;
    ret->first_sz = ret->in_sz * ret->In_Q_ct;
    ret->last_sz = ret->out_sz;
    ret->hidden_sz = ret->first_sz * 2;
    ret->hidden_ct = 16;

    ret->first = new IntNNL();
    ret->first->weights = new __int32* [ret->first_sz];
    for (__int32 i = 0; i < ret->first_sz; i++)
        ret->first->weights[i] = new __int32[ret->first_sz];

    ret->last = new IntNNL();
    ret->last->weights = new __int32* [ret->last_sz];
    for (__int32 i = 0; i < ret->last_sz; i++)
        ret->last->weights[i] = new __int32[ret->last_sz];

    ret->hidden = new IntNNL * [ret->hidden_ct];
    for (__int32 count = 0; count < ret->hidden_ct; count++) {
        ret->hidden[count]->weights = new __int32* [ret->hidden_sz];
        for (__int32 i = 0; i < ret->hidden_sz; i++)
            ret->hidden[count]->weights[i] = new __int32[ret->hidden_sz];
    }

    ret->first = new IntNNL [ret->first_sz];
    ret->last = new IntNNL [ret->last_sz];
    ret->hidden = new IntNNL * [ret->hidden_sz];
    for (__int32 i = 0; i < ret->hidden_ct; i++)
        ret->hidden[i] = new IntNNL[ret->hidden_sz];

    ret->kbpsz = 7919;
    ret->kbsz = 0;
    ret->Knowledge_Bank = create_dict(ret->kbpsz);

    ret->rwcap = 16;
    ret->rwtop = -1;
    ret->rewards = simp_vector_create(ret->rwcap);
    ret->dvcap = 16;
    ret->dvtop = -1;
    ret->dsnctvs = simp_vector_create(ret->dvcap);

    return ret;
}

void generate2SATs(AGI_Sys* stm) {

    // create space for the rewards and disincentives

    simp_vector_append(&(stm->KB_2CNF_A), &(stm->kbatop), &(stm->kbacap), 0);
    simp_vector_append(&(stm->KB_2CNF_A), &(stm->kbatop), &(stm->kbacap), 0);
    simp_vector_append(&(stm->KB_2CNF_B), &(stm->kbatop), &(stm->kbacap), 0);
    simp_vector_append(&(stm->KB_2CNF_B), &(stm->kbbtop), &(stm->kbbcap), 0);

    for (__int32 i = 0; i < stm->kbpsz; i++) {

        Dict_Entry* d = stm->Knowledge_Bank[i];

    }

    for (__int32 i = 0; i <= stm->rwtop; i++) {

    }

    for (__int32 i = 0; i <= stm->dvtop; i++) {

    }

}

void perform_inn(AGI_Sys* stm , __int32 input) {

    bool* input_b = new bool[stm->in_sz];

    for (__int32 i = 0; i < stm->in_sz; i++)
        input_b[i] = false;

    for (__int32 i = 0; i < stm->in_sz; i++) {
        input_b[i] = input & 1;
        input = input >> 1;
    }

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

    AGI_Sys * stm = instantiate();

    // We have considered the same example for which
    // Implication Graph was made
    is2Satisfiable(stm, n, m, a, b);

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
