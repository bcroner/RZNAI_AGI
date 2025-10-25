#ifndef __RZNAI_AGI_HPP__
#define __RZNAI_AGI_HPP__

typedef struct Dict_Entry_tag {

	unsigned __int64 state_action;
	unsigned __int32 vectored_state;

	Dict_Entry_tag* next;

} Dict_Entry ;

// integer neural network layer

typedef struct IntNNL_tag {

	__int32 ** weights;

	bool * firings;

} IntNNL;

typedef struct AGI_Sys_tag {

	unsigned __int64* Current_Input;
	unsigned __int64* Output_Unit;

	IntNNL* first;
	IntNNL* last;
	IntNNL** hidden;

	Dict_Entry* Knowledge_Bank;

	__int32** KB_2CNF;

	__int32 ** Stack;
	__int32 Stack_top;
	__int32 ** adj;
	__int32 ** adjInv;
	__int32* adj_sz;
	__int32* adjInv_sz;
	bool * visited;
	bool * visitedInv;
	__int32 * scc;
	__int32 counter;

} AGI_Sys;

Dict_Entry** create_dict(__int64 prime_sz);
void create_dict_entry(Dict_Entry ** d, __int64 prime_sz, unsigned __int64 sa, unsigned __int32 vs);
void remove_dict_entry(Dict_Entry** d, __int64 prime_sz, unsigned __int64 sa);
unsigned __int64* simp_vector_create(__int64 init_sz);
unsigned __int64 simp_vector_read(unsigned __int64 * v, __int64 vtop, __int64 vcap, __int64 loc);
void simp_vector_append(unsigned __int64** v, __int64 *vtop, __int64 *vcap, unsigned __int64 data);
unsigned __int64 * simp_stack_create(__int64 * tos);
unsigned __int64 simp_stack_pop(unsigned __int64* s, __int64* tos, __int64 vtop, __int64 vcap);
void simp_stack_push(unsigned __int64** s, __int64* tos, __int64* vcap, __int64 data);
void instantiate();

#endif