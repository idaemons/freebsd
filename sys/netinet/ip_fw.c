/*
 * Copyright (c) 1996 Alex Nash
 * Copyright (c) 1993 Daniel Boulet
 * Copyright (c) 1994 Ugen J.S.Antsilevich
 *
 * Redistribution and use in source forms, with and without modification,
 * are permitted provided that this entire comment appears intact.
 *
 * Redistribution in binary form may occur without any restrictions.
 * Obviously, it would be nice if you gave credit where credit is due
 * but requiring it would be too onerous.
 *
 * This software is provided ``AS IS'' without any warranties of any kind.
 *
 *	$Id: ip_fw.c,v 1.51 1996/10/12 19:49:36 bde Exp $
 */

/*
 * Implement IP packet firewall
 */

#ifndef IPFIREWALL_MODULE
#include "opt_ipfw.h"
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_var.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip_fw.h>

static int fw_debug = 1;
#ifdef IPFIREWALL_VERBOSE
static int fw_verbose = 1;
#else
static int fw_verbose = 0;
#endif
#ifdef IPFIREWALL_VERBOSE_LIMIT
static int fw_verbose_limit = IPFIREWALL_VERBOSE_LIMIT;
#else
static int fw_verbose_limit = 0;
#endif

LIST_HEAD (ip_fw_head, ip_fw_chain) ip_fw_chain;

#ifdef SYSCTL_NODE
SYSCTL_NODE(_net_inet_ip, OID_AUTO, fw, CTLFLAG_RW, 0, "Firewall");
SYSCTL_INT(_net_inet_ip_fw, OID_AUTO, debug, CTLFLAG_RW, &fw_debug, 0, "");
SYSCTL_INT(_net_inet_ip_fw, OID_AUTO, verbose, CTLFLAG_RW, &fw_verbose, 0, "");
SYSCTL_INT(_net_inet_ip_fw, OID_AUTO, verbose_limit, CTLFLAG_RW, &fw_verbose_limit, 0, "");
#endif

#define dprintf(a)	if (!fw_debug); else printf a

#define print_ip(a)	 printf("%ld.%ld.%ld.%ld",(ntohl(a.s_addr)>>24)&0xFF,\
				 		  (ntohl(a.s_addr)>>16)&0xFF,\
						  (ntohl(a.s_addr)>>8)&0xFF,\
						  (ntohl(a.s_addr))&0xFF);

#define dprint_ip(a)	if (!fw_debug); else print_ip(a)

static int	add_entry __P((struct ip_fw_head *chainptr, struct ip_fw *frwl));
static int	del_entry __P((struct ip_fw_head *chainptr, struct ip_fw *frwl));
static int	zero_entry __P((struct mbuf *m));
static struct ip_fw *
		check_ipfw_struct __P(( struct mbuf *m));
static int	ipopts_match __P((struct ip *ip, struct ip_fw *f));
static int	port_match __P((u_short *portptr, int nports, u_short port,
				int range_flag));
static int	tcpflg_match __P((struct tcphdr *tcp, struct ip_fw *f));
static int	icmptype_match __P((struct icmp *  icmp, struct ip_fw * f));
static void	ipfw_report __P((char *txt, int rule, struct ip *ip, int counter, struct ifnet *rif));

#ifdef IPFIREWALL_MODULE
static ip_fw_chk_t *old_chk_ptr;
static ip_fw_ctl_t *old_ctl_ptr;
#endif

static int	ip_fw_chk __P((struct ip **pip, int hlen, struct ifnet *rif,
			       int dirport, struct mbuf **m));
static int	ip_fw_ctl __P((int stage, struct mbuf **mm));

static char err_prefix[] = "ip_fw_ctl:";

/*
 * Returns 1 if the port is matched by the vector, 0 otherwise
 */
static inline int 
port_match(u_short *portptr, int nports, u_short port, int range_flag)
{
	if (!nports)
		return 1;
	if (range_flag) {
		if (portptr[0] <= port && port <= portptr[1]) {
			return 1;
		}
		nports -= 2;
		portptr += 2;
	}
	while (nports-- > 0) {
		if (*portptr++ == port) {
			return 1;
		}
	}
	return 0;
}

