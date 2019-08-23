#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<filesystem>	//In order to read the module directories
#include<fstream>		//To open and handle files
#include<regex>
#include<cmath> 		//In order to work with powers

using namespace std;
namespace fs = std::filesystem;

int CommandHandler(vector<string>& commands, string& command) {//The purpose of this function is to split the user's input into chunks

	if (command.back() != ' ') { command += " "; } //Add space to the end to make the splitting function work
	commands.clear();		//Clear the vector
	string tempstring = "";

	for (int i = 0; i < command.length(); i++) {
		if (command[i] == ' ' || command[i] == '\n') {
			commands.push_back(tempstring);
			tempstring = "";
		}
		else {
			tempstring += command[i];
		}
	}

	return 0;
}

int DirectoryReader(string directoryName) {

	//Iterate through the list of files in the payloads directory
	for (const auto& entry : fs::directory_iterator(directoryName)) {//Using C++17 filesystem to get list of filenames
		if (entry.path().string().find("odo") < entry.path().string().length()) { //Filtering files for .odo extension
			//Put correct filenames into a vector
			cout << entry.path().string().substr(9, entry.path().string().length() - 13) << "\t";
			string tempString;		//To store the lines within the file
			ifstream fileInput(entry.path());		//Open the file
			bool descriptionFound = false;
			while (getline(fileInput, tempString)) {
				if (tempString.find("<description>") < tempString.length()) {		//Look for the line which features the description
					cout << tempString.substr(14, tempString.length() - 29) << endl;		//Print the payload name and payload description to the screen
					descriptionFound = true;
				}
			}
			if (descriptionFound == false) {
				cout << "Could not find a description." << endl;
			}
		}
	}

	return 0;
}

int helpscreen(string selector) {

	//cout << "\nHELP SCREEN" << endl << endl;
	if (selector.find("a") < selector.length() || selector.find("p") < selector.length()) {//List payloads
		cout << "List of payloads:" << endl << endl;
		DirectoryReader("payloads");
		cout << endl;
	}
	if (selector.find("a") < selector.length() || selector.find("m") < selector.length()) {//List mutators
		cout << "List of mutators:" << endl << endl;
		DirectoryReader("mutators");
		cout << endl;
	}
	if (selector.find("a") < selector.length() || selector.find("c") < selector.length()) {//List of all commands
		cout << "List of recognised commands:" << endl << endl;
		cout << "assemble -p <payload> -m <mutator> -t <target platform> -o <output name>" << endl;
		cout << "\t-p <payload>\tThe name of the desired payload module." << endl;
		cout << "\t-m <mutator>\tThe name of the desired mutator module." << endl;
		cout << "\t-t <target platform>\tThe name of the desired architecture target." << endl;
		cout << "\t-o <output name>\tThe output name of the executable." << endl << endl;
		cout << "help <filter>" << endl;
		cout << "\t<filter>\tFilter for a specific help section. Examples: help payload, help mutator." << endl << endl;
		cout << "quit\tExit the program." << endl << endl;
	}
	if (selector.find("a") < selector.length() || selector.find("t") < selector.length()) {//List of all targets
		cout << "List of all target platforms:" << endl << endl;
		cout << "\tLinux" << endl << endl;
	}
	if (selector.find("u") < selector.length()) {//Extended usage description of the assembly command
		cout << "Correct syntax for the assembly command:" << endl;
		cout << "assemble -p <payload> -m <mutator> -t <target platform> -o <output name>" << endl;
		cout << "\t-p <payload>\tThe name of the desired payload module." << endl;
		cout << "\t-m <mutator>\tThe name of the desired mutator module." << endl;
		cout << "\t-t <target platform>\tThe name of the desired architecture target." << endl;
		cout << "\t-o <output name>\tThe output name of the executable." << endl << endl;
	}
	string selectorList = "acmptu";
	if (selectorList.find(selector) > selectorList.length()) {
		cout << "Unrecognised help command. Available help options are payload, mutator, target, and command.";
	}

	return 0;
}

void decompressor(string& inputString, vector<unsigned char>& outputVector) {

	for (int i = 0; i < inputString.length(); i++) {
		if (inputString[i] == 'x') {
			unsigned tempChar = inputString[i + 1];
			int count = 0;
			if (inputString[i + 2] <= '9') {
				count = 16 * (inputString[i + 2] - '0');
				//cout << " count1 = " << count << " ";
			}
			else {
				count = 16 * (inputString[i + 2] - '7');
				//cout << " count2 = " << count << " ";
			}
			if (inputString[i + 3] <= '9') {
				count += (inputString[i + 3] - '0');
				//cout << " count3 = " << count << " ";
			}
			else {
				count += (inputString[i + 3] - '7');
				//cout << " count4 = " << count << " ";
			}
			for (int j = 0; j < count; j++) {
				outputVector.push_back(tempChar);
			}
			i += 3;
		}
		else {
			outputVector.push_back(inputString[i]);
		}
	}
	cout << endl << endl;
	//for (int i = 0; i < outputVector.size(); i++) {
		//cout << outputVector[i];
	//}
}

