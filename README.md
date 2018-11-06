# polysat
A SAT solver which implements DPLL algorithm.
Made under an assignment done under Prof. Subhajit Roy

# HOW TO RUN
            g++ -o polysat polysat.cpp
            ./polysat dimacs.txt
  where, dimacs.txt contains the logical formula in DIMACS representation
  

# Algorithm
### Initial Operations
- Removed Duplicate elements from clauses
- Remove Tautologies (a V -a)
- Apply Unit Propogation on clauses of unit length
- Assign values to pure literals

### Propogation
- If all clauses are SAT then return SAT
- If decision level 0 led to conflict then return UNSAT
- Assign value to a variable chose by using MOMS algorithm.
- Apply unit propogation on it.
- In case of conflict, Backtrack to the previously unassigned literal and repeat

### Optimisation
- Applied Watchted list/literal Concept to optimise unit propogation

### Future Scope
- To implement CDCL(Conflict Driven Clause Learning) with it.