static int
tcpflg_match(struct tcphdr *tcp, struct ip_fw *f)
{
	u_char		flg_set, flg_clr;
	
	if ((f->fw_tcpf & IP_FW_TCPF_ESTAB) &&
	    (tcp->th_flags & (IP_FW_TCPF_RST | IP_FW_TCPF_ACK)))
		return 1;

	flg_set = tcp->th_flags & f->fw_tcpf;
	flg_clr = tcp->th_flags & f->fw_tcpnf;

	if (flg_set != f->fw_tcpf)
		return 0;
	if (flg_clr)
		return 0;

	return 1;
}

static int
icmptype_match(struct icmp *icmp, struct ip_fw *f)
{
	int type;

	if (!(f->fw_flg & IP_FW_F_ICMPBIT))
		return(1);

	type = icmp->icmp_type;

	/* check for matching type in the bitmap */
	if (f->fw_icmptypes[type / (sizeof(unsigned) * 8)] & 
		(1U << (type % (8 * sizeof(unsigned)))))
		return(1);

	return(0); /* no match */
}

static int
ipopts_match(struct ip *ip, struct ip_fw *f)
{
	register u_char *cp;
	int opt, optlen, cnt;
	u_char	opts, nopts, nopts_sve;

	cp = (u_char *)(ip + 1);
	cnt = (ip->ip_hl << 2) - sizeof (struct ip);
	opts = f->fw_ipopt;
	nopts = nopts_sve = f->fw_ipnopt;

	for (; cnt > 0; cnt -= optlen, cp += optlen) {
		opt = cp[IPOPT_OPTVAL];
		if (opt == IPOPT_EOL)
			break;
		if (opt == IPOPT_NOP)
			optlen = 1;
		else {
			optlen = cp[IPOPT_OLEN];
			if (optlen <= 0 || optlen > cnt) {
				return 0; /*XXX*/
			}
		}
		switch (opt) {

		default:
			break;

		case IPOPT_LSRR:
			opts &= ~IP_FW_IPOPT_LSRR;
			nopts &= ~IP_FW_IPOPT_LSRR;
			break;

		case IPOPT_SSRR:
			opts &= ~IP_FW_IPOPT_SSRR;
			nopts &= ~IP_FW_IPOPT_SSRR;
			break;

		case IPOPT_RR:
			opts &= ~IP_FW_IPOPT_RR;
			nopts &= ~IP_FW_IPOPT_RR;
			break;
		case IPOPT_TS:
			opts &= ~IP_FW_IPOPT_TS;
			nopts &= ~IP_FW_IPOPT_TS;
			break;
		}
		if (opts == nopts)
			break;
	}
	if (opts == 0 && nopts == nopts_sve)
		return 1;
	else
		return 0;
}

static void
ipfw_report(char *txt, int rule, struct ip *ip, int counter, struct ifnet *rif)
{
	struct tcphdr *tcp = (struct tcphdr *) ((u_long *) ip + ip->ip_hl);
	struct udphdr *udp = (struct udphdr *) ((u_long *) ip + ip->ip_hl);
	struct icmp *icmp = (struct icmp *) ((u_long *) ip + ip->ip_hl);
	if (!fw_verbose)
		return;
	if (fw_verbose_limit != 0 && counter > fw_verbose_limit)
		return;
	printf("ipfw: %d %s ",rule, txt);
	switch (ip->ip_p) {
	case IPPROTO_TCP:
		printf("TCP ");
		print_ip(ip->ip_src);
		printf(":%d ", ntohs(tcp->th_sport));
		print_ip(ip->ip_dst);
		printf(":%d", ntohs(tcp->th_dport));
		break;
	case IPPROTO_UDP:
		printf("UDP ");
		print_ip(ip->ip_src);
		printf(":%d ", ntohs(udp->uh_sport));
		print_ip(ip->ip_dst);
		printf(":%d", ntohs(udp->uh_dport));
		break;
	case IPPROTO_ICMP:
		printf("ICMP:%u.%u ", icmp->icmp_type, icmp->icmp_code);
		print_ip(ip->ip_src);
		printf(" ");
		print_ip(ip->ip_dst);
		break;
	default:
		printf("P:%d ", ip->ip_p);
		print_ip(ip->ip_src);
		printf(" ");
		print_ip(ip->ip_dst);
		break;
	}
	printf(" via %s%d", rif->if_name, rif->if_unit);
	if ((ip->ip_off & IP_OFFMASK)) 
		printf(" Fragment = %d",ip->ip_off & IP_OFFMASK);
	printf("\n");
	if (fw_verbose_limit != 0 && counter == fw_verbose_limit)
		printf("ipfw: limit reached on rule #%d\n", rule);
}

