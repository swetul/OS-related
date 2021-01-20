/*
	Name: Swetul Patel
	Scheduler algorithms and policies
*/

#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <queue>
#include <sstream>
//#include <cstdlib.h>

using namespace std;

const int TIMESLICE = 5;

int currentTIME = 0;

// time calculating global variables
int timePri0 = 0;
int timePri1 = 0;
int timePri2 = 0;

int timeType0 = 0;
int timeType1 = 0;
int timeType2 = 0;
int timeType3 = 0;

// count of each type and priority of process

int countPri0 = 0;
int countPri1 = 0;
int countPri2 = 0;

int countType0 = 0;
int countType1 = 0;
int countType2 = 0;
int countType3 = 0;


// Process interface
class Process
{
	private:
		string name;
		int type;
		int priority;
		int timeNeeded;
		int probIO;

	public:
		Process();			//default constructor
		Process(string, int, int, int, int);	// main constructor

		//getters
		string getName();
		int getType();
		int getPriority();
		int getTimeNeeded();
		int getIOprob();
		//setters
		void setName(string);
		void setType(int);
		void setPriority(int);
		void setTimeRemaining(int);
		void setIOProb(int);
};

//process Implementation

//costructors
Process::Process()
{
	this->name = "";
	this->type = 0;
	this->priority = 0;
    this->timeNeeded = 0;
	this->probIO = 0;
}

Process::Process(string a, int b, int c, int d, int e)
{
	this->name = a;
	this->type = b;
	this->priority = c;
    this->timeNeeded = d;
	this->probIO = e;
}

//Getters
string Process::getName(){return this->name;}
int Process::getType(){return this->type;}
int Process::getPriority(){return this->priority;}
int Process::getTimeNeeded(){return this->timeNeeded;}
int Process::getIOprob(){return this->probIO;}

//Setters

void Process::setName(string a){this->name = a;}
void Process::setType(int b){this->type = b;}
void Process::setPriority(int c){this->priority = c;}
void Process::setTimeRemaining(int d){this->timeNeeded = d;}
void Process::setIOProb(int e){this->probIO = e;}

//End of implementation

//helper functions

void readFile(string fileName, queue<Process> &processList)
{
	ifstream file;

    file.open(fileName.c_str());
    string line;

    while(getline(file, line))  //get the data from file
    {
        int i = 0;
    	string tokens[10];

    	stringstream tokenizer(line);

    	string midword;

    	while(getline(tokenizer, midword, ' '))					//make tokens from input line
    	{
    		tokens[i++] = midword;
    	}

        int one;
        int two;
        int three;
        int four;

        istringstream(tokens[1]) >> one;
        istringstream(tokens[2]) >> two;
        istringstream(tokens[3]) >> three;
        istringstream(tokens[4]) >> four;

    	Process newProcess = Process(tokens[0], one, two, three, four);
    	processList.push(newProcess);

    	//finding out how many of each type of process and thier priorities

    	int prio = newProcess.getPriority();
		int typ = newProcess.getType();
    	if(prio == 0){countPri0++;}
		if(prio == 1){countPri1++;}
		if(prio == 2){countPri2++;}

		if(typ == 0){countType0++;}
		if(typ == 1){countType1++;}
		if(typ == 2){countType2++;}
		if(typ == 3){countType3++;}
    }
    file.close();
}//end of readFile

void swap(Process &x, Process &y)
{
    Process temp = x;
    x = y;
    y = temp;
}//end of swap

//mimics a process currently running on the CPU
int inProcessor(Process &current)
{
	int time = TIMESLICE;
	int randProb = rand() % 100 + 1;

	if(randProb > current.getIOprob())
	{
		time = rand() % 5;
	}

	int remainTime = current.getTimeNeeded() - time;

	if(remainTime > 0)
	{
        currentTIME += time;
		current.setTimeRemaining(remainTime);
	}
	else
	{
        currentTIME += current.getTimeNeeded();


		remainTime = 0;
		current.setTimeRemaining(0);

		int prio = current.getPriority();
		int typ = current.getType();

		//update overall time
		if(prio == 0){timePri0 += currentTIME;}
		if(prio == 1){timePri1 += currentTIME;}
		if(prio == 2){timePri2 += currentTIME;}
		if(typ == 0){timeType0 += currentTIME;}
		if(typ == 1){timeType1 += currentTIME;}
		if(typ == 2){timeType2 += currentTIME;}
		if(typ == 3){timeType3 += currentTIME;}

	}
	return remainTime;
}

//SCHEDULER POLICIES

//Pure round robin, First Come - First Serve. NO priority is needed.


void pureRound(queue<Process> &list)
{
	while(list.size() > 0)
	{
        //Process temp = new Process();
        Process temp = list.front();

		int remainTime = inProcessor(temp);
		list.pop();

		if(remainTime > 0)
		{
			list.push(temp);
		}
	}
} //end pureRound

// Shortest time to completion first

