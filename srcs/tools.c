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

void		print_step_stats(t_ping_data *data, struct msghdr *msg, \
														int delay)
{
	char				str_addr[INET_ADDRSTRLEN];
	int 				ip;
	struct hostent		*he;
	char				hostname[1024];

	he = NULL;
	save_stats(data, &delay);
	if (data->msg_count == 3)
	{
		ip = ((struct iphdr *)(msg->msg_iov->iov_base))->saddr;
		inet_ntop(AF_INET, &ip, str_addr, INET_ADDRSTRLEN);
		ft_strcpy(hostname, str_addr);
		if (((struct iphdr *)(msg->msg_iov->iov_base))->saddr)
			he = gethostbyaddr((void *)&(((struct iphdr *)\
				(msg->msg_iov->iov_base))->saddr) , sizeof(((struct iphdr *)\
				(msg->msg_iov->iov_base))->saddr), AF_INET);
		if (he && he->h_name)
			ft_strcpy(hostname, he->h_name);
		printf("%d  %s (%s)  %.2f ms  %.2f ms  %.2f ms\n", \
							data->i, hostname, str_addr, \
			(float)(*(int*)data->stats_list->content) / 1000, \
			(float)(*(int*)data->stats_list->next->content) / 1000, \
			(float)(*(int*)data->stats_list->next->next->content) / 1000);
	}
}

void		free_msg(t_ping_data *data, struct msghdr *msg)
{
	free(msg->msg_iov->iov_base);
	free(msg->msg_iov);
	free(msg->msg_name);
	free(msg);
	if (data && data->stats_list)
		ft_lstdel(&data->stats_list, del);
}