#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <algorithm>
#include <math.h>

using namespace std;

struct block
{
	long long int v;		//value
	long long int t;		//truth value
};

//Variables to be used frequently
long long int i,j,k;
long long int clauses,variables;					//To store number of clauses and variables
vector <vector <long long int> > watchedlist;

//Vector to store out LOGIC FORMULA
vector <vector <long long int> > formula;
vector <vector <block> > newFormula;
vector <long long int> length;

//Vector to store unit clauses
vector <long long int> unitClause;
long long int unitClauseInitial; 					//To store the pos from where assumed unit clauses start


//A compare function to feed to sort (sort as per absolute value)
bool absoluteCompare(long long int a, long long int b)
{
	return (abs(a) < abs(b));
}

//A compare function to feed to sort (sort as per vector size)
bool setSizeCompare(vector <long long int> a, vector <long long int> b)
{
	return (a.size() < b.size());
}




//Read input in DIMACS representation from file
void readInputFromFile(char *filename)
{
	//Declare variable
	ifstream fin;

	//Open input file
	fin.open(filename);

	//Check for error
	if(!fin)
	{
		cerr << "Error: Unable to open file " <<filename<<"\n";
		exit(1);
	}

	//Ignore comment lines
	char line[1000];
	fin.getline(line,1000);
	while(line[0] == 'c' || line[0] == '\0')
	{
		fin.getline(line,1000);
	}

	//Take info of Number of variables and number of clauses
	char *token = strtok(line," ");   //Read p
	token = strtok(NULL, " ");        //Read cnf
	token = strtok(NULL, " ");        //Read number of variables
	variables = stoi(token);		  //Convert it to integer
	token = strtok(NULL, " ");        //Read number of clauses
	clauses = stoi(token);			  //Convert it to integer

	//Read Clauses and update our vector
	long long int buffer,max=0;

	vector <long long int> temp;

	temp.reserve(variables);		  //Reserving some space to save time
	formula.reserve(clauses);
	unitClause.reserve(variables);
	vector<long long int > v;
	watchedlist.assign(variables+1,v);

	while(!fin.eof())
	{
		buffer = 1;
		fin>>buffer;					//Read an integer value (literal)
		if(buffer > max) max = buffer;
		if(buffer == 0)					//Until end of clause is reached
		{
			formula.push_back(temp);	//Push one clause to the list of clauses
			temp.clear();				//Clear your temp
		} 
		else temp.push_back(buffer);	//Create a clause
	}

	//Check if number of clauses entered are according to given data
	if(formula.size() != clauses)
	{
		cerr << "Error: Number of Clauses entered do not match with the given data\n";
		clauses = formula.size();
	}
	if(max > variables)
	{
		cerr << "Error: Number of variables entered do not match with the given data\n";
		variables = max;
	}

	//Read Input!
}




//Function to remove duplicates from clauses
void removeDuplicates()
{
	long long int size = formula.size();
	for(i=0; i < size; ++i)
	{
		sort(formula[i].begin(),formula[i].end());                  //Sort each clause
		long long int subSize = formula[i].size();
		for(j=1 ; j < subSize;)
		{
			if(formula[i][j] == formula[i][j-1])					//If jth element in clause is equal to (j-1)th element in the clause
				formula[i].erase(formula[i].begin() + j),subSize--;			//j not incremented as element is deleted!
			else j++;												//Else move to the next element
		}
	}
}



//Function to remove tautologies form clauses
void removeTautologies()
{
	long long int formulaSize = formula.size();
	for(i=0; i < formulaSize; ++i)
	{
		sort(formula[i].begin(),formula[i].end(),absoluteCompare);			//Sort based on absolute value of numbers

		long long int size = formula[i].size();
		for(j=1;j < size; j++)
		{
			if(formula[i][j] == -formula[i][j-1])							//If a and -a are both in the clause implies a tautology
			{
				formula.erase(formula.begin() + i);							//Then remove that clause
				formulaSize--;
				i--;														//Take care of size reduction
				break;
			}
		}
	}
}



