# Ravine ECS
This repository is the **Entity Component System** architecture implementation to be integrated with the [Ravine](https://github.com/gabriellanzer/Ravine) framework. It is written using C++14 features and has a special storage focused on linear cache access to ensure extreme performance. That is achieved through the concept of cyclic arrays, and done through a couple of control operations. The performance was validated through a benchmark against Skypjack's [EnTT](https://github.com/skypjack/entt) library, and it's repo can be found [here](https://github.com/gabriellanzer/Ravine-ECS-Benchmark) (TL;DR: got almost twice the iteration performance - but EnTT has many more features), I also did an [integration with OpenGL and a comparison of the DOD ECS vs Bad OOD](https://github.com/gabriellanzer/Ravine-ECS-Showdown) - sorry linux users, Visual Studio solutions only...

## How to use

TODO - Document the general API usage. Code is mostly self-explanatory though... And the *main.cpp* shows an example. Besides that, the [OpenGL integration repo](https://github.com/gabriellanzer/Ravine-ECS-Showdown) is quite interesting in performance-wise operations (though very WIP).

## Storage Scheme 
To ensure the lowest cache-miss frequencies as possible, while mantaining a few nice features of linear access, I decided to have storage **arrays per component types**. Each of these arrays is holds groups of components, **ordered by their entities archetypes**. A given storage state is represented by the following diagram:

![Storage State Diagram](/images/groups_representation.png)


## Cyclic Arrays
There are a few interesting points. In this representation, each component has the internal entity id (position inside the group) written on it. It is possible to see that some groups have an offset with respect to their actual initial positions (id 0 is shifted). That is due to the **cyclic behaviour** of each group. Most notably, the **Blue Group** represents the entity archetype of types A and B, and it has different offsets on each of it's storage arrays (0 on Storage\<A> and 2 on Storage\<B>).

The concept of cyclic arrays is to avoid re-allocating the whole array each time a insertion needs to happen, and to avoid any kind of look-up table, which would trash the cache lines. In this concept, there are a few operations needed to maintain the arrays in order (respecting the offset). There is, also, a special way of iterating through those arrays - we want to squeeze every inch of the CPU.

Here is a representation of the cyclic array being pushed aside (clock-wise roll operation), which demonstrates how the cyclic behaviour works.

With offset at 0 (aligned):

![Cyclic Array 0](images/cyclic_array_0.png)

Being push aside (clock-wise):

![Cyclic Array 1](images/cyclic_array_1.png)
![Cyclic Array 2](images/cyclic_array_2.png)
![Cyclic Array 3](images/cyclic_array_3.png)

The final state, offset at 3:

![Cyclic Array 4](images/cyclic_array_4.png)

Instead of accessing this array at the first element, the iteration happens from the position 3 (zero-based) to position 5, and wraps around from 0 to 2. The idea is expanded through groups that are shared across many component storages (when they represent complex archetypes). The idea is to ensure each of the pointers match the correct components of a given entity, regardless of the group offset. The following chapter will explain the basic storage operations and how the actual iteration works.

## Storage Operations
For performance reasons, both the creation and removal of entities might be deferred to the end of the frame. From the storage perspective, it receives a list of components to work with, thus enabling the batching of operations. These operations could happen atomically, but require special handling of current iteration pointers (on the TODO list for now).

Keep in mind that the actual entity creation and destruction operations need to manage multiple storages at the same time, and every storage operation manages multiple component groups. All of that is bound to the registry of component groups, described above.

### **Groups Registry**
Components groups, as described on the [Storage Scheme](https://github.com/gabriellanzer/Ravine-ECS#storage-scheme), are definitions of **agroupments by entity archetypes** bound across multiple component storages. The registry holds the combination of every possible query result per archetype request. For instance, if given system requests for the components of type **A and B** it performs a query through the storages **A** and **B** for all the combinations of the archetype **{A, B}**, which are: **{A, B}, {A} and {B}**.

On a given case, it might happen that the storage **A** only has the group **{A, B}** registered whilst the storage **B** has both **{A, B} and {B}**, because there are no entities with a single (and only) component **A**. That lead me to create a distributed group registry, per component storage, whereas a new group is registered for each type of that group archetype. Aka.: A new group **{A, B, C}** will be registered on the storages **A**, **B** and **C** with all the possible archetype combinations that are valid for each storage type.

To get the valid combinations per storage, I perform the set exclusion for the archetype and compute all valid permutations without type, plus the own storage type. On the above example, the storage **A** will have combinations **{A, B, C}, {A, B} and {A, C}** as well as it's own type **{A}** registered, whilst the storage **B** will have combinations **{B, A, C}, {B, A}, {B, C}, {B}**; the storage **C** follows the same rule. Keep in mind that the order doesn't matter, so **{A, B} equals {B, A}**.

### **Insertion**
The following diagrams represent the insertion of components of a given type on the proper storage, **A, B and C** are groups of different archetypes whose components **a4, b6, b7 and c8** belong to. Keep in mind that the components are sorted the same way that the groups are.

![Creation Intro](images/creation_intro.png)

Check if the storage has enough size:

![Check Storage Size](images/creation_0.png)

Figure what component goes where:

![Figure insert positions](images/creation_1.png)

From **right to left**, start to open spaces for complex archetype groups. Roll group C three positions (for **a4, b6, b7**):

![Make space for A and B](images/creation_2.png)

To insert **c8** at the end of the array (before **c0**), shift **c7** left (which wraps to the end of the group):

![Shift for c8 insertion](images/creation_3.png)

Now we can insert **c8** and make space for **a4** by rolling the B group in a clock-wise manner:

![Roll B and insert c8](images/creation_4.png)

To insert **b6 and b7** shift components to the left and wrap around:

![Shift to insert b6 and b7](images/creation_5.png)

All space is open, now to insert **a4, b6 and b7**:

![Insert remaining components](images/creation_6.png)

The final storage state:

![Final insertion state](images/creation_7.png)

### **Removal**

The removal operation is design to work with a list of Components marked to be destroyed at the current frame. In the following diagrams, the components **a3, b4, b7, c4, c5 and c8** will be removed from the storage groups **A, B and C**. Like the creation operation, the components are sorted the same way of the groups they belong to.

![Removal Intro](images/removal_intro.png)

First, we compute the compression operation:.

![Compression Operations](images/removal_0.png)

Each compression happens on a per-group basis, from left to right:

![Compression of Group A](images/removal_1.png)

When a new group is compressed it opens space in between:

![Compression of Group B](images/removal_2.png)

That must be closed by rolling the group in a counter-clockwise manner:

![Filling the Gaps (A and B)](images/removal_3.png)

Inside each group, the compression happens from the left to the right as well:

![Compression of Group C (left)](images/removal_4.png)
![Compression of Group C (right)](images/removal_5.png)

The ammount of spaces to be filled are accumulated through the compression operations, so they need to to be calculated separatelly.

![Filling the Gaps (B and C)](images/removal_6.png)

And the final storage state:

![Final removal state](images/removal_7.png)

### **Iteration**

The iteration of components are bound to the [groups registry](https://github.com/gabriellanzer/Ravine-ECS#groups-registry). Whenever a system requests for an archetype of components, for instance **{A, B}**, it invokes queries for the storages of types **A** and **B**. Those queries return, per storage type, the list of groups that must be iterated through. That iteration happens as stated on the [cyclic array definition](https://github.com/gabriellanzer/Ravine-ECS#cyclic-arrays), with an aditional alignment limitation across groups. The following diagrams illustrate the iteration process:

![Iteration Intro](images/iteration_intro.png)

First we select the storages of type **A** and **B**:

![Storage Selection](images/iteration_0.png)

Then we perform a query of the entities with, at least, the archetype **{A, B}**, which return a selection of groups:

![Archetype Query](images/iteration_1.png)

The groups are ordered in the registry the same way they are allocated (left to right):

![Yellow Group Aligned](images/update_0.png)

A system has it's update function called for each group iteration step. The number of steps might range from 1 to N, with N beeing the number of types on the query archetype. This happens due to the groups alignment across storages. On step, the initial pointer per component type and array sizes are adjusted to match the step range. The Yellow Group has a single iteration step, because it is aligned:

![Yellow Iteration Step 1, It 0](images/update_0_0.png)
![Yellow Iteration Step 1, It 1](images/update_0_1.png)
![Yellow Iteration Step 1, It 2](images/update_0_2.png)

The blue group is misaligned across component storages:

![Blue Group Misaligned](images/update_1+2.png)

And iteration happens in two steps, from left to right and wraps around. The first step goes as following:

![Blue Iteration Step 1, It 0](images/update_1_0.png)
![Blue Iteration Step 1, It 1](images/update_1_1.png)
![Blue Iteration Step 1, It 2](images/update_1_2.png)

The second step is aligned:

![Blue Iteration Step 2, It 0](images/update_2_0.png)
![Blue Iteration Step 2, It 1](images/update_2_1.png)

As a result, a storage iteration has a best and worst cache-coherency access pattern. TODO: Finish results...