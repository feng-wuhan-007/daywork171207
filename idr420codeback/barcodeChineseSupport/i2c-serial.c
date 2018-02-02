/*
 * i2c-serial.c - collect pre-declarations of I2C devices
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/serial_core.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/kernel.h>
#include <linux/gfp.h>
#include<linux/slab.h>
#include <linux/input.h>
#include<linux/tty_flip.h>
#include <linux/delay.h>
#include"i2cserial/hi2c_c51.h"
#include"i2cserial/752.h"
#define SERIAL_I2C_TTY_MAJOR    207
#define SERIAL_TTY_MINORS       5
#define IRQ_WK2XXX		0
#define UART_LSR_OE 		0x80
#define PWR_SCAN		0x10
#define START_SCAN		0X11
#define PWR_DOWN		0x12

#ifndef DEBUG
#define DEBUG false
#endif
static DEFINE_MUTEX(wk2xxxs_lock);
static bool rcv=false;
//static void wk2xxx_work(struct work_struct *w);
static void wk2xxx_stop_tx(struct uart_port *port);
static void wk2xxx_start_tx(struct uart_port *port);
static void conf_wk2xxx_subport(struct uart_port *port);
#if 0
static void wk2xxx_rx_chars(struct uart_port *port)
{	
	struct wk2xxx_port *s = container_of(port,struct wk2xxx_port,port);
	u8 lsr,rxlvl;
	int ch, flg, ignored=0,status = 0,rx_count=0;
	if(DEBUG)
	if(DEBUG) printk("--i2c--wk2xxx_rx_chars");	
	lsr=read_reg(s->client_dev,(LSR<<3)&0xf9);
	rxlvl=read_reg(s->client_dev,(RXLVL<<3)&0xf9);
	printk("scandebug : ");
	char tmpbuf[512];
	int i =0,j=0;
	while(rxlvl)/*当接收缓冲区不为空时，就从接收寄存器里面读数据*/
        {
		
        	ch=read_reg(s->client_dev,(RHR<<3)&0xf9);
		//if(DEBUG) 
                //printk("%c",ch);
		tmpbuf[i++] = ch;

		//report_key(s,ch);
#if 0		//udelay(200);
		s->port.icount.rx++;

		flg = TTY_NORMAL;
		if (lsr&FIFO_DATERROR)
                {
			if (lsr & WK2XXX_PE)
		        {
				s->port.icount.parity++;
        		        status |= WK2XXX_STATUS_PE;
            			flg = TTY_PARITY;
        		}
		        if (lsr & WK2XXX_FE)
		        {
		                s->port.icount.frame++;
		                status |= WK2XXX_STATUS_FE;
		                flg = TTY_FRAME;
		        }
			
			if (lsr & WK2XXX_BI)
		        {
		               s->port.icount.brk++;
		               status |= WK2XXX_STATUS_BRK;
			       flg = TTY_BREAK;
		        }
			
	       	
			 goto ignore_char;       
		}

	error_return:
	if (uart_handle_sysrq_char(&s->port,ch))
        	goto ignore_char;
				
		uart_insert_char(&s->port, status, UART_LSR_OE, ch, flg);		
		rx_count++;	  
		if ((rx_count >= 8 ) && (s->port.state->port.tty != NULL)) 
		{		
			tty_flip_buffer_push(&s->port.state->port.tty);
	  		rx_count = 0;		
		}
#endif
        ignore_char:
		rxlvl=read_reg(s->client_dev,(RXLVL<<3)&0xf9);
		lsr=read_reg(s->client_dev,(LSR<<3)&0xf9);
        }
	out:
//	if((rx_count > 0)&&(s->port.state->port.tty != NULL))
//	{
//        	tty_flip_buffer_push(&s->port.state->port.tty);
//	}
	for(j=0;j<i;j++)
		report_key(s,tmpbuf[j]);
	
      return;
#ifdef SUPPORT_SYSRQ
        s->port.state->sysrq = 0;
#endif
      //  goto error_return;

}
#else
static inline void wk2xxx_report_chars(struct wk2xxx_port *s, char *chars, int size,char *lsr)
{
    int i = 0;
    int flg;
    int rx_count=0;
    int status=0;
    flg = TTY_NORMAL;

    printk("Scaner: ");
    for(i = 0; i < size; i++) {

        if (s->keyevent_enable)
            report_key(s, chars[i]);

        s->port.icount.rx++;
        if (lsr[i]&FIFO_DATERROR){
            if (lsr[i] & WK2XXX_PE)
            {
                s->port.icount.parity++;
                status |= WK2XXX_STATUS_PE;
                flg = TTY_PARITY;
            }
            if (lsr[i] & WK2XXX_FE)
            {
                s->port.icount.frame++;
                status |= WK2XXX_STATUS_FE;
                flg = TTY_FRAME;
            }
            /*	if (lsr[i] & WK2XXX_BI)
                {
                s->port.icount.brk++;
                status |= WK2XXX_STATUS_BRK;
                flg = TTY_BREAK;
                }*/
        }
        if (uart_handle_sysrq_char(&s->port,chars[i]))
            continue;
        printk("%02x ", chars[i]);
        uart_insert_char(&s->port, status, UART_LSR_OE, chars[i], flg);		
        rx_count++;
        if ((rx_count >= 8 ) && (s->port.state->port.tty != NULL)) 
        {		
            tty_flip_buffer_push(&s->port.state->port);
            rx_count = 0;		
        }
    }
    printk("\n");

    if((rx_count>0)&&(s->port.state->port.tty!=NULL)){
        tty_flip_buffer_push(&s->port.state->port);
        if(rcv==true){
            uart_insert_char(&s->port, status, UART_LSR_OE, 10, flg);
            // uart_insert_char(&s->port, status, UART_LSR_OE, 13, flg);
            tty_flip_buffer_push(&s->port.state->port);
            rcv=false;
        }
    }	
}


