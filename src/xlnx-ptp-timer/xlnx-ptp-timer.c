/*
 * Xilinx PTP : Linux driver for 1588 timer
 *
 * Author: Xilinx, Inc.
 *
 * 2014 (c) Xilinx, Inc. This file is licensed uner the terms of the GNU
 * General Public License version 2. This program is licensed "as is"
 * without any warranty of any kind, whether express or implied.
 *
 */
#include <linux/device.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ptp_clock_kernel.h>
#include <linux/platform_device.h>
#include <linux/of_irq.h>

#include "xlnx-ptp-timer.h"
#define DRIVER_NAME                    "xlnx_ptp_timer"

// Warning if you set this you will experience jitter on the clock
// only do when debuging the PTP flow an remove forr production
//#define XTIMER1588_ENABLE_DEBUG_MESS

/* Register offset definitions */

#define XTIMER1588_RTC_OFFSET_NS        0x00000 /* RTC Nanoseconds Field Offset Register */
#define XTIMER1588_RTC_OFFSET_SEC_L     0x00008 /* RTC Seconds Field Offset Register - Low */
#define XTIMER1588_RTC_OFFSET_SEC_H     0x0000C /* RTC Seconds Field Offset Register - High */
#define XTIMER1588_RTC_INCREMENT        0x00010 /* RTC Increment */
#define XTIMER1588_CURRENT_RTC_NS       0x00014 /* Current TOD Nanoseconds - RO */
#define XTIMER1588_CURRENT_RTC_SEC_L    0x00018 /* Current TOD Seconds -Low RO  */
#define XTIMER1588_CURRENT_RTC_SEC_H    0x0001C /* Current TOD Seconds -High RO */
#define XTIMER1588_INTERRUPT            0x00020 /* Write to Bit 0 to clear the interrupt */
#define XTIMER1588_8KPULSE              0x00024 /* 8kHz Pulse Offset Register */
#define XTIMER1588_CF_L                 0x0002C /* Correction Field - Low */
#define XTIMER1588_CF_H                 0x00030 /* Correction Field - Low */

#define XTIMER1588_RTC_MASK            ((1 << 26) - 1)
#define XTIMER1588_INT_SHIFT           0
#define NANOSECOND_BITS                20
#define NANOSECOND_MASK                ((1 << NANOSECOND_BITS) - 1)
#define SECOND_MASK                    ((1 << (32 - NANOSECOND_BITS)) - 1)
#define XTIMER1588_RTC_INCREMENT_SHIFT 20
#define PULSESIN1PPS                   128

struct xlnx_ptp_timer {
  struct                 device *dev;
  void __iomem          *baseaddr;
  struct ptp_clock      *ptp_clock;
  struct ptp_clock_info  ptp_clock_info;
  spinlock_t             reg_lock;
  int                    irq;
  int                    pps_enable;
  int                    countpulse;
  unsigned int           period;
};

static void xlnx_tod_read(struct xlnx_ptp_timer *timer, struct timespec64 *ts)
{
  u32 sec, nsec;
  nsec = in_be32(timer->baseaddr + XTIMER1588_CURRENT_RTC_NS);
  sec = in_be32(timer->baseaddr + XTIMER1588_CURRENT_RTC_SEC_L);

#ifdef XTIMER1588_ENABLE_DEBUG_MESS
  printk(KERN_INFO "ptp_xlnx_1588:xlnx_tod_read=%d.%09d\n", sec, nsec);
#endif

  ts->tv_sec = 0; // Clear to be safe. Change sec read to 64 above.
  ts->tv_sec = (u64) sec; // Linux now uses timespec64 as the function res
  ts->tv_nsec = nsec;
}

static void xlnx_rtc_offset_write(struct xlnx_ptp_timer *timer,
          const struct timespec64 *ts)
{

  out_be32((timer->baseaddr + XTIMER1588_RTC_OFFSET_SEC_H), (u32) ((ts->tv_sec>>32)&0xFFFFFFFF) );
  out_be32((timer->baseaddr + XTIMER1588_RTC_OFFSET_SEC_L), (u32) ((ts->tv_sec    )&0xFFFFFFFF) );
  out_be32((timer->baseaddr + XTIMER1588_RTC_OFFSET_NS),    (u32) ts->tv_nsec );

  // printk(KERN_INFO "xlnx_rtc_offset_write:ts.tv_sec:%lld, ts.tv_nsec:%d \n",ts->tv_sec,ts->tv_nsec);
}

