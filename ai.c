#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <assert.h>


#include "ai.h"
#include "utils.h"
#include "priority_queue.h"


struct heap h;

float get_reward( node_t* n );

/**
 * Function called by pacman.c
*/
void initialize_ai(){
	heap_init(&h);
}

/**
 * function to copy a src into a dst state
*/
void copy_state(state_t* dst, state_t* src){
	//Location of Ghosts and Pacman
	memcpy( dst->Loc, src->Loc, 5*2*sizeof(int) );

    //Direction of Ghosts and Pacman
	memcpy( dst->Dir, src->Dir, 5*2*sizeof(int) );

    //Default location in case Pacman/Ghosts die
	memcpy( dst->StartingPoints, src->StartingPoints, 5*2*sizeof(int) );

    //Check for invincibility
    dst->Invincible = src->Invincible;

    //Number of pellets left in level
    dst->Food = src->Food;

    //Main level array
	memcpy( dst->Level, src->Level, 29*28*sizeof(int) );

    //What level number are we on?
    dst->LevelNumber = src->LevelNumber;

    //Keep track of how many points to give for eating ghosts
    dst->GhostsInARow = src->GhostsInARow;

    //How long left for invincibility
    dst->tleft = src->tleft;

    //Initial points
    dst->Points = src->Points;

    //Remiaining Lives
    dst->Lives = src->Lives;

}

node_t* create_init_node( state_t* init_state ){
	node_t * new_n = (node_t *) malloc(sizeof(node_t));
	new_n->parent = NULL;
	new_n->priority = 0;
	new_n->depth = 0;
	new_n->num_childs = 0;
	copy_state(&(new_n->state), init_state);
	new_n->acc_reward =  get_reward( new_n );
	return new_n;

}


float heuristic( node_t* n ){
	float h = 0;

	int invincibility = 0;
	int life = 0;
	int game = 0;


	//FILL IN MISSING CODE
	if (n->state.Invincible == 1){
		invincibility = 10;
	}

	if (n->parent != NULL){
		if (n->state.Lives < n->parent->state.Lives){
			life = 10;
		}
	}

	if (n->state.Lives == 0){
		game = 100;
	}

	h = invincibility - life - game;

	return h;
}

float get_reward ( node_t* n ){
	float reward = 0;

	//FILL IN MISSING CODE
	if (n->parent != NULL){
		reward = heuristic(n) + n->state.Points - n->parent->state.Points;
	}
	else{
		reward = 0;
	}


	float discount = pow(0.99,n->depth);

	return discount * reward;
}

/**
 * Apply an action to node n and return a new node resulting from executing the action
*/
bool applyAction(node_t* n, node_t** new_node, move_t action){

	bool changed_dir = false;

	(*new_node)->parent = n; //1. points to parent

    //FILL IN MISSING CODE

    changed_dir = execute_move_t( &((*new_node)->state), action ); // utils.c line 5

	if (changed_dir == true){ //if action is applicable
		(*new_node)->depth = n->depth + 1;
		(*new_node)->priority =  (*new_node)->depth * -1 ; // 3. updates priority;
		(*new_node)->acc_reward = n->acc_reward + get_reward(*new_node); // 4&5. updates the accumulated reward
		(*new_node)->move = action;

		n->num_childs += 1;
	}

	return changed_dir;
}

//Function propagateBackScoreToFirstAction
/* Takes the pointer to auxiliary data struct (an array of floats, size 4)
 * that stores the acc_reward for the first 4 applicable actions.
 * Updates the content of the array based on max/avg propagation.
 */
void propagate_reward(propagation_t propagation, node_t* node, float **action_scores){

	if (propagation == max){
		int idx=0;
		node_t *node_tmp;
		node_tmp = node;

		while(node_tmp->parent->parent != NULL){
			node_tmp = node_tmp->parent;
		}

		idx = node_tmp->move;
		if (node->acc_reward > *(*action_scores+idx)){
			*(*action_scores+idx) = node->acc_reward;
		}
	}

	else if (propagation == avg){
		int idx=0;
		float tmp=0;
		float average=0;
		int children=0;
		node_t *node_tmp;
		node_tmp = node;

		while(node_tmp->parent->parent != NULL){
			tmp += node_tmp->acc_reward;
			node_tmp = node_tmp->parent;
			children++;
		}

		average = tmp / children;
		idx = node_tmp->move;
		if(average > *(*action_scores+idx)){
			*(*action_scores+idx) = average;
		}
	}
}


