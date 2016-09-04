#include <stdio.h>
#include <string.h>
#include "proposer.h"


void dump_paxos_message(paxos_message *m) {
	printf("{ .type = %d, .iid = %d, .ballot = %d, .value_ballot = %d, .value = { .len = %d, val = %s}}\n",
			m->type, m->u.accept.iid, m->u.accept.ballot, m->u.accept.value_ballot,
			m->u.accept.value.paxos_value_len,
			m->u.accept.value.paxos_value_val);
}

void dump_prepare_message(paxos_prepare *m) {
	printf("PREPARE: .iid = %d, .ballot = %d, .value_ballot = %d, .value = { .len = %d, val = %s}}\n",
			m->iid, m->ballot, m->value_ballot,
			m->value.paxos_value_len,
			m->value.paxos_value_val);
}

void dump_accept_message(paxos_accept *m) {
	printf("dump_accept_message\n");
	printf("ACCEPT: .iid = %d, .ballot = %d, .value_ballot = %d, .value = ",
			m->iid, m->ballot, m->value_ballot);
	if ((m->value.paxos_value_val) != NULL) {
		printf(" {.len=%d, val=%s}\n",
				m->value.paxos_value_len,
				m->value.paxos_value_val);
	} else printf("NULL }\n");
}

int main() {
	int ret;
	int id = 0;
	paxos_config.verbosity = PAXOS_LOG_DEBUG;
	int num_acceptors = 3;
	paxos_prepare pre;
	paxos_accept acpt;

	struct proposer *proposer = proposer_new(id, num_acceptors);

	char *s = (char *) "Hello";
	paxos_value v = { .paxos_value_len = 6, .paxos_value_val = s };
	
	paxos_promise promise = { .iid = 1, .ballot = 12, .value_ballot = 1, .aid = 0, .value = v };


	proposer_prepare(proposer, &pre);
	printf("proposer returns %d\n", ret);
	dump_prepare_message(&pre);
	memset(&pre, 0, sizeof(pre));

	ret = proposer_receive_promise(proposer, &promise, &pre);
	printf("proposer returns %d\n", ret);
	dump_prepare_message(&pre);

	ret = proposer_accept(proposer, &acpt);
	printf("proposer returns %d\n", ret);
	memset(&acpt, 0, sizeof(acpt));

	promise.aid = 2;
	ret = proposer_receive_promise(proposer, &promise, &pre);
	printf("proposer returns %d\n", ret);
	dump_prepare_message(&pre);

	ret = proposer_accept(proposer, &acpt);
	printf("proposer returns %d\n", ret);
	dump_accept_message(&acpt);
	memset(&acpt, 0, sizeof(acpt));

	proposer_free(proposer);
	return 0;
}