//Apply unit Clause propogation over formula
void initialUnitClausePropogation(long long int start)
{
	//Deleting all clauses with this unit clause as one of its input
	long long int unitSize = unitClause.size();						//Store sizes in variables to save time of calculating them everytime in loop
	long long int size = formula.size();

	for(i=start;i<unitSize;++i)											//Over all elements present in the unitClause vector
	{
		for(j=0;j<size;++j)											//For all clauses not unit clause
		{
			long long int clauseLength = formula[j].size();			//Small optimisation
			for(k=0;k<clauseLength;++k)
			{
				if(formula[j][k] > unitClause[i] && formula[j][k] > -unitClause[i]) break;		//Since the list is sorted

				if(formula[j][k] == unitClause[i])												//If unit clause is a subset of the clause
				{
					formula.erase(formula.begin() + j);											//Remove this TRUE clause
					size--;																		//Update size of our new formula
					j--;																		//Update j
					break;
				}

				if(formula[j][k] == -unitClause[i])												//If negation of unitClause if found
				{
					formula[j].erase(formula[j].begin() + k);									//Delete that from clause
					k--;																		//Update k accordingly
					clauseLength--;																//Update clauseLength accordingly

					if(formula[j].size() == 1)													//If now the clause become unit clause
					{
						if(!count(unitClause.begin(),unitClause.end(),formula[j][0]) && !count(unitClause.begin(),unitClause.end(),-formula[j][0])) 
						{
							unitClause.push_back(formula[j][0]);									//Add it to the unitClause vector
							unitSize++;			
						}													//Increase that vector size

						formula.erase(formula.begin() + j);										//Delete this unit clause from formula
						j--;																	//Update j accordingly
						size--;																	//Update size accordingly
						break;
					}
					if(formula.size()==0) return;
					break;
				}
			}
		}
	}

}



//If a unit length clause is present then remove all clauses having that unit length clause
void findUnitClauses()
{
	//A sort function to arrange all clauses based on their size...
	sort(formula.begin(),formula.end(),setSizeCompare);

	//Sort all clauses
	long long int size = formula.size();
	for(i=0; i<size; ++i ) sort(formula[i].begin(),formula[i].end());

	//eliminating all the unit clauses from our main formula list and putting them in new vector
	for(i=0;formula[i].size()==1;)
	{
		if(!count(unitClause.begin(),unitClause.end(),formula[i][0]) && !count(unitClause.begin(),unitClause.end(),-formula[i][0]))
		{
			unitClause.push_back(formula[i][0]);
			formula.erase(formula.begin() + i);
			if(formula.size()==0) break;
		}
	}

	//Unit Clause propogation
	initialUnitClausePropogation(0);
	
	//A sort function to arrange all clauses based on their size...
	sort(formula.begin(),formula.end(),setSizeCompare);

}



//Function to eliminate pure literal
int initialEliminatePureLiteral()
{
	//Declare an array to store the no. of positive and negative occurences of a variable... var[i][0] = number  of positive occurences of i, var[i][1] for negative
	long long int var[variables][2];

	//Initialise it
	for(i=0;i<variables;++i) var[i][0] = var[i][1] = 0;

	//Calculate occurences of each variable
	long long int size = formula.size();
	for(i=0; i<size; ++i)
	{
		long long int subSize = formula[i].size();
		for(j=0 ; j<subSize ; ++j)
		{
			if(formula[i][j] > 0) var[abs(formula[i][j])][0]++;		//For positive, index 0
			else var[abs(formula[i][j])][1]++;						//For negative, index 1
		}
	}

	//To store previous unitClause vector size
	long long int prevSize = unitClause.size();
	for(i=1 ; i<variables ; ++i)
	{
		if(!var[i][0])								//If anyone of them is 0 or both zero
		{ if(!count(unitClause.begin(),unitClause.end(),-i) && !count(unitClause.begin(),unitClause.end(),i)) unitClause.push_back(-i); } 
		else if(!var[i][1])
		{ if(!count(unitClause.begin(),unitClause.end(),i) && !count(unitClause.begin(),unitClause.end(),i)) unitClause.push_back(i); }
	}

	int benefit = 0;
	//Take these TRUE and apply unitClause propogation
	if(unitClause.size() > prevSize) 
	{
			initialUnitClausePropogation(prevSize);

			benefit = 1;
	}

	return benefit;
}