/*
 * We overload the "dirport" parameter:
 *
 *   If dirport is negative, packet is outgoing; otherwise incoming.
 *   The low order 16 bits of dirport, if non-zero, indicate that
 *   we should ignore all ``divert <port>'' rules, where <port> is
 *   the low order 16 bits.
 *
 * Return value:
 *
 *   -1		The packet was denied/rejected and has been dropped 
 *    0		The packet is to be accepted; route normally
 *   <port>	Divert the packet to divert <port>, if any socket
 *		is bound to it; otherwise just drop it.
 */

static int 
ip_fw_chk(struct ip **pip, int hlen,
	struct ifnet *rif, int dirport, struct mbuf **m)
{
	struct ip_fw_chain *chain;
	register struct ip_fw *f = NULL;
	struct ip *ip = *pip;
	struct tcphdr *tcp = (struct tcphdr *) ((u_long *) ip + ip->ip_hl);
	struct udphdr *udp = (struct udphdr *) ((u_long *) ip + ip->ip_hl);
	struct icmp *icmp = (struct icmp *) ((u_long *) ip + ip->ip_hl);
	struct ifaddr *ia = NULL, *ia_p;
	struct in_addr src, dst, ia_i;
	u_short src_port, dst_port, offset;

	src = ip->ip_src;
	dst = ip->ip_dst;

	/*
	 * If we got interface from which packet came-store pointer to it's
	 * first adress
	 */
	if (rif != NULL)
		ia = rif->if_addrlist;

