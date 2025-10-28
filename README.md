<h1 align="center">
System and Method for an AI Agent with Reasoning Capabilities
</h1>

Abstract

We introduce a new form of artificial intelligence that is capable of carrying out reasoning processes, making it possible for an AI agent to mentally investigate scenarios and form a plan of action in order to both achieve the Achieve State(s) (“correct decision(s)”) and avoid the Avoid State(s) (“incorrect decision(s)”).

Claims:

1. We give the AGI agent the ability to read from the Knowledge Bank by allocating a bit in the Output Unit we call the Output Recall Bit which triggers a lookup of a specific consequent recalled Unit (an Input Unit) that is mapped to by a (state, action) pair and we also allocate a bit in the Input Unit to indicate whether the incoming Input Unit is a result of the recall operation.  
2. To read from recall we construct boolean 2-satisfiability or 3-satisfiability instances from the Knowledge Bank and solve for arriving at each of the Achieve State(s) by encoding one clause per Achieve State 2-SAT instance that forces the current state C to be true in the form (C | F), where F is the false truth value, and forces said Achieve State S to be true in the form (S | F). Also to read from recall we construct boolean 2-satisfiability or 3-satisfiability instances from the Knowledge Bank and solve for evading each of the Avoid State(s) by encoding one clause per Avoid State 2-SAT instance that forces the current state C to be true in the form (C | F) and forces said Avoid State S’ to be true in the form (S’ | F).  
3. We re-target an artificial neuron’s firing output to a new artificial neuron’s input when the weight of the firing artificial neuron reaches a pre-set limit (we use 0 for this level in the provided reference source code).

Specification

Definitions:

1. Input Unit: This input unit is uniform in size, consisting of a set number of bits.  
2. Current Input: The Input Unit representing the most recent sensory or recall input.  
3. Input Unit Format: The assignments of the bits in the Input Unit.  
4. Input Recall Bit: A bit in the Input Unit Format indicating input is from knowledge bank data.  
5. Input Queue: A series of one or more Input Units, where when one queues in, the last queues off.  
6. Past Inputs: Input Units absent of the Current Input in the Input Queue in chronological order.  
7. Knowledge Bank- An ordered collection of input units that can be recalled from.  
8. Knowledge Bank Format- The coding of Input Units associated with actions for a given Input Unit.  
9. Output Unit: An action to take, formed from the neural network that receives the input unit.  
10. Output Unit Format: The assignment of the bits in the Output Unit.  
11. Neural Network: A neural network that accepts the Input Queue and resolves it to an Output Unit.  
12. Operator: An entity that is capable of carrying out some sort of action, such as a servo controller.  
13. Output Target: An Operator that accepts a parameter from an Output Unit sent to it.  
14. Recall Operator: A knowledge bank system traversable by the AI agent with cognitive actions.  
15. Recall Unit: A sequence of one or more bits of fixed size representing knowledge bank data.  
16. Recall Unit Format: The Assignment of the bits of the Recall Unit.  
17. Output Recall Bit: A bit in the Output Unit Format stating to read Input Unit from knowledge bank.  
18. Action Sequence: A series of one or more bits in the Output Unit used to address an Operator.  
19. Utility Sequence: The zero or more bits in the Action Sequence absent of the Output Recall Bit.  
20. Parameter Sequence: The portion of the output unit absent the action sequence.  
21. Achieve State(s): Target(s) for the AI agent to achieve, arrived at by making the correct decision(s).  
22. Avoid State(s): Target(s) for the AI agent to avoid, arrived at by making the incorrect decision(s).

Note that many of these terms only appear in the provided reference source code.

Description:

We receive input from either recall (from the knowledge bank) or from one or more sensors. We discuss a modified artificial neural network approach. We introduce a neural network composed entirely of signed integers. We compose successive layers of integral artificial neurons which are the analog of the hidden layers used in floating point approaches. The first layer receives the input and the last layer sends output bit signals to the Output Unit. An artificial neuron is composed of a series of signed integer weights that refer to the input bit signals leading from it to the next layer as well as indicators for which artificial neurons in the next layer are addressed by the output bit signals. We select a number of these integral artificial neurons for each of the layers and a number of hidden layers in between. A portion (half in the provided reference source code) of the integral artificial neurons of a given layer sends negative integer values to the next layer and another portion (again, we use half in the provided reference source code) sends positive integer values to the next layer. The threshold for an integral artificial neuron to “fire” (meaning send its outputs to the next layer) is a pre-set limit (we use 0 or higher in the provided reference source code). To test whether a firing of a given artificial neuron occurs, we take the sum of all the integers from all the previous layer’s firing (or in the case of the first layer, from the Input Unit).

