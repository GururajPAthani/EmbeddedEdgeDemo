#include <stdio.h>
#include <ddsc/dds.h>
#include "MyData.h" // Auto-generated from MyData.idl

int main (void) {
    dds_entity_t participant = dds_create_participant (DDS_DOMAIN_DEFAULT, NULL, NULL);
    dds_entity_t topic = dds_create_topic (participant, &MyData_Msg_desc, "MyTopicName", NULL, NULL);
    dds_entity_t reader = dds_create_reader (participant, topic, NULL, NULL);

    void *samples[1];
    dds_sample_info_t info[1];

    // ... logic for polling/waiting for data and reading ...

    dds_delete (participant);
    return 0;
}