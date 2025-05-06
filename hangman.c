#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORD_LENGTH 100
#define INPUT_BUFFER_SIZE 10
#define MAX_INCORRECT_GUESSES 6
#define ALPHABET_SIZE 26
#define SCREEN_CLEAR_LINES 50

char **loadWords(const char *filename, int *wordCount);
void freeWordList(char **wordList, int wordCount);
void clearScreen();

/**
 * @brief Loads words from a specified file into a dynamically allocated array
 * of strings.
 *
 * Opens the file, counts the words, allocates memory for the word list (array
 * of char*) and for each individual word (char*), reads words line by line,
 * stores them, and returns the list. Also updates the word count via the output
 * parameter.
 *
 * @param filename The path to the file containing words (one word per line).
 * @param wordCount A pointer to an integer where the number of loaded words
 * will be stored. This is an output parameter.
 * @return A pointer to the dynamically allocated array of word strings
 * (char**), or NULL if an error occurs (e.g., file not found, memory allocation
 * failed). The caller is responsible for freeing the allocated memory using
 * freeWordList (which we will define later).
 */
char **loadWords(const char *filename, int *wordCount) {
  *wordCount = 0;
  FILE *file = NULL;
  file = fopen(filename, "r");
  char **wordList = NULL;
  int i = 0;
  char *currentWord = NULL;

  if (file == NULL) {

    fprintf(stderr, "Could not open the word file: %s\n", filename);
    fprintf(stderr, "Please ensure the file exists in the same directory as "
                    "the program.\n");

    return NULL;
  }

  int lineCount = 0;
  char lineBuffer[MAX_WORD_LENGTH];

  while (fgets(lineBuffer, MAX_WORD_LENGTH, file) != NULL) {
    lineCount++;
  }

  if (ferror(file)) {
    perror("Error reading file during word count");
    fprintf(stderr, "An error occurred while trying to count words in '%s'.\n",
            filename);
    fclose(file);
    return NULL;
  }

  if (lineCount == 0) {
    fprintf(stderr,
            "Warning: Word file '%s' is empty or contains no valid lines.\n",
            filename);
    fclose(file);
    // We can return NULL or an empty list. Returning NULL is simpler for now.
    // *wordCount is already 0.
    return NULL;
  }
  rewind(file);

  size_t listSize = lineCount * sizeof(char *);
  wordList = (char **)malloc(listSize);
  if (wordList == NULL) {
    perror("Memory allocation failed for word list");
    fprintf(stderr, "Error: Could not allocate memory to store %d words.\n",
            lineCount);
    fclose(file);
    return NULL;
  }
  for (int k = 0; k < lineCount; ++k) {
    wordList[k] = NULL;
  }
  i = 0;

  while (fgets(lineBuffer, MAX_WORD_LENGTH, file) != NULL) {
    if (i >= lineCount) {
      fprintf(stderr,
              "Warning: Read more lines than initially counted. Stopping at %d "
              "lines.\n",
              lineCount);
      break;
    }
    size_t len = strlen(lineBuffer);

    if (len > 0 && lineBuffer[len - 1] == '\n') {
      lineBuffer[len - 1] = '\0';
      len = strlen(lineBuffer);
    }
    if (len == 0) {
      continue;
    }

    size_t wordMemorySize = len + 1;
    currentWord = (char *)malloc(wordMemorySize);

    if (currentWord == NULL) {
      perror("Memory allocation failed for word string");
      fprintf(stderr, "Error: Could not allocate memory for word #%d ('%s').\n",
              i + 1, lineBuffer);
      fprintf(stderr, "Cleaning up allocated memory before exiting...\n");
      for (int j = 0; j < i; j++) {
        if (wordList[j] != NULL) {
          printf("DEBUG: Freeing previous word #%d at %p\n", j + 1,
                 (void *)wordList[j]);
          free(wordList[j]);
        }
      }
      free(wordList);
      fclose(file);
      return NULL;
    }

    strcpy(currentWord, lineBuffer);
    wordList[i] = currentWord;

    i++;
  }
  if (ferror(file)) {
    perror("Error reading file during second pass");
    fprintf(stderr, "An error occurred while reading words from '%s'.\n",
            filename);
    fprintf(stderr, "DEBUG: Need to implement proper cleanup for partially "
                    "loaded list on read error!\\n");
    free(wordList); // Free the array of pointers
    fclose(file);   // Close the file
    return NULL;    // Signal failure
  }
  int finalWordCount = i;

  if (fclose(file) == EOF) {
    perror("Warning: Error closing file");
  }
  *wordCount = finalWordCount;

  return wordList;
}