	/*
	 * Go down the chain, looking for enlightment
	 */
	for (chain=ip_fw_chain.lh_first; chain; chain = chain->chain.le_next) {
		f = chain->rule;

		/* Check direction inbound */
		if (dirport >= 0 && !(f->fw_flg & IP_FW_F_IN))
			continue;

		/* Check direction outbound */
		if (dirport < 0 && !(f->fw_flg & IP_FW_F_OUT))
			continue;

		/* Fragments */
		if ((f->fw_flg & IP_FW_F_FRAG) && !(ip->ip_off & IP_OFFMASK))
			continue;

		/* If src-addr doesn't match, not this rule. */
		if ((f->fw_flg & IP_FW_F_INVSRC) != 0
		  ^ (src.s_addr & f->fw_smsk.s_addr) != f->fw_src.s_addr)
			continue;

		/* If dest-addr doesn't match, not this rule. */
		if ((f->fw_flg & IP_FW_F_INVDST) != 0
		  ^ (dst.s_addr & f->fw_dmsk.s_addr) != f->fw_dst.s_addr)
			continue;

		/* If a i/f name was specified, and we don't know */
		if ((f->fw_flg & IP_FW_F_IFNAME) && !rif)
			continue;

		/* If a i/f name was specified, check it */
		if ((f->fw_flg & IP_FW_F_IFNAME) && f->fw_via_name[0]) {

			/* Not same unit, don't match */
			if (!(f->fw_flg & IP_FW_F_IFUWILD) && rif->if_unit != f->fw_via_unit)
				continue;

			/* Not same name */
			if (strncmp(rif->if_name, f->fw_via_name, FW_IFNLEN))
				continue;
		}

		/* If a i/f addr was specified, check it */
		if (!(f->fw_flg & IP_FW_F_IFNAME) && f->fw_via_ip.s_addr) {
			int match = 0;

			for (ia_p = ia; ia_p != NULL; ia_p = ia_p->ifa_next) {
				if ((ia_p->ifa_addr == NULL))
					continue;
				if (ia_p->ifa_addr->sa_family != AF_INET)
					continue;
				ia_i.s_addr =
				    ((struct sockaddr_in *)
				    (ia_p->ifa_addr))->sin_addr.s_addr;
				if (ia_i.s_addr != f->fw_via_ip.s_addr)
					continue;
				match = 1;
				break;
			}
			if (!match)
				continue;
		}

		/* 
		 * Check IP options
		 */
		if (f->fw_ipopt != f->fw_ipnopt)
			if (!ipopts_match(ip, f))
				continue;
			
		/* If wildcard, match */
		if (f->fw_prot == IPPROTO_IP)
			goto got_match;

		/* If different, dont match */
		if (ip->ip_p != f->fw_prot) 
			continue;

		switch (ip->ip_p) {
		case IPPROTO_TCP:
			offset = ip->ip_off & IP_OFFMASK;
			if (offset == 1) {
				static int frag_counter = 0;
				++frag_counter;
				ipfw_report("Refuse", -1, ip, frag_counter, rif);
				m_freem(*m);
				return -1;
			}
			if ((offset == 0) &&
			    (f->fw_tcpf != f->fw_tcpnf) &&
			    !tcpflg_match(tcp, f))
				continue;

			src_port = ntohs(tcp->th_sport);
			dst_port = ntohs(tcp->th_dport);
			goto check_ports;

		case IPPROTO_UDP:
			src_port = ntohs(udp->uh_sport);
			dst_port = ntohs(udp->uh_dport);

check_ports:
			if (!port_match(&f->fw_pts[0], f->fw_nsp,
					src_port, f->fw_flg & IP_FW_F_SRNG))
				continue;
			if (!port_match(&f->fw_pts[f->fw_nsp], f->fw_ndp,
					dst_port, f->fw_flg & IP_FW_F_DRNG)) 
				continue;
			break;

		case IPPROTO_ICMP:
			if (!icmptype_match(icmp, f))
				continue;
			goto got_match;

		default:
			break;
		}

got_match:
		f->fw_pcnt++;
		f->fw_bcnt+=ip->ip_len;
		f->timestamp = time.tv_sec;
		if (f->fw_flg & IP_FW_F_PRN) {
			if ((f->fw_flg & IP_FW_F_COMMAND) == IP_FW_F_ACCEPT) {
				ipfw_report("Allow",
					f->fw_number, ip, f->fw_pcnt, rif);
			} else if ((f->fw_flg & IP_FW_F_COMMAND)
			    == IP_FW_F_DIVERT) {
				if (f->fw_divert_port != (dirport & 0xffff))
					ipfw_report("Divert", f->fw_number,
							ip, f->fw_pcnt, rif);
			} else if ((f->fw_flg & IP_FW_F_COMMAND)
			    == IP_FW_F_COUNT) {
				ipfw_report("Count",
					f->fw_number, ip, f->fw_pcnt, rif);
			} else {
				ipfw_report("Deny",
					f->fw_number, ip, f->fw_pcnt, rif);
			}
		}

		/* Take appropriate action */
		if ((f->fw_flg & IP_FW_F_COMMAND) == IP_FW_F_ACCEPT) {
			return 0;
		} else if ((f->fw_flg & IP_FW_F_COMMAND) == IP_FW_F_COUNT) {
			continue;
		} else if ((f->fw_flg & IP_FW_F_COMMAND) == IP_FW_F_DIVERT) {
			if (f->fw_divert_port == (dirport & 0xffff))
				continue;	/* ignore this rule */
			return (f->fw_divert_port);
		} else
			break;		/* ie, deny/reject */
	}

#ifdef DIAGNOSTIC
	if (!chain)			/* rule 65535 should always be there */
		panic("ip_fw: chain");
	if (!f)	
		panic("ip_fw: entry");
#endif