//Function to apply MOMS algorithm
long long int MOMS()
{
	long long int var[variables + 1][2];
	long long int valIndex,sign;
	

	long long int mainsize = newFormula.size();

	//Calculate minimum length of clauses
	long long int min = 1000000;
	long long int temporary = length.size();
	for(i=0;i<temporary;++i) if(length[i] < min && length[i]>1) min=length[i];

	while(1)
	{	
		for(i=0;i<variables+1;++i) var[i][0]=var[i][1]=0;

		//Checking how many times each variable occured in minimum length clauses
		for(i=0; i<mainsize; ++i)
		{
			if(length[i] == min)
			{
				if(newFormula[i][0].t == 1 ) continue;
				long long int newsize = newFormula[i].size();
				for(j=0; j<newsize ;++j)
				{
					if(newFormula[i][j].t == 0)
					{
						if(newFormula[i][j].v > 0) var[newFormula[i][j].v][0]++;
						else var[-newFormula[i][j].v][1]++;
					} 
				}


			}
		}

		long long int max=0;
		sign=-1;
		valIndex=-1;

		//Finding which variable occured the most no. of times
		for(i=0; i<variables+1; ++i)
		{
			if(var[i][0]  > max)
			{
				max = var[i][0];
				valIndex = i;
				sign = 0;
			}
			if(var[i][1] > max)
			{
				max = var[i][1];
				valIndex = i;
				sign = 1;
			}
		}

		if(valIndex != -1) break;
		else min++;
	}


	if(sign == 0)
		return valIndex;
	else
		return -valIndex;

}




//Unit Propogation
int unitPropagate(long long int choice)
{
	
	long long int size = watchedlist[abs(choice)].size();
	for(long long int p=0; p<size; ++p)
	{
		i = watchedlist[abs(choice)][p];
		//If this clause is already SAT
		if(newFormula[i][0].t == 1 ) continue;

		long long int insize = newFormula[i].size();
		for(j=0; j<insize; j++)
		{
			//If this clause contains our truth value
			if(newFormula[i][j].v == choice)
			{
				newFormula[i][j].t = 1;
				long long int temp1;
				temp1 = newFormula[i][j].v;

				//Swapping values with the first index i.e move truth value one to the first index
				newFormula[i][j].v = newFormula[i][0].v;
				newFormula[i][j].t = newFormula[i][0].t;
				newFormula[i][0].v = temp1;
				newFormula[i][0].t = 1;

				continue;

			}

			//If this clause contains negation of our truth value
			if(newFormula[i][j].v == -choice)
			{
				newFormula[i][j].t = -1;
				length[i]--;

				//If that truth value is in 1st or second index
				if(j==0 || j==1)
				{
					long long int find=-1;
					//Search for any unassigned value in the clause
					for(k=2;k<insize;++k)
					{
						if(newFormula[i][k].t == 0) {find = k;break;}
					}

					//If no index is there with unassigned value
					if(find == -1)
					{
						if(j==0)
						{
							if(newFormula[i][1].t == -1) return -1;			//If all values of the clause are assigned false => Contradiction
							else 											//Else this leftover unassigned value need to be true
							{
								if(!count(unitClause.begin(),unitClause.end(),newFormula[i][1].v) && !count(unitClause.begin(),unitClause.end(),-newFormula[i][1].v)) unitClause.push_back(newFormula[i][1].v);	//Else we know that the other element has to be true
							}
						}
						else if(j==1)
						{
							if(newFormula[i][0].t == -1) return -1;
							else
							{
								if(!count(unitClause.begin(),unitClause.end(),newFormula[i][0].v) && !count(unitClause.begin(),unitClause.end(),-newFormula[i][0].v)) unitClause.push_back(newFormula[i][0].v);
							}
						}
					}
					else 												//If there is an unassigned value in the clause then swap it with this one
					{
						//Swap this false value with some assigned index = find
						long long int temp1,temp2;
						temp1 = newFormula[i][j].v;
						temp2 = newFormula[i][j].t;
						newFormula[i][j].v = newFormula[i][find].v;
						newFormula[i][j].t = newFormula[i][find].t;
						newFormula[i][find].v = temp1;
						newFormula[i][find].t = temp2;
					}

				}
			}

		}

	}

	return 0;
}