/**
 * Find best action by building all possible paths up to budget
 * and back propagate using either max or avg
 */
//I just realised after I was done that I don't need to pass propagation as an argument
move_t get_next_move( state_t init_state, int budget, propagation_t propagation, char* stats ){
	move_t best_action = rand() % 4;
	
	clock_t start = clock(); //Start of timing
	//Auxilliary data structure to store first 4 applicable action rewards
	float *action_score_ptr[1];
	float best_action_score[4];
	*action_score_ptr = best_action_score;
	for(int i = 0; i < 4; i++){
		best_action_score[i] = INT_MIN;
	}

	unsigned generated_nodes = 0;
	unsigned expanded_nodes = 0;
	unsigned max_depth = 0;

	node_t* generated; //malloc in ai.c:191
	node_t* expanded;

	//Add the initial node
	node_t* n = create_init_node(&init_state); // ai.c:62
	generated_nodes++;

	//Create an empty Array for freeing
	node_t** explored;
	explored = (node_t**)malloc(sizeof(node_t*) * budget);
	assert(explored);

	//Create initial heap
	struct heap h;
	heap_init(&h); // priority_queue.c:4
	heap_push(&h, n); //Use the max heap API provided in priority_queue.h
	//heap_push(&h,n);

	bool action = false;

	//FILL IN THE GRAPH ALGORITHM
	while(h.count > 0){ //while frontier != empty
		if (expanded_nodes < budget){
			expanded = heap_delete(&h); //frontier.pop()
			explored[expanded_nodes] = expanded; //explored.add(node)
			expanded_nodes++;
			for (int j=0; j < 4; j++){
				generated = (node_t*)malloc(sizeof(node_t));
				assert(generated);
				copy_state(&(generated->state), &(expanded->state));
				action = applyAction(expanded, &generated, j);
				if (action == true){
					generated_nodes++;
					propagate_reward(propagation, generated, action_score_ptr); //PropagateBackToFirstAction()
					if(generated->state.Lives < expanded->state.Lives){ //if lostLife(newNode)
						free(generated); //delete newNode
					}
					else if(generated->state.Lives == expanded->state.Lives){
						heap_push(&h, generated); //frontier.add(newNode)
					}
				}
				else{ //Pacman collides with a wall
					free(generated);
				}
			}
		}
		else{
			break;
		}
	}
	emptyPQ(&h);
	max_depth = (explored[expanded_nodes-1])->depth;

	int break_ties[4]; //For breaking ties
	int c=0;
	float tmp = INT_MIN;
	for (int n=0; n < 4; n++){ //Choosing best_action
		if (best_action_score[n] > tmp){
			tmp = best_action_score[n];
			best_action = n;
		}
		else if (best_action_score[n] == tmp){ //There is a tie
			break_ties[c] = n;
			c++;
		}
	}
	//Break ties randomly
	if (c != 0){
		break_ties[c] = best_action;
		c++;
		best_action = break_ties[rand() % c];
	}

	//Updating global variables for printing to output.txt
	GlobalSecs += ((double)(clock() - start)) / CLOCKS_PER_SEC; //End of timing
	
	GlobalExpNodes += expanded_nodes;
	GlobalGenNodes += generated_nodes;
	if (max_depth > GlobalDepth){
		GlobalDepth = max_depth;
	}
	
	sprintf(stats, "Max Depth: %d Expanded nodes: %d  Generated nodes: %d\n",max_depth,expanded_nodes,generated_nodes);

	if(best_action == left)
		sprintf(stats, "%sSelected action: Left\n",stats);
	if(best_action == right)
		sprintf(stats, "%sSelected action: Right\n",stats);
	if(best_action == up)
		sprintf(stats, "%sSelected action: Up\n",stats);
	if(best_action == down)
		sprintf(stats, "%sSelected action: Down\n",stats);

	sprintf(stats, "%sScore Left %f Right %f Up %f Down %f",stats,best_action_score[left],best_action_score[right],best_action_score[up],best_action_score[down]);
	
	for (int k=0; k < expanded_nodes; k++){ //freeMemory(explored)
		free(explored[k]);
	}
	free(explored);
	
	return best_action;
}
