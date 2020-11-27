#include "utilcom.h"

u32 tick_get(void)
{
	static u32 tmr = 0;
	static u32 init = 0;
	if (init == 0)
	{
		init = 1;
		tmr = (u32)GetTickCount();
		return 0;
	}
	return ((u32)GetTickCount() - tmr);
}

u32 tick_cmp(u32 tmr, u32 tmo)
{
	return ((tick_get() - tmr) >= tmo) ? c_ret_ok : c_ret_nk;
}

wint_t *char2wchar(u8 *src, wint_t *dst)
{
	wint_t x;
	wint_t *px;

	px = dst;
	while (*src)
	{
		x = btowc(*src++);
		*px++ = x;
	}
	*px = 0;
	return dst;
}



const u8 digit_tab[] = { "0123456789ABCDEF" };

u8 __abs8(u8 x, u8 y)
{
	return ((x >= y) ? (x - y) : (y - x));
}

u16 __abs16(u16 x, u16 y)
{
	return ((x >= y) ? (x - y) : (y - x));
}

u32 __abs32(u32 x, u32 y)
{
	return ((x >= y) ? (x - y) : (y - x));
}

u8* __memset(u8 *buf, u8 dat, u32 len)
{
	u8 *px = buf;

	while (len)
	{
		*px++ = dat;
		len--;
	}
	return buf;
}

u8* __memcpy(u8 *dst, const u8 *src, u16 len)
{
	u8 *px = dst;
	while (len--) *px++ = *src++;
	return dst;
}

u8 __memcmp(u8 *dst, const u8 *src, u16 len)
{
	while ((*dst == *src) && len)
	{
		dst++; src++; len--;
	}
	if (len == 0) return 0;
	else return (*dst - *src);
}

u16   __strlen(u8  *buf)
{
	u8  *px = buf;
	while (*px) px++;
	return (u16)(px - buf);
}

u8* __strcpy(u8 *buf, const u8  *str)
{
	u8 *px = buf;
	while (*str) *px++ = *str++;
	*px = 0;
	return buf;
}

u8* __strncpy(u8 *buf, const u8 *str, u16 len)
{
	u8 *px = buf;
	while (*str&&len) { *px++ = *str++; len--; }
	*px = 0;
	return buf;
}

u8* __strcat(u8 *buf, const u8 *str)
{
	u8 *px = buf;
	while (*px) px++;
	while (*str) *px++ = *str++;
	*px = 0;
	return buf;
}

u8* __strncat(u8 *buf, const u8 *str, u16 len)
{
	u8 *px = buf;
	while (*px) px++;
	while (*str&&len) { *px++ = *str++; len--; }
	*px = 0;
	return buf;
}

u8 __strcmp(u8 *buf, const u8 *str)
{
	while (*buf == *str)
	{
		if (*str == 0) return 0;
		buf++; str++;
	}
	return *buf - *str;
}

u8 __strncmp(u8 *buf, const u8 *str, u16 len)
{
	while ((*buf == *str) && len)
	{
		len--;
		if ((*str == 0) || (len == 0)) return 0;
		buf++;
		str++;
	}
	return *buf - *str;
}

u8* __strchr(u8 *buf, u8 chr)
{
	while (*buf)
	{
		if (*buf == chr) return buf;
		buf++;
	}
	return null;
}

u8* __strrchr(u8 *buf, u8 chr)
{
	u16 x;

	x = __strlen(buf);
	while (x)
	{
		if (buf[x - 1] == chr) return buf + x - 1;
		x--;
	}
	return null;
}

u8* __strstr(u8 *buf, const u8 * str)
{
	u8 x;

	while (*buf)
	{
		if (*buf == *str)
		{
			x = 0;
			while (buf[x] == str[x])
			{
				if (str[x + 1] == 0) return buf;
				x++;
			}
		}
		buf++;
	}
	return null;
}

//ASCII�ַ��������໥ת��
u8 __hex2asc(u8 dig)	//һλʮ��������ת����ASCII���ַ�
{
	dig &= 0x0f;
	if (dig > 9) return dig - 10 + 'A';
	return dig += '0';
}
u8 __asc2hex(u8 asc)	//ascii->hex_one
{
	if ((asc >= '0') && (asc <= '9'))
		return asc - '0';
	else if ((asc >= 'a') && (asc <= 'f'))
		return asc + 10 - 'a';
	else if ((asc >= 'A') && (asc <= 'F'))
		return asc + 10 - 'A';
	else return 0xff;	//����һ�����ɴ�ӡ�ַ�						
}
u8 __dec2asc(u8 dig)	//һλʮ��������ת����ASCII���ַ�
{
	dig &= 0x0f;
	if (dig > 9) return ' ';//������󣬷��ؿո�
	return dig += '0';
}
u8 __asc2dec(u8 asc)	//ascii->hex_one
{
	if ((asc >= '0') && (asc <= '9'))
		return asc - '0';
	else return 0;
}

