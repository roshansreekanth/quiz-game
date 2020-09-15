#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int numberOfQuestions = 0;
int numberOfCharacters = 0;

void getCountQuestionsCharacters(char *filename)
{
    FILE *fileCounterPtr; 
    fileCounterPtr = fopen(filename, "r"); // I've assumed the file always ends with an empty new line
    if (fileCounterPtr == NULL){ printf("File does not exist\n"); return;}

    char c;
    
    while ((c = getc(fileCounterPtr)) != EOF) 
    {
        numberOfCharacters++;
        if(c == '\n')
        {
            numberOfQuestions++; // Everytime a new line is encountered it counts as a question
        }
    }

    fclose(fileCounterPtr);
    printf("The number of questions is: %d\n", numberOfQuestions);
}

void loadQuestionsAnswers(char* filename, char questions[numberOfQuestions][numberOfCharacters], char answers[numberOfQuestions][numberOfCharacters])
{
    char c;
    FILE *file;
    file = fopen(filename, "r");

    char sentences[numberOfQuestions][numberOfCharacters]; // Contains the question and the answer
    int letterCounter = 0;
    int sentenceCounter = 0;
    while ((c = getc(file)) != EOF) 
    {    
       sentences[sentenceCounter][letterCounter] = c;
       letterCounter++;
       if(c == '\n')
       {
           sentences[sentenceCounter][letterCounter] = '\0';
           sentenceCounter++;
           letterCounter = 0;
       }
    }
    
    for(int i = 0; i < numberOfQuestions; i++)
    {
        strcpy(questions[i], strtok(sentences[i], "?")); // Gets the question
        strcpy(answers[i], strtok(NULL, " ?")); // Gets the answer
    }

    for(int i = 0; i < numberOfQuestions; i++)
    {
        answers[i][strlen(answers[i]) - 1] = '\0'; // Adds a \0 to the end of each answer
    }
}

void convertToUpper(char answers[numberOfQuestions][numberOfCharacters]) // Converts answers to upper case so they are case insensitive
{
    for(int i = 0; i < numberOfQuestions; i++)
    {
        for(int j = 0; j < strlen(answers[i]); j++)
        {
            if (answers[i][j] >= 'a' && answers[i][j] <= 'z')
            {
                answers[i][j] -= 32;
            }
        }
    }
}

void randomize(int randomArray[], int size) // Uses Fisher-Yates Shuffle to randomize the indexes of an array
{
    for(int i = 0; i < size; i++)
    {
        randomArray[i] = i;
    }
    
    srand(time(NULL));
    for(int i = 0; i < size; i++)
    {
        int randomIndex = rand() % size;
        int temp = randomArray[i];
        randomArray[i] = randomArray[randomIndex];
        randomArray[randomIndex] = temp; // Swapping the index values
    }
    
}

void writeResultsToFile(char *fileName, int correctAnswers, int difficulty) // File Format: fileName correctAnswers numberOfQuestions difficulty\n
{
    FILE *filePtr;
    filePtr = fopen("quiz_history.txt","a");

    if (filePtr == NULL){ printf("File does not exist\n"); return;}
    fprintf(filePtr, "%s %d %d %d\n", fileName, correctAnswers, numberOfQuestions, difficulty);

    fclose(filePtr);
}

