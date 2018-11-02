#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <algorithm>

using namespace std;

//Vector to store out LOGIC FORMULA
vector <vector <long long int> > formula;

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
	char line[10000];
	fin.getline(line,10000);
	while(line[0] == 'c')
	{
		fin.getline(line,10000);
	}

	//Take info of Number of variables and number of clauses
	long long int clauses,variables;
	char *token = strtok(line," ");   //Read p
	token = strtok(NULL, " ");        //Read cnf
	token = strtok(NULL, " ");        //Read number of variables
	variables = stoi(token);		  //Convert it to integer
	token = strtok(NULL, " ");        //Read number of clauses
	clauses = stoi(token);			  //Convert it to integer

	//Read Clauses and update our vector
	long long int buffer,max=0;
	vector <long long int> temp;
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
	}
	if(max > variables)
	{
		cerr << "Error: Number of variables entered do not match with the given data\n";
	}


	//Read Input!
}




//Function to remove duplicates from clauses
void removeDuplicates()
{
	for(long long int i=0; i < formula.size(); ++i)
	{
		sort(formula[i].begin(),formula[i].end());                  //Sort each clause

		for(long long int j=1 ; j < formula[i].size();)
		{
			if(formula[i][j] == formula[i][j-1])					//If jth element in clause is equal to (j-1)th element in the clause
				formula[i].erase(formula[i].begin() + j);			//j not incremented as element is deleted!
			else j++;												//Else move to the next element
		}
	}
}



//Function to remove tautologies form clauses
void removeTautologies()
{
	for(long long int i=0; i < formula.size(); ++i)
	{
		sort(formula[i].begin(),formula[i].end(),absoluteCompare);			//Sort based on absolute value of numbers

		for(long long int j=1;j < formula[i].size(); j++)
		{
			if(formula[i][j] == -formula[i][j-1])							//If a and -a are both in the clause implies a tautology
			{
				formula.erase(formula.begin() + i);							//Then remove that clause
				i--;														//Take care of size reduction
				break;
			}
		}
	}
}



//If a unit length clause is present then remove all clauses having that unit length clause
void removeSupersets()
{
	//A sort function to arrange all clauses based on their size...
	sort(formula.begin(),formula.end(),setSizeCompare);

	//Sort all clauses
	for(long long int i=0; i<formula.size(); ++i ) sort(formula[i].begin(),formula[i].end());


	for(long long int i=0; i<formula.size() && formula[i].size() == 1;++i)
	{

		for(long long int j=i+1;j<formula.size();++j)
		{
			//If jth clause is a superset of ith clause
			if(includes(formula[j].begin(),formula[j].end(), formula[i].begin(),formula[i].end()))
			{
				formula.erase(formula.begin() + j);				//Then erase it as it will be SAT if ith clause is SAT
				j--;											//Take care of reduction in size
			}
		}
	}
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

	//Remove Supersets
	removeSupersets();

	for(long long int i=0; i<formula.size(); ++i)
	{
		for(long long int j=0; j<formula[i].size() ; ++j)
		{
			cout<<formula[i][j]<<" ";
		}
		cout<<endl;
	}
}




