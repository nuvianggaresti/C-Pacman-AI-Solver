# Pacman AI Solver (written in C)
Assignment 2 for COMP20003 Semester 2, 2019. Implementing modified Dijkstra's algorithm for an AI Pac-man solver using C.

The objective is to eat all food while avoiding ghosts. Pacman can also eat fruits which  allows it to eat the ghosts for a fixed period of time to earn bonus points. Once Pacman successfully eats all the food, it will level up. When Pacman has lost all lives, the game is over.

Dijkstra is implemented with 'budget'. The program will only expand nodes up to the budget, i.e. not until goal state (all food eaten) has been reached. The budget can be modified by user. My experimentation showed that budget is not necessarily correlated to AI performance, possibly due to ghosts' movements being a random factor.

Heuristic is used to choose the best action. It can be implemented using either the maximum or average of all children. I found that average propagation results in a more consistent performance, while maximum propagation can give better performances albeit less consistent. Tie is broken randomly.

Pacman performs similarly in all of Level 1, 2, and 3.

You can execute ./pacman <level> to play the game with keyboard.
<br /> To run the solver, you can execute ./pacman <level> <ai/aipause> <max/avg> <budget>
<br><br><br>

Acknowledgement:
<br />Base code was adapted by Nir Lipovetzky (2019) from Pacman For Console V1.3
<br />By: Mike Billars (michael@gmail.com)
<br />Date: 2014-04-26
