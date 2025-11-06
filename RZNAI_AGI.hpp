#ifndef __RZNAI_AGI_HPP__
#define __RZNAI_AGI_HPP__

#define TRUE_2SAT 1
#define FALSE_2SAT -1

typedef struct Simp_Queue_tag {

	__int32 data;

	Simp_Queue_tag* next;

} Simp_Queue ;

typedef struct Dict_Entry_tag {

	__int32 init_state;
	__int32 action_out;
	__int32 vect_state;

	Dict_Entry_tag* next;

} Dict_Entry ;

// integer neural network layer

typedef struct IntNNL_tag {

	__int32 ** weights;
	__int32 ** targets;

	bool * firings;

} IntNNL;

typedef struct AGI_Sys_tag {

	__int32 Current_Input;
	__int32 Output_Unit;

	__int32* Input_Queue;

	__int32 In_Q_ct;

	__int32 in_sz;
	__int32 out_sz;

	__int32** input_weights;
	__int32** input_targets;

	IntNNL** hidden;
	__int32  hidden_sz;
	__int32  hidden_ct;

	__int32 cycles_to_dec;
	__int32 dec_amt;
	__int32 inc_amt;

	Dict_Entry** Knowledge_Bank;
	__int32 kbsz;
	__int32 kbpsz;

	__int32* rewards;
	__int32  rwtop;
	__int32  rwcap;
	__int32* dsnctvs;
	__int32  dvtop;
	__int32  dvcap;

	Simp_Queue* bfs_queue;

} AGI_Sys;

void simp_queue_enqueue(Simp_Queue * queue, Simp_Queue * parm);
Simp_Queue* simp_queue_dequeue(Simp_Queue* queue);
Dict_Entry** create_dict(__int64 prime_sz);
void create_dict_entry(Dict_Entry ** d, __int64 prime_sz, __int32 is, __int32 oa, __int32 vs);
void remove_dict_entry(Dict_Entry ** d, __int64 prime_sz, __int32 is, __int32 oa);
__int32* simp_vector_create(__int32 init_sz);
__int32 simp_vector_read(__int32 * v, __int32 vtop, __int32 vcap, __int32 loc);
void simp_vector_append(__int32** v, __int32 *vtop, __int32 *vcap, __int32 data);
__int32 * simp_stack_create(__int32 * vtop);
__int32 simp_stack_pop(__int32* s, __int32 *vtop, __int32 vcap);
void simp_stack_push(__int32 ** s, __int32 *vop, __int32* vcap, __int32 data);
AGI_Sys * instantiate();
void generateBFSs(AGI_Sys* stm);
void perform_iann(AGI_Sys* stm);
void cycle(AGI_Sys * stm);

#endif