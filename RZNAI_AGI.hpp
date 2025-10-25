#ifndef __RZNAI_AGI_HPP__
#define __RZNAI_AGI_HPP__

typedef struct Dict_Entry_tag {

	__int32 init_state;
	__int32 action_out;
	__int32 vect_state;

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
	__int32 Stack_cap;
	__int32 ** adj;
	__int32 adj_top;
	__int32 adj_cap;
	__int32 ** adjInv;
	__int32 adjInv_top;
	__int32 adjInv_cap;
	__int32* adj_sz;
	__int32* adjInv_sz;
	bool * visited;
	bool * visitedInv;
	__int32 * scc;
	__int32 counter;

} AGI_Sys;

Dict_Entry** create_dict(__int64 prime_sz);
void create_dict_entry(Dict_Entry ** d, __int64 prime_sz, __int32 is, __int32 oa, __int32 vs);
void remove_dict_entry(Dict_Entry ** d, __int64 prime_sz, __int32 is, __int32 oa);
__int32* simp_vector_create(__int32 init_sz);
__int32 simp_vector_read(__int32 * v, __int32 vtop, __int32 vcap, __int32 loc);
void simp_vector_append(__int32** v, __int32 *vtop, __int32 *vcap, __int32 data);
__int32 * simp_stack_create(__int32 * vtop);
__int32 simp_stack_pop(__int32* s, __int32 *vtop, __int32 vcap);
void simp_stack_push(__int32 ** s, __int32 *vop, __int32* vcap, __int32 data);
void addEdges(AGI_Sys* stm, __int32 a, __int32 b);
void addEdgesInverse(AGI_Sys* stm, __int32 a, __int32 b);
void dfsFirst(AGI_Sys* stm, __int32 u);
void dfsSecond(AGI_Sys* stm, __int32 u);
void is2Satisfiable(AGI_Sys* stm, __int32 n, __int32 m, __int32 a[], __int32 b[]);
AGI_Sys * instantiate();

#endif