void pureLiteralSimplification()
{
	long long int var[variables+2][2];

	//Initialise
	for(i=0;i<variables+1;++i) var[i][0]=var[i][1]=0;

	//Checking how many times each variable occured in clauses
	long long int mainsize = newFormula.size();
	for(i=0; i<mainsize; ++i)
	{
		
		if(newFormula[i][0].t == 1 || newFormula[i][1].t == 1 ) continue;
		long long int newsize = newFormula[i].size();
		for(j=0; j<newsize ;++j)
		{
			if(newFormula[i][j].t == 0)
			{
				if(newFormula[i][j].v > 0) var[newFormula[i][j].v][0]++;
				else var[-newFormula[i][j].v][1]++;
			} 
		}
	}

	for(i=0;i<variables+1;++i)
	{
		if(!var[i][0] && var[i][1])
		{
			if(!count(unitClause.begin(),unitClause.end(),i) && !count(unitClause.begin(),unitClause.end(),-i)) unitClause.push_back(-i);
		}
		else if(var[i][0] && !var[i][1])
		{
			if(!count(unitClause.begin(),unitClause.end(),i) && !count(unitClause.begin(),unitClause.end(),-i)) unitClause.push_back(i);
		}
	}
}










//Propogate function to simply our set of clauses to the best it can
int Propogate(long long int choice)
{
	int conflict;					//To check for conflict
	long long int size = unitClause.size();
	long long int track = choice;
	while(1)
	{
		conflict = unitPropagate(unitClause[track]);		//Perform Unit Propogation
		pureLiteralSimplification();
		size = unitClause.size();

		if(conflict == -1) return -1;
		track++;
		if(track >= size) break;
	}

	

	return 0;
}



//Check if the formula is already satisfied
int check()
{
	long long int size = newFormula.size();
	for(i=0; i<size; ++i)
	{
		if(newFormula[i][0].t != 1) return 0;		//Check if all formulas are satisfied
	}
	return 1;
}


//To undo the assignment we did
void undo(long long int ind)
{
	for(i=unitClause.size()-1;i>=ind;--i)
	{
		long long int size = watchedlist[abs(unitClause[i])].size();
		for(long long int p=0;p<size;++p)
		{
			j = watchedlist[abs(unitClause[i])][p];
			long long int newsize = newFormula[j].size();
			for(k=0;k<newsize;++k)
			{
				if(newFormula[j][k].v == unitClause[i] || newFormula[j][k].v == -unitClause[i])
				{
					newFormula[j][k].t = 0;
					if(newFormula[j][k].v == -unitClause[i]) length[j]++;
				}	
			}
		}

			unitClause.pop_back();
	}

}




