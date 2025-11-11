// RZNAI_AGI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "RZNAI_AGI.hpp"

#ifndef __RZNAI_AGI_CPP__
#define __RZNAI_AGI_CPP__

using namespace std;

void simp_queue_enqueue(Simp_Queue* queue, Simp_Queue* parm) {

    Simp_Queue* temp = queue->next;
    queue->next = parm;
    parm->next = temp;

}

Simp_Queue* simp_queue_dequeue(Simp_Queue* queue) {

    Simp_Queue* mover = queue;
    if (mover->next == 0)
        return 0;
    while (mover->next->next != 0)
        mover = mover->next;

    Simp_Queue* ret = mover->next;
    mover->next = 0;

    return ret;
}

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
    ret->hidden_sz = ret->in_sz * ret->In_Q_ct;
    ret->hidden_ct = 16;

    ret->input_weights = new __int32* [ret->hidden_sz];
    ret->input_targets = new __int32* [ret->hidden_sz];
    for (__int32 i = 0; i < ret->hidden_sz; i++) {
        ret->input_weights[i] = new __int32[ret->hidden_sz >> 1];
        ret->input_targets[i] = new __int32[ret->hidden_sz >> 1];
        for (__int32 j = 0; j < ret->hidden_sz >> 1; j++) {
            ret->input_weights[i][j] = j % 2 == 0 ? -16384 : 16384;
            ret->input_targets[i][j] = (j << 1) % ret->hidden_sz;
        }
    }

    ret->hidden = new IntNNL * [ret->hidden_ct];
    for (__int32 count = 0; count < ret->hidden_ct; count++) {
        ret->hidden [count] = new IntNNL();
        ret->hidden [count]->weights = new __int32* [ret->hidden_sz];
        ret->hidden [count]->targets = new __int32* [ret->hidden_sz];
        for (__int32 i = 0; i < ret->hidden_sz; i++) {
            ret->hidden[count]->weights[i] = new __int32[ret->hidden_sz >> 1];
            ret->hidden[count]->targets[i] = new __int32[ret->hidden_sz >> 1];
            for (__int32 j = 0; j < ret->hidden_sz >> 1; j++) {
                ret->hidden[count]->weights[i][j] = j % 2 == 0 ? -16384 : 16384;
                ret->hidden[count]->targets[i][j] = (j << 1) % ret->hidden_sz;
            }
        }
        ret->hidden[count]->firings = new bool[ret->hidden_sz];
        for (__int32 i = 0; i < ret->hidden_sz; i++)
            ret->hidden[count]->firings[i] = false;
    }

    ret->cycles_to_dec = 1048576;
    ret->dec_amt = 1;
    ret->inc_amt = 1;

    ret->kbpsz = 7919;
    ret->kbsz = 0;
    ret->kbsts = 0;
    ret->Knowledge_Bank = create_dict(ret->kbpsz);

    ret->rwcap = 16;
    ret->rwtop = -1;
    ret->rewards = simp_vector_create(ret->rwcap);
    ret->dvcap = 16;
    ret->dvtop = -1;
    ret->dsnctvs = simp_vector_create(ret->dvcap);

    return ret;
}

__int32* executeBFS(AGI_Sys* stm, __int32 cur, bool rw, __int32 ix) {

    __int32* parent = new __int32[stm->kbsts];

    Simp_Queue* bfs_queue = new Simp_Queue();

    bool* visited = new bool [stm->kbsts];

    for (__int32 i = 0; i < stm->kbsts; i++)
        visited[i] = false;

    visited[cur] = true;

    Simp_Queue* c = new Simp_Queue();
    c->data = cur;
    simp_queue_enqueue (bfs_queue, c);

    while (!bfs_queue->next != 0) {
        Simp_Queue* cv = simp_queue_dequeue(bfs_queue);

        Dict_Entry* pos = stm->Knowledge_Bank[cv->data % stm->kbpsz]->next;
        while (pos != 0 && pos->init_state != cv->data)
            pos = pos->next;

        if (pos == 0)
            continue;

        while ( pos != 0 && pos->init_state == cv->data) {

            if (!visited[pos->vect_state]) {
                parent[pos->vect_state] = pos->init_state;
                visited[pos->vect_state] = true;
                Simp_Queue* v = new Simp_Queue();
                v->data = pos->vect_state;
                simp_queue_enqueue(bfs_queue, v);
            }

            pos = pos->next;
        }
    }

    // count path from stm->rewards[ix]/stm->dsnctvs[i] back to cur

    __int32 count_path = 1;
    for (__int32 tracker = rw ? stm->rewards[ix] : stm->dsnctvs[ix]; parent[tracker] != cur; tracker = parent[tracker])
        count_path++;

    __int32* ret = new __int32[count_path];
    ret[count_path-1] = rw ? stm->rewards[ix] : stm->dsnctvs[ix];
    __int32 tracker = rw ? stm->rewards[ix] : stm->dsnctvs[ix];
    for (__int32 i = 0; i < count_path; i++) {
        parent[count_path - 1 - i] = parent[tracker];
        tracker = parent[tracker];
    }

    return ret;
}

