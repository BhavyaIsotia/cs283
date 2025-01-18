//Run make to compile the program.
//Use ./stringfun to test your program.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here


int setup_buff(char *buff, char *user_str, int buff_size) {
    size_t user_str_len = 0;
    
    // Check for null pointer inputs
    if (buff == NULL || user_str == NULL) {
        return -2; 
    }

    // Calculate length of input string
    while (*(user_str + user_str_len) != '\0') {
        user_str_len++;
    }

    // Ensure user input is not longer than buffer size
    if (user_str_len >= buff_size) {
        return -1; 
    }

    char *buff_ptr = buff;
    char prev_char = ' ';  // Keeps track of the last character to handle extra spaces
    
    for (size_t i = 0; i < user_str_len; i++) {
        char curr_char = *(user_str + i);

        if (curr_char == '\t') {
            curr_char = ' ';  // Convert tabs to spaces
        }

        // Skip consecutive spaces
        if (curr_char == ' ' && prev_char == ' ') {
            continue;
        }

        *buff_ptr = curr_char;
        buff_ptr++;

        prev_char = curr_char;
    }

    // Fill the rest of the buffer with dots
    while (buff_ptr < buff + buff_size) {
        *buff_ptr = '.';
        buff_ptr++;
    }

    return user_str_len;  // Return the length of the string after setup
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len) {
    int count = 0;
    int in_word = 0;

    // We'll Iterate through each character in the buffer up to the length of the string
    for (int i = 0; i < str_len; i++) {
        
        // Check if the current character is not a space, period, or null character
        if (buff[i] != ' ' && buff[i] != '.' && buff[i] != '\0') {
            if (!in_word) {
                count++;
                in_word = 1;
            }
        } else {
            // If the current character is a space, period, or null character,
            // reset the in_word flag, indicating we're outside a word
            in_word = 0;
        }
    }
    return count;
}

void reverse_string(char *buff, int str_len) {
    printf("Reversed String: ");
    for (int i = str_len - 1; i >= 0; i--) {
        putchar(buff[i]);
    }
    putchar('\n');
}


void word_print(char *buff, int str_len) {
    printf("Word Print\n");
    printf("----------\n");

    int word_start = -1;
    int word_index = 1;

    for (int i = 0; i <= str_len; i++) {
        if (buff[i] != ' ' && buff[i] != '.' && word_start == -1) {
            word_start = i;
        }

        if ((buff[i] == ' ' || buff[i] == '.' || buff[i] == '\0') && word_start != -1) {
            int word_len = i - word_start;
            printf("%d. ", word_index++);
            for (int j = word_start; j < i; j++) {
                putchar(buff[j]);
            }
            printf(" (%d)\n", word_len);
            word_start = -1;
        }
    }
}




//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE? What if argv[1] does not exist?
    // ANSWER: This check makes sure that the program is being run with the correct number of arguments,
    // If `argc` is less than 2, it means `argv[1]` does not exist, and trying to access it would lead
    // to undefined behavior. By checking that `argc` is at least 2 and that the first argument starts 
    // with a '-' (indicating a valid option), we can safely proceed without causing errors or crashes.

    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    // ANS: This makes sure that the user will provide at least one additional argument 
    // after the option (argv[2]) to act as the input string. Without this check, 
    // the program would try to access argv[2], which may not exist, causing a 
    // segmentation fault. This step helps prevent that issue by ensuring the program has the necessary arguments before proceeding.


    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string


    // TODO #3: Allocate space for the buffer using malloc
    buff = (char *)malloc(BUFFER_SZ * sizeof(char));
    if (buff == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(99);
    }



    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            
            break;
        
        case 'r':
            reverse_string(buff, user_str_len);
            break;

        case 'w':
            word_print(buff, user_str_len);
            break;

        case 'x':
            if (argc != 5) {
                printf("Error: Insufficient arguments for -x option\n");
                exit(2); 
            }

            printf("Not Implemented!\n");
            break;  
        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting: DONE
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
    
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//     ANS: Providing both the pointer and the length to helper functions is a good practice because I feel that it adds flexibility,
//          safety, and clarity. While the buffer is 50 bytes in our case here, passing the length allows the code to handle different buffer sizes in the future.