int assembly(string payloadFile, string mutatorFile, string targetArch, string fileName) {
	//This is the main assembly function of the program.
	//It will start by reading the framework code, the payload code, and the mutator code.
	//The framework contains a 512 byte section for the payload code, and a 512 byte section for the mutator code.
	//It will add code to the instruction section of the framework which will call the mutator code.
	//The mutator code will always have a call to the payload.

	cout << "Beginning assembly phase..." << endl;

	bool validate = false;
	vector<string> acceptedTargets = {"linux"};

	//Check validity of outputfile name
	regex e("(.*)([[:punct:]])(.*)");
	if (regex_match(fileName, e)) {
		cout << "ERROR: The filename entered contains invalid characters. Please enter a new filename which doesn't contain punctuation." << endl;
		return 1;
	}
	if (fileName.length() > 12) {
		fileName = fileName.substr(0, 12);
	}
	cout << "test" << endl;

	//Check for existance of payload module
	string tempFileName = "";
	if (payloadFile.find(".odo") > payloadFile.length()) {
		payloadFile += ".odo";
	}
	if (mutatorFile.find(".odo") > mutatorFile.length()) {
		mutatorFile += ".odo";
	}

	for (const auto& entry : fs::directory_iterator("payloads")) {//Using C++17 filesystem to get list of filenames
		if (entry.path().string().find(payloadFile) < entry.path().string().length()) {
			cout << entry.path().string() << endl;
			//cout << "FOUND PAYLOAD" << endl;
			validate = true;
		}
	}
	if (!validate) {
		cout << "ERROR: Could not find the selected payload module. Please select a valid payload module." << endl << endl;
		helpscreen("p");
		return 2;
	}
	
	validate = false;
	//Check for existance of mutator module
	for (const auto& entry : fs::directory_iterator("mutators")) {//Using C++17 filesystem to get list of filenames
		if (entry.path().string().find(mutatorFile) < entry.path().string().length()) {
			cout << entry.path().string() << endl;
			//cout << "FOUND MUTATOR" << endl;
			validate = true;
		}
	}
	if (!validate) {
		cout << "ERROR: Could not find the selected mutator module. Please select a valid mutator module." << endl << endl;
		helpscreen("m");
		return 3;
	}

	//Check validity of target architecture
	validate = false;
	for (string comp : acceptedTargets) {
		if (targetArch.compare(comp) == 0) {
			validate = true;
		}
	}
	if (!validate) {
		cout << "ERROR: Could not find the selected target architecture. Please select a valid target." << endl << endl;
		helpscreen("t");
		return 3;
	}

	//Check if outputfilename already exists
	tempFileName = fileName;
	while (tempFileName.length() < 12) {
		tempFileName += "_";
	}
	tempFileName += "0";

	cout << tempFileName << endl;

	for (const auto& entry : fs::directory_iterator(fs::current_path())) {//Using C++17 filesystem to get list of filenames
		if (entry.path().string().find(tempFileName) < entry.path().string().length()) {
			cout << "ERROR: Chosen output file name already exists. Please select an unused file name.";
			return 4;
		}
	}

	string frameworkCode, payloadCode, payloadSize, mutatorCode, tempString;

	//Load the framework
	ifstream frameworkInput("framework.odo");
	while (getline(frameworkInput, tempString)) {
		frameworkCode = tempString;
	}
	frameworkInput.close();

	//Load the payload code
	validate = false;
	ifstream payloadInput("payloads/" + payloadFile); //Open the chosen payload file and read the code
	while (getline(payloadInput, tempString)) {
		if (tempString.find("<code>") < tempString.length()) {
			payloadCode = tempString.substr(7, tempString.length() - 15);
			validate = true;
		}
	}
	payloadInput.close();
	if (!validate) {
		cout << "WARNING: Payload Module contains no code section! Assembly process will continue..." << endl;
	}else{
		cout << endl << "Succesfully opened and read " << payloadFile << "..." << endl;
	}
	validate = false;
	//Load the mutator code
	ifstream mutatorInput("mutators/" + mutatorFile); //Open the chosen payload file and read the code
	while (getline(mutatorInput, tempString)) {
		if (tempString.find("<code>") < tempString.length()) {
			mutatorCode = tempString.substr(7, tempString.length() - 15);
			validate = true;
		}
	}
	mutatorInput.close();
	if (!validate) {
		cout << "WARNING: Mutator Module contains no code section! Assembly process will continue..." << endl;
	}else{
		cout << endl << "Succesfully opened and read " << mutatorFile << "..." << endl;
	}
	
	//Create a vector to store the binary code, also decompress if needed
	vector<unsigned char> tempBinary;
	decompressor(frameworkCode, tempBinary);

	//Add code to instruction section which calls the mutator and then ends the program
	//Call mutator; mov eax, 1; mov ebx, 0; int 80h;
	string instructions = "E8FB110000B801000000BB00000000CD80"; 

	for (int i = 0; i < instructions.length(); i++) {
		tempBinary[8192 + i] = instructions[i];
	}

	//Write the payload code to the payload section of the framework
	for (int i = 0; i < payloadCode.length(); i++) {
		tempBinary[16384 + i] = payloadCode[i];
	}

	//Write the mutator code to mutator section of the framework
	for (int i = 0; i < mutatorCode.length(); i++) {
		tempBinary[17408 + i] = mutatorCode[i];
	}

	//In order to help the mutator, the filename of the program is saved to the framework.
	//The filename of the next copy is also stored.

	string tempName = "2E2F";	// 2E2F = ./

	//Converts an integer to hex ascii
	for (int i = 0; i < fileName.length(); i++) {
		unsigned char r1 = floor(fileName[i] / 16);
		unsigned char r2 = fileName[i] % 16;

		if (r1 <= 9) {
			r1 += 48;
		}
		else {
			r1 += 55;
		}

		if (r2 <= 9) {
			r2 += 48;
		}
		else {
			r2 += 55;
		}
		tempName += r1;
		tempName += r2;
	}

	while (tempName.length() < 28) { //max length of filename is 16 bytes, first 2 bytes are reserved for ./, and the last two are reserved for iterator and 00.
		tempName += "5F";		//Add underscores to extend filelength to 14 bytes.
	}

	tempName += "3000";		//Add the bytes 30, and 00 to the filename. 30 is ascii for 0.

	//cout << "TEMP NAME = " << tempName << endl;

	//Write the name of the file to the framework.
	for (int i = 0; i < tempName.length(); i++) {
		tempBinary[18432 + i] = tempName[i];
	}

	//Remove the final two bytes of the file name and replace with 31 00.
	tempName = tempName.substr(0, tempName.length() - 4);
	tempName += "3100";

	//Write the second filename to the framework.
	for (int i = 0; i < tempName.length(); i++) {
		tempBinary[18464 + i] = tempName[i];
	}

	//Write the filesize to the framework
	int fileSize = tempBinary.size() / 2;  //Dividing by 2 since the tempbinary deals with half bytes.
	int tempAddress[4];
	for (int i = 3; i > -1; i--) {
		int power = pow(16, i);
		int count = floor(fileSize / power);
		tempAddress[i] = count;
		fileSize -= count * power;
	}

	//convert to ascii
	for (int i = 0; i < 4; i++) {
		if (tempAddress[i] < 9) tempAddress[i] += 48;
		else tempAddress[i] += 55;
	}

	//Write the filesize to the tempBinary
	tempBinary[18624] = tempAddress[1];
	tempBinary[18625] = tempAddress[0];
	tempBinary[18626] = tempAddress[3];
	tempBinary[18627] = tempAddress[2];

	//Write the binary code to the output file.
	//The following for loop converts from ascii to hex, and then convert to an integer.
	//It is then written to the output file 1 bytes at a time.

	
	while (fileName.length() < 12) {
		fileName += "_";
	}
	fileName += "0";
	
	ofstream outputFile(fileName, ios::binary);
	for (int i = 0; i < tempBinary.size(); i++) {
		int sum = 0;
		if (tempBinary[i] <= '9') {
			sum += (tempBinary[i] - '0') * 16;
		}
		else {
			sum += (tempBinary[i] - '7') * 16;
		}
		if (tempBinary[i + 1] <= '9') {
			sum += (tempBinary[i + 1] - '0');
		}
		else {
			sum += (tempBinary[i + 1] - '7');
		}
		unsigned char tempChar = sum;
		outputFile << tempChar;
		i++;
	}
	if(outputFile){
		cout << "Successfully assembled the output file..." << endl;
	}
	outputFile.close();
	
	//Write useful information to the screen
	cout << endl << "End of assembly phase..." << endl;
	cout << endl << "The following file has been produced." << endl << endl;
	cout << "--------------------" << endl;
	cout << "Filename: " << fileName << endl;
	cout << "Payload: " << payloadFile << endl;
	cout << "Mutator: " << mutatorFile << endl;
	cout << "Filesize: " << tempBinary.size() << endl;
	cout << "--------------------" << endl << endl;
	return 0;
}

