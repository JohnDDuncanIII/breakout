//============================================================================
// Name        : HighScores.cpp
// Author      : John D. Duncan, III
// Description : This class implements the functions defined in the HighScores.h file.
// 				 ... it will create and manage a vector of high scores.
//============================================================================

// for file I/O
#include <iostream>
#include <fstream>
// for easy to use arrays
#include <vector>
// for sorting
#include <algorithm>
// used for iterating through vector
#include <iterator>
// for making pairs
#include <utility>
// for maximum/minimum possible values of ints
#include <limits>
// for getting the date/time
#include <ctime>
// for pausing the running thread
#include <chrono>
#include <thread>
// obviously include our header file for the Class
#include <random>
#include "HighScores.h"

using namespace std;

// the default number of player scores to store
const int HighScores::DEFAULT_NUMBER_OF_SCORES 	= 20;
// the default name of the high score list
const string HighScores::DEFAULT_TITLE 			= "HighScores";
// the lowest possible integer value
const int HighScores::LOWEST_VAL 				= numeric_limits<int>::min(); // minimum int value
// the highest possible integer value
const int HighScores::HIGHEST_VAL 				= numeric_limits<int>::max(); // max int value

/* a generic struct that we can use to compare two values in a vector
 * This makes sorting much easier, since we can just invoke the required struct
 */
struct CompareHigh {
	template<typename T, typename U>
	bool operator() (const pair<T,U> &x, const pair<T,U> &y) const {
		return x.second < y.second;
	}
};

struct CompareLow {
	template<typename T, typename U>
	bool operator() (const pair<T,U> &x, const pair<T,U> &y) const {
		return x.second > y.second;
	}
};
/*
 * Default constructor for our HighScores object
 * Initializes the variables the efficient way
 */
HighScores::HighScores():
	listTitle(DEFAULT_TITLE),
	maxScores(DEFAULT_NUMBER_OF_SCORES),
	highestBest(true),
	vec(maxScores, make_pair("", LOWEST_VAL)) {}

/*
 * Parameterized constructor for our HighScores object
 * Initializes the variables the efficient way
 */
HighScores::HighScores(string t, int n, bool h):
	listTitle(t),
	maxScores(n),
	highestBest(h),
	vec(maxScores, make_pair("", highestBest ? LOWEST_VAL : HIGHEST_VAL)) {}


// Destructor
HighScores::~HighScores() {
	//used to destroy stuff
}

bool HighScores::addScore(string name, int score) {
	//int oldScoreLow = vec[0].second;
	int oldScore = vec[vec.size()].second;

	if (isHighScore(score)) {
		for (unsigned int i = 0; i < vec.size(); i++) {
			if ((vec[i].second < score && highestBest) || (vec[i].second > score && !highestBest)) {
				// create the pair, add it to the vector
				vec[i] = make_pair(name, score);
				// keep vector sorted
				sortVec();
				if(oldScore < score && highestBest || oldScore > score && !highestBest) {
					topScoreDate = getDate();
				}
				return true;
			}
		}
	}
	return false;
}

bool HighScores::isHighScore(int score) const {
	// iterate through the vector
	for (const auto &p : vec) {
		// if the score is valid, we will return true
		if( ((p.second < score) && highestBest) || ((p.second > score) && !highestBest) )
			return true;
	}
	return false;
}

void HighScores::clearScores() {
	for(int i = 0; i < vec.size(); i++) {
		 vec[i] = make_pair("", highestBest ? LOWEST_VAL : HIGHEST_VAL);
	}
}

int HighScores::totalScores() {
	int toReturn = 0;
	for (const auto &pos : vec){
		// if the integer value at the location is anything other than default...
		if(pos.second != LOWEST_VAL || pos.second != HIGHEST_VAL)
			toReturn++; // increment the counter
	}
	return toReturn;
}

