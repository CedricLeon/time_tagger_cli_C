#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define CHANNELS 6                  // Number of input channels
#define EXPERIMENT_DURATION_S 10    // Duration of experiment, in seconds
#define TIME_WINDOW_MS 1000         // Length of time window, in milliseconds

///////////////////////////////////////////// STRUCTURES /////////////////////////////////////////////

// Node structure for a linked list
typedef struct Node {
    int time_code;                  // Time code of the event in milliseconds since the start of the experiment
    struct Node* next;
} Node;

// global variables
Node* buffer[CHANNELS];             // Array of linked lists, 1 list for each input channel
Node* last_node[CHANNELS];          // Array of last nodes in each list (for faster appending)
int coincidence_counter = 0;
int accident_counter = 0;

// Create a new node and initialize it with a given time code 
Node* create_node(int time_code) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->time_code = time_code;
    new_node->next = NULL;
    return new_node;
}

// Append a time code to the end corresponding channel linked list (Also keep track of the last node of each list)
void append_time_code(int channel, int time_code) {
    Node* new_node = create_node(time_code);
    if (buffer[channel] == NULL) {
        buffer[channel] = new_node;
    } else {
        last_node[channel]->next = new_node;
    }
    last_node[channel] = new_node;
}


void cleanup() {
    // Free all nodes in the buffer
    for (int i = 0; i < CHANNELS; i++) {
        Node* current = buffer[i];
        while (current != NULL) {
            Node* next = current->next;
            free(current);
            current = next;
        }
        // No need to free last_node[i] since it is a pointer to a node in buffer[i]
    }
}

///////////////////////////////////////////// TIME-TAGGER /////////////////////////////////////////////

void start_experiment() {
    // Record start time
    struct timeval start_time;
    gettimeofday(&start_time, NULL);
    printf("Experiment started: start_time = %ld.%06ld seconds since the Epoch.\n", 
           (long int)start_time.tv_sec, (long int)start_time.tv_usec);
    struct timeval current_time, elapsed_time;
    current_time = start_time;

    printf("Enter a channel numbers (0-%d) until the end of the experiment (When the time is over, you need to provide a last input that wont be recorded).\n", CHANNELS - 1);
    int channel;

    // While the experiment is running, listen for a signal
    while (1) {
        scanf("%d", &channel);
        // Get the elapsed time in milliseconds
        gettimeofday(&current_time, NULL);
        timersub(&current_time, &start_time, &elapsed_time);
        if (elapsed_time.tv_sec > EXPERIMENT_DURATION_S) {
            // If the experiment is over, dont record the last signal
            break;
        }

        if (channel >= 0 && channel < CHANNELS) {
            long int time_code = elapsed_time.tv_sec * 1000 + elapsed_time.tv_usec / 1000;
            printf("Channel %d: time_code = %ld.\n", channel, time_code, 
                   (long int)current_time.tv_sec, (long int)current_time.tv_usec);
            // Store the time code in the corresponding channel linked list
            append_time_code(channel, time_code);
        } else {
            printf("Invalid channel number, must be between 0 and %d.\n", CHANNELS - 1);
        }
    }
}

void end_experiment() {
    // Compare all time codes in the buffer
    for (int i = 0; i < CHANNELS; i++) {
        // Start with the first time code (node) of the channel
        Node* current_i = buffer[i];
        while (current_i != NULL) {
            int coincidence_count = 0;
            // For each of the other channels, check if there is a time code within the time window 
            for (int j = i + 1; j < CHANNELS; j++) {
                Node* current_j = buffer[j];
                // We check only until the end of the list or until the time code is outside the time window (since the list is "sorted")
                while (current_j != NULL && current_j->time_code <= current_i->time_code + TIME_WINDOW_MS) {
                    if (abs(current_j->time_code - current_i->time_code) <= TIME_WINDOW_MS) {
                        coincidence_count++;
                        if (coincidence_count == 1) {
                            printf("Coincidence: Channel %d (time code %d) and Channel %d (time code %d)\n", 
                                    i, current_i->time_code, j, current_j->time_code);
                        }else if (coincidence_count > 1) {
                            printf("/!\\ ACCIDENT /!\\ (Channel %d (time code %d) and Channel %d (time code %d)\n", 
                                    i, current_i->time_code, j, current_j->time_code);
                        }
                    }
                    current_j = current_j->next;
                }
            }
            if (coincidence_count == 1) {
                coincidence_counter++;
            } else if (coincidence_count > 1) { // If there are more than 1 coincidences, it is an accident
                accident_counter++;
            }
            // Move to the next time code (node) of the channel
            current_i = current_i->next;
        }
    }
}

void print_buffer() {
    // Find the maximum length of the linked lists
    int max_length = 0;
    for (int i = 0; i < CHANNELS; i++) {
        Node* current = buffer[i];
        int length = 0;
        while (current != NULL) {
            length++;
            current = current->next;
        }
        if (length > max_length) {
            max_length = length;
        }
    }

    // Print the channel numbers as a header row
    printf("\nAll the time codes:\n");
    for (int i = 0; i < CHANNELS; i++) {
        printf("%-10d ", i);
    }
    printf("\n");

    // Print the time codes in a matrix format
    for (int i = 0; i < max_length; i++) {
        for (int j = 0; j < CHANNELS; j++) {
            Node* current = buffer[j];
            int k = 0;
            while (k < i && current != NULL) {
                current = current->next;
                k++;
            }
            if (current != NULL) {
                printf("%-10d ", current->time_code);
            } else {
                printf("           "); // Print 10 spaces if there is no time code
            }
        }
        printf("\n\n");
    }
}

int main() {
    printf("Time Tagger with experiment duration %d seconds and time window %d milliseconds.\n", 
           EXPERIMENT_DURATION_S, TIME_WINDOW_MS);
    start_experiment();
    print_buffer();
    end_experiment();
    printf("Total number of coincidences: %d\n", coincidence_counter);
    printf("Total number of accidents: %d\n", accident_counter);
    cleanup();
    return 0;
}