void generateBFSs(AGI_Sys* stm) {

    // create space for the rewards and disincentives

    __int32** rwpaths = new __int32* [stm->rwtop + 1];
    __int32** dvpaths = new __int32* [stm->dvtop + 1];

    for (__int32 i = 0; i < stm->rwtop + 1; i++)
        rwpaths [i] = executeBFS(stm, stm->Current_Input, true, i);
       
    for (__int32 i = 0; i < stm->dvtop + 1; i++)
        dvpaths [i] = executeBFS(stm, stm->Current_Input, false, i);

}

void perform_iann(AGI_Sys* stm) {

    bool* input_b = new bool[stm->in_sz * stm->In_Q_ct];

    for (__int32 i = 0; i < stm->In_Q_ct; i++) {

        __int32 temp_input = stm->Input_Queue[i];

        for (__int32 j = 0; j < stm->in_sz; j++) {
            input_b[i * stm->in_sz + j] = temp_input & 1;
            temp_input >> 1;
        }

    }

    for (__int32 i = 0; i < stm->hidden_sz; i++) {
        __int32 * weight_sums = new __int32 [stm->hidden_sz];
        for (__int32 j = 0; j < stm->hidden_sz; j++) {
            weight_sums[j] = 0;
            for (__int32 k = 0; k < stm->hidden_sz >> 1; k++)
                weight_sums[stm->input_targets[j][k]] += input_b[i] ? stm->input_weights[j][k] : 0;
            if (weight_sums[j] >= 0)
                stm->hidden[0]->firings[j] = true;
        }
    }

    for (__int32 count = 1; count < stm->hidden_ct; count++) {

        for (__int32 i = 0; i < stm->hidden_sz; i++) {
            __int32* weight_sums = new __int32[stm->hidden_sz];
            for (__int32 j = 0; j < stm->hidden_sz; j++) {
                weight_sums[j] = 0;
                for (__int32 k = 0; k < stm->hidden_sz >> 1; k++)
                    weight_sums[stm->hidden[count - 1]->targets[j][k]] += stm->hidden [count - 1]->firings[i] ? stm->hidden [count - 1]->weights[j][k] : 0;
                if (weight_sums[j] >= 0)
                    stm->hidden[i]->firings[j] = true;
            }
        }

    }
}

void cycle(AGI_Sys * stm) {

    // read input

    // update Knowledge Bank with (previous input state, output action) -> (newly read input)

    // feed into IANN and fetch output bit sequence

    // construct 2SAT instance if read from recall input bit is set to 0 and output recall bit is set to 1

        // if ( input state, output action ) exists in Knowledge Bank, fetch input state vectored to it
        // else if ( input state, output action ) does not exist in Knowledge Bank, return null (0) input state

    // else if read from recall input bit is set to 1 and output recall bit is set to 1

        // if ( input state, output action ) exists in Knowledge Bank, fetch input state vectored to it
        // else if ( input state, output action ) does not exist in Knowledge Bank, return null (0) input state

    // else read from sensory if output recall bit is set to 0

    // fetch any reward or disincentive feedback and take appropriate action on IANN as well as update AGI_Sys reward and disincentive vectors

}

//  Driver function to test above functions
int main()
{
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
