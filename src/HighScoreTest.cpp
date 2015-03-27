/*
 * HighScoreTest.cpp
 *
 *  Created on: Feb 2, 2015
 *      Author: duncjo01
 */
#include <iostream>
// for the random number generators
#include <random>
#include "HighScores.h"

using namespace std;

int testFunct() {
	random_device rd;
	/* pass the random device into the ``random number engine
	   ... based on Mersenne Twister algorithm."
	   ... It will produce "high quality unsigned integer random numbers." */
	mt19937_64 gen(rd());
	// choose integers between 1 and 100 for random score values
	uniform_int_distribution<> dis(1, 100);
	// choose integers between 65 and 90 for random name values
	// ... (will be converted to chars)
	uniform_int_distribution<> ascii(65, 90);

	HighScores highscore ("HighestBest hiscores", 30, true);
	string str;
	int num;

	cout << "Please note: when the list is in HighestBest mode, it will print least to greatest" << endl;
	cout << "When the list is in LowestBest mode, it will print greatest to least" << endl;
	cout << "The ``better\" value will always be at the end of the list\n" << endl;
	cout << "Populating ``" << highscore.getTitle() <<"\" with random data." << endl;
	cout << "Names will be random ascii values between values 65 and 90 " << endl;
	cout << "Scores will be random values between 1 and 100" << endl;

	for(int i = 0; i < 35; i++) {
		num = dis(gen);
		for(int j=0; j<5; j++)
			str += char(ascii(gen));
		highscore.addScore(str, num);
		str = "";
	}

	string fileName = "data.dat";
	cout << "Population complete!\n" << endl;
	highscore.print();
	cout << "\nWriting data from ``" << highscore.getTitle() << "\" out to " << fileName << endl;
	highscore.writeToFile(fileName);

	HighScores newList ("New HighestBest hiscores", 30, true);
	cout << "Now reading in from file " << fileName << " into object ``" << newList.getTitle() << "\"" << endl;
	newList.readFromFile(fileName);
	cout << "Successfully read data into new object from file! " << endl;
	cout << "The output below should be identical to the first vector output!\n" << endl;
	newList.print();
	cout << "Changing title of the new list to ``New List\"" << endl;
	newList.setTitle("New List");
	newList.print();

	cout << "\nCreating new object that will read in the existing list data" << endl;
	cout << "This time, the data will be from greatest to least (thanks to our boolean parameter!)" << endl;
	//cout << "And this list will also be limited to 25 slots. ";
	HighScores reverseList ("lowestBest hiscores", 30, false);
	reverseList.readFromFile(fileName);
	reverseList.setTitle("lowestBest hiscores");
	reverseList.print();
	cout << "Subtracting 5 from the maximum highscore size and reading in from file again" << endl;
	HighScores smallerReverseList ("smaller lowestBest hiscores", reverseList.getSize()-5, false);
	smallerReverseList.readFromFile(fileName);
	smallerReverseList.setSize(reverseList.getSize()-5);
	smallerReverseList.print();
	cout << "The five highest values were removed, since our boolean says that the lowest values are the best" << endl;
	cout << "Total number of scores stored in " << smallerReverseList.getTitle() << ": " << smallerReverseList.totalScores() << endl;

	cout << "\nTesting iterator for individual getName(int i) and getScore(int i) functions" << endl;
	for (int i = 0; i < smallerReverseList.totalScores(); i++) {
		cout << "pos" << i << " NAME: " << smallerReverseList.getName(i) << ", SCORE: " << smallerReverseList.getScore(i) << endl;
	}

	cout << "The last time a top (#1) score was achieved was on: " << smallerReverseList.getTopScoreDate();
	return 0;
}