/**
 * @brief Frees the memory allocated for the word list.
 * Frees each individual word string, then frees the array of pointers.
 * @param wordList The list of words (array of char*).
 * @param wordCount The number of words in the list.
 */
void freeWordList(char **wordList, int wordCount) {
  if (wordList == NULL) {
    return;
  }
  for (int i = 0; i < wordCount; i++) {
    if (wordList[i] != NULL) {
      free(wordList[i]);
      wordList[i] = NULL;
    }
  }
  free(wordList);
}

void clearScreen() {
  // Method 1: System calls (OS-dependent)
  // Uncomment the appropriate line for your OS, or use conditional compilation
  // system("cls"); // For Windows
  // system("clear"); // For Linux/macOS

  // Method 2: Printing newlines (Portable)
  // Print a bunch of newlines to push old content off the screen.
  for (int i = 0; i < SCREEN_CLEAR_LINES; i++) {
    printf("\n");
  }
}

void pauseForUser() {
  printf("Press Enter to continue...");
  char discard;
  // Clear any remaining characters from the current input line first
  while ((discard = getchar()) != '\n' && discard != EOF)
    ;
  // Wait for a new Enter press
  while ((discard = getchar()) != '\n' && discard != EOF)
    ;
}

int main() {
  srand(time(NULL));
  // Program logic will go here in later steps.
  // Task 20: Call loadWords here.
  // Task 21: Add error checking after calling loadWords.

  printf("Welcome to Hangman!\n"); // Simple message

  // Placeholder call to loadWords (will be refined in Task 20)
  int loadedWordCount = 0;
  char **wordList = loadWords("words.txt", &loadedWordCount);

  // Placeholder check (will be refined in Task 21)
  if (wordList == NULL) {
    fprintf(stderr, "Error loading words from file.\n");
    return 1;
  }
  if (loadedWordCount <= 0) {
    fprintf(stderr, "Error: No words were loaded (%d). Cannot start game.\\n",
            loadedWordCount);
    // Even if wordListMain isn't NULL in some strange case, we need to free it.
    freeWordList(wordList, loadedWordCount); // Use the cleanup function
    return 1;                                // Indicate failure
  }
  // If we reach here, loading was successful!
  printf("Word list loaded successfully. Ready to play!\n\n");

  int randomIndex = rand() % loadedWordCount;
  char *secretWord = wordList[randomIndex];
  printf("DEBUG: Random word selected: %s\n", secretWord);
  printf("DEBUG: Maximum incorrect guesses allowed: %d\n",
         MAX_INCORRECT_GUESSES);

  int incorrectGuesses = 0;
  size_t wordLength = strlen(secretWord);
  char displayWord[wordLength + 1];
  for (size_t j = 0; j < wordLength; j++) {
    displayWord[j] = '_';
  }
  displayWord[wordLength] = '\0';

  char guessedLetters[ALPHABET_SIZE + 1] = {0};
  int numGuessedLetters = 0;

  int gameOver = 0;
  int playerWon = 0;

  printf("\nStarting the game loop...\n");

  // Task 4.1: Start a `while` loop that continues as long as `gameOver` is
  // false (0).
  while (!gameOver) { // Loop continues as long as gameOver is 0
    // --- Inside the loop: Turn logic ---

    // Task 4.2: Clear screen (optional)
    // system("clear"); // Linux/macOS
    // system("cls");   // Windows
    clearScreen();
    printf("--- HANGMAN ---\n");

    // Task 4.3: Display Hangman drawing (call drawHangman function - Step 7)
    printf("Hangman state (Incorrect guesses: %d/%d)\n", incorrectGuesses,
           MAX_INCORRECT_GUESSES);
    // drawHangman(incorrectGuesses); // Placeholder for Step 7

    // Task 4.4: Display the displayWord (e.g., _ _ a _ _)
    printf("Word: ");
    for (size_t i = 0; i < wordLength; i++) {
      printf("%c ", displayWord[i]); // Print with spaces
    }
    printf("\n");

    // Task 4.5: Display incorrect guesses remaining
    int guessesRemaining = MAX_INCORRECT_GUESSES - incorrectGuesses;
    printf("Incorrect guesses remaining: %d\n", guessesRemaining);

    // Task 4.6: Display letters already guessed
    printf("Guessed letters: %s\n", guessedLetters);

    // Task 4.7: Prompt user for guess
    printf("Enter your guess (a single letter): ");
    char inputBuffer[INPUT_BUFFER_SIZE]; // Buffer to hold the raw input line
    char currentGuess = '\0';            // Initialize guess character

    // Read a line from standard input (keyboard)
    if (fgets(inputBuffer, sizeof(inputBuffer), stdin) == NULL) {
      // Handle error or EOF (End Of File) condition
      // If fgets returns NULL, it could be an error or EOF.
      // EOF is often triggered by Ctrl+D (Unix/Linux/macOS) or Ctrl+Z then
      // Enter (Windows).
      if (feof(stdin)) {
        printf("\nEOF detected on input. Exiting game.\n");
      } else {
        // A read error occurred, less common with stdin
        perror("Error reading input");
      }
      gameOver = 1; // Set flag to exit the game loop
      continue;     // Skip the rest of this loop iteration
    }

    if (strlen(inputBuffer) != 2) {
      printf("Invalid input format. Please enter exactly one letter and press "
             "Enter.\n");
      pauseForUser();
      continue;
    } else {
      currentGuess = inputBuffer[0];
      currentGuess = tolower(currentGuess);
      if (!isalpha(currentGuess)) {
        printf("Invalid input. Please enter a letter (a-z).\n");
        pauseForUser();
        continue;
      }
    }

    // Placeholder for where the rest of Step 5 and Step 6 logic goes
    if (isalpha(currentGuess)) {

      // Task 5.6: Check if the letter was already guessed.
      // Use strchr (from <string.h>) to search within guessedLetters.
      // It returns NULL if the character is not found.
      if (strchr(guessedLetters, currentGuess) != NULL) {
        // Task 5.7: Handle the case where the letter was already guessed.
        printf("\\n-> You already guessed '%c'. Try a different letter.\\n",
               currentGuess);
        pauseForUser();
        continue; // Skip the rest of this turn
      } else {
        // Letter is new and valid!
        guessedLetters[numGuessedLetters] = currentGuess;
        numGuessedLetters++;
        guessedLetters[numGuessedLetters] = '\0';

        // --- TODO: Implement Step 6 (Process Correct/Incorrect) logic here ---\
                // Use 'currentGuess'
        int correctGuess = 0;
        printf("\n-> Processing new guess '%c'...\n", currentGuess);
        printf("    -> Checking '%c' against secret word '%s'...\n",
               currentGuess, secretWord); // Debug
        for (size_t i = 0; i < wordLength; i++) {
          if (secretWord[i] == currentGuess) {
            printf("Found match at index %zu! (Will update displayWord)\n",
                   i);                     // Debug
            displayWord[i] = currentGuess; // Task 6.3
            correctGuess = 1;              // Task 6.4
          }
        }

        if (!correctGuess) {
          printf(
              "Guess '%c' was INCORRECT. (Will increment incorrectGuesses)\n",
              currentGuess);  // Debug
          incorrectGuesses++; // Task 6.5
        } else {
          printf("Guess '%c' was CORRECT.\n", currentGuess); // Debug
        }

      } // End of check for new/repeat guess

    } // End of validation checks block

    if (guessesRemaining <= 0 &&
        !gameOver) { // Temporary exit condition for testing
      printf("DEBUG: Forcing game over (out of guesses).\n");
      gameOver = 1;
      pauseForUser();
    }
  } // End of while (!gameOver) loop
  clearScreen();
  // --- After the loop (Game Over) ---
  printf("\n--- Game Over --- \n");
  // Check playerWon flag (from Step 8) to display final message
  if (playerWon) {
    printf("Congratulations! You guessed the word: %s\n", secretWord);
  } else {
    printf("Sorry, you ran out of guesses. The word was: %s\n", secretWord);
  }

  // --- Memory cleanup ---
  printf("\nCleaning up allocated memory...\n");
  // A return value of 0 typically indicates that the program executed
  // successfully.
  freeWordList(wordList, loadedWordCount);
  printf("\nGame Over. Thanks for playing!\n");
  return 0;
}
