//Loosely inspired by https://github.com/2coolerino/cli_todo_list

#include <iostream>
#include <string>
#include <fstream>
#include <map>

using namespace std;

// g++ -o todo.exe todo.cpp
const string FILE_NAME = "todo_data.txt";
const string HELP_TEXT = 
"Available Commands:"
"\n(no arguments)		Display the todo list."
"\n--help			Show a list of available commands."
"\nadd <item>			Add <item> to the todo list."
"\ndelete <item>		Remove <item> from the todo list."
"\ndelete -i <item_number>	Remove the item at the provided index."
"\ntick <item>			Tick <item> on the todo list."
"\ntick -i <item_number>	Tick the item at the provided index."
"\nuntick <item>		Untick <item> on the todo list."
"\nuntick -i <item_number>	Untick the item at the provided index."
"\nclean			Remove all ticked items from the todo list."
;

map<string, bool> read(){
	//First char of each line is 0 or 1 - bit value of checked
	map<string, bool> list;

	fstream dataFile;
	dataFile.open(FILE_NAME, ios::in);
	if(dataFile.is_open()){
		string line;
		while(getline(dataFile, line)){
			if(line.length() > 1){
				char tick_char = line[0];
				bool tick = tick_char == '1';
				string item = line.substr(1);
				list[item] = tick;
			}
		}
	}
	return list;
}

void write(map<string, bool> &list){
	fstream dataFile;
	dataFile.open(FILE_NAME, ios::out);
	if(dataFile.is_open()){
		for(auto &item: list){
			dataFile << item.second << item.first << "\n";
		}
	}
}

void printList(map<string, bool> &list){
	if(list.size() == 0){
		cout << "Nothing on your todo list." << endl;
		return;
	}
	int i = 1;
	for(auto &item: list){
		cout << i << ". ";
		if(item.second){
			cout << "\u2713" << " ";
		}
		cout << item.first << endl; 
		i++;
	}
}

string getItem(map<string, bool> &list, int index){
	auto it = list.begin();
	advance(it, index);
	return it->first;
}

string getItem(map<string, bool> &list, string indexAsString){
	for(int i = 1; i <= list.size(); i++){
		if(indexAsString == to_string(i)){
			return getItem(list, i - 1);
		}	
	}
	cout << "No item exists of that number." << endl;
	return "";
}

void help(){
	cout << HELP_TEXT << endl;
}

bool addItem(map<string, bool> &list, string item){
	if(list.find(item) == list.end()){
		list[item] = 0;
		return true;
	} else {
		cout << "Item already exists." << endl;
		return false;
	}
}

bool deleteItem(map<string, bool> &list, string item){
	if(list.find(item) != list.end()){
		list.erase(item);
		return true;
	} else {
		cout << "Item doesn't exist." << endl;
		return false;
	}
}

bool tickItem(map<string, bool> &list, string item){
	if(list.find(item) != list.end()){
		if(list[item] == 0){
			list[item] = 1;
			return true;
		} else {
			cout << "Item already ticked." << endl;
		}
	} else {
		cout << "Item doesn't exist." << endl;
	}
	return false;
}

bool untickItem(map<string, bool> &list, string item){
	if(list.find(item) != list.end()){
		if(list[item] == 1){
			list[item] = 0;
			return true;
		} else {
			cout << "Item not ticked yet." << endl;
		}
	} else {
		cout << "Item doesn't exist." << endl;
	}
	return false;
}

bool cleanList(map<string, bool> &list){
	int itemsRemoved = 0;
	for(auto &item : list){
		if(item.second == 1){
			list.erase(item.first);
			itemsRemoved++;
		}
	}
	cout << to_string(itemsRemoved) << " item(s) removed." << endl;
	return itemsRemoved != 0;
}

int main(int argc, char* argv[]){

	//No command, so just display the current list
	if(argc < 2){
		map<string, bool> list = read();
		printList(list);
		exit(0);
	}

	string command = argv[1];
	//Check for commands which don't need file access first
	if(command == "--help"){
		help();
		exit(0);
	}

	//File-using commands
	map<string, bool> list = read();

	bool needsWritten = false;
	if(command == "add" && argc == 3){
		//Add next argument to the list
		string item = argv[2];
		needsWritten = addItem(list, item);
	} else if(command == "delete" && argc == 3){
		//Delete next argument from the list
		string item = argv[2];
		needsWritten = deleteItem(list, item);
	} else if(command == "delete" && argc == 4){
		string mod = argv[2];
		if(mod == "-i"){
			//Delete item from list at index specified by next argument
			string index = argv[3];
			string item = getItem(list, index);
			if(item != ""){
				needsWritten = deleteItem(list, item);	
			}
		}
	} else if(command == "tick" && argc == 3){
		//Tick the next argument
		string item = argv[2];
		needsWritten = tickItem(list, item);
	} else if(command == "tick" && argc == 4){
		string mod = argv[2];
		if(mod == "-i"){
			string index = argv[3];
			string item = getItem(list, index);
			if(item != ""){
				needsWritten = tickItem(list, item);
			}
		}
	} else if(command == "untick" && argc == 3){
		//Untick the next argument
		string item = argv[2];
		needsWritten = untickItem(list, item);
	} else if(command == "untick" && argc == 4){
		string mod = argv[2];
		if(mod == "-i"){
			string index = argv[3];
			string item = getItem(list, index);
			if(item != ""){
				needsWritten = untickItem(list, item);
			}
		}
	} else if(command == "clean" && argc == 2){
		//Remove all ticked items from the list
		needsWritten = cleanList(list);
	} else {
		//Something went wrong
		cout << "todo: command not recognised." << endl
			<< "Try 'todo --help' for more information." << endl;
		exit(1);
	}

	if(needsWritten){
		//Update file
		write(list);
		printList(list);
	}
	
	return 0;
}