static void wk2xxx_rx_chars(struct uart_port *port)
{
	struct wk2xxx_port *s = container_of(port,struct wk2xxx_port,port);
	int i = 0, num = 0, rx = 0;
	char recvbuf[512];
	char lsrbuf[512];
	if(DEBUG) printk("i2c---rx_chars\n");
	while ((rx = read_reg(s->client_dev, (RXLVL<<3)&0xf9)) > 0) {
		for (i = 0; i < rx && num < sizeof(recvbuf); i++) {			
			lsrbuf[num]=read_reg(s->client_dev,(LSR<<3)&0xf9);
			recvbuf[num++] = read_reg(s->client_dev, (RHR<<3)&0xf9);
		}
		
		if (num == sizeof(recvbuf)) {
			wk2xxx_report_chars(s, recvbuf, num, lsrbuf);
			num = 0;
		}
	}
	
	wk2xxx_report_chars(s, recvbuf, num, lsrbuf);
#ifdef SUPPORT_SYSRQ
        s->port.state->sysrq = 0;
#endif
}
#endif

static void wk2xxx_tx_chars(struct uart_port *port)//
{ 
        struct wk2xxx_port *s = container_of(port,struct wk2xxx_port,port);
	uint8_t txlvl;
	int count;
	
	if(DEBUG) printk("--i2c--wk2xxx_tx_chars\n");	
	txlvl=read_reg(s->client_dev,(TXLVL<<3)&0Xf9);
	count=s->port.fifosize;
        if (s->port.x_char)
	{
		write_reg(s->client_dev,(THR<<3)&0Xf9,s->port.x_char);
                s->port.icount.tx++;
                s->port.x_char = 0;
                return;
	}
	
	if(uart_circ_empty(&s->port.state->xmit) || uart_tx_stopped(&s->port))
         {
		wk2xxx_stop_tx(&s->port);
                return;
         }

	do{
		if(!txlvl|uart_circ_empty(&s->port.state->xmit))
			break;	
		write_reg(s->client_dev,(THR<<3)&0Xf9,s->port.state->xmit.buf[s->port.state->xmit.tail]);
		s->port.state->xmit.tail = (s->port.state->xmit.tail + 1) & (UART_XMIT_SIZE - 1);
        	s->port.icount.tx++;
		txlvl=read_reg(s->client_dev,(TXLVL<<3)&0Xf9);
	}while(--count>0);
	rcv=true;
	if (uart_circ_chars_pending(&s->port.state->xmit) < WAKEUP_CHARS)
        	uart_write_wakeup(&s->port);
		if (uart_circ_empty(&s->port.state->xmit))
			wk2xxx_stop_tx(&s->port);
}



static void work_irq(struct uart_port *port){
     u8 iir,ier,pass_counter=0;
     struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);
     iir=read_reg(s->client_dev,(IIR<<3)&0xf9);
     ier=read_reg(s->client_dev,(IER<<3)&0xf9); 
     do{
	if((iir&RHR_RXOVT_INT)||(iir&RHR_INT)){
		wk2xxx_rx_chars(&s->port);
	}
	if((iir&THR_INT)&&(ier&THR_IER)){
	//uart_write_wakeup(&s->port);		 
	wk2xxx_tx_chars(&s->port);
		 return;	
	}
	if(pass_counter>ISR_PASS_LIMIT)
		break;
	iir=read_reg(s->client_dev,(IIR<<3)&0xf9);
     	ier=read_reg(s->client_dev,(IER<<3)&0xf9); 
	}while((iir&RHR_RXOVT_INT)||(iir&RHR_INT)||((iir&THR_INT)&&(ier&THR_IER)));
}

static int wk2xxx_dowork(struct wk2xxx_port *s)
{    
	if(DEBUG) printk("-i2c-wk2xxx_dowork---in---\n");

	if (!s->force_end_work&&!work_pending(&s->work)&& !s->suspending)
	{
		queue_work(s->workqueue, &s->work);
		return 1;	
	}

	return 0;

}



