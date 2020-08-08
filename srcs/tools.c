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
	char				*str_addr;
	int 				ip;

	save_stats(data, &delay);
	if (data->msg_count == 3)
	{
		ip = ((struct iphdr *)(msg->msg_iov->iov_base))->saddr;
		if ((str_addr = (char *)malloc(INET_ADDRSTRLEN)) == NULL)
			return;
		inet_ntop(AF_INET, &ip, str_addr, INET_ADDRSTRLEN);
		printf("%d %s  %.2f  %.2f  %.2f\n", data->i, str_addr, \
			(float)(*(int*)data->stats_list->content) / 1000, \
			(float)(*(int*)data->stats_list->next->content) / 1000, \
			(float)(*(int*)data->stats_list->next->next->content) / 1000);
		free(str_addr);
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