	/*
	 * At this point, we're going to drop the packet.
	 * Send an ICMP only if all of the following are true:
	 *
	 * - The packet is an incoming packet
	 * - The packet matched a deny rule
	 * - The packet is not an ICMP packet
	 * - The rule has the special ICMP reply flag set
	 */
	if (dirport >= 0
	    && (f->fw_flg & IP_FW_F_COMMAND) == IP_FW_F_DENY
	    && (ip->ip_p != IPPROTO_ICMP)
	    && (f->fw_flg & IP_FW_F_ICMPRPL)) {
		icmp_error(*m, ICMP_UNREACH, ICMP_UNREACH_PORT, 0L, 0);
		return -1;
	}
	m_freem(*m);
	return -1;
}

static int
add_entry(struct ip_fw_head *chainptr, struct ip_fw *frwl)
{
	struct ip_fw *ftmp = 0;
	struct ip_fw_chain *fwc = 0, *fcp, *fcpl = 0;
	u_short nbr = 0;
	int s;

	fwc = malloc(sizeof *fwc, M_IPFW, M_DONTWAIT);
	ftmp = malloc(sizeof *ftmp, M_IPFW, M_DONTWAIT);
	if (!fwc || !ftmp) {
		dprintf(("%s malloc said no\n", err_prefix));
		if (fwc)  free(fwc, M_IPFW);
		if (ftmp) free(ftmp, M_IPFW);
		return (ENOSPC);
	}

	bcopy(frwl, ftmp, sizeof(struct ip_fw));
	ftmp->fw_pcnt = 0L;
	ftmp->fw_bcnt = 0L;
	fwc->rule = ftmp;
	
	s = splnet();

	if (!chainptr->lh_first) {
		LIST_INSERT_HEAD(chainptr, fwc, chain);
		splx(s);
		return(0);
        } else if (ftmp->fw_number == (u_short)-1) {
		if (fwc)  free(fwc, M_IPFW);
		if (ftmp) free(ftmp, M_IPFW);
		splx(s);
		return (EINVAL);
        }

	/* If entry number is 0, find highest numbered rule and add 100 */
	if (ftmp->fw_number == 0) {
		for (fcp = chainptr->lh_first; fcp; fcp = fcp->chain.le_next) {
			if (fcp->rule->fw_number != (u_short)-1)
				nbr = fcp->rule->fw_number;
			else
				break;
		}
		if (nbr < (u_short)-1 - 100)
			nbr += 100;
		ftmp->fw_number = nbr;
	}

	/* Got a valid number; now insert it, keeping the list ordered */
	for (fcp = chainptr->lh_first; fcp; fcp = fcp->chain.le_next) {
		if (fcp->rule->fw_number > ftmp->fw_number) {
			if (fcpl) {
				LIST_INSERT_AFTER(fcpl, fwc, chain);
			} else {
				LIST_INSERT_HEAD(chainptr, fwc, chain);
			}
			break;
		} else {
			fcpl = fcp;
		}
	}

	splx(s);
	return (0);
}

static int
del_entry(struct ip_fw_head *chainptr, struct ip_fw *frwl)
{
	struct ip_fw_chain *fcp;
	int s;

	s = splnet();

	fcp = chainptr->lh_first; 
	if (frwl->fw_number != (u_short)-1) {
		for (; fcp; fcp = fcp->chain.le_next) {
			if (fcp->rule->fw_number == frwl->fw_number) {
				LIST_REMOVE(fcp, chain);
				splx(s);
				free(fcp->rule, M_IPFW);
				free(fcp, M_IPFW);
				return 0;
			}
		}
	}

	splx(s);
	return (EINVAL);
}

