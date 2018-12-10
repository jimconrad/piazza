#include <map>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <iostream>
#include "CSVScanner.cpp"

/*
 * Tallies poll data from Piazza files.
 *
 * C++ implementation of similar Java program written by jimconrad.
 *
 * @author Patrick Chapman
 */
class TallyPiazzaPollResults {
	private:
		std::map<std::string, std::map<std::string, std::string>> pollResults;

		const int MAXPOLLS = 256;

		/*
		 * Prints usage for the program
		 */
		void printUsage() {
			std::cerr << "Usage Error!\n";
			std::cerr << "TallyPiazzaPollResults pollFile1 pollFile2 [...] -a pollFile1Answer pollFile2Anser [...]\n";
			std::cerr << "Every poll file must include an answer!\n";
			std::cerr << "Maximum number of polls is 256!\n";	
		}

		/*
		 * Processes statistics from piazza poll file
		 *
		 * @param pollStatFile, the name of the piazza poll file
		 */
		int processPiazzaStatFile(std::string pollStatFile) {
			CSVScanner csv (pollStatFile); //read file in as csv

			std::vector<std::string> headers = csv.readRow();

			//parsing the csv
			while(csv.hasNext()) {
				std::vector<std::string> fields = csv.readRow();
				std::string bsuName = "";

				if((fields.size() >= 3) && fields[1].find(',') != std::string::npos) {
					std::string eMailList = fields[1];
					std::transform(eMailList.begin(), eMailList.end(), eMailList.begin(), ::tolower);
					int bsu = eMailList.find("boisestate@edu");

					std::string bsuEdu("boisestate@edu");
					int bsu2 = bsu + bsuEdu.length();

					while((bsu > 0) && eMailList.at(bsu) != ',') {
						bsu--;
					}
					int bsu1 = bsu+1;

					if((bsu != -1)) bsuName = eMailList.substr(bsu1, bsu2);

					else bsuName = eMailList;

					if((bsuName.length() > 2) &&
						(bsuName.at(0) == '"') &&
						(bsuName.at(bsuName.length()-1) != '"')) {
						
						bsuName += '"';
					}

				} else if (fields.size() >= 3) {
					bsuName = fields[1];
				} else {
					return 1;
				}

				if(fields.size() >= 3) {
					TallyPiazzaPollResults::tallyVote(pollStatFile, fields[0], bsuName, fields[2]);
				}
			}

			return 0;
		}

		/*
		 * Tallies the student's vote for a specific poll and inserts into pollResults
		 *
		 * @param pollName, name of the poll
		 * @param studentName, name of the student
		 * @param studentKey, key of the student for the map
		 * @param, studentChoice, student's choice for the poll
		 */
		void tallyVote(std::string pollName, std::string studentName,
				std::string studentKey, std::string studentChoice) {
			std::map<std::string, std::string> pollData;

			pollData = pollResults[studentKey];

			if(pollData.empty()) {
				pollResults.insert(std::pair<std::string, std::map<std::string, std::string>>(studentKey, pollData));
			}

			pollData.insert(std::pair<std::string, std::string>(pollName, studentChoice));

			pollResults[studentKey] = pollData;



		}

		/*
		 * Removes quotations from a string
		 *
		 * @param s, string to remove quotes from
		 */
		std::string unQuote(std::string s) {
			std::string result = s;
			if(result.length() < 2) return result;

			if(result.at(0) == '"') {
				result = result.substr(1, result.length());

				int p;
				if((p = result.find('"')) > 0) {
					result = result.substr(0, p);
				}
			}
			return result;
		}

	public:
		/*
		 * Main processing of the piazza polls
		 *
		 * @param arc, mimic argc from command line
		 * @param argv, mimic argv from command line
		 */
		int main(int argc, const char * argv[]) {
			int exitStatus = 0;

			//No files provided
			if (argc <= 1){
				printUsage();
				exitStatus++;
				return exitStatus;
			}

			//iterate through all arguments that are a file name
			int i;
			for(i = 1; i < argc && std::string("-a").compare(argv[i]) != 0; i++){
				std::string pollStatFileName(argv[i]);
				processPiazzaStatFile(pollStatFileName);

			}


			//iterate through all arguments that are an answer
			std::map<std::string, std::string> pollAnswers;
			int j = i;
			for(i = i+1; i < argc; i++) {
				std::string answer(argv[i]);
				std::string pollStatFileName(argv[i-j]);
				pollAnswers.insert(std::pair<std::string, std::string>(pollStatFileName, answer));
			}


			std::set<std::string> allPollNames; //contains all poll names

			//iterating through poll name data
			for(auto studIt = pollResults.begin(); studIt != pollResults.end(); ++studIt){
				std::set<std::string> studentPolls;
				std::map<std::string, std::string> pollMap = studIt->second;

				//all polls that a student participated in
				for(auto poll = pollMap.begin(); poll != pollMap.end(); ++poll) {
					studentPolls.insert(poll->first);
				}

				//initializing a set of all poll names
				for(auto studPoll = studentPolls.begin(); studPoll != studentPolls.end(); ++studPoll) {
					if(allPollNames.count(*studPoll) != 1) {
						allPollNames.insert(*studPoll);
					}
				}
			}

			//Answer for every poll
			if(allPollNames.size() != pollAnswers.size()){
				printUsage();
				return ++exitStatus;
			}	

			//setting up answer headers
			for(auto pollName = allPollNames.begin(); pollName != allPollNames.end(); ++pollName) {
				std::cout << ", " << pollAnswers[*pollName];
			}
			std::cout << "\n";

			//setting up column headers
			std::cout << "studentKey";
			for(auto pollName = allPollNames.begin(); pollName != allPollNames.end(); ++pollName) {
				std::cout << ", " << *pollName;
			}

			std::cout << ", participated, awarded";
			std::cout << "\n";

			//iterating through poll results
			for(auto studInfo = pollResults.begin(); studInfo != pollResults.end(); ++studInfo) {
				int attempted = 0; //number of poll attempts by user
				int correct = 0; //number of correct answers across all polls

				std::cout << studInfo->first; //this is the student email

				std::map<std::string, std::string> pollData = pollResults[studInfo->first];

				int answerIt = 0; //index for poll answers
				for(auto poll = allPollNames.begin(); poll != allPollNames.end(); ++poll) {
					std::string choice = pollData[*poll];

					//checking if poll attempted
					if(choice.empty()) choice = " ";

					else attempted++;

					//checking if answer is correct
					if(unQuote(choice).compare(pollAnswers[*poll]) == 0) correct++;

					answerIt++;
					std::cout << ", " << unQuote(choice);
				}

				//adding columns for attempted, correct, and total answer size
				std::cout << ", " << attempted << ", " << correct;
				std::cout << "\n";
			}

			return exitStatus;
		}
};

/*
 * Main program
 */
int main(int argc, const char * argv[]) {
	//Currently can't just treat the function
	//as if it's static. Potential point to
	//look at for refactoring.
	TallyPiazzaPollResults tally;
	return tally.main(argc, argv);
}
