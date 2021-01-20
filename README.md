# QuadTree
I am a student from HoWest DAE in Belgium and the following is an implementation of my research project for the class Gameplay Programming. My chosen topic is the QuadTree.

## Description
### What is a quadtree?
A quadtree is a tree data structure in which each node has exactly 4 children. The idea is to take a space and subdivide it into 4 subsections  (quad), and each subsection can 
also be subsectioned into 4, etc... We call it a tree because each node has a reference to all 4 of its subsections. Each quad has a set capacity, when that capacity is reached
the quad will subdivide itself into 4 new subsections.


### What is the purpose of a quadtree?
Let's think of a scenario where we have a space filled with agents. These agents must be able to interact with eachother. The fastest way to do this is to have each agent check
every other agent. However, if we have a 100 agents we would need to do 100 checks 100 times, which would, without any optimizations, result in 10 000 checks (O(n^2)). This
is where the use of the quadtree comes in. When using the quadtree we can give each agent a set range where they only check the agents within that range. When the tree is
subdivided into multiple sections, only the sections within the range need to be checked (O(log n)), which reduces the amount of checks we must do greatly.

![](https://github.com/GianniWalraet/QuadTree/blob/master/Media/QuadTreeExample.gif)

## My implementation
### Framework
For my implementation I will be using a framework in C++ that was provided by DAE. We'll be implementing the quadtree in a scenario where Flocking is implemented for the agents.

The most important classes in the framework which we will be using are:

- SteeringAgent.cpp/SteeringAgent.h
  - Contains all the functionality for the agents we are using
- FlockingSteeringBehaviors.cpp/FlockingSteeringBehaviors.h
  - Contains SteeringBehaviors for Flocking for the SteeringAgents
- QuadTree.cpp/QuadTree.h
  - New class created to implement the quadtree
- TheFlock.cpp/TheFlock.h
  - This class will execute our code for the quadtree, the steeringAgents and their behaviors

### Functionality of the QuadTree Class
Below is an overview of what the QuadTree header file contains. I will be going over the functionality of all the functions. Firstly I will give a short description on the
functionality of the member variables.

- m_BoundingBox (Rectf)
  - The boundaries of the Quad
- m_Capacity (int)
  - The maximum amount of agents before the quad should subdivide
- m_pAgents (vector of Steeringagents)
  - Vector of agents that stores the agents that are inside the quad
- m_pTopLeft, m_pTopRight, m_pBottomLeft, m_pBottomRight (QuadTree)
  - The children of the current Quad
- m_IsDivided (bool)
  - If this variable is true, the Quad is divided into subsections. If false it is not.
  
![alt text](https://github.com/GianniWalraet/QuadTree/blob/master/Media/QT_Header.PNG)

#### Constructor and Destructor
The constructor and destructor functionality is quite self-explanatory. The constructor takes a couple parameters (bottom and left location and width and height of the quadtree
and the capacity) and initializes all of the members variables. By default it is not divided. The destructor deletes the subdivisions (if they are not nullpointers).

![alt text](https://github.com/GianniWalraet/QuadTree/blob/master/Media/QT_Construct_Destruct.PNG)

#### AddAgent
Used to loop over all of the agent at the start of the program and insert them into the QuadTree. The tree will also make subdivisions when it reaches
its capacity. Once it is subdivided, it will also recursively loop over its children. This function will only be used once at the start of the program.

![alt text](https://github.com/GianniWalraet/QuadTree/blob/master/Media/QT_AddAgent.PNG)

#### Subdivide & Merge
The subdivide function will subdivide the Quad, making each child node into a new QuadTree. It will also set m_IsDivided to true. The Merge function will delete the child nodes
and make them nullpointers. Subdivide is called when the capacity of a quad is reached, merge is called when there are fewer agents than the max capacity in the quad.

![alt text](https://github.com/GianniWalraet/QuadTree/blob/master/Media/QT_Subdivide.PNG)
![alt text](https://github.com/GianniWalraet/QuadTree/blob/master/Media/QT_Merge.PNG)

#### RegisterNeighbours
Handles communication between agents for each agent. This function has 4 parameters:

- pAgent
  - The agent which we are currently checking its neighbours for.
- queryRadius
  - The range where the agent should look for neighbours. Only agents inside the radius will be registered.
- pNeighbours
  - The vector that holds our neighbours. We will add found neighbours into this vector.
- nrOfNeighbours
  - The amount of neighbours. Increments each time we find a new neighbour.

If there are no agents in the current Quad or only the agent we are checking, the function will return. It will also return if the current Quad is not inside the queryRadius.
If we pass all of these requirements, it means the agent is inside the quad with other agents. We will loop over the agents and add them to the pNeighbours vector if they are
inside the queryRadius. We'll be using a memory pool for the neighbours, so we'll also increment our nrOfNeighbours each time we add a new neighbour. More info on the memory pool
can be found here https://en.wikipedia.org/wiki/Memory_pool.  Once this is done, we check if the quad has been divided yet. If it is, we recursively repeat the same process for
each of its children. If it is not, we step out of the function.

![alt text](https://github.com/GianniWalraet/QuadTree/blob/master/Media/QT_RegisterNeighbours.PNG)

#### Update
The Update function is our most important function. It will consistently update the QuadTree for each agent by subdividing and merging depending on the capacity of the Quad.
First, it will check if our current agent is inside the bounds of the Quad. If it is not, we'll check if the agent is in the agent vector of the quad, and remove it from
the vector if it is. After doing this, if the Quad is divided and the agent vector is smaller than the capacity, we will merge the Quad. However, if our agent IS inside the
bounds, we will skip this process. We'll then proceed to check if the size of the agent vector is lower than the capacity. If so and it has been divided, we will merge it.
We'll also add the agent to the agent vector. If the size of the agent vector is not lower than the capacity, we will subdivide the Quad if it hasn't been subdivided, and 
proceed to recursively Update each of its children.

![alt text](https://github.com/GianniWalraet/QuadTree/blob/master/Media/QT_Update.PNG)

#### Render
A simple function to render our QuadTree so we can check if it works correctly.

![alt text](https://github.com/GianniWalraet/QuadTree/blob/master/Media/QT_Render.PNG)

### Using the QuadTree Class
The final step is to implement the QuadTree class in our framework. We'll do this in the TheFlock class. First we will initialize the QuadTree with a position, width and height.
We'll give it a capacity of 4. Next add all of the agents to the QuadTree. Finally we will continuously update it and register each agents' neighbours.

![alt text](https://github.com/GianniWalraet/QuadTree/blob/master/Media/QT_Initialize.PNG)
![alt text](https://github.com/GianniWalraet/QuadTree/blob/master/Media/QT_UpdateTree.PNG)

## Result
Below is an example of my implementation. We can see the tree update itself as the agents move around. 1 agent moves seperately from the others, this one will be evaded by the
other agents. When the agents are in 1 spot there are a lot of subdivisions there, and quads further away have much fewer subdivisions. We can apply flockingbehaviors to our
agents, and the will execute these together with their registered neighbours. We have a fully functional QuadTree.

![alt text](https://github.com/GianniWalraet/QuadTree/blob/master/Media/Result.gif)

## Conlusion
We can conclude from this project that the best way to implement a QuadTree is by making use of recursive functions. Though there is always room for more features, the
implementation works as intended and runs smoothly. Potential idea to add later on:
- Make it so the tree can only subdivide a set amount of times


I hope everything was explained well and taught you something about how QuadTrees work!