static void wk2xxx_work(struct work_struct *w)
{ 
       struct wk2xxx_port *s = container_of(w, struct wk2xxx_port, work);
	uint8_t rx;

        
	int work_tx_empty_flag;
	int work_start_tx_flag; 

	int work_stop_rx_flag;
	int work_stop_tx_flag;
	
	int work_irq_flag;
	//int work_irq_fail;
	int work_conf_flag;
	do {
        	mutex_lock(&wk2xxxs_lock);
	        //spin_lock(&s->conf_lock);
		work_tx_empty_flag = s->tx_empty_flag;
		if(work_tx_empty_flag)
		s->tx_empty_flag = 0;
        	work_start_tx_flag = s->start_tx_flag;
		if(work_start_tx_flag)
		s->start_tx_flag = 0;
		work_stop_tx_flag = s->stop_tx_flag;
		if(work_stop_tx_flag)
		s->stop_tx_flag = 0;
		work_stop_rx_flag = s->stop_rx_flag;
		if(work_stop_rx_flag)
		s->stop_rx_flag = 0;
		
		work_conf_flag = s->conf_flag;
		if(work_conf_flag)
		s->conf_flag = 0;


		work_irq_flag = s->irq_flag;
		if(work_irq_flag)
		s->irq_flag = 0;
		
		mutex_unlock(&wk2xxxs_lock);
	
		if(work_conf_flag)
		{
		conf_wk2xxx_subport(&s->port);
		}		
				
		if(work_tx_empty_flag){
		rx=read_reg(s->client_dev,(LSR<<3)&0xf9);
		s->tx_empty=(rx&LSR_0)<=0;
		// uart_circ_clear(&s->port.state->xmit);		
		}

		if(work_start_tx_flag){				 
		rx=read_reg(s->client_dev,(IER<<3)&0xf9);
		 write_reg(s->client_dev,(IER<<3)&0xf9,rx|THR_IER);//禁能THR中断		
		 
		}
        	if(work_stop_tx_flag)
        	{
		rx=read_reg(s->client_dev,(IER<<3)&0xf9);
		rx&=~THR_IER;
		//write_reg(s->client_dev,(IER<<3)&0xf9,rx);//禁能THR中断
		/*rx=read_reg(s->client_dev,(IIR<<3)&0xf9);
		rx&=~THR_INT;
		write_reg(s->client_dev,(IIR<<3)&0xf9,rx);//去除THR中断标识*/
        	}
		if(work_stop_rx_flag)
		{
		rx=read_reg(s->client_dev,(IER<<3)&0xf9);
		rx&=~RHR_IER;
		//write_reg(s->client_dev,(IER<<3)&0xf9,rx);//禁能RHR中断
		/*rx=read_reg(s->client_dev,(IIR<<3)&0xf9);
		rx&=~RHR_INT;
		write_reg(s->client_dev,(IIR<<3)&0xf9,rx);//禁能RHR中断*/
		}

		if(work_irq_flag)
		{
		     work_irq(&s->port);
		     s->irq_fail = 1;
		}

        }while (!s->force_end_work && \
		(work_irq_flag || work_stop_rx_flag || \
		 work_stop_tx_flag || work_tx_empty_flag || work_conf_flag));


	if(s->conf_fail)
        {
		conf_wk2xxx_subport(&s->port);
		s->conf_fail =0;
        }

		if(s->tx_empty_fail)
		{
        	rx=read_reg(s->client_dev,(LSR<<3)&0xf9);
		s->tx_empty=(rx&LSR_0)<=0;
		s->tx_empty_fail =0;
		}

        if(s->start_tx_fail)
        {
		rx=read_reg(s->client_dev,(LCR<<3)&0xf9);	
		write_reg(s->client_dev,(LCR<<3)&0xf9,rx&(~0x80));
		wk2xxx_tx_chars(&s->port);
		s->start_tx_fail =0;
        }
		
        if(s->stop_tx_fail)
        {
		rx=read_reg(s->client_dev,(IER<<3)&0xf9);
		rx&=~THR_INT;
		//write_reg(s->client_dev,(IER<<3)&0xf9,rx);//禁能THR中断
		s->stop_tx_fail =0;
		
        }
		
        if(s->stop_rx_fail)
        {
		rx=read_reg(s->client_dev,(IER<<3)&0xf9);
		rx&=~RHR_INT;
		//write_reg(s->client_dev,(IER<<3)&0xf9,rx);//禁能RHR中断
		s->stop_rx_fail =0;
        }
		
	if(s->irq_fail)
	{
		s->irq_fail = 0;
            	enable_irq(s->port.irq);
	}
	return;
}

static irqreturn_t wk2xxx_irq(int irq, void *dev_id)//
{
	struct wk2xxx_port *s = dev_id;
	if(DEBUG) printk("-i2c-wk2xxx_irq---in---\n");
	disable_irq_nosync(s->port.irq);
	if(wk2xxx_dowork(s))
	{
		s->irq_flag = 1;
	}
	else
	{
		//if(!s->irq_flag)
		s->irq_fail = 1;
	}	
	if(DEBUG) printk("-i2c-wk2xxx_irq---exit---\n");
	return IRQ_HANDLED;
}

