#include "QuadCopterConfig.h"

void Delay_nop(vu32 nCnt)
{
	while (nCnt--) {
	}
}

void Delay_1us(vu32 nCnt_1us)
{
	u32 nCnt;

	for (; nCnt_1us != 0; nCnt_1us--)
		for (nCnt = 45; nCnt != 0; nCnt--);
}

void Delay_10us(vu32 nCnt_10us)
{
	u32 nCnt;

	for (; nCnt_10us != 0; nCnt_10us--)
		for (nCnt = 556; nCnt != 0; nCnt--);
}

void Delay_100us(vu32 nCnt_100us)
{
	u32 nCnt;

	for (; nCnt_100us != 0; nCnt_100us--)
		for (nCnt = 5400; nCnt != 0; nCnt--);
}

void Delay_1ms(vu32 nCnt_1ms)
{
	u32 nCnt;

	for (; nCnt_1ms != 0; nCnt_1ms--)
		for (nCnt = 56580; nCnt != 0; nCnt--);
}

void Delay_10ms(vu32 nCnt_10ms)
{
	u32 nCnt;

	for (; nCnt_10ms != 0; nCnt_10ms--)
		for (nCnt = 559928; nCnt != 0; nCnt--);
}

void Delay_100ms(vu32 nCnt_100ms)
{
	u32 nCnt;

	for (; nCnt_100ms != 0; nCnt_100ms--)
		for (nCnt = 5655000; nCnt != 0; nCnt--);
}