static void xlnx_rtc_offset_read(struct xlnx_ptp_timer *timer,
         struct timespec64 *ts)
{
  ts->tv_sec  =  in_be32(timer->baseaddr + XTIMER1588_RTC_OFFSET_SEC_H);
  ts->tv_sec  =  (ts->tv_sec<<32);
  ts->tv_sec  += in_be32(timer->baseaddr + XTIMER1588_RTC_OFFSET_SEC_L);
  ts->tv_nsec =  (u64) in_be32(timer->baseaddr + XTIMER1588_RTC_OFFSET_NS);
}

static int xlnx_ptp_gettime(struct ptp_clock_info *ptp, struct timespec64 *ts)
{
  unsigned long flags;
  struct xlnx_ptp_timer *timer = container_of(ptp,
    struct xlnx_ptp_timer, ptp_clock_info);

  spin_lock_irqsave(&timer->reg_lock, flags);

  xlnx_tod_read(timer, ts);

  spin_unlock_irqrestore(&timer->reg_lock, flags);

#ifdef XTIMER1588_ENABLE_DEBUG_MESS
  printk(KERN_INFO "ptp_xlnx_1588:gettime ts.tv_sec:%d, ts.tv_nsec:%d\n",ts->tv_sec,ts->tv_nsec);
#endif

  return 0;
}

/*
 * PTP clock operations
 */
/*
 61  * @adjfreq:  Adjusts the frequency of the hardware clock.
 62  *            parameter delta: Desired frequency offset from nominal frequency
 63  *            in parts per billion
*/
static int xlnx_ptp_adjfreq(struct ptp_clock_info *ptp, s32 ppb)
{
  struct xlnx_ptp_timer *timer = container_of(ptp,
  struct xlnx_ptp_timer, ptp_clock_info);

  u32 preWr, postWr;

  int neg_adj;
  u64 freq;
  u32 diff, incval;

  neg_adj = 0;
  preWr = in_be32(timer->baseaddr + XTIMER1588_RTC_INCREMENT);

  /*
   * Convert the frequency from the device tree in picoseconds
   * into units of 1/(2^20) ns
   * The / 1000 is a bit nasty. There might be a more optimal way of
   * doing this calculation
   */
  incval = div_u64((u64) timer->period * (1 << 20), 1000000ULL);

  if (ppb < 0) {
    neg_adj = 1;
    ppb = -ppb;
  }

  freq = incval;
  freq *= ppb;
  diff = div_u64(freq, 1000000000ULL);

  incval = neg_adj ? (incval - diff) : (incval + diff);
  out_be32((timer->baseaddr + XTIMER1588_RTC_INCREMENT), incval);

  postWr = in_be32(timer->baseaddr + XTIMER1588_RTC_INCREMENT);

#ifdef XTIMER1588_ENABLE_DEBUG_MESS
  printk(KERN_INFO "ptp_xlnx_1588:adjfreq adjust frq by :%u Diff=%u Neg=%d TimerPeriod=%u %u %u\n", 
    incval, 
    diff, 
    neg_adj, 
    timer->period,
    preWr,
    postWr
  );
#endif

  return 0;
}