bool HighScores::readFromFile(const string &filename) {
	// open input stream
	ifstream inFile(filename.c_str());
	// check to see if the file exists
	if (inFile.is_open()) {
		clearScores();
		string line;
		// continue to loop while we still have more lines to parse
		while (getline(inFile, line)) {
			// As defined in the writeToFile, the title will be
			// ... contained after ``Title:"
			if(line.find("Title:") != string::npos) {
				// gets the title
				string title = line.substr(line.find(":")+2, line.size());
				// sets the title
				setTitle(title);
			}
			// As defined in the writeToFile, the size will be
			// ... contained after ``Size:"
			if(line.find("Size:") != string::npos) {
				int size = atoi(line.substr(line.find(":")+2, line.size()).c_str());
				setSize(size);
			}
			// As defined in the writeToFile, the vector will be
			// ... contained after ``contains:"
			if(line.find("contains:") != string::npos) {
				// gets the raw output from my print() method
				// ... and removes some unnecessary brackets
				string rawScores = line.substr(line.find(":")+2, line.size());
				// the name of the player
				string name;
				// the score of the player
				int score;
				for(unsigned int i = 0; i < maxScores; i++) {
					// gets the name. substr call set up so it removes the brackets
					// ... that were added during print() output
					name 		= rawScores.substr	(rawScores.find_first_of('[') + 1, rawScores.find_first_of(']') - 1);
					// removes last entry from string
					rawScores 	= rawScores.substr(rawScores.find_first_of(']') + 1, rawScores.size());

					// gets the score. substr call set up so it removes the brackets
					// ... that were added during print() output
					score 		= atoi(rawScores.substr	(rawScores.find_first_of('[') + 1, rawScores.find_first_of(']') - 1).c_str());
					// removes last entry from string
					rawScores 	= rawScores.substr(rawScores.find_first_of(']') + 2, rawScores.size());

					// adds this score from the file to the object
					addScore(name, score);
				}
				// adds the final name and sc                                                                                          ore to the vector
				name 		=  rawScores.substr	(rawScores.find_first_of('[') + 1, rawScores.find_first_of(']') - 1);
				rawScores 	=  rawScores.substr(rawScores.find_first_of(']') + 1, rawScores.size());
				score		=  atoi(rawScores.substr(rawScores.find_first_of('[') + 1, rawScores.find_first_of(']') - 1).c_str());
				//addScore(name, score);
			}
		}
		return true;
	}
	inFile.close();
	return false;
}

void HighScores::writeToFile(const string &filename) const {
	// open output stream
	ofstream outFile (filename.c_str());

	// check to see if the file exists
	// ... (it probably will, since we just created it...)
	if (outFile.is_open()) {
		// output the title
		outFile << "Title: " << listTitle << "\n";
		// output number of scores
		outFile << "Size: " << maxScores << "\n";
		// output the vector
		outFile << listTitle << " contains: ";

		for (const auto &pos : vec)
			outFile << "[" << pos.first << "][" << pos.second << "] ";
		outFile << endl;
	}
	else cout << "Unable to open file";
	outFile.close();
}


void HighScores::print() const {
	// prints the vector. similar to an ArrayList's
	//.. toString() method in Java
	cout << listTitle << " contains: [ ";
	for (const auto &pos : vec){
		if(pos.first != "" && (pos.second != LOWEST_VAL || pos.second != HIGHEST_VAL))
			cout << "[" << pos.first << "][" << pos.second << "] ";
	}
	cout << "]" << endl;
}

void HighScores::sortVec() {
	// uses ternary operator to sort the vector by either
	// ... lowest to highest or highest to lowest
	highestBest ? sort(vec.begin(), vec.end(), CompareHigh()) :
			sort(vec.begin(), vec.end(), CompareLow());
}

string HighScores::getName (int i) {
	return vec[i].first; // return name at position i
}

int HighScores::getScore (int i) {
	return vec[i].second; // return score at position i
}

string HighScores::getTitle () {
	return listTitle;
}

int HighScores::getSize () {
	return maxScores;
}

void HighScores::setTitle (string title) {
	listTitle = title; // set the title of the object
}

void HighScores::setSize (int size) {
	maxScores = size; // set the max amount of scores in object
}

string HighScores::getTopScoreDate (){
	return topScoreDate;
}

string HighScores::getDate () {
	string toReturn;
	// get time now
	time_t t = time(0);
	// Converts ``the simple time pointed to by time to broken-down
	// time representation, expressed relative to the user's specified time zone."
	tm * now = localtime( & t );

	return toReturn += asctime(now);
}

vector<pair<string, int>> HighScores::getVector() {
	return vec;
}