If an Achieve State is reached and rewards are available, increment all positive firing neuronal weights by one and decrement all negative firing neuronal weights by one (meaning one further from zero on the negative side). If an Avoid State is reached and disincentives are available, decrement all positive firing neuronal weights by one and decrement all firing weights by one (meaning one closer to zero on the negative side). If an Achieve State is reached and rewards are not available, remove said Achieve State from the data structure that contains Achieve States. If an Avoid State is reached and disincentives are not available, remove said Avoid State from the data structure that contains Avoid States.

When an integral artificial neuron weight reaches a pre-set limit (we use 0 in the provided source code), we “rewire” the artificial neuron to target a different artificial neuron of the next layer in the manner of your choice, whether round-robin, random, or some other method. We select a value for the weight for this new connection that is not equal to the pre-set limit that triggers our re-targeting (in the provided source code, it’s a nonzero value). Observe that for this to occur, our artificial neurons wire to the next layer sparsely, meaning that not all neurons from a layer are addressed by any neuron of the previous layer. We also take care to avoid having a neuron from one layer to target the same neuron of the next layer multiple times.

Once we have obtained our hash of the Current Input, we conjoin this to the end of the Input Queue and dequeue the oldest input from the beginning of the Input Queue. This is our short-term memory. We then perform another hash of the entire Input Queue, again either using option 1, using a hash of your choice, or option 2, using an additional neural network. With the hash of the entire Input Queue, we then seek to obtain an Output Unit. The Output Unit consists of an Action Sequence and a Parameter Sequence. This is the part of the cycle which addresses motivation and the Achieve State. When an Achieve State is reached, its identity is marked in the knowledge bank, either with an indicator bit in the location of the knowledge bank, or a database consisting of locations in the knowledge bank that, when reached last time, resulted in the Achieve State. This indicator is the Positive Feedback that the AGI needs in order to make decisions. The Knowledge Bank format appears as follows, with the bits representing a given Action Sequence conjoined to the given Parameter Sequence to form a composite unit:

Input State A \- Action Sequence 0 \-\> Location X  
Input State A \- Action Sequence 1 \-\> Location Y  
Input State A \- Action Sequence 2 \-\> Location Z

Locations X, Y, and Z contain hashes of inputs reached when Action Sequences 0, 1, and 2 were performed after Input Hash A, respectively.

This is a logical implication in the form of (JK-\>P) && (JL-\>Q) && (JM-\>R), which can be represented in 2CNF (conjunctive normal form) (\~P | JK) && (\~Q | JL) && (\~R | JM). A 2SAT instance can be solved in linear time and a 3SAT instance can be solved quickly. If you elect to use the 3SAT approach, simply pad each 2CNF clause with an additional false value to force them to have exactly 3 literals. If an action sequence is found that leads to an Achieve State, the next action to perform is the first action branching from the current state (current location in the Knowledge Bank) to the next state in the sequence eventually leading to an Achieve State. We repeat for all Achieve State(s) in the system and we are left with action/step pairs. We repeat for all Avoid State(s) in the system and we are again left with action/step pairs.

To select the most appropriate action, we find action matches between the Achieve action/step pairs and the Avoid action/step pairs where the number of steps in the Avoid action/step pair is less than or equal to the number of steps in the matching Achieve action/step pairs. We eliminate these actions as options. We also eliminate all other Avoid actions from action/step pairs that don’t match any actions from the Achieve action/step pairs. If two or more Achieve States remain and are available from the current state, the one with the fewest intermediate states is selected. If there are multiple sequences leading to an Achieve State that are all tied for the fewest intermediate states, one is chosen at random. If no Achieve State action/step pairs are available, an action to perform is chosen at random from the remaining actions that have not been eliminated as possibilities. If no actions remain available, we select the action/step pair that results in the greatest number of steps before an Avoid State is reached. The output from this 2CNF solver portion of the cycle is the Output Unit, which consists of the Output Recall Bit, the Action Sequence, and the Parameter Sequence.

One of the output bits is a recall bit where the AGI reads from its knowledge bank based on the portion of the output unit contained in the rest of the output unit (the Utility Sequence). For the AGI to perform tasks such as controlling robotics or outputting to a display terminal, there will be a portion of the Output Unit that contains the Utility Sequence. This Utility Sequence is then parsed so that the remainder of the Output Unit received from the 2CNF logic solver- the Parameter Sequence- is sent to the correct Output Target as identified by the Action Sequence from the solution of the 2CNF instance from the prior step of the cycle. The recall action accepts the Action Sequence and the Parameter Sequence from the Output Unit and uses that to address a location in the Knowledge Bank. The data from this location is fetched and sent into the input as the new Current Input. It is worth noting that the Knowledge Bank is updated with every single cycle with a location within the Knowledge Bank vectoring with an action to another location, possibly a new location, to reflect a dynamic world. If the recall bit is not set in the Output Unit, then the Parameter Sequence is sent to the Output Target addressed by the Action Sequence, and the action is performed according to the Parameter Sequence, and input comes from the current sensory source to begin the next cycle.