static u_int wk2xxx_tx_empty(struct uart_port *port)// or query the tx fifo is not empty?
{
	struct wk2xxx_port *s = container_of(port,struct wk2xxx_port,port);

        if(DEBUG) printk( "vk32xx_tx_empty()---------in---\n");

        //s->tx_empty_flag = 1;
	if(!(s->tx_empty_flag || s->tx_empty_fail)) //tx_empty_flag为真时不会循环访问，两个都为假则再次进入访问
	{
        if(wk2xxx_dowork(s)) //排队成功则设置标志为真不用再次循环访问
	{
	        s->tx_empty_flag = 1;
	}
	else
	{
        	s->tx_empty_fail = 1;
	}
	}      

        if(DEBUG) printk( "wk2xxx_tx_empty----------exit---\n");

	return s->tx_empty;


}

static void wk2xxx_set_mctrl(struct uart_port *port, u_int mctrl)//nothing
{
if(DEBUG) printk( "i2c-wk2xxx_set_mctrl---------exit---\n");
}
static u_int wk2xxx_get_mctrl(struct uart_port *port)// since no modem control line
{       
if(DEBUG) printk( "i2c-wk2xxx_get_mctrl---------exit---\n");


        return TIOCM_CTS | TIOCM_DSR | TIOCM_CAR;
}


/*
 *  interrupts disabled on entry
 */

static void wk2xxx_stop_tx(struct uart_port *port)//
{
    struct wk2xxx_port *s = container_of(port,struct wk2xxx_port,port);
    if(DEBUG) printk( "i2c-wk2xxx_stop_tx------in---\n");
	//s->stop_tx_flag=1;    
	if(!(s->stop_tx_flag||s->stop_tx_fail))
	{
		if(wk2xxx_dowork(s))
		{
			s->stop_tx_flag =1;
		}
		else
		{
			s->stop_tx_fail =1;
		}
	}

if(DEBUG) printk( "i2c-wk2xxx_stop_tx------exit---\n");
		
}

/*
 *  * interrupts may not be disabled on entry
*/
static void wk2xxx_start_tx(struct uart_port *port)
{	
        struct wk2xxx_port *s = container_of(port,struct wk2xxx_port,port);   
	if(DEBUG) printk("i2c-wk2xxx_start_tx------in---\n");
	//s->start_tx_flag = 1;
		if(!(s->start_tx_flag||s->start_tx_fail))
		{
			if(wk2xxx_dowork(s))
			{
			s->start_tx_flag = 1;
			}
			else
			{
			s->start_tx_fail = 1;
			}
		}

		if(DEBUG) printk( "i2c-wk2xxx_start_tx------exit---\n");

}

/*
 *  * Interrupts enabled
*/

static void wk2xxx_stop_rx(struct uart_port *port)
{
        struct wk2xxx_port *s = container_of(port,struct wk2xxx_port,port);
	if(DEBUG) printk("i2ci2c-wk2xxx_stop_rx------in---\n");
	//s->stop_rx_flag=1; 
	if(!(s->stop_rx_flag ||s->stop_rx_fail ))
		{
        	if(wk2xxx_dowork(s))
			{
				s->stop_rx_flag = 1;
			}
			else
			{
				s->stop_rx_fail = 1;
			}
		}
if(DEBUG) printk( "i2c-wk2xxx_stop_rx------exit---\n");


}


/*
 *  * No modem control lines
 *   */
static void wk2xxx_enable_ms(struct uart_port *port)    //nothing
{
if(DEBUG) printk( "i2c-wk2xxx_enable_ms------exit---\n");


}
/*
 *  * Interrupts always disabled.
*/   
static void wk2xxx_break_ctl(struct uart_port *port, int break_state)
{
if(DEBUG) printk( "i2c-wk2xxx_break_ctl------exit---\n");


	//break operation, but there  seems no such operation in vk32  
}


static int wk2xxx_startup(struct uart_port *port)//i
{
	struct wk2xxx_port *wk2xxxs = container_of(port,struct wk2xxx_port,port);
	//write_reg(wk2xxxs->client_dev,(IER<<3)&0Xf9,0x0f);
	if(DEBUG) printk("i2c wk2xxx_startup\n");
	//将startup里面的代码放到了probe中
	//sc752_ini(wk2xxxs->client_dev);
	//uart_circ_clear(&wk2xxxs->port.state->xmit);
	wk2xxxs->force_end_work=0;
	write_reg(wk2xxxs->client_dev,(IER<<3)&0xf9,0);
	write_reg(wk2xxxs->client_dev,(IER<<3)&0xf9,0xcf);//使能中断模式操作
	return 0;
}
//* Power down all displays on reboot, poweroff or halt *

static void wk2xxx_shutdown(struct uart_port *port)//
{
    // struct wk2xxx_port *s = container_of(port,struct wk2xxx_port,port);
	if(DEBUG) printk( "i2c-wk2xxx_shutdown------in---\n");	
	//write_reg(s->client_dev,(IER<<3)&0xf9,0x0);//使能中断模式操作
	//s->force_end_work=1;
	if(DEBUG) printk( "i2c-wk2xxx_shutdown-----exit---\n");
        return;

}

