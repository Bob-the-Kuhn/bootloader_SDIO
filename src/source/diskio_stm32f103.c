/*
 * 2017, Aurelio Colosimo <aurelio@aureliocolosimo.it>
 * This file is part of fatfs-stm32f4:
 * https://github.com/colosimo/fatfs-stm32
 * MIT License
 */

#include <basic.h>
#include <gpio.h>
#include <log.h>
#include "ff.h"
#include "diskio.h"

#define RESP_NONE  0
#define RESP_SHORT 1
#define RESP_LONG  2

#define CT_MMC   0x01    /* MMC ver 3 */
#define CT_SD1   0x02    /* SD ver 1 */
#define CT_SD2   0x04    /* SD ver 2 */
#define CT_SDC   (CT_SD1|CT_SD2) /* SD */
#define CT_BLOCK 0x08    /* Block addressing */

#define ACMD(x) ((x) | 0x80)

static DSTATUS dstatus = STA_NOINIT;

static uint16_t card_rca;
static uint8_t card_type;
static uint8_t card_info[36]; /* CSD, CID, OCR */

extern uint32_t k_ticks(void);
extern uint32_t k_ticks_freq(void);

uint32_t k_elapsed(uint32_t tprev);
uint32_t ms_to_ticks(uint32_t ms);
void k_delay(const uint32_t ms);

static void byte_swap(uint8_t *dest, uint32_t src)
{
	int i;
	for (i = 0; i < 4; i ++)
		dest[i] = src >> (24 - 8 * i);
}

static int send_cmd(uint16_t idx, uint32_t arg, int resp_type, uint32_t *buf)
{
	uint32_t cmd;
	uint32_t s;
	uint32_t start;

	k_delay(1);

	if (idx & 0x80) { /* ACMD class */
		if (!send_cmd(55, ((uint32_t)card_rca) << 16, RESP_SHORT, buf)
			|| !(buf[0] & 0x00000020))
			return 0;
	}

	idx &= 0x3F;
	wr32(R_SDIO_ICR, 0xc007ff);

	wr32(R_SDIO_ARG, arg);

	cmd = BIT10 | idx;
	if (resp_type == RESP_SHORT)
		cmd |= BIT6;
	else if (resp_type == RESP_LONG)
		cmd |= BIT6 | BIT7;

	wr32(R_SDIO_CMD, cmd);
	start = k_ticks();

	while (1) {

		if (k_elapsed(start) > ms_to_ticks(100)) {
			break;
		}

		s = rd32(R_SDIO_STA);
		if (resp_type == RESP_NONE)
			return s & BIT7 ? 1 : 0;
		else {
			/* check if timeout */
			if (s & BIT3) {
				//err("%s timeout idx=%d arg=%08x\n", __func__, idx, (uint16_t)arg);
				return 0;
			}

			/* check if crc err */
			if (s & BIT1) {
				if (idx == 1 || idx == 12 || idx == 41)
					break;
				//err("%s crcfail idx=%d arg=%08x\n", __func__, idx, (uint16_t)arg);
				return 0;
			}

			if (s & BIT6)
				break;
		}
	}

	buf[0] = rd32(R_SDIO_RESP1);
	if (resp_type == RESP_LONG) {
		buf[1] = rd32(R_SDIO_RESP2);
		buf[2] = rd32(R_SDIO_RESP3);
		buf[3] = rd32(R_SDIO_RESP4);
	}

	return 1;
}

static int check_tran(uint32_t tout_ms)
{
	uint32_t t;
	uint32_t resp;

	t = k_ticks();

	while (k_elapsed(t) < ms_to_ticks(tout_ms)) {
		if (send_cmd(13, ((uint32_t)card_rca) << 16, RESP_SHORT, &resp)
			&& ((resp & 0x01e00) == 0x00800))
			return 1;
	}
	return 0;
}