int main(int argc, char* argv[]) {

	bool clv = false;
	if (argc > 1) {
		clv = true;
		for (int i = 0; i < argc - 1; i++) {
			argv[i] = argv[i + 1];   //Shift the variables one place to the left
		}
	}
	else {
		//Hello Screen

		cout << endl;
		cout << "     ____    _____     ____  " << endl;
		cout << "    / __ \\  |  __ \\   / __ \\ " << endl;
		cout << "   | |  | | | |  | | | |  | |" << endl;
		cout << "   | |  | | | |  | | | |  | |" << endl;
		cout << "   | |__| | | |__| | | |__| |" << endl;
		cout << "    \\____/  |_____/   \\____/ " << endl << endl << endl;


		cout << "Welcome! ODO, Obviously Different Output, is a program for creating mutating software by combining a payload and mutator module into an executable file." << endl << endl;
		cout << "Usage: assemble -p <payload> -m <mutator> -t <target platform> -o <output name>" << endl;
		cout << "Required arguments: " << endl;
		cout << "\t-p <payload name>		The name of the desired payload module." << endl;
		cout << "\t-m <mutator name> 		The name of the required mutator module." << endl;
		cout << "\t-t <target architecture> 	The target architecture of the executable." << endl;
		cout << "\t-o <ouput name>			The output name of the executable." << endl << endl;
		cout << "Type help for a list of available payloads, mutators, and target platforms. Type quit to exit the program." << endl;
	}

	//Declaring Variables
	string command = "";		//The main command that accepts the user's input
	vector<string> commands;		//This vector holds the individual components of the user's command
	string acceptedCommands[3] = { "assemble","help","quit" };

	do {//Begin Main Loop, loops until command == "quit"
		if (!clv) {
			do {
				cout << endl << "odo> ";
				getline(cin, command); //Accept the user's input
			} while (command.length() < 1);
			transform(command.begin(), command.end(), command.begin(), ::tolower);

			//Split user input into separate commands based on spaces
			CommandHandler(commands, command);
		}
		else {
			if (argc > 1) {
				for (int i = 0; i < argc - 1; i++) {
					commands.push_back(argv[i]);
				}
			}
			else {
				cout << "Invalid command" << endl;
				helpscreen("c");
				return 0;
			}
		}

		//See if command is a valid command
		if (commands[0].compare(acceptedCommands[0]) == 0) {
			//cout << "ASSEMBLY FUNCTION" << endl;//Testing: Ensuring the command was recognised
			bool valid = false;
			//Checking For Assembly Command Validity
			if (commands.size() == 9) {//Quickly checks to see if there are enough arguments given.
				string parameterCheck = commands[1] + commands[3] + commands[5] + commands[7]; //Adds arguments together to see if they all exist. Also note they have to be spaced out.
				string parameters[4] = { "-p","-m","-t","-o" };  //The required arguments
				int parCheck[4] = { 0,0,0,0 };

				for (int i = 0; i < 4; i++) {
					parCheck[i] = parameterCheck.find(parameters[i])+2;
					//cout << parCheck[i] << endl;
				}

				if (parameterCheck.find(parameters[0]) < parameterCheck.length() && parameterCheck.find(parameters[1]) < parameterCheck.length() && parameterCheck.find(parameters[2]) < parameterCheck.length() && parameterCheck.find(parameters[3]) < parameterCheck.length()) {//Checking to make sure that the user has entered every required argument
					if (commands[2].length() > 0 && commands[4].length() > 0 && commands[6].length() > 0 && commands[8].length() > 0) {
						cout << "Valid Syntax" << endl; //Testing: Ensuring the loop is running
						assembly(commands[parCheck[0]], commands[parCheck[1]], commands[parCheck[2]], commands[parCheck[3]]); //Pass the argument values to the assembly function
						valid = true;
					}
				}
			}
			
			if(!valid) {
				cout << "Invalid command usage!" << endl << endl;
				helpscreen("u");
			}

		}
		else if (commands[0].compare(acceptedCommands[1]) == 0) {
			if (commands.size() > 1) {
				string tempString = "";
				tempString += commands[1][0];
				helpscreen(tempString); //Take first letter of second command
			}
			else {
				helpscreen("a"); //Calling the help function. a = show all help sections.
			}
		}
		else if (commands[0].compare(acceptedCommands[2]) == 0) {
			cout << "Exiting..." << endl; //Testing
		}
		else {
			cout << endl << "INVALID COMMAND" << endl;
			helpscreen("c"); //Calling help function, listing valid commands. cu = command, usage
		}



	} while (commands[0].compare(acceptedCommands[2]) != 0 && clv == false);


	return 0;
}