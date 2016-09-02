#include <stdio.h>
#include "acceptor.h"

void dump_paxos_message(paxos_message *m) {
	printf("{ .type = %d, .iid = %d, .ballot = %d, .value_ballot = %d, .value = { .len = %d, val = %s}}\n",
			m->type, m->u.accept.iid, m->u.accept.ballot, m->u.accept.value_ballot,
			m->u.accept.value.paxos_value_len,
			m->u.accept.value.paxos_value_val);
}

int main() {
	int ret;
	int id = 2;
	struct acceptor *acc = acceptor_new(id);

	char *s = (char *) "Hello";
	paxos_value v = { .paxos_value_len = 6, .paxos_value_val = s };
	
	paxos_prepare pre = { .iid = 1, .ballot = 2, .value_ballot = 0, .aid = 0, .value = {0, NULL} }; 
	paxos_accept ack = { .iid = 1, .ballot = 2, .value_ballot = 1, .aid = 0, .value = v }; 
	paxos_prepare old_pre = { .iid = 1, .ballot = 1, .value_ballot = 0, .aid = 0, .value = {0, NULL} }; 
	paxos_accept old_ack = { .iid = 1, .ballot = 1, .value_ballot = 0, .aid = 0, .value = {0, NULL} }; 

	paxos_message out;

	ret = acceptor_receive_prepare(acc, &pre, &out);
	printf("acceptor returns %d\n", ret);
	dump_paxos_message(&out);
	ret = acceptor_receive_accept(acc, &ack, &out);
	printf("acceptor returns %d\n", ret);
	dump_paxos_message(&out);
	ret = acceptor_receive_prepare(acc, &old_pre, &out);
	printf("acceptor returns %d\n", ret);
	dump_paxos_message(&out);
	ret = acceptor_receive_accept(acc, &old_ack, &out);
	printf("acceptor returns %d\n", ret);
	dump_paxos_message(&out);

	acceptor_free(acc);
	return 0;
}
