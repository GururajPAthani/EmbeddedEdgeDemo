#include <stdio.h>
#include <ddsc/dds.h>
#include "MyData.h" // Auto-generated from MyData.idl

int main (void) {
    dds_entity_t participant = dds_create_participant (DDS_DOMAIN_DEFAULT, NULL, NULL);
    dds_entity_t topic = dds_create_topic (participant, &MyData_Msg_desc, "MyTopicName", NULL, NULL);
    dds_entity_t writer = dds_create_writer (participant, topic, NULL, NULL);

    MyData_Msg *msg = MyData_Msg__alloc();
    // ... logic to set message fields and write data ...

    MyData_Msg_free (msg, DDS_FREE_ALL);
    dds_delete (participant); // Cleans up topic and writer as well
    return 0;
}
