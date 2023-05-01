/*Submitted by Daniyal Ishfaq (i160201) and Joan Shirazi (i16-0285)*/

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <cstdlib>
#include <sys/wait.h>
#include <fstream>
#include <queue>
using namespace std;

struct Instruction
{
	string fileName;
	string type;
	int integerValue;
	int size;
};

struct Process
{
	int pid;
	int startTime;
	int CPUTime;
	int integerValue;
	int PC;
	Instruction * Instr;
};

queue<Process *> Running;
queue<Process *> Blocked;
queue<Process *> Ready;

void broadCast(queue<Process*>re,queue<Process*>ru,queue<Process*>bl,int sec)
{
	cout << endl << endl;
					cout << "<---------------------------------->" << endl;
					cout << "Process Current state" << endl;
					cout << "<---------------------------------->" << endl;
					cout << "Current Time: " << sec << endl; 
					if(ru.empty()){
						cout<<"No Process in Running State."<<endl;
					}
					else{
						cout << "Running Process" << endl;
						cout << "pid: " << ru.front()->pid << " " << "Value: " << ru.front()->integerValue 
							<< " Start Time: " << ru.front()->startTime << " Cpu Time: " << ru.front()->CPUTime << endl;
					}
					cout << "Blocked Processes" << endl;
					
					while (!bl.empty())
					{
						cout << "pid: " << bl.front()->pid << " Value: " << bl.front()->integerValue 
							<< " Start Time: " << bl.front()->startTime << " Cpu Time: " << bl.front()->CPUTime << endl;
						bl.pop();
					}
					cout << endl;
					cout << "Processes ready to be executed" << endl;
					while (!re.empty())
					{
						cout << "pid: " << re.front()->pid << " Value: " << re.front()->integerValue 
							<< " Start Time: " << re.front()->startTime << " Cpu Time: " << re.front()->CPUTime << endl;
						re.pop();
					}
					cout << endl << endl;
					exit(0);
}
void ReadCommand(int);
void insert(Instruction *&, Instruction, int &);
void AddProcess(Process *&, Process, int &);
char command[20];
int main()
{
	int id = 0;
	int numOfProcess = 0;
	Process *ProcessArray = new Process [numOfProcess];	
	int CpuTime = 0;
	queue<string> CommandQueue;
	int controllerPipe[2];
	int main_controller = pipe(controllerPipe);
	pid_t ControllerProcess = fork();

	while (true)
	{
		if (ControllerProcess > 0)
		{
			signal(SIGALRM, ReadCommand);
			alarm(2);
			pause();
			close(controllerPipe[0]);
			//cout << "In charge writing on the buffer" << endl;
			//sleep(2);
			write(controllerPipe[1], command, sizeof(command));
			//wait(NULL);
			//sleep(2);
			//cout << "The In charge wrote " << command << " on the buffer" << endl;
			if (strcmp(command, "end") == 0)
			{
				return 0;
			}
		}

		else if (ControllerProcess == 0)
		{
			close(controllerPipe[1]);
			read(controllerPipe[0], command, sizeof(command));
			if (strcmp(command, "end") == 0)
			{
				return 0;
			}

			else if (command[0] == 'C' && command[1] == 'R' && command[2] == 'T')
			{
				cout << endl << endl;
				Process P;
				int size = 0;
				P.startTime = CpuTime;
				P.CPUTime = 0;
				P.Instr = new Instruction[size];
				P.pid = id;
				id++;
				P.PC = 0;
				string com = string(command);
				//cout << com << endl;
				const char * f = (com.substr(4, com.length() - 1)).c_str();
				ifstream MyFile(f);
				string c;
				string val;
				int i = 0;
				if (MyFile)
				{
					cout << "<-----------Creating Process----------->" << endl;
					while (MyFile >> c >> val)
					{
						string instruction = c + val;
						Instruction I;
						if (c != "R")
						{
							int value = atoi(val.c_str());
							I.integerValue = value;
						}
						else
							I.fileName = val;
						
						I.type = c;
						cout << "I.type: " << I.type << endl;
						insert(P.Instr, I, size);
					}

					P.Instr->size = size;
					AddProcess(ProcessArray, P, numOfProcess);
					Ready.push(&ProcessArray[numOfProcess-1]);
				}

				else 
				{
					cout << "<--------Invalid Process name---------->" << endl;
				}
				cout << endl << endl;
			}

			else if (strcmp(command, "UNB") == 0)
			{
				Ready.push(Blocked.front());
				Blocked.pop();
			}

			else if (strcmp(command, "INC") == 0)
			{
				if (Ready.empty() && Running.empty())
				{
					cout << "<--------No process to execute------->" << endl;
				}
				if (Running.empty() && !Ready.empty())
				{
					Running.push(Ready.front());
					Ready.pop();
					cout << "<----------Process Executing---------->" << endl;
					cout << "Type: " << Running.front()->Instr[Running.front()->PC].type << endl;
					if (Running.front()->Instr[Running.front()->PC].type == "S")
					{
						Running.front()->integerValue = Running.front()->Instr[Running.front()->PC].integerValue;
						Running.front()->PC++;
						Running.front()->CPUTime++;
					}

					else if (Running.front()->Instr[Running.front()->PC].type == "A")
					{
						Running.front()->integerValue += Running.front()->Instr[Running.front()->PC].integerValue;
						Running.front()->PC++;
						Running.front()->CPUTime++;
					}

					else if (Running.front()->Instr[Running.front()->PC].type == "D")
					{
						Running.front()->integerValue -= Running.front()->Instr[Running.front()->PC].integerValue;
						Running.front()->PC++;
						Running.front()->CPUTime++;
					}
					else if (Running.front()->Instr[Running.front()->PC].type == "B")
					{
						Blocked.push(Running.front());
						Running.pop();
						Running.front()->PC++;
						Running.front()->CPUTime++;
					}
					else if (Running.front()->Instr[Running.front()->PC].type == "E")
					{
						Running.pop();
						Running.front()->PC++;
						Running.front()->CPUTime++;
					}

					else if (Running.front()->Instr[Running.front()->PC].type == "R")
					{
						int size = 0;
						string com = Running.front()->Instr[Running.front()->PC].fileName;
						Process P;
						P.Instr = new Instruction [size];
						P.pid = Running.front()->pid;
						P.CPUTime = Running.front()->CPUTime;
						P.startTime = Running.front()->startTime;
						P.PC = 0;
						const char * f = com.c_str();
						cout << f << endl;
						ifstream MyFile(f);
						string c;
						string val;
						int i = 0;
						while (MyFile >> c >> val)
						{
							string instruction = c + val;
							Instruction I;
							if (c != "R")
							{
								int value = atoi(val.c_str());
								I.integerValue = value;
							}
							else
								I.fileName = val;
							
							I.type = c;
							insert(P.Instr, I, size);
						}
						/*for (int i=0; i<size; i++)
						{
							cout << P.Instr[i].type << " ";
							if (P.Instr[i].type == "R")
								cout << P.Instr[i].fileName << endl;
						
							else
								cout << P.Instr[i].integerValue << endl;
						}*/
						Running.front()->PC++;
						Running.front()->CPUTime++;
						P.Instr->size = size;
						AddProcess(ProcessArray, P, numOfProcess);
						Running.pop();
						Running.push(&ProcessArray[numOfProcess-1]);
					}
					if ((Running.front()->PC) >= Running.front()->Instr->size)
					{
						cout << endl << endl;
						cout << "<---------Program execution completed!!----------->" << endl;
						Running.pop();
						if (!Ready.empty())
						{
							Running.push(Ready.front());
							Ready.pop();
						}

						else 
						{
							cout << "<---------No new program to execute.---------->" << endl;
						}
						cout << endl << endl;
					}
					else if (((Running.front()->CPUTime) % 3 == 0) && (!Ready.empty()) && (Running.front()->CPUTime > 0))
					{
						cout << endl << endl;
						cout << "<----------Context Switched!---------->" << endl << endl;
						Process * temp = Ready.front();
						Ready.pop();
						Ready.push(Running.front());
						Running.pop();
						Running.push(temp);
						cout << endl << endl; 
					}
					CpuTime++;
				}

				else 
				{
					cout << "<----------Process Executing---------->" << endl;
					if (Running.front()->Instr[Running.front()->PC].type == "S")
					{
						Running.front()->integerValue = Running.front()->Instr[Running.front()->PC].integerValue;
						Running.front()->PC++;
						Running.front()->CPUTime++;
					}

					else if (Running.front()->Instr[Running.front()->PC].type == "A")
					{
						Running.front()->integerValue += Running.front()->Instr[Running.front()->PC].integerValue;
						Running.front()->PC++;
						Running.front()->CPUTime++;
					}

					else if (Running.front()->Instr[Running.front()->PC].type == "D")
					{
						Running.front()->integerValue -= Running.front()->Instr[Running.front()->PC].integerValue;
						Running.front()->PC++;
						Running.front()->CPUTime++;
					}
					else if (Running.front()->Instr[Running.front()->PC].type == "B")
					{
						Blocked.push(Running.front());
						Running.front()->CPUTime++;
						Running.front()->PC++;
						Running.pop();
						continue;
					}
					else if (Running.front()->Instr[Running.front()->PC].type == "E")
					{
						Running.pop();
						Running.front()->PC++;
						Running.front()->CPUTime++;
					}

					else if (Running.front()->Instr[Running.front()->PC].type == "R")
					{
						int size = 0;
						string com = Running.front()->Instr[Running.front()->PC].fileName;
						Process P;
						P.Instr = new Instruction [size];
						P.pid = Running.front()->pid;
						P.CPUTime = Running.front()->CPUTime;
						P.startTime = Running.front()->startTime;
						P.PC = 0;
						const char * f = com.c_str();
						cout << f << endl;
						ifstream MyFile(f);
						string c;
						string val;
						int i = 0;
						while (MyFile >> c >> val)
						{
							string instruction = c + val;
							Instruction I;
							if (c != "R")
							{
								int value = atoi(val.c_str());
								I.integerValue = value;
							}
							else
								I.fileName = val;
							
							I.type = c;
							insert(P.Instr, I, size);
						}
						/*for (int i=0; i<size; i++)
						{
							cout << P.Instr[i].type << " ";
							if (P.Instr[i].type == "R")
								cout << P.Instr[i].fileName << endl;
						
							else
								cout << P.Instr[i].integerValue << endl;
						}*/
						Running.front()->PC++;
						Running.front()->CPUTime++;
						P.Instr->size = size;
						AddProcess(ProcessArray, P, numOfProcess);
						Running.pop();
						Running.push(&ProcessArray[numOfProcess-1]);
					}
					if ((Running.front()->PC) >= Running.front()->Instr->size)
					{
						cout << endl << endl;
						cout << "<---------Program execution completed!!----------->" << endl;
						Running.pop();
						if (!Ready.empty())
						{
							Running.push(Ready.front());
							Ready.pop();
						}

						else 
						{
							cout << "<---------No new program to execute.---------->" << endl;
						}
						cout << endl << endl;
					}
					else if (((Running.front()->CPUTime) % 3 == 0) && (!Ready.empty()) && (Running.front()->CPUTime > 0))
					{
						cout << endl << endl;
						cout << "<----------Context Switched!---------->" << endl << endl;
						Process * temp = Ready.front();
						Ready.pop();
						Ready.push(Running.front());
						Running.pop();
						Running.push(temp);
						cout << endl << endl; 
					}
					CpuTime++;
					cout << "At the end of else condition" << endl;
				}
			}
			else if (strcmp(command, "PRT") == 0)
			{
				pid_t BroadCasterProcess = fork();
				if (BroadCasterProcess == 0)
				{
					broadCast(Ready,Running,Blocked,CpuTime);
				}
			}
		}
	}

	wait(NULL);
	return 0;
}

void ReadCommand(int signum)
{
	cout << "Enter your command:" << endl;
	cin.getline(command, 20);
}

void insert(Instruction *& Array, Instruction I, int & size)
{
	Instruction * temp = new Instruction[size];
	for (int i=0; i<size; i++)
	{
		temp[i] = Array[i];
	}

	delete [] Array;
	Array = new Instruction [size + 1];
	for (int i=0; i<size; i++)
		Array[i] = temp[i];

	delete [] temp;
	Array[size] = I;
	size++;
}

void AddProcess(Process *& Array, Process P, int &size)
{
	Process * temp = new Process[size];
	for (int i=0; i<size; i++)
	{
		temp[i] = Array[i];
	}

	//delete [] Array;
	Array = new Process [size + 1];
	for (int i=0; i<size; i++)
		Array[i] = temp[i];

	delete [] temp;
	Array[size] = P;
	size++;
}

void DisplayQueue(queue<Process *> Q)
{

}