//Our main solve function
int solve()
{
	//Vector to keep track of assumed indexes
	vector <block> assumedIndexes;
	int conflict;

	while(1)
	{

		while(conflict!=-1)
		{
			//Apply MOMS to choose efficient variable
			long long int choice = MOMS();
			cout<<choice<<" ";
			//Assign it true value
			block x;
			x.v = unitClause.size();
			x.t = 0;
			assumedIndexes.push_back(x);
			unitClause.push_back(choice);

			//Propogate this value
			conflict = Propogate(unitClause.size()-1);

			if(check()) return 1;
		}
		cout<<endl;
		// for(k=0;k<assumedIndexes.size();++k) cout<<"("<<assumedIndexes[k].v<<","<<assumedIndexes[k].t<<")"; cout<<endl;
		// if(assumedIndexes.size()==140 && assumedIndexes.back().t==1)
		// {
		// 	for(i=0;i<unitClause.size();++i) 
		// 		cout<<"#"<<unitClause[i]<<endl;

		// 	for( i=0; i<newFormula.size(); ++i)
		// 	{
		// 		for( j=0; j<newFormula[i].size() ; ++j)
		// 		{
		// 			cout<<"("<<newFormula[i][j].v<<","<<newFormula[i][j].t<<") ";
		// 		}
		// 		cout<<endl;
		// 	}
		// }
		
		while(1)
		{
			if(assumedIndexes.back().t != 1) break;
			undo(assumedIndexes[assumedIndexes.size()-1].v);
			assumedIndexes.pop_back();
		}

		if(assumedIndexes.size()==0) return 0;

		long long int temo = unitClause[assumedIndexes.back().v];
		undo(assumedIndexes[assumedIndexes.size()-1].v);

		unitClause.push_back(-temo);
		assumedIndexes.back().t = 1;

		//Propogate this value
		conflict = Propogate(unitClause.size()-1);

		if(check()) return 1;	

		cout<<assumedIndexes.size()<<" "<<conflict<<endl;
		for(i=0;i<assumedIndexes.size();++i) cout<<assumedIndexes[i].t; cout<<endl;
		
	}
	
	/* ***** To change it soon!!!!!  ****** */
	return 0;
}












//Function to check for Satisfiability by using semantic Tableau + DPLL approach
int CheckForSatisfiability()
{
	//If all clauses are SAT
	if(formula.size() == 0) return 1;

	//If any clause is empty
	long long int size = formula.size();
	for(i=0;i<size;++i) if(formula[i].size() == 0) return 0;

	//A sort function to arrange all clauses based on their size...
	sort(formula.begin(),formula.end(),setSizeCompare);

	newFormula.reserve(clauses);
	vector <block> tempo;
	tempo.reserve(variables);

	//Creating a new data structure to track truth value of the assignments also
	for(i=0; i<size; ++i)
	{
		long long int temp = formula[i].size();


		for(j=0; j<temp; ++j)
		{
			block x;
			x.v = formula[i][j];
			x.t = 0;
			tempo.push_back(x);

			watchedlist[abs(x.v)].push_back(i);
		}

		newFormula.push_back(tempo);
		length.push_back(formula[i].size());
		tempo.clear();
	}

	
	return solve(); 
	
}







int main(int argc, char *argv[])
{
	//Checking if the user entered the filename or not
	if(argc == 1)
	{
		cout<<"Please Enter the DIMACS file name as command line input\n";
		return 0;
	}

	//Read From file and make the corresponding vector (Doubly Linked List)
	readInputFromFile(argv[1]);

	// Remove duplicates from each clause as ava == a!!
	removeDuplicates();

	//Remove Tautologies
	removeTautologies();

	// Remove Supersets
	findUnitClauses();

	//If a literal has only one polarity over all clauses assign it TRUE without any harm
	int benefit = initialEliminatePureLiteral();
	while(benefit) benefit = initialEliminatePureLiteral();


	unitClauseInitial = unitClause.size();

	//Solve It
	int satisfiability = CheckForSatisfiability();

	if(satisfiability == 1) cout<<"SAT\n";
	else cout<<"UNSAT\n";

	for(i=0;i<unitClause.size();++i) 
		cout<<"#"<<unitClause[i]<<endl;

	for( i=0; i<newFormula.size(); ++i)
	{
		for( j=0; j<newFormula[i].size() ; ++j)
		{
			cout<<"("<<newFormula[i][j].v<<","<<newFormula[i][j].t<<") ";
		}
		cout<<endl;
	}

	return 0;
}