void shortestTime(queue<Process> &list)
{
    int size = (list.size());
    Process sort[100000]; // = new Process[size];
    Process temp[100000]; // = new Process[size];
    //cout << size<< endl;
    //cout << "he -----  " << endl;
	for(int i = 0; i < size; i++)
	{
		Process itm = list.front();
		temp[i] = itm;

		list.pop();
	}

	//insertion sort

	for(int i = 0; i < size; i++)
	{
		int lowestTime = temp[i].getTimeNeeded();
		int lowestPos = i;
		//find pos with shortest time.
		for(int j = i; j < size; j++)
		{
			if(temp[j].getTimeNeeded() < lowestTime)
			{
				lowestTime = temp[j].getTimeNeeded();
				lowestPos = j;
			}
		}
		if(i != lowestPos)
		{
			//swap position
			sort[i] = temp[lowestPos];
			temp[lowestPos] = temp[i];
			temp[i] = sort[i];
		}
		else
		{
			sort[i] = temp[lowestPos];
		}
	}
	//put sorted(by shortest time) processes into a queue
	for(int i = 0; i < size; i++)
	{
		list.push(sort[i]);
	}

	//run the simulator

	while(list.size() > 0)
	{
		Process temp0 = list.front();

		int remainTime = inProcessor(temp0);
		//since the first in the queue has the shortest remaining time, only remove process if its done with its full execution
        swap(list.front(), temp0);
		if(remainTime == 0)
		{
			list.pop();
		}
	}
}//end of STCF

void priorityRound(queue<Process> &list)
{
	queue< queue<Process> > multiLevel;

    queue<Process> p0 = queue<Process>();
    queue<Process> p1 = queue<Process>();
    queue<Process> p2 = queue<Process>();

	while(list.size() > 0)
	{
		Process temp = list.front();
		if(temp.getPriority() == 0)
		{
			p0.push(temp);
		}
		if(temp.getPriority() == 1)
		{
			p1.push(temp);
		}
		if(temp.getPriority() == 2)
		{
			p2.push(temp);
		}
		list.pop();
	}
    multiLevel.push(p0);
    multiLevel.push(p1);
    multiLevel.push(p2);

	//run pure round robin on each queue in the multi-level queue
	for(int i = 0; i < 3; i++)
	{
        queue<Process> run = multiLevel.front();
		pureRound(run);
        multiLevel.pop();
	}
}// end of Priority Round-Robin

/* main function


*/
int main(int argc, char** argv)
{
	if(argc == 0)							//no arguments given
	{
		cout << "Please re-try and provide a Scheduler to run as an argument. Use 1 for Pure round-robin, 2 for shortest time and 3 for priority round-robin scheduler. " << '\n' << endl;
		return EXIT_FAILURE;
	}
    int schedulerType;
    istringstream(argv[1]) >> schedulerType;

	if(schedulerType > 3 || schedulerType < 1)
	{
		cout << "Invalid Scheduler selected. Use 1 for Pure round-robin, 2 for shortest time and 3 for priority round-robin. " << '\n' << endl;
		return EXIT_FAILURE;
	}


	string fileName;
	cout << "Enter the filename to be read: " ;           //request filename

	while(true)
    {
        ifstream file;
        getline(cin, fileName);                                           //obtain filename
        file.open(fileName.c_str());
        if(file.fail())
        {
            cout << "An error occured while opening the file, please verify the file exists and re-enter the file name: ";
        }
        else
        {
            cout << "Found correct file with the processes \n" << endl;
            file.close();
            break;
        }
        file.close();
    }

    //read file and put into queue
    queue<Process> processList;
    readFile(fileName, processList);



    //simulate the scheduler policy selected by user
    if(schedulerType == 1)
    {
    	cout << "Using Pure Round-Robin" << '\n' << endl;
    	pureRound(processList);
    }
    else if(schedulerType == 2)
    {
    	cout << "Using Shortest time to completion first" << '\n' << endl;
    	shortestTime(processList);
    }
    else if(schedulerType == 3)
    {
    	cout << "Using PRIORITY Round-Robin" << '\n' << endl;
    	priorityRound(processList);

    }
else
{

	cout << "Incorrect Scheduler selected. " << '\n' <<endl;
	return EXIT_FAILURE;
}

    //PRINT THE SUMMARY HERE
    cout << "Average run time per priority: " << endl;
    cout << "Priority 0 average run time: " << (timePri0/countPri0) << endl;
    cout << "Priority 1 average run time: " << (timePri1/countPri1) << endl;
    cout << "Priority 2 average run time: " << (timePri2/countPri2) << endl;

    cout << "Average run time per type: " << endl;
    cout << "Type 0 average run time: " << (timeType0/countType0) << endl;
    cout << "Type 1 average run time: " << (timeType1/countType1) << endl;
    cout << "Type 2 average run time: " << (timeType2/countType2) << endl;
    cout << "Type 3 average run time: " << (timeType3/countType3) << endl;

    return EXIT_SUCCESS;
}// end of file
