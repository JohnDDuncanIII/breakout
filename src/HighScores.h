/*
 * HighScores.h
 *
 *  Created on: Jan 27, 2015
 *      Author: cpresser
 * Appended on: Feb 02, 2015
 *  	Author: John D. Duncan, III
 * This header file defines the constants, protected instant variables,
 * ... and functions that we will be implementing in our HighScores.cpp
 */

#ifndef HIGHSCORES_H_
#define HIGHSCORES_H_

using namespace std;

/**
 * Class HighScores stores a list of high scores for a game.
 */
class HighScores {

public:
	//Default values for title and max number of scores
	static const int DEFAULT_NUMBER_OF_SCORES;
	static const string DEFAULT_TITLE;
	static const int LOWEST_VAL;
	static const int HIGHEST_VAL;

	/**
	 * Default constructor: no arguments.
	 * sets default values for the instance variable
	 */
	HighScores();

	/**
	 * Constructor:
	 * parameters:
	 *              title: the title of this high score list.
	 *              number: the maximum number of high scores to be stored
	 *              highestBest: determines the order of the scores
	 *                      true: The best score will be the highest (e.g. pinball)
	 *                      false: The best score will be the lowest (e.g. golf)
	 */
	HighScores(string title, int number, bool highestBest);

	/**
	 * Destructor: clean up resources
	 */
	virtual ~HighScores();

	/**
	 * addScore: Add a player's name and score to the list if it is
	 *              good enough to merit being a high score.
	 *              Handles whether or not the list is sorted least to greatest,
	 * 					... or greatest to least.
	 * 				Also keeps the list sorted after every addition to it
	 *      parameters:
	 *              name: the name of the player
	 *              score: the player's score
	 *      return value:
	 *              true if the player and score are added to the list
	 *              false if the score was not good enough to make the list
	 */
	bool addScore(string name, int score);

	/*
	 * isHighScore: determine if a score is good enough to be on the list.
	 * Since there are a maximum number that can be stored, this may cause a
	 * poorer score to be removed.
	 * Note: "good enough" can be either higher or lower than others, depending
	 * on whether higher scores are better, or lower scores are better.
	 *
	 *      parameters:
	 *
	 *              score: the score to test
	 *      return value:
	 *              true: the score is good enough
	 *              false: it is not good enough.
	 */
	bool isHighScore(int score) const;

	/**
	 * clearScores: empty the high scores so there are no longer any stored.
	 */
	void clearScores();

	/**
	 * totalScores: the number of scores stored. Note: this will be less than
	 * or equal to the maximum.
	 *      return value: the total number of high scores that are stored.
	 */
	int totalScores();


	/**
	 * readFromFile: read the high score data from the file given
	 * by the filename.
	 *      parameter: filename the name of the file to be read
	 *      return value:
	 *              true: the file was successfully opened and read
	 *              false: the file open or read failed.
	 *
	 */
	bool readFromFile(const string &filename);

	/**
	 * writeToFile: write the high score data to a file given
	 * by the filename.
	 *      parameter: filename the name of the file to be read
	 */
	void writeToFile(const string &filename) const;

	/**
	 * print: print the contents of this high score table to cout.
	 * Does not print out any spaces that are default placeholders in the vector
	 */
	void print() const;

	/**
	 * Sorts the vector, either from least to greatest or greatest to least,
	 * ... based on the value of the integer highscore value
	 */
	void sortVec();

	/**
	 * Gets the name at position 'i' in the vector
	 *  	parameter: i the position in the vector to get
	 *      return value:
	 *              the value of the string at position i in the vector
	 */
	string getName(int i);

	/**
		 * Gets the score at position 'i' in the vector
		 *  	parameter: i the position in the vector to get
		 *      return value:
		 *              the value of the integer at position i in the vector
		 */
	int getScore(int i);

	string getTitle ();

	int getSize ();

	/*
	 * Sets the title of the high score list
	 * 			parameter: what you want to change the title of the list to
	 */
	void setTitle (string title);

	/*
	 * Sets the size of the high score list
	 * 			parameter: what you want to change the size of the list to
	 */
	void setSize (int size);

	/*
	 * Gets the current date, in UNIX format [ex. ``Mon Feb  2 19:53:14 EST 2015"]
	 */
	string getDate ();
	/*
	 * Returns the most recent date that a top score was achieved
	 */
	string getTopScoreDate ();

	vector<pair<string, int>> getVector();

// Instance variables
protected:
	// defines the title of the HighScore object
	string 	listTitle;
	// defines the maximum number of high scores allowed
	// ... in the HighScore object
	int 	maxScores;
	// determines whether or not the highest numbers should be considered the best, or the lowest numbers
	bool 	highestBest;
	vector<pair<string, int>> vec;
	// holds the last date that a user got an all time high score
	string topScoreDate;
};

#endif /* HIGHSCORES_H_ */