static int
zero_entry(struct mbuf *m)
{
	struct ip_fw *frwl;
	struct ip_fw_chain *fcp;
	int s;

	if (m) {
		frwl = check_ipfw_struct(m);

		if (!frwl)
			return(EINVAL);
	}
	else
		frwl = NULL;

	/*
	 *	It's possible to insert multiple chain entries with the
	 *	same number, so we don't stop after finding the first
	 *	match if zeroing a specific entry.
	 */
	s = splnet();
	for (fcp = ip_fw_chain.lh_first; fcp; fcp = fcp->chain.le_next)
		if (!frwl || frwl->fw_number == fcp->rule->fw_number) {
			fcp->rule->fw_bcnt = fcp->rule->fw_pcnt = 0;
			fcp->rule->timestamp = 0;
		}
	splx(s);

	if ( frwl )
		printf("ipfw: Entry %d cleared.\n", frwl->fw_number);
	else
		printf("ipfw: Accounting cleared.\n");
	return(0);
}

static struct ip_fw *
check_ipfw_struct(struct mbuf *m)
{
	struct ip_fw *frwl;

	if (m->m_len != sizeof(struct ip_fw)) {
		dprintf(("%s len=%d, want %d\n", err_prefix, m->m_len,
		    sizeof(struct ip_fw)));
		return (NULL);
	}
	frwl = mtod(m, struct ip_fw *);

	if ((frwl->fw_flg & ~IP_FW_F_MASK) != 0) {
		dprintf(("%s undefined flag bits set (flags=%x)\n",
		    err_prefix, frwl->fw_flg));
		return (NULL);
	}

	/* If neither In nor Out, then both */
	if (!(frwl->fw_flg & (IP_FW_F_IN | IP_FW_F_OUT)))
		frwl->fw_flg |= IP_FW_F_IN | IP_FW_F_OUT;

	if ((frwl->fw_flg & IP_FW_F_SRNG) && frwl->fw_nsp < 2) {
		dprintf(("%s src range set but n_src_p=%d\n",
		    err_prefix, frwl->fw_nsp));
		return (NULL);
	}
	if ((frwl->fw_flg & IP_FW_F_DRNG) && frwl->fw_ndp < 2) {
		dprintf(("%s dst range set but n_dst_p=%d\n",
		    err_prefix, frwl->fw_ndp));
		return (NULL);
	}
	if (frwl->fw_nsp + frwl->fw_ndp > IP_FW_MAX_PORTS) {
		dprintf(("%s too many ports (%d+%d)\n",
		    err_prefix, frwl->fw_nsp, frwl->fw_ndp));
		return (NULL);
	}
	/*
	 *	ICMP protocol doesn't use port range
	 */
	if ((frwl->fw_prot != IPPROTO_TCP) &&
	    (frwl->fw_prot != IPPROTO_UDP) &&
	    (frwl->fw_nsp || frwl->fw_ndp)) {
		dprintf(("%s port(s) specified for non TCP/UDP rule\n",
		    err_prefix));
		return(NULL);
	}

	/*
	 *	Rather than modify the entry to make such entries work, 
	 *	we reject this rule and require user level utilities
	 *	to enforce whatever policy they deem appropriate.
	 */
	if ((frwl->fw_src.s_addr & (~frwl->fw_smsk.s_addr)) || 
		(frwl->fw_dst.s_addr & (~frwl->fw_dmsk.s_addr))) {
		dprintf(("%s rule never matches\n", err_prefix));
		return(NULL);
	}

	/* Diverting to port zero is illegal */
	if ((frwl->fw_flg & IP_FW_F_COMMAND) == IP_FW_F_DIVERT
	     && frwl->fw_divert_port == 0) {
		dprintf(("ip_fw_ctl: can't divert to port 0\n"));
		return (NULL);
	}
	return frwl;
}

