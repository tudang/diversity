#include <stdio.h>
#include "learner.h"

#define NUM_ACCEPTORS 3


int main() {
	int ret;
	struct learner *l = learner_new(NUM_ACCEPTORS);
	learner_set_instance_id(l, -1);

	char *s = (char *) "Hello";
	struct paxos_value v = { .paxos_value_len = 6, .paxos_value_val = s };
	struct paxos_accepted ack0 = { .iid = 0, .ballot = 1, .value_ballot = 1, .aid = 0, .value = v }; 
	struct paxos_accepted ack1 = { .iid = 0, .ballot = 1, .value_ballot = 1, .aid = 1, .value = v }; 
	struct paxos_accepted ack2 = { .iid = 0, .ballot = 1, .value_ballot = 1, .aid = 2, .value = v }; 

	struct paxos_accepted out;
	learner_receive_accepted(l, &ack0);
	ret = learner_deliver_next(l, &out);
	printf("Learner returns %d\n", ret);
	
	learner_receive_accepted(l, &ack1);
	ret = learner_deliver_next(l, &out);
	printf("Learner returns %d\n", ret);

	learner_receive_accepted(l, &ack2);
	ret = learner_deliver_next(l, &out);
	printf("Learner returns %d\n", ret);

	learner_free(l);
	return 0;
}
