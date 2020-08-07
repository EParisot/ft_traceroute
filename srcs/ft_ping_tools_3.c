/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping_tools_3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eparisot <eparisot@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/05 23:09:06 by eparisot          #+#    #+#             */
/*   Updated: 2020/08/05 23:09:08 by eparisot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_traceroute.h"

int			dns_err(t_ping_data *data, struct addrinfo *hints, \
												struct addrinfo **result)
{
	int err;

	err = 0;
	if ((err = getaddrinfo(data->target, NULL, hints, result)) != 0)
	{
		if (err != -5 && err != -2)
			fprintf(stderr, \
	"ft_traceroute: %s: Temporary failure in name resolution\n", data->target);
		else if (err == -5)
			fprintf(stderr, \
	"ft_traceroute: %s: No address associated with hostname!\n", data->target);
		else if (err == -2)
			fprintf(stderr, "ft_traceroute: %s: Name or service not known\n",\
				data->target);
		return (-1);
	}
	return (0);
}

void		print_step_stats(t_ping_data *data, struct msghdr *msg)
{
	char				*str_addr;
	int 				ip;

	ip = ((struct iphdr *)(msg->msg_iov->iov_base))->saddr;
	if ((str_addr = (char *)malloc(INET_ADDRSTRLEN)) == NULL)
		return;
	inet_ntop(AF_INET, &ip, str_addr, INET_ADDRSTRLEN);
	printf("%d: %s\n", data->i, str_addr);
	free(str_addr);
}

void		print_pkt_stats(t_ping_data *data, int received_size, \
														int delay)
{
	printf("%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%.2f ms\n", \
		(int)(received_size - sizeof(struct iphdr)), data->target, \
		data->target_addr, data->msg_count, data->last_ttl, \
		(float)(delay) / 1000);
	save_stats(data, &delay);
}
