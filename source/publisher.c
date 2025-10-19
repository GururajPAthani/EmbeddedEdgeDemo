#include <stdio.h>
#include <ddsc/dds.h>
#include <unistd.h>     // For usleep
#include <string.h>     // For strncpy and snprintf
#include <stdlib.h>     // For dds_fatal (utility)
// #include <ddsc/dds_public_string.h> // Sometimes needed for dds_string_copy, but we removed it.
#include "MyData.h"     // Auto-generated from MyData.idl

// Helper function to print fatal errors (since dds_fatal may be missing)
void dds_fatal(const char *msg) {
    fprintf(stderr, "FATAL ERROR: %s\n", msg);
    exit(EXIT_FAILURE);
}

int main (void) {
    dds_entity_t participant;
    dds_entity_t topic;
    dds_entity_t writer;
    
    // Create Participant, Topic, and Writer entities
    participant = dds_create_participant (DDS_DOMAIN_DEFAULT, NULL, NULL);
    if (participant < 0) dds_fatal("dds_create_participant failed");
    
    topic = dds_create_topic (participant, &MyData_Msg_desc, "MyTopicName", NULL, NULL);
    if (topic < 0) dds_fatal("dds_create_topic failed");
    
    writer = dds_create_writer (participant, topic, NULL, NULL);
    if (writer < 0) dds_fatal("dds_create_writer failed");

    printf("Publisher started on topic 'MyTopicName'.\n");

    // Allocate the message sample
    // Using MyData_Msg_alloc() for fixed-size fields is typical
    MyData_Msg *msg = MyData_Msg__alloc(); 
    if (msg == NULL) dds_fatal("MyData_Msg__alloc failed");

    // Loop from 0 to 255
    for (long i = 0; i <= 255; i++) {
        // --- Logic to set message fields ---
        msg->userID = i;
        
        // 1. Prepare the string content
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "Message count: %ld", i);
        
        // 2. FIX: Use strncpy for fixed-size IDL strings
        // msg->message is an array, not a pointer, for string<256>
        strncpy(msg->message, buffer, sizeof(msg->message) - 1);
        msg->message[sizeof(msg->message) - 1] = '\0'; // Ensure null termination
        
        // --- Write the data sample ---
        dds_return_t rc = dds_write(writer, msg);
        if (rc < 0) dds_fatal("dds_write failed");

        // Use %ld and cast the long value for safe printing
        printf("Published: UserID = %ld, Message = %s\n", (long)msg->userID, msg->message);
        
        // Delay for a short period
        usleep(100000); 
    }

    printf("\nFinished publishing 256 messages. Cleaning up...\n");

    // Cleanup - Corrected free function name
    MyData_Msg_free (msg, DDS_FREE_ALL); 
    dds_delete (participant);
    return 0;
}