DSTATUS disk_initialize(uint8_t pdrv)
{
	uint32_t resp[4];
	uint32_t start;
	uint16_t cmd;
	/* uint32_t clkcr; */
	int i;

	dstatus = STA_NOINIT;

  //PC8  SDIO_D0  (must not enable TIM8_CH3)
  //PC9  SDIO_D1  (must not enable TIM8_CH4)
  //PC10 SDIO_D2  (must not enable UART4_TX)
  //PC11 SDIO_D3  (must not enable UART4_RX)
  //PC12 SDIO_CK  (must not enable UART5_TX)
  //PD2  SDIO_CMD (must not enable TIM3_ETR/UART5_RX)
  

	/* SDIO_D0 */
	gpio_mode(IO(PORTC, 8), Output_Push_Pull_Alternate_Function, Speed_High);

	/* SDIO_D1 */
	gpio_mode(IO(PORTC, 9), Output_Push_Pull_Alternate_Function, Speed_High);

	/* SDIO_D2 */
	gpio_mode(IO(PORTC, 10), Output_Push_Pull_Alternate_Function, Speed_High);

	/* SDIO_D3 */
	gpio_mode(IO(PORTC, 11), Output_Push_Pull_Alternate_Function, Speed_High);

	/* SDIO_CK */
	gpio_mode(IO(PORTC, 12), Output_Push_Pull_Alternate_Function, Speed_High);

	/* SDIO_CMD */
	gpio_mode(IO(PORTD, 2), Output_Push_Pull_Alternate_Function, Speed_High);

	dstatus &= ~STA_NOINIT;

  #ifdef HAS_SD_DETECT
    if (gpio_rd(IO(PORTB, 15)))
      dstatus |= STA_NODISK;
  #endif

	if (dstatus & STA_NODISK)
		return RES_OK;

	or32(RCC_APB2ENR, BIT11);
	wr32(R_SDIO_CLKCR, BIT8 | ((42000 / 400) - 2)); /* clk set to 400kHz */
	wr32(R_SDIO_POWER, 0b11);

	send_cmd(0, 0, 0, NULL);

	start = k_ticks();
	if (send_cmd(8, 0x1AA, RESP_SHORT, resp) && ((resp[0] & 0xfff) == 0x1aa)) {
		/* sdc v2 */
		card_type = 0;
		do {
			if (send_cmd(ACMD(41), 0x40ff8000, RESP_SHORT, resp) &&
			    (resp[0] & BIT31)) {
				card_type = (resp[0] & BIT30) ? CT_SD2 | CT_BLOCK : CT_SD2;
				//log("card type: SD2\n");
				break;
			}
		} while (k_elapsed(start) < ms_to_ticks(1000));

		if (!card_type) {
			//err("could not read card type\n");
			goto fail;
		}
	}
	else {
		/* sdc v1 or mmc */
		if (send_cmd(ACMD(41), 0x00ff8000, RESP_SHORT, resp)) {
			/* ACMD41 is accepted -> sdc v1 */
			card_type = CT_SD1;
			cmd = ACMD(41);
		} else {
			/* ACMD41 is rejected -> mmc */
			card_type = CT_MMC;
			cmd = 1;
		}
		while (1) {
			if (send_cmd(cmd, 0x00FF8000, RESP_SHORT, resp) &&
			    (resp[0] & BIT31)) {
				break;
			}
			if (k_elapsed(start) > ms_to_ticks(1000)) {
				//err("cmd %d failed\n", cmd);
				goto fail;
			}
		}
	}

	byte_swap(&card_info[32], resp[0]);
  //log("card OCR: %08x\n", ((uint*)card_info)[8]);

	/* card state 'ready' */
	if (!send_cmd(2, 0, RESP_LONG, resp)) /* enter ident state */
		goto fail;

	for (i = 0; i < 4; i++)
		byte_swap(&card_info[16 + i * 4], resp[i]);

	/* card state 'ident' */
	if (card_type & CT_SDC) {
		if (!send_cmd(3, 0, RESP_SHORT, resp))
			goto fail;
		card_rca = (uint16_t)(resp[0] >> 16);
	} else {
		if (!send_cmd(3, 1 << 16, RESP_SHORT, resp))
			goto fail;
		card_rca = 1;
	}

	/* card state 'standby' */
	if (!send_cmd(9, ((uint32_t)card_rca) << 16, RESP_LONG, resp))
		goto fail;

	for (i = 0; i < 4; i++)
		byte_swap(&card_info[i * 4], resp[i]);

	if (!send_cmd(7, ((uint32_t)card_rca) << 16, RESP_SHORT, resp))
		goto fail;

	/* card state 'tran' */
	if (!(card_type & CT_BLOCK)) {
		if (!send_cmd(16, 512, RESP_SHORT, resp) || (resp[0] & 0xfdf90000))
			goto fail;
	}

	//if (card_type & CT_SDC) {
	//	/* Set wide bus */
	//	if (!send_cmd(ACMD(6), 2, RESP_SHORT, resp) || (resp[0] & 0xfdf90000))
	//		goto fail;
	//	or32(R_SDIO_CLKCR, BIT11);
	//}

	/* increase clock up to 4MHz */
	//and32(R_SDIO_CLKCR, ~0xff);
	//or32(R_SDIO_CLKCR, 10);

	dstatus &= ~STA_NOINIT;

	return RES_OK;

fail:
	dstatus = STA_NOINIT;
	and32(RCC_APB2ENR, ~BIT11);
	wr32(R_SDIO_POWER, 0b00);
	return RES_ERROR;
}