static void conf_wk2xxx_subport(struct uart_port *port)//i
{	
    	struct wk2xxx_port *s = container_of(port,struct wk2xxx_port,port);
	uint8_t lcr,scr_ss,baud0_ss,baud1_ss,pres_ss;

	if(DEBUG) printk( "-i2c--conf_wk2xxx_subport------in---\n");
	lcr = s->new_lcr;
	scr_ss = s->new_scr;
	baud0_ss=s->new_baud0;
	baud1_ss=s->new_baud1;
	pres_ss=s->new_pres;
    	/*dat=read_reg(s->client_dev,(IER<<3)&0xf9);
    	//write_reg(s->client_dev,(IER<<3)&0xf9,dat&(~(RHR_INT | THR_INT)));
        //local_irq_restore(flags);
        // do{
        //   dat=read_reg(s->client_dev,(LSR<<3)&0xf9);
        //} while (dat & WK2XXX_TBUSY);
        // then, disable everything
        fcr=read_reg(s->client_dev,(FCR<<3)&0xf9);        
	fcr &= 0x0f;
        fcr |= scr_ss;*/
	
        //write_reg(s->client_dev,(FCR<<3)&0xf9,fcr&(~(0x06)));
	//udelay(2);
        // set the parity, stop bits and data size //
        // set the baud rate //
        //write_reg(s->client_dev,(IER<<3)&0xf9,old_sier);
	write_reg(s->client_dev, (LCR<<3)&0xf9,0x80|lcr);
	write_reg(s->client_dev,(DLL<<3)&0xf9 ,baud0_ss);
	write_reg(s->client_dev,(DLH<<3)&0xf9 ,baud1_ss);
	write_reg(s->client_dev, (LCR<<3)&0xf9,0x7f&lcr);	
	//write_reg(s->client_dev, (FCR<<3)&0xf9,0x07);
	udelay(200);
       // write_reg(s->client_dev,(IER<<3)&0xf9,0x0f);
}


// change speed
static void wk2xxx_termios( struct uart_port *port, struct ktermios *termios,
		    struct ktermios *old)
{
    	struct wk2xxx_port *s = container_of(port,struct wk2xxx_port,port);
	int baud = 0;
	uint8_t lcr,baud1,baud0,pres;
	u8 lsr=0;
	unsigned short cflag;
	unsigned short lflag;
	//u32 param_new, param_mask;
	cflag = termios->c_cflag;
	lflag = termios->c_lflag;
	
	baud1=0;
	baud0=0;
	pres=0;
	baud = tty_termios_baud_rate(termios);
	if(DEBUG) printk( "-i2c-vk32xx_termios---cflag=%d,lflag=%d,baud=%d---in---\n",cflag,lflag,baud);

	switch (baud) {
	case 1200:
		baud1=0x04;
    		baud0=0xe2;
    		pres=0;
		 break;		   
        case 1800:
		baud1=0x03;
		baud0=0x41;
		pres=0;
            	break;
        case 2000:
		baud1=0x02;
		baud0=0xee;
		pres=0;
            	break;
        case 2400:
		baud1=0x02;
		baud0=0x71;
		pres=0;
            	break;
        case 3600:
		baud1=0x01;
		baud0=0xa0;
		pres=0;
            	break;
        case 4800:
		baud1=0x01;
		baud0=0x38;
		pres=0;
            	break;
        case 7200:
		baud1=0x00;
		baud0=0xd0;
		pres=0;
                break;
	case 9600:
		baud1=0x00;
		baud0=0x9c;
		pres=0;
		break;
	case 19200:
		baud1=0x00;
		baud0=0x4e;
		pres=0;
		break;
	case 115200:
		baud1=0x00;
		baud0=0x0d;
		pres=0;
		break;
	default:
		baud1=0x00;
		baud0=0x00;
		pres=0;
	}
	//if(mcr&0x80){ baud1=baud1>>2; baud0=baud0>>2; }
	tty_termios_encode_baud_rate(termios, baud, baud);

	//s->port.state->port.tty->port->low_latency = 1;
	termios->c_lflag &= ~ECHO;

	lcr =0;
       
        if (!(cflag & CSTOPB))
 		lcr&=~WK2XXX_STPL;//one  stop_bits
        else
		lcr|=WK2XXX_STPL;//two  stop_bits
                        
        
        if (cflag & PARENB) {
                lcr|=WK2XXX_PAEN;//enbale spa
                if (!(cflag & PARODD)){
                        lcr |= WK2XXX_PAM1;//PAM0=1
                        lcr |= WK2XXX_PAM0;//PAM1=0
                }
                else{
                        lcr |= WK2XXX_PAM0;
                        lcr &= ~WK2XXX_PAM0;
                }
        }
        else{
                lcr&=~WK2XXX_PAEN;
        }

	if ((cflag&CSIZE)==CS8){
		lcr&=0xfc;
		lcr|=0x03;
	}else if((cflag&CSIZE)==CS5){
		lcr&=0xfc;
	}else if((cflag&CSIZE)==CS6){
		lcr&=0xfc;
		lcr|=0x01;
	}else if((cflag&CSIZE)==CS7){
		lcr&=0xfc;
		lcr|=0x02;
	}


	
	s->new_baud1=baud1;
	s->new_baud0=baud0;
	s->new_pres=pres;