static int xlnx_ptp_adjtime(struct ptp_clock_info *ptp, s64 delta)
{
  unsigned long flags;
  struct xlnx_ptp_timer *timer = container_of(ptp, 
    struct xlnx_ptp_timer,
    ptp_clock_info);

  // Declare time structures
  struct timespec64 offset, then;
  #ifdef XTIMER1588_ENABLE_DEBUG_MESS
  struct timespec64 store;
  #endif
  
  // Get times for reporting
  u32 osec, onsec, nsec, nnsec;
  onsec = in_be32(timer->baseaddr + XTIMER1588_CURRENT_RTC_NS);
  osec = in_be32(timer->baseaddr + XTIMER1588_CURRENT_RTC_SEC_L);
  then = ns_to_timespec64(delta);
  
  // dev_info(timer->dev, "ptp_xlnx_1588:adjtime - delta: %lld\n",delta);
  // dev_info(timer->dev, "ptp_xlnx_1588:adjtime ts.tv_sec:%ld, ts.tv_nsec:%ld\n",then.tv_sec,then.tv_nsec);
  spin_lock_irqsave(&timer->reg_lock, flags);

        // The 1588 timer continually applies the nanosecond offset to the internal nanosecond timer to give the 
        // TOD nanosenonds. This means that any adjustment of the nanosecond offset register must be done by modifying 
        // relative to the value that is already in the offset register.
        // The Seconds are applied as a step offset to the current value in the TOD register. 
        // As we are performing an add to the value we read from the offset registers we must ensure that the 
        // second field is clear before we add the incoming delta to the offset.
  xlnx_rtc_offset_read(timer, &offset);
  offset.tv_sec = 0;  
  offset = timespec64_add(offset, then);

  //dev_info(timer->dev, "ptp_xlnx_1588:adjtime current offset ts.tv_sec:%ld, ts.tv_nsec:%ld\n",offset.tv_sec,offset.tv_nsec);
  // set offset to equal delta (in ns), i.e. the difference between the current reported time and our desired time
  //dev_info(timer->dev, "ptp_xlnx_1588:adjtime new offset ts.tv_sec:%ld, ts.tv_nsec:%ld\n",offset.tv_sec,offset.tv_nsec);
  
  // Copy for debug
  // store.tv_sec = offset.tv_sec;
  // store.tv_nsec = offset.tv_nsec;
  out_be32((timer->baseaddr + XTIMER1588_RTC_OFFSET_SEC_H), (u32) ((offset.tv_sec>>32) & 0xffffffff));
  out_be32((timer->baseaddr + XTIMER1588_RTC_OFFSET_SEC_L), (u32) ((offset.tv_sec    ) & 0xffffffff));
  out_be32((timer->baseaddr + XTIMER1588_RTC_OFFSET_NS), offset.tv_nsec );

  // xlnx_rtc_offset_write(timer, (const struct timespec64 *)&offset);
  //xlnx_rtc_offset_write(timer, (const struct timespec64 *)&then);
  spin_unlock_irqrestore(&timer->reg_lock, flags);

        //xlnx_ptp_gettime(ptp, &jjTemp);
  nnsec = in_be32(timer->baseaddr + XTIMER1588_CURRENT_RTC_NS);
  nsec = in_be32(timer->baseaddr + XTIMER1588_CURRENT_RTC_SEC_L);
  
  // Call the print after we have set the time
  #ifdef XTIMER1588_ENABLE_DEBUG_MESS
  printk(KERN_INFO "ptp_xlnx_1588:adjtime delta=%lld : ot=%u.%09u del=%lld.%09lld ooff=%lld.%09lld noff=%lld.%09lld nt=%u.%9u\n",

       delta,
       osec, onsec,
       then.tv_sec,   then.tv_nsec,
       store.tv_sec,  store.tv_nsec,
       offset.tv_sec, offset.tv_nsec,
       nsec, nnsec
       );

       printk(KERN_INFO "ptp_xlnx_1588:adjtime %uns %u_%us\n",
     in_be32(timer->baseaddr + XTIMER1588_CURRENT_RTC_NS),
     in_be32(timer->baseaddr + XTIMER1588_CURRENT_RTC_SEC_H),
     in_be32(timer->baseaddr + XTIMER1588_CURRENT_RTC_SEC_L)
         );
    #endif

  // printk(KERN_INFO "ptp_xlnx_1588:adjtime secSize=%d nsSize=%d\n",sizeof(offset.tv_sec), sizeof(offset.tv_nsec));

  return 0;
}

/**
 * xlnx_ptp_settime - Set the current time on the hardware clock
 * @ptp: ptp clock structure
 * @ts: ts timespec64 containing the new time for the cycle counter
 *
 * The seconds register is written first, then the nanosecond
 * The hardware loads the entire new value when a nanosecond register
 * is written
 **/