DSTATUS disk_status(uint8_t pdrv)
{
	return dstatus;
}

DRESULT disk_read(uint8_t pdrv, uint8_t* buf, uint32_t sector, uint16_t count)
{
	uint32_t resp;
	int cmd;
	int rd;
	uint32_t t;
	uint32_t sta;
	uint32_t data;

	if (count < 1 || count > 127)
		return RES_PARERR;

	if (dstatus & STA_NOINIT)
		return RES_NOTRDY;

	if (!(card_type & CT_BLOCK))
		sector *= 512;


	if (!check_tran(500))
		return RES_ERROR;

	cmd = (count > 1) ? 18 : 17;

	t = k_ticks();
	rd = 0;

	wr32(R_SDIO_DCTRL, (0b1001 << 4) | BIT1);
	wr32(R_SDIO_DLEN, 512 * count);
	wr32(R_SDIO_DTIMER, 400000);

	sta = 0;
	if (!send_cmd(cmd, sector, RESP_SHORT, &resp) || (resp & 0xc0580000))
		return RES_ERROR;
	wr32(R_SDIO_ICR, 0xff);
	or32(R_SDIO_DCTRL, BIT0);

	while (k_elapsed(t) < ms_to_ticks(1000)) {

		sta = rd32(R_SDIO_STA);

		if (sta & (BIT3 | BIT5 | BIT9)) {
			//err("%s SDIO_STA: %08x\n", __func__, (uint16_t)sta);
			break;
		}

		if (sta & BIT19)
			continue;

		if (rd == 0 && !(sta & BIT21))
			continue;

		data = rd32(R_SDIO_FIFO);

		buf[rd++] = data;
		buf[rd++] = data >> 8;
		buf[rd++] = data >> 16;
		buf[rd++] = data >> 24;

		if (rd == 512 * count)
			break;
	}

	if (rd < 512 * count || cmd == 18)
		send_cmd(12, 0, RESP_SHORT, &resp);

	return rd32(R_SDIO_DCOUNT) ? RES_ERROR : RES_OK;
}

DRESULT disk_write(uint8_t pdrv, const uint8_t* buf, uint32_t sector, uint16_t count)
{
	uint32_t resp;
	int cmd;
	int wr;
	uint32_t t;
	uint32_t sta;
	uint32_t data;

	if (count < 1 || count > 127)
		return RES_PARERR;

	if (dstatus & STA_NOINIT)
		return RES_NOTRDY;

	/* FIXME Check Write Protect Pin, if any */

	if (!(card_type & CT_BLOCK))
		sector *= 512;

	if (!check_tran(500))
		return RES_ERROR;

	if (count == 1) /* Single block write */
		cmd = 24;
	else { /* Multiple block write */
		cmd = (card_type & CT_SDC) ? ACMD(23) : 23;
		if (!send_cmd(cmd, count, RESP_SHORT, &resp) || (resp & 0xC0580000))
				return RES_ERROR;
		cmd = 25;
	}

	t = k_ticks();
	wr = 0;
	wr32(R_SDIO_DCTRL, (0b1001 << 4));
	wr32(R_SDIO_DLEN, 512 * count);
	wr32(R_SDIO_DTIMER, 400000);

	if (!send_cmd(cmd, sector, RESP_SHORT, &resp) || (resp & 0xC0580000)) {
		//err("%s %d\n", __func__, __LINE__);
		return RES_ERROR;
	}

	wr32(R_SDIO_ICR, 0xff);
	or32(R_SDIO_DCTRL, BIT0);

	while (k_elapsed(t) < ms_to_ticks(1000)) {

		sta = rd32(R_SDIO_STA);

		if (sta & (BIT3 | BIT9)) {
			//err("%s SDIO_STA: %08x\n", __func__, (uint16_t)sta);
			break;
		}

		if (sta & BIT16)
			continue;

		data = buf[wr++];
		data = data | (((uint32_t)buf[wr++]) << 8);
		data = data | (((uint32_t)buf[wr++]) << 16);
		data = data | (((uint32_t)buf[wr++]) << 24);

		wr32(R_SDIO_FIFO, data);

		if (wr >= 512 * count) {
			break;
		}
	}

	if (wr < 512 * count || (cmd == 25 && (card_type & CT_SDC)))
		send_cmd(12, 0, RESP_SHORT, &resp);

	return wr < 512 * count ? RES_ERROR : RES_OK;
}

DRESULT disk_ioctl(uint8_t pdrv, uint8_t cmd, void *buff)
{
	/* Dummy, unneeded */
	return RES_OK;
}