	s->new_lcr = lcr;
	s->new_scr = lsr;

	/*termios->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
	termios->c_oflag &= ~OPOST;
	termios->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	termios->c_cflag &= ~(CSIZE | PARENB);
	termios->c_cflag |= CS8;*/

        // bugfix: remove ICANON by hehj
	termios->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
        // bugfix: 0x0d be transformed to 0x0a, hehj
	termios->c_iflag &= ~ICRNL;

	if(!(s->conf_flag|| s->conf_fail))
	{
	if(wk2xxx_dowork(s))
	{
		s->conf_flag =1;
	}
	else
	{
		s->conf_fail =1;
	}
	}
	//conf_wk2xxx_subport(&s->port);
	 return ;
}


static const char *wk2xxx_type(struct uart_port *port)
{



        if(DEBUG) printk( "wk2xxx_type-------------out-------- \n");

        return port->type == PORT_WK2XXX ? "wk2xxx" : NULL;//this is defined in serial_core.h
}

/*
* Release the memory region(s) being used by 'port'.
*/
static void wk2xxx_release_port(struct uart_port *port)
{
        if(DEBUG) printk( "wk2xxx_release_port\n");
}

/*
* Request the memory region(s) being used by 'port'.
*/
static int wk2xxx_request_port(struct uart_port *port)//no such memory region needed for vk32
{
        if(DEBUG) printk( "wk2xxx_request_port\n");
        return 0;
}

static void set_gpio(int val,int count){
	if(DEBUG) printk("******i2c REQUEST GPIO \"%s\"!\n", (gpio_request(val, "routon_test_gpio") == 0) ? "success" : "failed");
	gpio_direction_output(val, count);
	gpio_set_value(val, count);
	gpio_free(val);
}

static int wk2xxx_ioctl(struct uart_port *port,unsigned int i,unsigned long j)//no such memory region needed for vk32
{
    struct wk2xxx_port *s = container_of(port,struct wk2xxx_port,port);
    switch(i){
        case PWR_SCAN:	
            set_gpio(87,1);
            set_gpio(82,0);
            set_gpio(82,1);
            set_gpio(83,1);
            set_gpio(83,0);
            break;
        case START_SCAN:
            set_gpio(83,1);
            set_gpio(83,0);
            break;
        case PWR_DOWN:	
            set_gpio(83,1);
            set_gpio(82,0);
            set_gpio(87,0);
            break;

        case TIOCPKT:
            s->keyevent_enable = j;
            printk("hehj ioctl keyevent_enable: %ld\n", j);
            break;

        default:
            break;	
    }
    return 0;
}

/*
* Configure/autoconfigure the port*/
static void wk2xxx_config_port(struct uart_port *port, int flags)
{
        struct wk2xxx_port *s = container_of(port,struct wk2xxx_port,port);


        if(DEBUG) printk( "wk2xxx_config_port \n");


        if (flags & UART_CONFIG_TYPE && wk2xxx_request_port(port) == 0)
               s->port.type = PORT_WK2XXX;
}

/*
* Verify the new serial_struct (for TIOCSSERIAL).
* The only change we allow are to the flags and type, and
* even then only between PORT_vk32xx and PORT_UNKNOWN
*/
static int wk2xxx_verify_port(struct uart_port *port, struct serial_struct *ser)
{
        int ret = 0;
 	if(DEBUG) printk( "wk2xxx_verify_port \n");
        if (ser->type != PORT_UNKNOWN && ser->type != PORT_WK2XXX)
                ret = -EINVAL;
        if (port->irq != ser->irq)
                ret = -EINVAL;
        if (ser->io_type != SERIAL_IO_PORT)
                ret = -EINVAL;
        if (port->iobase != ser->port)
                ret = -EINVAL;
        if (ser->hub6 != 0)
                ret = -EINVAL;
        return ret;
}

static struct uart_ops wk2xxx_pops = {
        tx_empty:       wk2xxx_tx_empty,
        set_mctrl:      wk2xxx_set_mctrl,
        get_mctrl:      wk2xxx_get_mctrl,
        stop_tx:        wk2xxx_stop_tx,
        start_tx:       wk2xxx_start_tx,
        stop_rx:        wk2xxx_stop_rx,
        enable_ms:      wk2xxx_enable_ms,
        break_ctl:      wk2xxx_break_ctl,
        startup:        wk2xxx_startup,
        shutdown:       wk2xxx_shutdown,
	ioctl:		wk2xxx_ioctl,
	set_termios:    wk2xxx_termios,
        type:           wk2xxx_type,
        release_port:   wk2xxx_release_port,
        request_port:   wk2xxx_request_port,
        config_port:    wk2xxx_config_port,
        verify_port:    wk2xxx_verify_port,
};

static struct wk2xxx_port* wk2xxxs; /* the chips */

