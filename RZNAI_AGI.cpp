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

    __int32* parent = new __int32[stm->kbsts];

    for (__int32 i = 0; i < stm->kbsts; i++)
        parent[i] = 0;

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
        tracker = parent[tracker];
        parent[count_path - 1 - i] = tracker;
    }

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

    bool* output_b = new bool[stm->out_sz];

    for (__int32 i = 0; i < stm->out_sz; i++)
        output_b[i] = false;

    __int32* weight_sums = new __int32[stm->out_sz];
    for (__int32 i = 0; i < stm->out_sz; i++)
        weight_sums[i] = 0;

    for (__int32 i = 0; i < stm->hidden_sz; i++)
        if (stm->hidden[stm->hidden_ct - 1]->firings[i])
            for (__int32 j = 0; j < stm->hidden_sz >> 1; j++)
                for (__int32 k = 0; k < stm->out_sz >> 1; k++)
                    weight_sums[stm->output_targets[j][k]] += stm->output_weights[j][k];

    for (__int32 i = 0; i < stm->out_sz; i++)
        if (weight_sums[i] >= 0)
            output_b[i] = true;

    delete[] weight_sums;

    __int32 ret_output = 0;

    for (__int32 i = 0; i < stm->out_sz; i++)
        ret_output |= (0x1 << i);

    return ret_output;
}

bool terminate_program(__int32 cycles) {

    return cycles >= 2000000000;    // 2 billion as an example of how long to run the program
}

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

__int32 read_sensory(__int32 sensor) {

    __int32 input;

    switch (sensor) {
        case 0: input = in_0(); // get actual reading from sensor 0
        case 1: input = in_1(); // get actual reading from sensor 1
    }

    // set read from sensory
    input = input << 1;
    input = input & 0x0;

    return input;
}

__int32 read_from_recall_next(AGI_Sys *stm, __int32 previous_input_state, __int32 previous_output_action, bool rw) {

    if (rw) {
        if (stm->kb_rw_path == 0)
            return 0;
        __int32 ix = 0;
        while (stm->kb_rw_path[ix] != -1 && stm->kb_rw_path[ix] != previous_input_state)
            ix;
        if (stm->kb_rw_path[ix] == -1 || stm->kb_rw_path[ix + 1] == -1)
            return 0;
        __int32 ret_val = stm->kb_rw_path[ix + 1];
        ret_val = (ret_val << 1) & 0x1; // indicates reading from rewards
        ret_val = (ret_val << 1) & 0x1; // indicates read from recall
        return ret_val;
    }
    else {
        if (stm->kb_dv_path == 0)
            return 0;
        __int32 ix = 0;
        while (stm->kb_dv_path[ix] != -1 && stm->kb_dv_path[ix] != previous_input_state)
            ix;
        if (stm->kb_dv_path[ix] == -1 || stm->kb_dv_path[ix + 1] == -1)
            return 0;
        __int32 ret_val = stm->kb_rw_path[ix + 1];
        ret_val = (ret_val << 1) & 0x0; // indicates reading from disincentives
        ret_val = (ret_val << 1) & 0x1; // indicates read from recall
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
        ret_val = (ret_val << 1) & 0x1; // indicates reading from rewards
        ret_val = (ret_val << 1) & 0x1; // indicates read from recall
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
        ret_val = (ret_val << 1) & 0x0; // indicates reading from disincentives
        ret_val = (ret_val << 1) & 0x1; // indicates read from recall
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
            input = read_sensory(sensor);
        else if (read_from_recall_input || (prev_recall_rwdv != recall_rwdv))
            input = read_from_recall_new(stm, previous_input_state, previous_output_action);
        else
            input = read_from_recall_next(stm, previous_input_state, previous_output_action); 

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

//  Driver function to test above functions
int main()
{
    AGI_Sys* stm = instantiate();
    cycle(stm);
}

#endif