//�ַ���ת��Ϊ��ֵ
u32 __str2dec(u8 *buf)
{
	u32 r = 0;
	u8 x, sign = 0;

	while (*buf == ' ') buf++;
	if (*buf == '-') { sign = 1; buf++; }
	while (*buf)
	{
		if ((*buf >= '0') && (*buf <= '9')) x = *buf - '0';
		else break;
		r = r * 10 + x;
		buf++;
	}
	return (sign ? 0xffffffff - r + 1 : r);
}
u32 __str2hex(u8 *buf)
{
	u32 r = 0;
	u8 x, sign = 0;

	while (*buf == ' ') buf++;
	if (*buf == '-') { sign = 1; buf++; }
	while (*buf)
	{
		if ((*buf >= '0') && (*buf <= '9')) x = *buf - '0';
		else if ((*buf >= 'a') && (*buf <= 'f')) x = *buf - 'a' + 10;
		else if ((*buf >= 'A') && (*buf <= 'F')) x = *buf - 'A' + 10;
		else break;
		r = r * 16 + x;
		buf++;
	}
	return (sign ? 0xffffffff - r + 1 : r);
}

u16 __hz2hex16(u8 *buf)		// ��������ת�������֣�Ŀ������������
{
	return (u16)(buf[0] << 8 | buf[1]);
}

//�ַ���ת����16���ƻ����
void __str2bufhex(u8 *buf, const u8 *str)	//"102C"->0x10,0x2c
{
	while (1)
	{
		if (*str == 0) return;
		*buf = __asc2hex(*str++);

		if (*str == 0) return;
		*buf *= 16;
		*buf += __asc2hex(*str++);
		buf++;
	}
}
//16���ƻ����ת�����ַ���
void __bufhex2str(u8 *str, const u8 *buf, u8 len)	//0x10,0x2c->"102C"
{
	while (len--)
	{
		*str++ = __hex2asc((*buf) >> 4);
		*str++ = __hex2asc((*buf) & 0x0f);
		buf++;
	}
	*str = 0;
}
//�ַ���ת����10���ƻ����
void __str2bufdec(u8 *buf, const u8 *str)	//"1024"->0x0a,0x18
{
	while (1)
	{
		if (*str == 0) return;
		*buf = __asc2dec(*str++);

		if (*str == 0) return;
		*buf *= 10;
		*buf += __asc2dec(*str++);
		buf++;
	}
}
//10���ƻ����ת�����ַ���
void __bufdec2str(u8 *str, const u8 *buf, u8 len)	//0x0a,0x18->"1024"
{
	while (len--)
	{
		*str++ = __dec2asc((*buf) >> 4);
		*str++ = __dec2asc((*buf) & 0x0f);
		buf++;
	}
	*str = 0;
}

struct st_pf
{
	union un_pf_fg
	{
		u8 r;
		struct st_pf_fg
		{
			u8 fill_zero_null : 1;		//Ĭ�����ո�
			u8 align_left_right : 1;		//Ĭ���Ҷ���					
			u8 has_prefix : 1;			//#
			u8 is_lower : 1;
			u8 is_upper : 1;
			u8 is_neg : 1;				//���ǰ׺-
			u8 is_long : 1;				//%..l  ��char���Ϳɱ���ֵ����
		}b;
	}f;
	u8 base;							//10,16,//8
	u8 width;							//�����ֵ����Сλ��
	union un_da
	{
		u32 ud;
		s32 sd;
	}d;									//����������ֵ
	u8 *ps;								//����RAM�����ַ����ַ���
	const u8 *pg;                  //����ROM�ַ���
};