static unsigned char keycode[] = {
	KEY_1,KEY_2,KEY_3,KEY_4, KEY_5, KEY_6,KEY_7,KEY_8,KEY_9,KEY_0,KEY_A,KEY_B,  		// B
	KEY_C, KEY_D, KEY_E,KEY_F,KEY_G,KEY_H,KEY_I,KEY_J,KEY_K,KEY_L,KEY_M,KEY_N,  		// N
	KEY_O,KEY_P,KEY_Q,KEY_R,KEY_S,KEY_T,KEY_U,KEY_V,KEY_W,KEY_X,KEY_Y,KEY_Z,  		// Z
	KEY_GRAVE,KEY_MINUS,KEY_EQUAL, KEY_LEFTBRACE,KEY_RIGHTBRACE,KEY_SEMICOLON, 		// ;, :
	KEY_APOSTROPHE,KEY_BACKSLASH,KEY_COMMA,KEY_ENTER,KEY_LEFTSHIFT,KEY_DOT,KEY_SLASH,KEY_SPACE 			// OK, ENTER
};


static struct uart_driver wk2xxx_uart_driver = {
        owner:                  THIS_MODULE,
        major:        		SERIAL_I2C_TTY_MAJOR,
        driver_name:            "ttyIIC",
        dev_name:               "ttyI2C",
        minor:                  SERIAL_TTY_MINORS,
        nr:                     1,
        cons:                   NULL//WK2Xxx_CONSOLE,
};

static int i2c_serial_probe(struct i2c_client * client,const struct i2c_device_id *id){
          int result,i;
          struct device_node *np = client->dev.of_node;	
	 // struct resource *res;
          if(DEBUG) printk("%s\n", __FUNCTION__);
          if (!np) {
                   dev_err(&client->dev, "no device tree\n");
                   return -EINVAL;      
          }
	 
         wk2xxxs=kzalloc(sizeof(*wk2xxxs), GFP_KERNEL);
	 if(wk2xxxs==NULL){
             if(DEBUG) printk("%s:Alloc GFP_KERNEL memory failed.",__func__);
             return -ENOMEM; 
         }
         memset(wk2xxxs,0,sizeof(*wk2xxxs));
	 mutex_lock(&wk2xxxs_lock);
	 result=uart_register_driver(&wk2xxx_uart_driver);
	 if(result){
         	if(DEBUG) printk("Couldn't register wk2xxx uart driver\n");
         	mutex_unlock(&wk2xxxs_lock);
 		goto exit_tty;
         	return result;
	 }
	 wk2xxxs->client_dev=client;
         wk2xxxs->irq_pin=of_get_named_gpio_flags(np, "irq_i2c_gpio", 0, (enum of_gpio_flags *)(&wk2xxxs->irq_flags));
         wk2xxxs->rst_pin=of_get_named_gpio_flags(np, "rst_i2c_gpio", 0, (enum of_gpio_flags *)(&wk2xxxs->rst_flags));
	 //udelay(200);
	 wk2xxxs->tx_done=0; 

         wk2xxxs->keyevent_enable = 1;

	 wk2xxxs->port.line=0;
	 wk2xxxs->port.ops=&wk2xxx_pops;
	 wk2xxxs->port.uartclk=WK_CRASTAL_CLK;
	 wk2xxxs->port.fifosize=64;
	 wk2xxxs->port.iobase =0x4d;
	 //wk2xxxs->port.mapbase =0x4d;
	 //wk2xxxs->port.regshift =2;
	 wk2xxxs->port.iotype = SERIAL_IO_PORT;
	 wk2xxxs->port.flags = UPF_BOOT_AUTOCONF;
	 wk2xxxs->port.irq=gpio_to_irq(wk2xxxs->irq_pin);
         result = uart_add_one_port(&wk2xxx_uart_driver, &(wk2xxxs->port));
         if(result<0){
	 	mutex_unlock(&wk2xxxs_lock);
		goto exit_uart_port;
           	if(DEBUG) printk("uart_add_one_port failed for line i:= %d with error %d\n",i,result);
	}
	if(DEBUG) printk("i2c uart_add_one_port = 0x%d\n",result);
	mutex_unlock(&wk2xxxs_lock);
	sc752_ini(client);
	wk2xxxs->workqueue = create_singlethread_workqueue("ttyiic5");
	if(!wk2xxxs->workqueue){
            if(DEBUG) printk("i2c5 cannot create workqueue\n");
            return -EBUSY;
        }
	if(DEBUG) printk("i2c uart_port startup\n");
	INIT_WORK(&wk2xxxs->work, wk2xxx_work);
	if (wk2xxxs->wk2xxx_hw_suspend)
          wk2xxxs->wk2xxx_hw_suspend(0);
		
	uart_circ_clear(&wk2xxxs->port.state->xmit);
	
	if(DEBUG) printk("i2c request irq\n");
	if(request_irq(wk2xxxs->port.irq, wk2xxx_irq,IRQF_SHARED|IRQF_TRIGGER_LOW,"wkxxxiic", wk2xxxs)< 0){
                //为中断引脚申请中断
                wk2xxxs->port.irq = 0;
                destroy_workqueue(wk2xxxs->workqueue);
                wk2xxxs->workqueue = NULL;
                return -EBUSY;
        }
	

	if(DEBUG) printk(KERN_ERR "i2c registerdev start\n");
	wk2xxxs->input_keys = input_allocate_device(); /*分配一个设备结构体*/  
        if (!wk2xxxs->input_keys) /*判断分配是否成功*/  
        {
                if(DEBUG) printk(KERN_ERR "i2c button.c: Not enough memory\n");  
                result = -ENOMEM;  
                goto err_free_dev;  
        }
  
	wk2xxxs->input_keys->name="sc16is750";
	wk2xxxs->input_keys->phys="/dev/input/inputsc16is750";
	wk2xxxs->input_keys->hint_events_per_packet=512; 
	wk2xxxs->input_keys->evbit[0] = BIT_MASK(EV_KEY) | BIT(EV_SYN);/*设置按键信息*/  
        set_bit(EV_REP,wk2xxxs->input_keys->evbit);	
	for(i = 0; i < sizeof(keycode)/sizeof(unsigned char); i++){
		set_bit(keycode[i],wk2xxxs->input_keys->keybit);
		//input_set_capability(wk2xxxs->input_keys,EV_KEY,keycode[i]);	
	}
	if(DEBUG) printk(KERN_ERR "i2c registerdev end\n");
	wk2xxxs->input_keys->id.bustype=BUS_I2C;

	//fengmc  delete 171215
#if 0
	wk2xxxs->input_keys->id.bustype=BUS_I2C;
	result = input_register_device(wk2xxxs->input_keys); /*注册一个输入设备*/  
         if (result)  
        {
		input_free_device(wk2xxxs->input_keys);
		input_unregister_device(wk2xxxs->input_keys);
                if(DEBUG) printk(KERN_ERR "i2c button.c: Failed to register device\n");  
                goto err_unregister;  
        }
#endif   
	//end 
	write_reg(wk2xxxs->client_dev,(IER<<3)&0Xf9,0x0f);//使能中断模式操作
        return 0;
	//fengmc delete 171215
#if 0 
err_unregister:
	input_unregister_device(wk2xxxs->input_keys);
#endif
	//end
err_free_dev:  /*以下是错误处理*/  
        input_free_device(wk2xxxs->input_keys);
  
exit_uart_port:
	uart_remove_one_port(&wk2xxx_uart_driver,&(wk2xxxs->port));
exit_tty:
         uart_unregister_driver(&wk2xxx_uart_driver);

 return result;	 
}


