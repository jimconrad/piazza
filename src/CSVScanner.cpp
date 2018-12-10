#include <fstream>
#include <istream>
#include <string>
#include <vector>


/*
 * Parses through a CSV file
 *
 * Based on similar Java implementation
 *
 *@author: Patrick Chapman
 */
class CSVScanner {

	private:
		std::ifstream file;

		/*
		 * The state machine for a simplified CSV parser
		 * @author kq7b
		 */
		enum ParserStates {
			SpinWhite,
			QuotedField,
			FindDelimiter,
			PlainField,
			Finished
		};

		/*
		 * Internal class to parse a single row of data from a CSV file
		 */
		class CSVRowScanner {
			private:
				std::string whiteSpace = "\t";
				const char QUOTE = '"';
				const char DELIMITER = ',';

				std::string row;
				unsigned int p; //char position index into the row
				ParserStates state;
				std::string field; //CSV fields

			public:

				/*
				 * Constructor for CSVRowScanner
				 *
				 * @param row, the row from the CSV
				 */
				CSVRowScanner(std::string row) {
					this->row = row;
					this->p = 0;
					this->state = ParserStates::SpinWhite;
					this->field = "";
				}

				/* 
				 * Gets the next CSV field from the row
				 *
				 * @return The next field
				 */
				std::string next() {
					//iterating through each char
					while(hasNext()) {
						//grabbing the next char
						char c = this->row.at(p++);
				
					       //The state machine for the row	
						switch(state) {

							//Spinning white spaces
							case SpinWhite:
								if(whiteSpace.find(c) == std::string::npos){
									std::string charStr(1,c);
									this->field = charStr;
									if(c == QUOTE) state = ParserStates::QuotedField;

									else state = ParserStates::PlainField;	
								}
								break;

							//Parse the quoted fields
							case QuotedField:
								this->field += c;
								if(c == QUOTE) state = ParserStates::FindDelimiter;

								break;

							//Find the delimiter
							case FindDelimiter:
								if(c == DELIMITER){
									std::string result = field;
									this->field = "";
									state = ParserStates::SpinWhite;

									return result;
								}
								break;

							//Parse a normal field
							case PlainField:
								if(c == DELIMITER) {
									std::string result = field;
									this->field = "";
									state = ParserStates::SpinWhite;

									return result;
								} else {
									this->field += c;
								}
								break;
							case Finished:
								break;

						}
					}
					
					std::string result = field;
					this->field = ""; //reset for next field	
					this->state = ParserStates::Finished;
					return result; //returns the field


				}


				/*
				 * Returns if there is more data in the row
				 */
				bool hasNext() {
					return p<row.length();
				}
		};
	public:
		bool hasNext();
		std::vector<std::string> readRow();
		CSVScanner(std::string fileStr);
};

/*
 * Returns if the file has more data
 */
bool CSVScanner::hasNext() {
	return file.peek() != EOF;
}

/*
 * Reads in row from CSV file
 *
 * @return content from the CSV fields
 */
std::vector<std::string> CSVScanner::readRow() {
	std::string row;
	std::getline(file, row);

	// std::cout << row << "\n";
	CSVRowScanner fieldScanner(row);

	std::vector<std::string> tokenList;
	while(fieldScanner.hasNext()) {
		std::string token = fieldScanner.next();
		tokenList.push_back(token);
	}

	return tokenList;
}


/*
 * Constructor for the CSV scanner
 */
CSVScanner::CSVScanner(std::string fileStr) {
	file = std::ifstream(fileStr);
	
	//Does the file exist?
	if(!file.good()){
		std::cerr << "File " << fileStr << " does not exist!\n";
		exit(400);
	}
}
