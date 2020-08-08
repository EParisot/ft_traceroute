/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eparisot <eparisot@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/05 23:08:24 by eparisot          #+#    #+#             */
/*   Updated: 2020/08/05 23:08:29 by eparisot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_traceroute.h"

static int		check_and_wait(t_ping_data *data, struct msghdr *msg)
{
	int			ret;
	char		*str_addr;

	ret = 0;
	if ((str_addr = (char *)malloc(INET_ADDRSTRLEN)) == NULL)
		return (-1);
	inet_ntop(AF_INET, &(((struct iphdr *)msg->msg_iov->iov_base)->saddr), \
												str_addr, INET_ADDRSTRLEN);
	if (ft_strcmp(str_addr, data->target_addr) || \
		((struct icmphdr *)(msg->msg_iov->iov_base + \
		((struct iphdr *)msg->msg_iov->iov_base)->ihl * \
		sizeof(unsigned int)))->type == 11)
		ret = -2;
	data->last_ttl = ((struct iphdr *)msg->msg_iov->iov_base)->ttl;
	if (ret == 0 && ((struct icmphdr *)(msg->msg_iov->iov_base + \
		((struct iphdr *)msg->msg_iov->iov_base)->ihl * \
		sizeof(unsigned int)))->un.echo.id != getpid() && data->verbose)
	{
		fprintf(stderr, "ft_traceroute: Someone else packet showed up!\n");
		ret = -1;
	}
	g_keyboard_interrupt = g_keyboard_interrupt & 0x10;
	free(str_addr);
	return (ret);
}

static int		send_and_receive(t_ping_data *data, struct msghdr *msg, \
				struct sockaddr *addr_struct, long int delay)
{
	int				received_size;
	struct timeval	start;
	struct timeval	end;
	t_ping_pkt		*pkt;
	int				ret;

	pkt = NULL;
	received_size = 0;
	ret = 0;
	if ((pkt = build_pkt(data)) == NULL)
		return (-1);
	alarm(TIMEOUT);
	gettimeofday(&start, NULL);
	if (sendto(data->sockfd, pkt, sizeof(t_ping_pkt), 0, addr_struct, \
						sizeof(struct sockaddr)) <= 0)
		return (-1);
	received_size = recvmsg(data->sockfd, msg, 0);
	gettimeofday(&end, NULL);
	delay = (received_size > -1) ? ((end.tv_sec * 1000000 + end.tv_usec) - \
			(start.tv_sec * 1000000 + start.tv_usec)) : 0;
	ret = check_and_wait(data, msg);
	print_step_stats(data, msg, delay, addr_struct);
	free(pkt);
	return (ret);
}

static int		ping_loop(t_ping_data *data, \
				struct sockaddr *addr_struct, int ret)
{
	struct msghdr	*msg;
	struct timeval	tv_out;

	msg = NULL;
	tv_out.tv_sec = TIMEOUT;
	tv_out.tv_usec = 0;
	while (g_keyboard_interrupt < 10 && (data->i)++ < TTL_MAX && ret)
	{
		data->msg_count = 0;
		data->stats_list = NULL;
		if (setsockopt(data->sockfd, SOL_IP, IP_TTL, &(data->i), \
			sizeof(data->i)) || setsockopt(data->sockfd, SOL_SOCKET, \
			SO_RCVTIMEO, (const char*)&tv_out, sizeof(tv_out)) != 0)
			return (-1);
		if ((msg = build_msg(addr_struct)) == NULL)
		{
			fprintf(stderr, "ft_traceroute: Error building msg!\n");
			return (-1);
		}
		while ((data->msg_count)++ < 3)
			if ((ret = send_and_receive(data, msg, addr_struct, 0)) == -1)
				fprintf(stderr, "ft_traceroute: Error sending pkt\n");
		free_msg(data, msg);
	}
	return (0);
}

int				exec_ping(t_ping_data *data)
{
	struct sockaddr	addr_struct;
	struct timeval	start;
	struct timeval	end;

	ft_memset(&addr_struct, 0, sizeof(struct sockaddr));
	addr_struct.sa_family = AF_INET;
	ft_memcpy(addr_struct.sa_data, data->sock_addr, 14);
	signal(SIGINT, sig_handler);
	signal(SIGALRM, sig_handler);
	g_keyboard_interrupt = 1;
	gettimeofday(&start, NULL);
	printf("FT_TRACEROUTE to %s (%s), %d hops max, %ld byte packets\n", \
							data->target, data->target_addr, \
							TTL_MAX, PING_PKT_S - sizeof(struct icmphdr));
	if (ping_loop(data, &addr_struct, -2) < 0)
		return (-1);
	gettimeofday(&end, NULL);
	return (0);
}

int				ft_traceroute(t_ping_data *data)
{
	if ((int)getuid() != 0)
	{
		if (data->verbose)
			fprintf(stderr, \
				"ft_traceroute: socket: Insufficient Permissions\n");
		return (-1);
	}
	if (dns_lookup(data) != 0)
		return (-1);
	if ((data->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		if (data->verbose)
			fprintf(stderr, \
				"ft_traceroute: Socket file descriptor not received\n");
		return (-1);
	}
	exec_ping(data);
	close(data->sockfd);
	return (0);
}