static int i2c_serial_remove(struct i2c_client *i2c){
	//int result;
	struct wk2xxx_port *s = container_of(&i2c,struct wk2xxx_port,client_dev);	
	//fengmc delete 171215
#if 0
	input_unregister_device(s->input_keys);  //fengmc注销
#endif	
	//end
	if (s->port.irq)
	{
        	disable_irq_nosync(s->port.irq);		
		free_irq(s->port.irq,s);
	}
	s->force_end_work=1;
	if (s->workqueue) 
	{
		flush_workqueue(s->workqueue);
		destroy_workqueue(s->workqueue);
		s->workqueue = NULL;
	}
	
	
  return 0;
}



static const struct i2c_device_id i2c_serial_id[] = {
         {"sc16is750", 0},
         {},
};
MODULE_DEVICE_TABLE(i2c, i2c_serial_id);

static const struct of_device_id i2c_serial_of_match[] = {
        {
                    .compatible = "sc16is750_760",
                    .data = NULL,
        },
        { },
};
MODULE_DEVICE_TABLE(of, i2c_serial_of_match);


static struct i2c_driver i2c_serial_driver = {
         .driver = {
                 .name = "sc16is750",
                 .owner = THIS_MODULE,
                 .of_match_table =of_match_ptr(i2c_serial_of_match),
         },
         .probe = i2c_serial_probe,
         .remove = i2c_serial_remove,
         .id_table = i2c_serial_id,
 };
 
 static int __init i2c_serial_init(void)
 {
         int ret;
 	 
         if(DEBUG) printk("%s\n", __FUNCTION__);
         ret = i2c_add_driver(&i2c_serial_driver);
         if (ret != 0)
                 pr_err("Failed to register ts I2C driver: %d\n", ret);
         return ret;
 }
 module_init(i2c_serial_init);
 
 static void __exit i2c_serial_exit(void)
 {
        int result;
	if(DEBUG) printk("%s\n", __FUNCTION__);
	result=uart_remove_one_port(&wk2xxx_uart_driver,&(wk2xxxs->port));
	if(result<0){
           	if(DEBUG) printk("uart_remove_one_port failed with error %d\n",result);
	}
	 mutex_lock(&wk2xxxs_lock);
	uart_unregister_driver(&wk2xxx_uart_driver);
	if(result<0){
           	if(DEBUG) printk("Couldn't unregister wk2xxx uart driver\n");
	}
 	mutex_unlock(&wk2xxxs_lock);
     i2c_del_driver(&i2c_serial_driver);
 }
 module_exit(i2c_serial_exit);

 MODULE_LICENSE("Dual BSD/GPL");
 MODULE_LICENSE("GPL");