u16 __vsprintf(u8 *buf, const s8 *fmt, va_list ap)
{
	struct st_pf pf;
	u32 u32x;
	u8 *pbuf, digit_len, x;

	pbuf = buf;
	while (*fmt)
	{
		if (*fmt != '%') { *pbuf++ = *fmt++; continue; }
		if (*(fmt + 1) == '%') { *pbuf++ = '%'; fmt += 2; continue; }
		fmt++; __memset((u8*)&pf, 0, sizeof(pf)); pf.base = 10; pf.width = 1;
		while (pf.width)
		{
			switch (*fmt++)
			{
			case '0':pf.f.b.fill_zero_null = 1; break;
			case ' ':pf.f.b.fill_zero_null = 0; break;
			case '+':pf.f.b.align_left_right = 0; break;
			case '-':pf.f.b.align_left_right = 1; break;
			case '#':pf.f.b.has_prefix = 1; break;
			default:fmt--; pf.width = 0; break;
			}
		}
		while ((*fmt >= '0') && (*fmt <= '9')) { pf.width = pf.width * 10 + *fmt - '0'; fmt++; }
		if ((*fmt == 'l') || (*fmt == 'L')) { pf.f.b.is_long = 1; fmt++; }
		switch (*fmt++)
		{
		case 'c':*pbuf++ = (char)va_arg(ap, int); break;
		case 's':pf.ps = va_arg(ap, unsigned char*); while (*(pf.ps))	*pbuf++ = *(pf.ps)++; break;
		case 'S':pf.pg = va_arg(ap, const unsigned char*);
			x = 0;
			while (*(pf.pg + x))
			{
				*pbuf++ = *(pf.pg + x);
				x++;
			}
			break;
		case 'x':pf.f.b.is_lower = 1; goto pf_data_hex_com;
		case 'X':pf.f.b.is_upper = 1;
		pf_data_hex_com:
			pf.base = 16;
		case 'u':       //va_arg() x/i/u/d������ָ������2��4����֧��b���η�
			if (pf.f.b.is_long) pf.d.ud = va_arg(ap, unsigned long);
			else pf.d.ud = va_arg(ap, unsigned int);
			goto pf_data_com;
		case 'd':
			if (pf.f.b.is_long) pf.d.sd = va_arg(ap, long);
			else pf.d.sd = va_arg(ap, int);
			if (pf.d.sd<0) { pf.f.b.is_neg = 1; pf.d.ud = -pf.d.sd; }
		pf_data_com:
			digit_len = 0; u32x = pf.d.ud;
			do { digit_len++; u32x /= pf.base; } while (u32x != 0);
			for (u32x = 1, x = digit_len - 1; x>0; x--) u32x *= pf.base;
			if (pf.f.b.is_neg) *pbuf++ = '-';
			if (pf.f.b.has_prefix)
			{
				if (pf.f.b.is_lower) { *pbuf++ = '0'; *pbuf++ = 'x'; }
				else if (pf.f.b.is_upper) { *pbuf++ = '0'; *pbuf++ = 'X'; }
			}
			if ((pf.width>digit_len) && (pf.f.b.align_left_right == 0))
				while (pf.width != digit_len) { *pbuf++ = pf.f.b.fill_zero_null ? '0' : ' '; pf.width--; }
			while (u32x != 0)
			{
				*pbuf = digit_tab[pf.d.ud / u32x]; if ((*pbuf >= 'A') && (pf.f.b.is_lower)) *pbuf += 0x20;
				pbuf++; pf.d.ud %= u32x; u32x /= pf.base;
			}
			if ((pf.width>digit_len) && (pf.f.b.align_left_right == 1))
				while (pf.width != digit_len) { *pbuf++ = ' '; pf.width--; }
		default:break;
		}
	}
	*pbuf = 0;
	return (u16)(pbuf - buf);
}

u16 __sprintf(u8 *buf, const s8 *fmt, ...)
{
	u16 ret;
	va_list ap;

	va_start(ap, fmt);
	ret = __vsprintf(buf, fmt, ap);
	va_end(ap);
	return ret;
}

//	���ַ���str��ͳ���ַ�chr����
u8 __strchrs(u8* str, u8 chr)
{
	u8 x = 0;

	while (*str)
	{
		if (*str == chr) x++;
		str++;
	}
	return x;
}

//	�����ַ���str�е�n��chr��λ��,���󷵻�NULL
u8* __strnchr(u8 *str, u8 chr, u8 n)
{
	while (*str)
	{
		if (*str == chr)
		{
			n--;
			if (n == 0) return str;
		}
		str++;
	}
	return NULL;
}

//	�����h���ַ�chr��һ�ַ�����h+1���ַ�chrǰһ�ַ�֮����ַ�����
u16 __strlen_ht(u8 *str, u8 chr, u8 h)
{
	u8 *px, *py;

	if (h == 0) 				// ���ַ�����ͷ����һ��chr
	{
		if (*str == chr)		// ��һ���ַ�chr
			return 0;
		px = str;
		if (px == NULL) return 0;
	}
	else
	{
		px = __strnchr(str, chr, h);
		if (px == NULL) return 0;
		px++;
	}

	py = __strnchr(str, chr, h + 1);
	if (py == NULL) py = str + __strlen(str);

	return (u16)(py - px);
}

//	���Ƶ�h���ַ�chr��һ�ַ�����h+1���ַ�chrǰһ�ַ�
u8 __strncpy_ht(u8* buf, u8* str, u8 chr, u8 h, u16 maxLen)
{
	u8 *px, *py;
	u16 len;

	if (h == 0) 					// ���ַ�����ͷ����һ��chr
	{
		if (*str == chr)
		{
			buf[0] = 0;
			return c_ret_ok;
		}
		px = str;
		if (px == NULL) return c_ret_nk;
	}
	else
	{
		px = __strnchr(str, chr, h);
		if (px == NULL) return c_ret_nk;
		px++;
	}

	py = __strnchr(str, chr, h + 1);
	if (py == NULL) py = str + __strlen(str);

	len = (u16)(py - px);
	if (len >= maxLen) len = maxLen;
	__strncpy(buf, px, len);
	return c_ret_ok;
}
