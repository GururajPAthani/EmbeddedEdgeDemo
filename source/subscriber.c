#include <stdio.h>
#include <ddsc/dds.h>
#include <unistd.h> // For usleep
#include <stdlib.h> // For dds_fatal and EXIT_FAILURE
#include "MyData.h" // Auto-generated from MyData.idl

// FIX A: Add the necessary dds_fatal helper function definition
void dds_fatal(const char *msg) {
    fprintf(stderr, "FATAL ERROR: %s\n", msg);
    exit(EXIT_FAILURE);
}

int main (void) {
    dds_entity_t participant;
    dds_entity_t topic;
    dds_entity_t reader;
    
    // Create Participant, Topic, and Reader entities
    participant = dds_create_participant (DDS_DOMAIN_DEFAULT, NULL, NULL);
    if (participant < 0) dds_fatal("dds_create_participant failed");
    
    topic = dds_create_topic (participant, &MyData_Msg_desc, "MyTopicName", NULL, NULL);
    if (topic < 0) dds_fatal("dds_create_topic failed");
    
    reader = dds_create_reader (participant, topic, NULL, NULL);
    if (reader < 0) dds_fatal("dds_create_reader failed");
    
    printf("Subscriber started on topic 'MyTopicName'. Waiting for data...\n");

    // Arrays to hold the sample data and information
    void *samples[1];
    dds_sample_info_t info[1];
    dds_return_t rc;

    // Allocate memory for the sample array
    samples[0] = MyData_Msg__alloc();

    // Loop to continuously wait for and read messages
    while (1) {
        // Wait for max 1 second for a sample to arrive
        rc = dds_take(reader, samples, info, 1, 1);

        if (rc < 0) {
            dds_fatal("dds_take failed");
        }
        
        // Process received samples
        if (rc > 0) {
            for (int i = 0; i < rc; i++) {
                if (info[i].valid_data) {
                    MyData_Msg *msg = (MyData_Msg *)samples[i];
                    
                    // FIX D: Cast msg->userID to long for safe printing with %ld
                    printf("Received: UserID = %ld, Message = %s\n", (long)msg->userID, msg->message);
                    
                    // Exit loop after receiving the final message
                    if (msg->userID == 255) {
                        printf("Received final message (255). Exiting.\n");
                        goto cleanup;
                    }
                }
            }
        }
        
        // Wait briefly before checking again if no samples were received
        usleep(10000); 
    }

cleanup:
    // FIX B: Corrected free function name: MyData_Msg_free (one underscore)
    MyData_Msg_free(samples[0], DDS_FREE_ALL); 
    dds_delete (participant);
    return 0;
}