void loadClues(char answers[numberOfQuestions][numberOfCharacters], char clues[numberOfQuestions][numberOfCharacters], int difficulty)
{
    switch (difficulty)
    {
        case 1:
            for(int i = 0; i < numberOfQuestions; i++)
            {
                strcpy(clues[i], "?\0");
            }
            break;
        case 2:
            for(int i = 0; i < numberOfQuestions; i++)
            {
                char repeatString[strlen(answers[i]) + 1];

                for(int j = 0; j < strlen(answers[i]); j++)
                {
                    repeatString[j] = '-';
                }
                repeatString[strlen(answers[i])] = '\0';
                strcpy(clues[i], repeatString);
            }
            break;
        case 3:
            for(int i = 0; i < numberOfQuestions; i++)
            {
                int stringLength = strlen(answers[i]);
                char repeatString[stringLength + 1];
                repeatString[0] = answers[i][0];
                repeatString[stringLength -1] = answers[i][stringLength - 1];
                for(int j = 1; j < strlen(answers[i]) - 1; j++) // The  bounds of the loop exclude the first and last letter from changing into a '-'
                {
                    repeatString[j] = '-';
                }
                repeatString[strlen(answers[i])] = '\0';
                strcpy(clues[i], repeatString);
            }
            break;
        case 4:
            for(int i = 0; i < numberOfQuestions; i++)
            {  
                srand(time(NULL));
                int stringLength = strlen(answers[i]);
                char repeatString[stringLength + 1];

                int firstRandom = rand() % (stringLength - 1); 
                int secondRandom = rand() % (stringLength - 1);

                while(secondRandom == firstRandom) // If the randomly generated indexes are the same
                {
                    secondRandom = (rand() % (stringLength - 1));
                }

                for(int j = 0; j < stringLength; j++)
                {
                    repeatString[j] = j != firstRandom && j != secondRandom ? '-' : answers[i][j];
                }
                repeatString[strlen(answers[i])] = '\0';
                strcpy(clues[i], repeatString);
            }
            break;

        case 5: // Difficulty level 5 shuffles the answer
            srand(time(NULL));
            for(int i = 0; i < numberOfQuestions; i++)
            {
                int stringLength = strlen(answers[i]);
                char repeatString[stringLength + 1];
                int randomIndexes[stringLength];
                randomize(randomIndexes, stringLength); // Shuffles the indexes

                for(int j = 0; j < stringLength; j++)
                {
                    int randomIndex = randomIndexes[j];
                    repeatString[randomIndex] = answers[i][j]; 
                }
                
                repeatString[strlen(answers[i])] = '\0';
                
                if(strcmp(repeatString, answers[i]) == 0) // If the shuffled letters happen match up with the answer, redo the case
                {
                    loadClues(answers, clues, difficulty);
                }

                strcpy(clues[i], repeatString);
            }
            break;
        
        case 6: // Diffiulty level 6 chooses a random difficulty level and calls the function again
            srand(time(NULL));
            difficulty = (rand() % 5) + 1;
            loadClues(answers, clues, difficulty);
            
    }
}

int startGame(char questions[numberOfQuestions][numberOfCharacters], char answers[numberOfQuestions][numberOfCharacters], char clues[numberOfQuestions][numberOfCharacters])
{
    int correctQuestions = 0;
    int wrongQuestionsIndexes[numberOfQuestions];
    int wrongQuestionCounter = 0;
    int randomIndexes[numberOfQuestions];

    randomize(randomIndexes, numberOfQuestions); // Asks the questions in random order

    for(int i = 0; i < numberOfQuestions; i++) 
    {
        int randomIndex = randomIndexes[i];
        int characterCount = 0; // Current length of the actual answer
        int enteredAnswerLength = 0; // Current length of what the user inputs

        int correctAnswer = 1;

        printf("%s?\n", questions[randomIndex]);
        printf("Clue: %s\n", clues[randomIndex]);

        char input = 0;
        while(input != '\n') // Compares the input and the answer character by character
        {
            input = getchar();
            
            if (input >= 'a' && input <= 'z') // Makes the inputs capital so it is case insensitive
            {
                input -= 32;
            }

            
            if(enteredAnswerLength >  strlen(answers[randomIndex]) || (input != answers[randomIndex][characterCount] && input != '\n'))
            {
                correctAnswer = 0;
            }

            if(characterCount < strlen(answers[randomIndex])) // Excludes the \0
            {
                characterCount++;
            }
            
            enteredAnswerLength++;
        }

        if(enteredAnswerLength -1 != characterCount) // If the user's answer is smaller than the actual answer it is wrong
        {
            correctAnswer = 0;
        }

        if(correctAnswer)
        {
            printf("Correct!\n");
            correctQuestions++;
        }
        else
        {
            printf("Wrong Answer\n");
            wrongQuestionsIndexes[wrongQuestionCounter] = randomIndex;
            wrongQuestionCounter++;
        }
    }

    printf("Your score is %d/%d\n", correctQuestions, numberOfQuestions);

    if(wrongQuestionCounter > 0)
    {
        printf("Would you like to view the answers to the questions you got wrong? (y/n)\n");
        char input;
        scanf("%c", &input);

        if(input == 'y')
        {
            for(int i = 0; i < wrongQuestionCounter; i++)
            {
                int wrongQuestionIndex = wrongQuestionsIndexes[i];
                printf("%s? %s\n", questions[wrongQuestionIndex], answers[wrongQuestionIndex]);
            }
        }
    }
    return correctQuestions;
}

int main(int argc, char** argv) 
{
    // argv[0] : program name, argv[1]:file name, argv[2]: difficulty level
    getCountQuestionsCharacters(argv[1]);

    // Can read any number of questions dynamically as it reads the number of questions from the file.
    char questions[numberOfQuestions][numberOfCharacters];
    char answers[numberOfQuestions][numberOfCharacters];
    char clues[numberOfQuestions][numberOfCharacters];

    loadQuestionsAnswers(argv[1], questions, answers);
    convertToUpper(answers);

    loadClues(answers, clues, atoi(argv[2]));
    int correctQuestions = startGame(questions, answers, clues);
    writeResultsToFile(argv[1], correctQuestions, atoi(argv[2]));
}