static int xlnx_ptp_settime(struct ptp_clock_info *ptp,
          const struct timespec64 *ts)
{
  struct xlnx_ptp_timer *timer = container_of(ptp,
    struct xlnx_ptp_timer,
    ptp_clock_info);
  struct timespec64 delta, ts_tmp;
  struct timespec64 tod;
  struct timespec64 tod_non64;
  struct timespec64 offset;
  unsigned long flags;

  ts_tmp.tv_sec = (u32) ts->tv_sec;
  ts_tmp.tv_nsec = ts->tv_nsec;

  spin_lock_irqsave(&timer->reg_lock, flags);

  /* First zero the offset */
  offset.tv_sec = 0;
  offset.tv_nsec = 0;
  xlnx_rtc_offset_write(timer, &offset);

  /* Get the current timer value */
  xlnx_tod_read(timer, &tod);
  
  tod_non64.tv_sec  = (int)  tod.tv_sec;
  tod_non64.tv_nsec = (long) tod.tv_nsec;

  /* Subtract the current reported time from our desired time */
  // delta = timespec_sub((struct timespec64)*ts, tod);
  delta = timespec64_sub(ts_tmp, tod_non64);

  /* Don't write a negative offset */
  if (delta.tv_sec <= 0) {
  //dev_info(timer->dev, "ptp_xlnx_1588:settime negative offset\n");
    delta.tv_sec = 0;
    if (delta.tv_nsec < 0) {
      delta.tv_nsec = 0;
    }
  }

  xlnx_rtc_offset_write(timer, &delta);
  spin_unlock_irqrestore(&timer->reg_lock, flags);

  //Call the print after we have set the time
#ifdef XTIMER1588_ENABLE_DEBUG_MESS
  printk(KERN_INFO "ptp_xlnx_1588:settime tod=%6u.%u ts=%6u.%u delta=%6u.%u tmTmp=%6u.%u | TSnsHex=0x08x\n",
    tod.tv_sec,  tod.tv_nsec, 
    ts->tv_sec,   ts->tv_nsec,
    ts_tmp.tv_sec,   ts_tmp.tv_nsec,
    delta.tv_sec, delta.tv_nsec,
    ts->tv_nsec
    );
#endif
  return 0;
}

static int xlnx_ptp_enable(struct ptp_clock_info *ptp,
        struct ptp_clock_request *rq, int on)
{
  struct xlnx_ptp_timer *timer = container_of(ptp,
    struct xlnx_ptp_timer,
    ptp_clock_info);

#ifdef XTIMER1588_ENABLE_DEBUG_MESS
  printk(KERN_ALERT "ptp_xlnx_1588: ENABLE PTP %ld\n", 0);
#endif

  switch (rq->type) {
    case PTP_ENABLE_PPS:
      timer->pps_enable = 1;
      return 0;
    default:
      break;
  }

  return -EOPNOTSUPP;
}

static struct ptp_clock_info xlnx_ptp_clock_info = {
  .owner    = THIS_MODULE,
  .name    = "Xilinx Timer",
  .max_adj  = 999999999,
  .n_ext_ts  = 0,
  .pps     = 1,
  .adjfreq  = xlnx_ptp_adjfreq,
  .adjtime  = xlnx_ptp_adjtime,
  .gettime64  = xlnx_ptp_gettime,
  .settime64  = xlnx_ptp_settime,
  .enable    = xlnx_ptp_enable,
};

/* module operations */

/**
 * xlnx_ptp_timer_isr - Interrupt Service Routine
 * @irq:               IRQ number
 * @priv:              pointer to the timer structure
 *
 * Returns: IRQ_HANDLED for all cases
 *
 * Handles the timer interrupt. The timer interrupt fires 128 times per
 * secound. When our count reaches 128 emit a PTP_CLOCK_PPS event
 */
static irqreturn_t xlnx_ptp_timer_isr(int irq, void *priv)
{
  struct xlnx_ptp_timer *timer = priv;
  struct ptp_clock_event event;

  event.type = PTP_CLOCK_PPS;
  ++timer->countpulse;
  if (timer->countpulse >= PULSESIN1PPS) {
    timer->countpulse = 0;
    if ((timer->ptp_clock) && (timer->pps_enable)) {
      ptp_clock_event(timer->ptp_clock, &event);
    }
  }
out:
  out_be32((timer->baseaddr + XTIMER1588_INTERRUPT),  (1 << XTIMER1588_INT_SHIFT) );
  return IRQ_HANDLED;
}

