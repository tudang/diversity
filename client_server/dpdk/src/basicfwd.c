/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2015 Intel Corporation
 */

#include <stdint.h>
#include <inttypes.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <rte_arp.h>
#include <rte_icmp.h>
#include <rte_ip.h>
#include <rte_hexdump.h>

#define RX_RING_SIZE 1024
#define TX_RING_SIZE 1024

#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE 32

static const struct rte_eth_conf port_conf_default = {
	.rxmode = {
		.max_rx_pkt_len = ETHER_MAX_LEN,
		.ignore_offload_bitfield = 1,
	},
};

uint32_t my_ip = IPv4(192, 168, 10, 97);

/* basicfwd.c: Basic DPDK skeleton forwarding example. */

/*
 * Initializes a given port using global settings and with the RX buffers
 * coming from the mbuf_pool passed as a parameter.
 */
static inline int
port_init(uint16_t port, struct rte_mempool *mbuf_pool)
{
	struct rte_eth_conf port_conf = port_conf_default;
	const uint16_t rx_rings = 1, tx_rings = 1;
	uint16_t nb_rxd = RX_RING_SIZE;
	uint16_t nb_txd = TX_RING_SIZE;
	int retval;
	uint16_t q;
	struct rte_eth_dev_info dev_info;
	struct rte_eth_txconf txconf;

	if (!rte_eth_dev_is_valid_port(port))
		return -1;

	rte_eth_dev_info_get(port, &dev_info);
	if (dev_info.tx_offload_capa & DEV_TX_OFFLOAD_MBUF_FAST_FREE)
		port_conf.txmode.offloads |=
			DEV_TX_OFFLOAD_MBUF_FAST_FREE;

	/* Configure the Ethernet device. */
	retval = rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);
	if (retval != 0)
		return retval;

	retval = rte_eth_dev_adjust_nb_rx_tx_desc(port, &nb_rxd, &nb_txd);
	if (retval != 0)
		return retval;

	/* Allocate and set up 1 RX queue per Ethernet port. */
	for (q = 0; q < rx_rings; q++) {
		retval = rte_eth_rx_queue_setup(port, q, nb_rxd,
				rte_eth_dev_socket_id(port), NULL, mbuf_pool);
		if (retval < 0)
			return retval;
	}

	txconf = dev_info.default_txconf;
	txconf.txq_flags = ETH_TXQ_FLAGS_IGNORE;
	txconf.offloads = port_conf.txmode.offloads;
	/* Allocate and set up 1 TX queue per Ethernet port. */
	for (q = 0; q < tx_rings; q++) {
		retval = rte_eth_tx_queue_setup(port, q, nb_txd,
				rte_eth_dev_socket_id(port), &txconf);
		if (retval < 0)
			return retval;
	}

	/* Start the Ethernet port. */
	retval = rte_eth_dev_start(port);
	if (retval < 0)
		return retval;

	/* Display the port MAC address. */
	struct ether_addr addr;
	rte_eth_macaddr_get(port, &addr);
	printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8
			   " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
			port,
			addr.addr_bytes[0], addr.addr_bytes[1],
			addr.addr_bytes[2], addr.addr_bytes[3],
			addr.addr_bytes[4], addr.addr_bytes[5]);

	/* Enable RX in promiscuous mode for the Ethernet device. */
	rte_eth_promiscuous_enable(port);

	return 0;
}

