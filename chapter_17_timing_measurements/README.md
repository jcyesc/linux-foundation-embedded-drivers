
# Chapter 17 - Timinig measurements

To check the unit measures, we can check man:


```shell
man units
```

If the frequency is 1000Hz (1000 cycles per second), this means
that every 1ms a timer interrupt will happen and we have 1ms
to process the request if we don't want to mix the next interrupt.

If we are developing a Kernel and we want to make sure that we are not
missing timer interrupts, we should check the processor register for interrupts
to see if we haven't missed anyone.


