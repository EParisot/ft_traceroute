/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strswap.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klhnikov <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/21 20:06:11 by klhnikov          #+#    #+#             */
/*   Updated: 2017/11/21 22:02:52 by klhnikov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

void	ft_strswap(char *a, char *b)
{
	char	c;

	while (*a && *b)
	{
		c = *a;
		*a = *b;
		*b = c;
		a++;
		b++;
	}
}
