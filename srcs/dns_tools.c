/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping_tools.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eparisot <eparisot@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/05 23:08:42 by eparisot          #+#    #+#             */
/*   Updated: 2020/08/05 23:08:45 by eparisot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_traceroute.h"

unsigned short		calc_checksum(void *msg, int msg_size)
{
	unsigned int	sum;
	unsigned short	res;
	unsigned short	*buf;

	sum = 0;
	buf = (unsigned short *)msg;
	while (msg_size > 1)
	{
		sum += *buf++;
		msg_size -= 2;
	}
	if (msg_size == 1)
		sum += *(unsigned char *)buf;
	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);
	sum += (sum >> 16);
	res = ~sum;
	return (res);
}

void				set_addr_info_struct(struct addrinfo *hints)
{
	ft_memset(hints, 0, sizeof(struct addrinfo));
	hints->ai_family = AF_UNSPEC;
	hints->ai_socktype = SOCK_DGRAM;
	hints->ai_flags = AI_PASSIVE | AI_CANONNAME;
	hints->ai_protocol = 0;
	hints->ai_canonname = NULL;
	hints->ai_addr = NULL;
	hints->ai_next = NULL;
}

void				free_addr_info(struct addrinfo *result)
{
	struct addrinfo *tmp;

	while (result)
	{
		tmp = result;
		result = result->ai_next;
		free(tmp->ai_canonname);
		free(tmp);
	}
}

static char			*dns_lookup_b(struct addrinfo *result)
{
	struct sockaddr_in	*addr_in;
	char				*str_addr;

	str_addr = NULL;
	if (result->ai_addr->sa_family == AF_INET)
	{
		addr_in = (struct sockaddr_in *)result->ai_addr;
		if ((str_addr = (char *)malloc(INET_ADDRSTRLEN)) == NULL)
			return (NULL);
		inet_ntop(AF_INET, &(addr_in->sin_addr), str_addr, INET_ADDRSTRLEN);
	}
	else if (result->ai_addr->sa_family == AF_INET6)
	{
		fprintf(stderr, "ft_traceroute: IPV6 Not Implemented\n");
		return (NULL);
	}
	return (str_addr);
}

int					dns_lookup(t_ping_data *data)
{
	struct addrinfo hints;
	struct addrinfo *result;

	result = NULL;
	set_addr_info_struct(&hints);
	if (dns_err(data, &hints, &result) == -1)
	{
		data->stats_list = NULL;
		return (-1);
	}
	if (result)
	{
		if ((data->target_addr = dns_lookup_b(result)) == NULL)
		{
			data->stats_list = NULL;
			return (-1);
		}
		ft_memcpy(data->sock_addr, result->ai_addr->sa_data, 14);
	}
	free_addr_info(result);
	return (0);
}