static int
ip_fw_ctl(int stage, struct mbuf **mm)
{
	int error;
	struct mbuf *m;

	if (stage == IP_FW_GET) {
		struct ip_fw_chain *fcp = ip_fw_chain.lh_first;
		*mm = m = m_get(M_WAIT, MT_SOOPTS);
		for (; fcp; fcp = fcp->chain.le_next) {
			memcpy(m->m_data, fcp->rule, sizeof *(fcp->rule));
			m->m_len = sizeof *(fcp->rule);
			m->m_next = m_get(M_WAIT, MT_SOOPTS);
			m = m->m_next;
			m->m_len = 0;
		}
		return (0);
	}
	m = *mm;
	/* only allow get calls if secure mode > 2 */
	if (securelevel > 2) {
		if (m) (void)m_free(m);
		return(EPERM);
	}
	if (stage == IP_FW_FLUSH) {
		while (ip_fw_chain.lh_first != NULL && 
		    ip_fw_chain.lh_first->rule->fw_number != (u_short)-1) {
			struct ip_fw_chain *fcp = ip_fw_chain.lh_first;
			int s = splnet();
			LIST_REMOVE(ip_fw_chain.lh_first, chain);
			splx(s);
			free(fcp->rule, M_IPFW);
			free(fcp, M_IPFW);
		}
		if (m) (void)m_free(m);
		return (0);
	}
	if (stage == IP_FW_ZERO) {
		error = zero_entry(m);
		if (m) (void)m_free(m);
		return (error);
	}
	if (m == NULL) {
		printf("%s NULL mbuf ptr\n", err_prefix);
		return (EINVAL);
	}

	if (stage == IP_FW_ADD || stage == IP_FW_DEL) {
		struct ip_fw *frwl = check_ipfw_struct(m);

		if (!frwl) {
			if (m) (void)m_free(m);
			return (EINVAL);
		}

		if (stage == IP_FW_ADD)
			error = add_entry(&ip_fw_chain, frwl);
		else
			error = del_entry(&ip_fw_chain, frwl);
		if (m) (void)m_free(m);
		return error;
	}
	dprintf(("%s unknown request %d\n", err_prefix, stage));
	if (m) (void)m_free(m);
	return (EINVAL);
}

void
ip_fw_init(void)
{
	struct ip_fw deny;

	ip_fw_chk_ptr = ip_fw_chk;
	ip_fw_ctl_ptr = ip_fw_ctl;
	LIST_INIT(&ip_fw_chain);

	bzero(&deny, sizeof deny);
	deny.fw_prot = IPPROTO_IP;
	deny.fw_number = (u_short)-1;
	add_entry(&ip_fw_chain, &deny);

	printf("IP packet filtering initialized, "
#ifdef IPDIVERT
		"divert enabled, ");
#else
		"divert disabled, ");
#endif
#ifndef IPFIREWALL_VERBOSE
	printf("logging disabled\n");
#else
	if (fw_verbose_limit == 0)
		printf("unlimited logging\n");
	else
		printf("logging limited to %d packets/entry\n",
		    fw_verbose_limit);
#endif
}

#ifdef IPFIREWALL_MODULE

#include <sys/exec.h>
#include <sys/sysent.h>
#include <sys/lkm.h>

MOD_MISC(ipfw);

static int
ipfw_load(struct lkm_table *lkmtp, int cmd)
{
	int s=splnet();

	old_chk_ptr = ip_fw_chk_ptr;
	old_ctl_ptr = ip_fw_ctl_ptr;

	ip_fw_init();
	splx(s);
	return 0;
}

static int
ipfw_unload(struct lkm_table *lkmtp, int cmd)
{
	int s=splnet();

	ip_fw_chk_ptr =  old_chk_ptr;
	ip_fw_ctl_ptr =  old_ctl_ptr;

	while (ip_fw_chain.lh_first != NULL) {
		struct ip_fw_chain *fcp = ip_fw_chain.lh_first;
		LIST_REMOVE(ip_fw_chain.lh_first, chain);
		free(fcp->rule, M_IPFW);
		free(fcp, M_IPFW);
	}
	
	splx(s);
	printf("IP firewall unloaded\n");
	return 0;
}

int
ipfw_mod(struct lkm_table *lkmtp, int cmd, int ver)
{
	DISPATCH(lkmtp, cmd, ver, ipfw_load, ipfw_unload, lkm_nullcmd);
}
#endif