static int xlnx_ptp_timer_remove(struct platform_device *pdev)
{
  struct xlnx_ptp_timer *timer = platform_get_drvdata(pdev);
  free_irq(timer->irq, timer->dev);
  if (timer->ptp_clock) {
    ptp_clock_unregister(timer->ptp_clock);
    timer->ptp_clock = NULL;
  }
  return 0;
}

static int xlnx_ptp_timer_request_irq(struct xlnx_ptp_timer *timer)
{
  int err;
  /* Enable interrupts */
  err = request_irq(timer->irq,
        xlnx_ptp_timer_isr,
        0,
        DRIVER_NAME,
        timer);
  if (err) {
    return err;
  }
  dev_info(timer->dev, "Acquired ptp_irq: 0x%x\n", timer->irq);
  return 0;
}

static int xlnx_ptp_timer_probe(struct platform_device *pdev)
{
  struct xlnx_ptp_timer *timer;
  struct resource *r_mem;
  int err = 0;
  u32 value;

  dev_info(&pdev->dev, "----------------------- Loading xlnx_ptp_timer\n");

  timer = devm_kzalloc(&pdev->dev, sizeof(*timer), GFP_KERNEL);
  if (!timer) {
    dev_err(&pdev->dev, "could not allocated memory for private data\n");
    return -ENOMEM;
  }

  timer->dev = &pdev->dev;

  r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
  if (!r_mem) {
    dev_err(&pdev->dev, "no IO resource defined\n");
    return -ENXIO;
  }

  timer->baseaddr = devm_ioremap_resource(&pdev->dev, r_mem);
  if (IS_ERR(timer->baseaddr)) {
    err = PTR_ERR(timer->baseaddr);
    return err;
  }

  timer->irq = irq_of_parse_and_map(pdev->dev.of_node, 0);
  if (timer->irq <= 0) {
    dev_err(&pdev->dev, "could not determine Timer IRQ\n");
    err = -ENOMEM;
    return err;
  }

  err = of_property_read_u32(pdev->dev.of_node, "xlnx,period", &value);
  if (!err) {
    dev_info(&pdev->dev, "PTP PERIOD %d\n", value);
    timer->period = value;
    timer->period = 4000000;
    dev_info(&pdev->dev, "PTP PERIOD OVERIDDEN %d\n", timer->period);
  } else {
    dev_err(&pdev->dev, "no PTP PERIOD specified\n");
    err = -ENXIO;
    goto out;
  }

  spin_lock_init(&timer->reg_lock);

  timer->ptp_clock_info = xlnx_ptp_clock_info;

  timer->ptp_clock = ptp_clock_register(&timer->ptp_clock_info,
                &pdev->dev);

  if (IS_ERR(timer->ptp_clock)) {
    err = PTR_ERR(timer->ptp_clock);
    dev_err(&pdev->dev, "Failed to register ptp clock\n");
    goto out;
  }

  err = xlnx_ptp_timer_request_irq(timer);
  if (err)
    goto out;

  platform_set_drvdata(pdev, timer);

    dev_info(&pdev->dev, "----------------------- Success loading xlnx_ptp_timer\n");

  return 0;
free_irqs:
  free_irq(timer->irq, &pdev->dev);
out:
  timer->ptp_clock = NULL;
  return err;
}

static struct of_device_id timer_1588_of_match[] = {
  { .compatible = "xlnx,timer-1588-2.0", },
    { /* end of table */ }
};
MODULE_DEVICE_TABLE(of, timer_1588_of_match);

static struct platform_driver xlnx_ptp_timer_driver = {
  .probe  = xlnx_ptp_timer_probe,
  .remove  = xlnx_ptp_timer_remove,
  .driver  = {
          .name = DRIVER_NAME,
          .owner = THIS_MODULE,
          .of_match_table = timer_1588_of_match,
  },
};

module_platform_driver(xlnx_ptp_timer_driver);

MODULE_AUTHOR("Xilinx, Inc.");
MODULE_DESCRIPTION("PTP Timer driver");
MODULE_LICENSE("GPL v2");
