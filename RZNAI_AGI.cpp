// RZNAI_AGI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "RZNAI_AGI.hpp"

#ifndef __RZNAI_AGI_CPP__
#define __RZNAI_AGI_CPP__

// Integration knobs.  All three default to the standalone behaviour, so an
// unconfigured build is unchanged.
//
//   RZNAI_AGI_MAX_CYCLES        how long cycle() runs before terminating
//   RZNAI_AGI_EXTERNAL_SENSORS  define to supply your own in_0 / in_1 and
//                               leave the simulation stubs out of the build
//   RZNAI_AGI_NO_MAIN           define to supply your own driver

#ifndef RZNAI_AGI_MAX_CYCLES
#define RZNAI_AGI_MAX_CYCLES 2000000000
#endif

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
        (*v)[*vtop] = data;
    else {
        __int32* newv = new __int32[*vcap * 2];
        for (__int64 i = 0; i < *vcap * 2; i++)
            newv[i] = 0;
        for (__int64 i = 0; i < *vcap; i++)
            newv[i] = (*v)[i];
        *vcap *= 2;
        delete[] * v;
        *v = newv;
        (*v)[*vtop] = data;
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
    ret->out_addr_sz = 2;
    ret->sensory_bits = 1;
    ret->In_Q_ct = 7;
    ret->hidden_sz = ret->in_sz * ret->In_Q_ct * 2;
    ret->hidden_ct = 16;

    ret->output_weights = new __int32*[ret->hidden_sz];
    ret->output_targets = new __int32*[ret->hidden_sz];
    for (__int32 i = 0; i < ret->hidden_sz; i++) {
        ret->output_weights [i] = new __int32 [ret->out_sz >> 1];
        ret->output_targets [i] = new __int32 [ret->out_sz >> 1];
        for (__int32 j = 0; j < ret->out_sz >> 1; j++) {
            ret->output_weights[i][j] = (j % 2 == 0 ? -16384 : 16384);
            ret->output_targets[i][j] = j % ret->out_sz;
        }
    }

    ret->input_weights = new __int32* [ret->in_sz * ret->In_Q_ct];
    ret->input_targets = new __int32* [ret->in_sz * ret->In_Q_ct];
    for (__int32 i = 0; i < ret->in_sz * ret->In_Q_ct; i++) {
        ret->input_weights[i] = new __int32[ret->hidden_sz >> 1];
        ret->input_targets[i] = new __int32[ret->hidden_sz >> 1];
        for (__int32 j = 0; j < ret->hidden_sz >> 1; j++) {
            ret->input_weights[i][j] = ( j % 2 == 0 ? -16384 : 16384 );
            ret->input_targets[i][j] = j % ret->hidden_sz;
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
                ret->hidden[count]->targets[i][j] = j % ret->hidden_sz;
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

    for (__int32 i = 0; i < ret->kbpsz; i++) {
        ret->Knowledge_Bank[i] = new Dict_Entry();
        ret->Knowledge_Bank[i]->next = 0;
        ret->Knowledge_Bank[i]->action_out = 0;
        ret->Knowledge_Bank[i]->init_state = 0;
        ret->Knowledge_Bank[i]->vect_state = 0;
    }

    // cycle() shifts and writes Input_Queue on its very first statement, and
    // perform_iann() reads it; nothing allocated it.
    ret->Current_Input = 0;
    ret->Input_Queue = new __int32 [ret->In_Q_ct];
    for (__int32 i = 0; i < ret->In_Q_ct; i++)
        ret->Input_Queue[i] = 0;

    ret->kb_rw_path = 0;
    ret->kb_dv_path = 0;

    ret->rwcap = 16;
    ret->rwtop = -1;
    ret->rewards = simp_vector_create(ret->rwcap);
    ret->dvcap = 16;
    ret->dvtop = -1;
    ret->dsnctvs = simp_vector_create(ret->dvcap);

    return ret;
}

void destroy_agi(AGI_Sys* stm) {

    for (__int32 i = 0; i < stm->out_sz >> 1; i++) {
        delete[] stm->output_weights[i];
        delete[] stm->output_targets[i];
    }
    delete[] stm->output_weights;
    delete[] stm->output_targets;

    for (__int32 i = 0; i < stm->hidden_sz; i++) {
        delete[] stm->input_weights[i];
        delete[] stm->input_targets[i];
    }
    delete[] stm->input_weights;
    delete[] stm->input_targets;

    
    for (__int32 count = 0; count < stm->hidden_ct; count++) {

        for (__int32 i = 0; i < stm->hidden_sz >> 1; i++) {
            delete[] stm->hidden[count]->weights[i];
            delete[] stm->hidden[count]->targets[i];
        }
      
        delete[] stm->hidden[count]->weights;
        delete[] stm->hidden[count]->targets;
        delete[] stm->hidden[count]->firings;
    }

    delete[] stm->hidden;

    for (__int32 i = 0; i < stm->kbpsz; i++)
        delete[] stm->Knowledge_Bank[i];
    delete[] stm->Knowledge_Bank;

    if (stm->kb_rw_path != 0)
        delete[] stm->kb_rw_path;
    if (stm->kb_dv_path != 0)
        delete[] stm->kb_dv_path;

    delete[] stm->rewards;
    delete[] stm->dsnctvs;

}

__int32* executeBFS(AGI_Sys* stm, __int32 cur, bool rw, __int32 ix) {

    const __int32 goal = rw ? stm->rewards[ix] : stm->dsnctvs[ix];

    // UNRESOLVED, and the reason this function cannot yet do its job:
    // parent[] and visited[] are sized by kbsts but indexed by raw state
    // values. kbsts is set to 0 in instantiate() and never incremented, so
    // both arrays are zero-length and `visited[cur] = true` corrupts the heap.
    // Sizing them correctly is not enough either -- states are arbitrary
    // 32-bit input words, not dense node numbers, so the BFS needs a
    // state-to-index mapping that does not exist anywhere in this codebase.
    // Choosing one is a design decision rather than a repair.
    //
    // Until it is made, refuse to search rather than corrupt memory, and
    // return a well-formed single-element path so callers stay safe:
    // generateBFSs() scans the result for `goal`, so this reads as a
    // zero-distance path and costs nothing.
    if (stm->kbsts <= 0) {
        __int32* ret = new __int32[1];
        ret[0] = goal;
        return ret;
    }

    __int32* parent = new __int32[stm->kbsts];
    bool* visited = new bool [stm->kbsts];

    for (__int32 i = 0; i < stm->kbsts; i++) {
        parent[i] = 0;
        visited[i] = false;
    }

    Simp_Queue* bfs_queue = new Simp_Queue();
    bfs_queue->next = 0;

    if (cur >= 0 && cur < stm->kbsts)
        visited[cur] = true;

    Simp_Queue* c = new Simp_Queue();
    c->data = cur;
    c->next = 0;
    simp_queue_enqueue (bfs_queue, c);

    // `while (!bfs_queue->next != 0)` ran the loop only while the queue was
    // EMPTY, then dequeued from it and dereferenced the null result.
    while (bfs_queue->next != 0) {
        Simp_Queue* cv = simp_queue_dequeue(bfs_queue);
        if (cv == 0)
            break;

        __int32 bucket = cv->data % stm->kbpsz;
        if (bucket < 0 || bucket >= stm->kbpsz) {
            delete cv;
            continue;
        }

        Dict_Entry* pos = stm->Knowledge_Bank[bucket]->next;
        while (pos != 0 && pos->init_state != cv->data)
            pos = pos->next;

        while ( pos != 0 && pos->init_state == cv->data) {

            __int32 v = pos->vect_state;
            if (v >= 0 && v < stm->kbsts && !visited[v]) {
                parent[v] = pos->init_state;
                visited[v] = true;
                Simp_Queue* q = new Simp_Queue();
                q->data = v;
                q->next = 0;
                simp_queue_enqueue(bfs_queue, q);
            }

            pos = pos->next;
        }

        delete cv;
    }

    delete bfs_queue;

    // count path from stm->rewards[ix]/stm->dsnctvs[i] back to cur.
    // The original walk had no termination guard, so an unreachable goal ran
    // off the end of parent[]; bound it by the number of states.

    __int32 count_path = 1;
    {
        __int32 tracker = goal;
        while (count_path <= stm->kbsts &&
               tracker >= 0 && tracker < stm->kbsts &&
               parent[tracker] != cur) {
            tracker = parent[tracker];
            count_path++;
        }
    }

    // The original filled parent[] in this loop instead of ret[], so the array
    // it returned was left almost entirely uninitialised.
    __int32* ret = new __int32[count_path];
    for (__int32 i = 0; i < count_path; i++)
        ret[i] = goal;

    {
        __int32 tracker = goal;
        for (__int32 i = 1; i < count_path; i++) {
            if (tracker < 0 || tracker >= stm->kbsts)
                break;
            tracker = parent[tracker];
            ret[count_path - 1 - i] = tracker;
        }
    }

    delete[] parent;
    delete[] visited;

    return ret;
}

void generateBFSs(AGI_Sys* stm) {

    // create space for the rewards and disincentives

    __int32** rwpaths = new __int32* [stm->rwtop + 1];
    __int32** dvpaths = new __int32* [stm->dvtop + 1];

    for (__int32 i = 0; i < stm->rwtop + 1; i++)
        rwpaths [i] = executeBFS(stm, stm->Current_Input >> 1, true, i);
       
    for (__int32 i = 0; i < stm->dvtop + 1; i++)
        dvpaths [i] = executeBFS(stm, stm->Current_Input >> 1, false, i);

    __int32 rw_dist = 2000000000;

    for (__int32 i = 0; i < stm->rwtop + 1; i++) {
        __int32 cur_dist = 0;
        __int32 ix = 0;
        while (rwpaths[i][ix] != stm->rewards[i]) {
            ix++;
            cur_dist++;
        }
        if (cur_dist < rw_dist) {
            rw_dist = cur_dist;
            if (stm->kb_rw_path != 0)
                delete[] stm->kb_rw_path;
            stm->kb_rw_path = new __int32[cur_dist + 1];
            for (__int32 j = 0; j < cur_dist; j++)
                stm->kb_rw_path[j] = rwpaths[i][j];
            stm->kb_rw_path[cur_dist] = -1;
        }

    }

    __int32 dv_dist = 2000000000;

    for (__int32 i = 0; i < stm->dvtop + 1; i++) {
        __int32 cur_dist = 0;
        __int32 ix = 0;
        while (dvpaths[i][ix] != stm->dsnctvs[i]) {
            ix++;
            cur_dist++;
        }
        if (cur_dist < dv_dist) {
            dv_dist = cur_dist;
            if (stm->kb_dv_path != 0)
                delete[] stm->kb_dv_path;
            stm->kb_dv_path = new __int32[cur_dist + 1];
            for (__int32 j = 0; j < cur_dist; j++)
                stm->kb_dv_path[j] = dvpaths[i][j];
            stm->kb_dv_path[cur_dist] = -1;
        }

    }

}

__int32 perform_iann(AGI_Sys* stm) {

    // input_weights and input_targets have one row per queued input bit, not
    // hidden_sz rows.  Indexing them by a hidden-unit number reads past the
    // end of both arrays and dereferences whatever follows.
    const __int32 in_units = stm->in_sz * stm->In_Q_ct;

    bool* input_b = new bool[in_units];

    for (__int32 i = 0; i < stm->In_Q_ct; i++) {

        __int32 temp_input = stm->Input_Queue[i];

        for (__int32 j = 0; j < stm->in_sz; j++) {
            input_b[i * stm->in_sz + j] = temp_input & 1;
            temp_input >>= 1;
        }

    }

    __int32* weight_sums = new __int32[stm->hidden_sz];

    // input layer: in_units input bits fan out into hidden_sz hidden units
    for (__int32 j = 0; j < stm->hidden_sz; j++)
        weight_sums[j] = 0;

    for (__int32 i = 0; i < in_units; i++)
        for (__int32 k = 0; k < stm->hidden_sz >> 1; k++)
            weight_sums[stm->input_targets[i][k]] += input_b[i] ? stm->input_weights[i][k] : 0;

    for (__int32 j = 0; j < stm->hidden_sz; j++)
        stm->hidden[0]->firings[j] = weight_sums[j] >= 0;

    // hidden layers: layer count-1 drives layer count
    for (__int32 count = 1; count < stm->hidden_ct; count++) {

        for (__int32 j = 0; j < stm->hidden_sz; j++)
            weight_sums[j] = 0;

        for (__int32 i = 0; i < stm->hidden_sz; i++)
            for (__int32 k = 0; k < stm->hidden_sz >> 1; k++)
                weight_sums[stm->hidden[count - 1]->targets[i][k]] +=
                    stm->hidden[count - 1]->firings[i] ? stm->hidden[count - 1]->weights[i][k] : 0;

        for (__int32 j = 0; j < stm->hidden_sz; j++)
            stm->hidden[count]->firings[j] = weight_sums[j] >= 0;
    }

    delete[] weight_sums;

    bool* output_b = new bool[stm->out_sz];
    __int32* out_sums = new __int32[stm->out_sz];

    for (__int32 i = 0; i < stm->out_sz; i++) {
        output_b[i] = false;
        out_sums[i] = 0;
    }

    for (__int32 i = 0; i < stm->hidden_sz; i++)
        if (stm->hidden[stm->hidden_ct - 1]->firings[i])
            for (__int32 k = 0; k < stm->out_sz >> 1; k++)
                out_sums[stm->output_targets[i][k]] += stm->output_weights[i][k];

    for (__int32 i = 0; i < stm->out_sz; i++)
        if (out_sums[i] >= 0)
            output_b[i] = true;

    __int32 ret_output = 0;

    for (__int32 i = 0; i < stm->out_sz; i++)
        if (output_b[i])
            ret_output |= (0x1 << i);

    delete[] out_sums;
    delete[] output_b;
    delete[] input_b;

    return ret_output;
}

bool terminate_program(__int32 cycles) {

    return cycles >= RZNAI_AGI_MAX_CYCLES;
}

#ifndef RZNAI_AGI_EXTERNAL_SENSORS

__int32 in_0() {

    // actually read from sensor 0

    // simulation:
    return 0;
}

__int32 in_1() {

    // actually read from sensor 1

    // simulation:
    return 1;
}

#endif // RZNAI_AGI_EXTERNAL_SENSORS

__int32 read_sensory(AGI_Sys *stm, __int32 sensor) {

    __int32 input;

    switch (sensor) {
        case 0: input = in_0(); break; // get actual reading from sensor 0
        case 1: input = in_1(); break; // get actual reading from sensor 1
        default: return 0;             // no such sensor: nothing was read
    }

    __int32 sensor_id = 0;

    for (__int32 i = 0; i < stm->sensory_bits; i++) {
        sensor_id = sensor_id << 1;
        sensor_id |= 0x1;
    }

    sensor_id = sensor & sensor_id; // sensor id, clipped to sensory_bits

    // set read from sensory
    input = (input << stm->sensory_bits);
    input = input | sensor_id;
    input = (input << 1) | 0x0; // indicates NOT read from recall

    return input;
}

__int32 read_from_recall_next(AGI_Sys *stm, __int32 previous_input_state, __int32 previous_output_action, bool rw) {

    if (rw) {
        if (stm->kb_rw_path == 0)
            return 0;
        __int32 ix = 0;
        while (stm->kb_rw_path[ix] != -1 && stm->kb_rw_path[ix] != previous_input_state)
            ix++;
        if (stm->kb_rw_path[ix] == -1 || stm->kb_rw_path[ix + 1] == -1)
            return 0;
        __int32 ret_val = stm->kb_rw_path[ix + 1];
        ret_val = (ret_val << 1);
        ret_val |= 0x1; // indicates reading from rewards
        ret_val = (ret_val << 1);
        ret_val |= 0x1; // indicates read from recall
        return ret_val;
    }
    else {
        if (stm->kb_dv_path == 0)
            return 0;
        __int32 ix = 0;
        while (stm->kb_dv_path[ix] != -1 && stm->kb_dv_path[ix] != previous_input_state)
            ix++;
        if (stm->kb_dv_path[ix] == -1 || stm->kb_dv_path[ix + 1] == -1)
            return 0;
        __int32 ret_val = stm->kb_dv_path[ix + 1];
        ret_val = (ret_val << 1);
        ret_val |= 0x0; // indicates reading from disincentives
        ret_val = (ret_val << 1);
        ret_val |= 0x1; // indicates read from recall
        return ret_val;
    }
}
__int32 read_from_recall_new(AGI_Sys *stm, __int32 previous_input_state, __int32 previous_output_action, bool rw) {

    generateBFSs(stm);

    if (rw) {

        if (stm->kb_rw_path == 0 || stm->kb_rw_path[0] == -1)
            return 0;

        __int32 i = 0;
        while (stm->kb_rw_path[i + 1] != -1 && stm->kb_rw_path[i] != previous_input_state)
            i++;
        if (stm->kb_rw_path[i + 1] == -1 || stm->kb_rw_path[i] != previous_input_state)
            return 0;
        __int32 kb_line = previous_input_state % stm->kbpsz;
        Dict_Entry* cur_entry = stm->Knowledge_Bank[kb_line]->next;
        while (cur_entry != 0 && cur_entry->init_state != previous_input_state)
            cur_entry = cur_entry->next;
        if (cur_entry == 0)
            return 0;
        while (cur_entry != 0 && cur_entry->init_state == previous_input_state && cur_entry->action_out != previous_output_action)
            cur_entry = cur_entry->next;
        if (cur_entry == 0)
            return 0;

        __int32 ret_val = cur_entry->vect_state;
        ret_val = (ret_val << 1);
        ret_val |= 0x1; // indicates reading from rewards
        ret_val = (ret_val << 1);
        ret_val |= 0x1; // indicates read from recall
        return ret_val;
    }
    else {
        if (stm->kb_dv_path == 0 || stm->kb_dv_path[0] == -1)
            return 0;

        __int32 i = 0;
        while (stm->kb_dv_path[i + 1] != -1 && stm->kb_dv_path[i] != previous_input_state)
            i++;
        if (stm->kb_dv_path[i + 1] == -1 || stm->kb_dv_path[i] != previous_input_state)
            return 0;
        __int32 kb_line = previous_input_state % stm->kbpsz;
        Dict_Entry* cur_entry = stm->Knowledge_Bank[kb_line]->next;
        while (cur_entry != 0 && cur_entry->init_state != previous_input_state)
            cur_entry = cur_entry->next;
        if (cur_entry == 0)
            return 0;
        while (cur_entry != 0 && cur_entry->init_state == previous_input_state && cur_entry->action_out != previous_output_action)
            cur_entry = cur_entry->next;
        if (cur_entry == 0)
            return 0;

        __int32 ret_val = cur_entry->vect_state;
        ret_val = (ret_val << 1);
        ret_val |= 0x0; // indicates reading from disincentives
        ret_val = (ret_val << 1);
        ret_val |= 0x1; // indicates read from recall
        return ret_val;
    }
}

bool get_rw(__int32 cycle) {
    // actually fetch reward bit

    // simulation:
    return cycle % 32767;
}

bool get_dv(__int32 cycle) {
    // actually fetch disincentive bit

    // simulation:
    return cycle % 65537;
}

void out_0(__int32 parm) {

    // send parm to servo 0, for example
}

void out_1(__int32 parm) {

    // send parm to servo 1, for example
}

void out_2(__int32 parm) {

    // send parm to servo 2, for example
}

void out_3(__int32 parm) {

    // send parm to servo 3, for example
}

void handle_output(AGI_Sys* stm, __int32 output) {

    __int32 output_addr_mask = 0;
    for (__int32 i = 0; i < stm->out_addr_sz; i++)
        output_addr_mask |= (0x1 << i);

    __int32 temp_output = output >> 1;
    __int32 out_addr = temp_output & output_addr_mask;
    __int32 out_parm = temp_output >> stm->out_addr_sz;

    // actually send the parameter out_parm to the output addressed by out_addr

    // simulation:

    switch (out_addr) {
    case 0: out_0(out_parm);
    case 1: out_1(out_parm);
    case 2: out_2(out_parm);
    case 3: out_3(out_parm);
    };

}

void cycle(AGI_Sys * stm) {

    __int32 cycle = 0;
    __int32 sensor = 0;
    __int32 previous_input_state = 0;
    __int32 previous_output_action = 0;

    bool out_read_from_recall = false;

    // NOTE: in_read_from_recall and read_from_recall_input are never assigned
    // after this point, so `if (!in_read_from_recall)` below is always true.
    // The model therefore always reads from a sensor and never from recall,
    // which makes read_from_recall_new(), read_from_recall_next(),
    // generateBFSs(), executeBFS() and every Knowledge Bank lookup unreachable
    // -- create_dict_entry() writes to the bank every cycle and nothing reads
    // it back.
    //
    // out_read_from_recall already holds the decision (output & 0x1); the
    // missing step is propagating it here, e.g. at the end of the loop body
    //     read_from_recall_input = !in_read_from_recall;
    //     in_read_from_recall    = out_read_from_recall;
    //
    // That one change is NOT sufficient on its own, and is deliberately not
    // made here. Enabling recall exposes two open questions:
    //   1. executeBFS() cannot search until the state-to-index mapping is
    //      decided -- see the comment there.
    //   2. Nothing returns the model to sensory input once bit 0 latches, so
    //      it reads a sensor once and then never looks at the world again.
    // Measured: with recall enabled the model served 1 sensory reading in 2000
    // cycles and recalled 0 for the rest.
    bool in_read_from_recall = false;
    bool read_from_recall_input = false;
    bool prev_recall_rwdv = true; // true bit indicates rewards, false bit indicates disincentives
    bool recall_rwdv = true;

    __int32 sensor_mask = 0;
    
    for (__int32 i = 0; i < stm->sensory_bits; i++) {
        sensor_mask = sensor_mask << 1;
        sensor_mask |= 0x1;
    }

    // while not terminate program

    while (!terminate_program(cycle)) {

        for (__int32 i = stm->In_Q_ct - 1; i >= 1; i--)
            stm->Input_Queue[i] = stm->Input_Queue[i - 1];

        out_read_from_recall = false;

        // read input

        __int32 input = 0;

        if (!in_read_from_recall)
            input = read_sensory(stm, sensor);
        else if (read_from_recall_input || (prev_recall_rwdv != recall_rwdv))
            input = read_from_recall_new(stm, previous_input_state, previous_output_action, recall_rwdv);
        else
            input = read_from_recall_next(stm, previous_input_state, previous_output_action, recall_rwdv); 

        stm->Current_Input = input;
        stm->Input_Queue[0] = stm->Current_Input;

        // feed into IANN and fetch output bit sequence

        __int32 output = perform_iann(stm);

        // update Knowledge Bank with (previous input state, output action) -> (newly read input)

        bool entry_exists = false;
        __int32 kb_line = previous_input_state % stm->kbpsz;
        Dict_Entry* cur_entry = stm->Knowledge_Bank[kb_line]->next;
        while (cur_entry != 0 && cur_entry->init_state != previous_input_state)
            cur_entry = cur_entry->next;
        if (cur_entry == 0)
            entry_exists = false;
        while (cur_entry != 0 && cur_entry->init_state == previous_input_state && cur_entry->action_out != previous_output_action)
            cur_entry = cur_entry->next;
        if (cur_entry == 0)
            entry_exists = false;
        else
            entry_exists = true;

        if (entry_exists)
            remove_dict_entry(stm->Knowledge_Bank, stm->kbpsz, previous_input_state, previous_output_action);

        create_dict_entry(stm->Knowledge_Bank, stm->kbpsz, previous_input_state, input >> 1, output);

        out_read_from_recall = output & 0x1;
        sensor = (output >> 1) & sensor_mask;
        prev_recall_rwdv = recall_rwdv;
        recall_rwdv = (output >> 1) & 0x1; // least significant sensory id doubles as recall identity selector, rw or dv

        if (!out_read_from_recall)
            handle_output(stm, output);

        // fetch any reward or disincentive feedback and take appropriate action on IANN as well as update AGI_Sys reward and disincentive vectors

        bool rw = get_rw(cycle);
        bool dv = get_dv(cycle);

        if (rw) {

            bool found = false;
            for (__int32 i = 0; i < stm->rwtop + 1; i++)
                if (stm->rewards[i] != input)
                    continue;
                else
                    found = true;
               
            if (!found)
                simp_vector_append(&(stm->rewards), &(stm->rwtop), &(stm->rwcap), input);

            found = false;

            __int32 ix = 0;
            for (ix = 0; ix < stm->dvtop + 1; ix++)
                if (stm->dsnctvs[ix] != input)
                    continue;
                else
                    found = true;

            if (found) {
                while (ix < stm->dvtop) {
                    stm->dsnctvs[ix] = stm->dsnctvs[ix + 1];
                    ix++;
                }
                stm->dvtop--;
            }

            bool* inputs = new bool[stm->in_sz * stm->In_Q_ct];

            for (__int32 i = 0; i < stm->In_Q_ct; i++)
                for (__int32 j = 0; j < stm->in_sz; j++)
                    inputs[i * stm->in_sz + j] = false;

            for (__int32 i = 0; i < stm->In_Q_ct; i++) {
                __int32 temp_in = stm->Input_Queue[i];
                for (__int32 j = 0; j < stm->in_sz; j++) {
                    inputs[i * stm->in_sz + j] = temp_in & 1;
                    temp_in = temp_in >> 1;
                }
            }

            __int32* sums = new __int32[stm->hidden_sz];
            for (__int32 i = 0; i < stm->hidden_sz; i++)
                sums[i] = 0;
            for (__int32 i = 0; i < stm->in_sz * stm->In_Q_ct; i++)
                if (inputs[i])
                    for (__int32 j = 0; j < stm->hidden_sz; j++)
                        for (__int32 k = 0; k < stm->hidden_sz >> 1; k++)
                            if (stm->input_targets[i][k] == j && stm->hidden[0]->firings[j])
                                stm->input_weights[i][k] += (k % 2 == 0 ? -stm->inc_amt : stm->inc_amt);

            for (__int32 i = 1; i < stm->hidden_ct; i++)
                for (__int32 j = 0; j < stm->hidden_sz; j++)
                    if (stm->hidden[i]->firings[j])
                        for (__int32 k = 0; k < stm->hidden_sz >> 1; k++)
                            stm->hidden[i]->weights[j][k] += (k % 2 == 0 ? -stm->inc_amt : stm->inc_amt);

            __int32 temp_output = output;

            for (__int32 i = 0; i < stm->hidden_sz; i++)
                for (__int32 j = 0; j < stm->out_sz >> 1; j++) {
                    __int32 temp_output = output;
                    for (__int32 k = 0; k < stm->out_sz; k++) {
                        if (temp_output & 0x1)
                            stm->output_weights[i][j] += (j % 2 == 0 ? -stm->inc_amt : stm->inc_amt);
                        temp_output = temp_output >> 1;
                    }
                }

        }
        if (dv) {

            bool found = false;
            for (__int32 i = 0; i < stm->dvtop + 1; i++)
                if (stm->rewards[i] != input)
                    continue;
                else
                    found = true;

            if (!found)
                simp_vector_append(&(stm->rewards), &(stm->dvtop), &(stm->dvcap), input);

            found = false;

            __int32 ix = 0;
            for (ix = 0; ix < stm->dvtop + 1; ix++)
                if (stm->dsnctvs[ix] != input)
                    continue;
                else
                    found = true;

            if (found) {
                while (ix < stm->dvtop) {
                    stm->dsnctvs[ix] = stm->dsnctvs[ix + 1];
                    ix++;
                }
                stm->dvtop--;
            }

            bool* inputs = new bool[stm->in_sz * stm->In_Q_ct];

            for (__int32 i = 0; i < stm->In_Q_ct; i++)
                for (__int32 j = 0; j < stm->in_sz; j++)
                    inputs[i * stm->in_sz + j] = false;

            for (__int32 i = 0; i < stm->In_Q_ct; i++) {
                __int32 temp_in = stm->Input_Queue[i];
                for (__int32 j = 0; j < stm->in_sz; j++) {
                    inputs[i * stm->in_sz + j] = temp_in & 1;
                    temp_in = temp_in >> 1;
                }
            }

            __int32* sums = new __int32[stm->hidden_sz];
            for (__int32 i = 0; i < stm->hidden_sz; i++)
                sums[i] = 0;
            for (__int32 i = 0; i < stm->in_sz * stm->In_Q_ct; i++)
                if (inputs[i])
                    for (__int32 j = 0; j < stm->hidden_sz; j++)
                        for (__int32 k = 0; k < stm->hidden_sz >> 1; k++)
                            if (stm->input_targets[i][k] == j && stm->hidden[0]->firings[j])
                                stm->input_weights[i][k] -= (k % 2 == 0 ? -stm->dec_amt : stm->dec_amt);

            for (__int32 i = 1; i < stm->hidden_ct; i++)
                for (__int32 j = 0; j < stm->hidden_sz; j++)
                    if (stm->hidden[i]->firings[j])
                        for (__int32 k = 0; k < stm->hidden_sz >> 1; k++)
                            stm->hidden[i]->weights[j][k] -= (k % 2 == 0 ? -stm->dec_amt : stm->dec_amt);

            __int32 temp_output = output;

            for (__int32 i = 0; i < stm->hidden_sz; i++)
                for (__int32 j = 0; j < stm->out_sz >> 1; j++) {
                    __int32 temp_output = output;
                    for (__int32 k = 0; k < stm->out_sz; k++) {
                        if (temp_output & 0x1)
                            stm->output_weights[i][j] -= (j % 2 == 0 ? -stm->dec_amt : stm->dec_amt);
                        temp_output = temp_output >> 1;
                    }
                }

        }

        // make sure to update rw and dv vectors if input matches any in these vectors and no rw/dv occurred

        if (!rw && !dv) {
            bool input_read_from_recall = input & 0x1;
            __int32 temp_input = input >> 1;
            if (!input_read_from_recall) {
                for (__int32 i = 0 ; i < stm->rwtop + 1; i++)
                    if (stm->rewards[i] == temp_input) {
                        for (__int32 j = i; j < stm->rwtop; j++)
                            stm->rewards[j] = stm->rewards[j + 1];
                        stm->rwtop--;
                    }

                for (__int32 i = 0; i < stm->dvtop + 1; i++)
                    if (stm->dsnctvs[i] == temp_input) {
                        for (__int32 j = i; j < stm->dvtop; j++)
                            stm->dsnctvs[j] = stm->dsnctvs[j + 1];
                        stm->dvtop--;
                    }
            }
        }

        // check if current cycle is stm->cycles_to_dec. If so, bitwise shift down by one bit, then set current cycle back to 0.
            // if any new weights reach zero, retarget artificial neuron to next neuron higher than current neuron mod layer size (% stm->hidden_sz)

        if (cycle % stm->cycles_to_dec == 0) {

            for (__int32 i = 0; i < stm->In_Q_ct; i++)
                for (__int32 j = 0 ; j < stm->in_sz; j++)
                    for (__int32 k = 0; k < stm->hidden_sz >> 1; k++) {
                        stm->input_weights[i * stm->in_sz + j][k] = stm->input_weights[i][j] >> 1;
                        if (stm->input_weights[i * stm->in_sz + j][k] == 0) {
                            bool* exists = new bool[stm->in_sz * stm->In_Q_ct];
                            for (__int32 l = 0; l < stm->in_sz * stm->In_Q_ct; l++)
                                exists[k] = false;
                            for (__int32 l = 0; l < stm->In_Q_ct; l++)
                                for (__int32 m = 0; m < stm->in_sz; m++)
                                    if ((stm->Input_Queue[l] >> m ) & 0x1)
                                        exists[l * stm->in_sz + m] = true;
                            __int32 ix = stm->input_targets[i * stm->in_sz + j][k] + 1;
                            while (!exists[ix % (stm->In_Q_ct * stm->in_sz)])
                                ix++;
                            stm->input_targets[i * stm->in_sz + j][k] = ix % (stm->in_sz * stm->In_Q_ct);
                            stm->input_weights[i * stm->in_sz + j][k] = k % 2 == 0 ? -16384 : 16384;
                            delete[] exists;
                        }
                    }

            for (__int32 i = 0; i < stm->hidden_ct; i++)
                for (__int32 j = 0; j < stm->hidden_sz; j++)
                    for (__int32 k = 0; k < stm->hidden_sz >> 1; k++) {
                        stm->hidden[i]->weights[j][k] = stm->hidden[i]->weights[j][k] >> 1;
                        if (stm->hidden[i]->weights[j][k] == 0) {
                            bool* exists = new bool[stm->hidden_sz];
                            for (__int32 l = 0; l < stm->hidden_sz; l++)
                                exists[l] = false;
                            for (__int32 l = 0; l < stm->hidden_sz >> 1; l++)
                                exists[stm->hidden[i]->targets[j][l]] = true;
                            __int32 ix = stm->hidden[i]->targets[j][k] + 1;
                            while (!exists[ix % (stm->hidden_sz >> 1)])
                                ix++;
                            stm->hidden[i]->targets[j][k] = ix % stm->hidden_sz;
                            stm->hidden[i]->weights[j][k] = k % 2 == 0 ? -16384 : 16384;
                            delete[] exists;
                        }
            }
            for (__int32 i = 0; i < stm->hidden_sz; i++)
                for (__int32 j = 0; j < stm->out_sz >> 1; j++) {
                    stm->output_weights[i][j] = stm->output_weights[i][j] >> 1;
                    if (stm->output_weights[i][j] == 0) {
                        bool* exists = new bool[stm->out_sz];
                        for (__int32 k = 0; k < stm->out_sz; k++)
                            exists[k] = false;
                        for (__int32 k = 0; k < stm->out_sz >> 1; k++)
                            if (stm->output_targets[i][k])
                                exists[k] = true;
                        __int32 ix = stm->output_targets[i][j] + 1;
                        while (!exists[ix % stm->hidden_sz >> 1])
                            ix++;
                        stm->output_targets[i][j] = ix % stm->out_sz;
                        stm->output_weights[i][j] = j % 2 == 0 ? -16384 : 16384;
                        delete[] exists;
                    }
                }
        }

        for (__int32 i = 0; i < stm->hidden_ct; i++)
            for (__int32 j = 0; j < stm->hidden_sz; j++)
                stm->hidden[i]->firings[j] = false;

        cycle++;
        previous_input_state = input;
        previous_output_action = output;
    }

}

#ifndef RZNAI_AGI_NO_MAIN

//  Driver function to test above functions
int main()
{
    AGI_Sys* stm = instantiate();
    cycle(stm);
}

#endif // RZNAI_AGI_NO_MAIN

#endif