static void packet_handler(struct rte_mbuf *pkt_in, uint16_t port)
{
	struct ether_hdr *eth_hdr = rte_pktmbuf_mtod_offset(pkt_in, struct ether_hdr *, 0);
	uint16_t offset = sizeof (struct ether_hdr);
	rte_hexdump(stdout, "struct ether", eth_hdr, offset);
	if (rte_be_to_cpu_16(eth_hdr->ether_type) == ETHER_TYPE_ARP) {
		struct ether_addr d_addr;
		struct arp_hdr *arp_hdr = rte_pktmbuf_mtod_offset(pkt_in, struct arp_hdr *, offset);
		if (arp_hdr->arp_data.arp_tip == my_ip) {
			if (arp_hdr->arp_op == rte_cpu_to_be_16(ARP_OP_REQUEST)) {
				arp_hdr->arp_op = rte_cpu_to_be_16(ARP_OP_REPLY);
				/* Switch src and dst data and set bonding MAC */
				ether_addr_copy(&eth_hdr->s_addr, &eth_hdr->d_addr);
				rte_eth_macaddr_get(port, &eth_hdr->s_addr);
				ether_addr_copy(&arp_hdr->arp_data.arp_sha, &arp_hdr->arp_data.arp_tha);
				arp_hdr->arp_data.arp_tip = arp_hdr->arp_data.arp_sip;
				rte_eth_macaddr_get(port, &d_addr);
				ether_addr_copy(&d_addr, &arp_hdr->arp_data.arp_sha);
				arp_hdr->arp_data.arp_sip = my_ip;
			}
		}
	}
	else if (rte_be_to_cpu_16(eth_hdr->ether_type) == ETHER_TYPE_IPv4)
	{
		struct ipv4_hdr *ip = rte_pktmbuf_mtod_offset(pkt_in, struct ipv4_hdr *, offset);
		offset += sizeof (struct ipv4_hdr);
		if (ip->next_proto_id == IPPROTO_ICMP) {
			struct icmp_hdr *icmp = rte_pktmbuf_mtod_offset(pkt_in, struct icmp_hdr *, offset);
			if (icmp->icmp_type == IP_ICMP_ECHO_REQUEST) {
				icmp->icmp_type = IP_ICMP_ECHO_REPLY;
				/* Update ICMP checksum */
				icmp->icmp_cksum = icmp->icmp_cksum + (IP_ICMP_ECHO_REQUEST << 8) + ~(IP_ICMP_ECHO_REPLY << 8);
			}
		}
	}
}

static void rx_handler(struct rte_mbuf **bufs, uint16_t nb_rx, uint16_t port)
{
	uint16_t i;
	for (i = 0; i < nb_rx; i++)
	{
		packet_handler(bufs[i], port);
	}
}

/*
 * The lcore main. This is the main thread that does the work, reading from
 * an input port and writing to an output port.
 */
static __attribute__((noreturn)) void
lcore_main(void)
{
	uint16_t port;

	/*
	 * Check that the port is on the same NUMA node as the polling thread
	 * for best performance.
	 */
	RTE_ETH_FOREACH_DEV(port)
		if (rte_eth_dev_socket_id(port) > 0 &&
				rte_eth_dev_socket_id(port) !=
						(int)rte_socket_id())
			printf("WARNING, port %u is on remote NUMA node to "
					"polling thread.\n\tPerformance will "
					"not be optimal.\n", port);

	printf("\nCore %u forwarding packets. [Ctrl+C to quit]\n",
			rte_lcore_id());

	/* Run until the application is quit or killed. */
	for (;;) {
		/*
		 * Receive packets on a port and forward them back
		 */
		RTE_ETH_FOREACH_DEV(port) {

			/* Get burst of RX packets. */
			struct rte_mbuf *bufs[BURST_SIZE];
			const uint16_t nb_rx = rte_eth_rx_burst(port, 0,
					bufs, BURST_SIZE);

			rx_handler(bufs, nb_rx, port);

			if (unlikely(nb_rx == 0))
				continue;

			/* Send burst of TX packets. */
			const uint16_t nb_tx = rte_eth_tx_burst(port, 0,
					bufs, nb_rx);

			/* Free any unsent packets. */
			if (unlikely(nb_tx < nb_rx)) {
				uint16_t buf;
				for (buf = nb_tx; buf < nb_rx; buf++)
					rte_pktmbuf_free(bufs[buf]);
			}
		}
	}
}

/*
 * The main function, which does initialization and calls the per-lcore
 * functions.
 */
int
main(int argc, char *argv[])
{
	struct rte_mempool *mbuf_pool;
	unsigned nb_ports;
	uint16_t portid;

	/* Initialize the Environment Abstraction Layer (EAL). */
	int ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "Error with EAL initialization\n");

	argc -= ret;
	argv += ret;

	/* Check that there is an even number of ports to send/receive on. */
	nb_ports = rte_eth_dev_count_avail();
	if (nb_ports < 1)
		rte_exit(EXIT_FAILURE, "Error: number of ports must be greater than 0\n");

	/* Creates a new mempool in memory to hold the mbufs. */
	mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", NUM_MBUFS * nb_ports,
		MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());

	if (mbuf_pool == NULL)
		rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");

	/* Initialize all ports. */
	RTE_ETH_FOREACH_DEV(portid)
		if (port_init(portid, mbuf_pool) != 0)
			rte_exit(EXIT_FAILURE, "Cannot init port %"PRIu16 "\n",
					portid);

	if (rte_lcore_count() > 1)
		printf("\nWARNING: Too many lcores enabled. Only 1 used.\n");

	/* Call lcore_main on the master core only. */
	lcore_main();

	return 0;
}
