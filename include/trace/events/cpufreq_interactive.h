#undef TRACE_SYSTEM
#define TRACE_SYSTEM cpufreq_interactive

#if !defined(_TRACE_CPUFREQ_INTERACTIVE_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_CPUFREQ_INTERACTIVE_H

#include <linux/tracepoint.h>

DECLARE_EVENT_CLASS(set,
	TP_PROTO(u32 cpu_id, unsigned long targfreq,
	         unsigned long actualfreq),
	TP_ARGS(cpu_id, targfreq, actualfreq),

	TP_STRUCT__entry(
	    __field(          u32, cpu_id    )
	    __field(unsigned long, targfreq   )
	    __field(unsigned long, actualfreq )
	   ),

	TP_fast_assign(
	    __entry->cpu_id = (u32) cpu_id;
	    __entry->targfreq = targfreq;
	    __entry->actualfreq = actualfreq;
	),

	TP_printk("cpu=%u targ=%lu actual=%lu",
	      __entry->cpu_id, __entry->targfreq,
	      __entry->actualfreq)
);

<<<<<<< HEAD
DEFINE_EVENT(set, cpufreq_interactive_up,
	TP_PROTO(u32 cpu_id, unsigned long targfreq,
	     unsigned long actualfreq),
	TP_ARGS(cpu_id, targfreq, actualfreq)
);

DEFINE_EVENT(set, cpufreq_interactive_down,
=======
DEFINE_EVENT(set, cpufreq_interactive_setspeed,
>>>>>>> upstream/4.3_primoc
	TP_PROTO(u32 cpu_id, unsigned long targfreq,
	     unsigned long actualfreq),
	TP_ARGS(cpu_id, targfreq, actualfreq)
);

DECLARE_EVENT_CLASS(loadeval,
	    TP_PROTO(unsigned long cpu_id, unsigned long load,
<<<<<<< HEAD
		     unsigned long curfreq, unsigned long targfreq),
	    TP_ARGS(cpu_id, load, curfreq, targfreq),
=======
		     unsigned long curtarg, unsigned long curactual,
		     unsigned long newtarg),
		    TP_ARGS(cpu_id, load, curtarg, curactual, newtarg),
>>>>>>> upstream/4.3_primoc

	    TP_STRUCT__entry(
		    __field(unsigned long, cpu_id    )
		    __field(unsigned long, load      )
<<<<<<< HEAD
		    __field(unsigned long, curfreq   )
		    __field(unsigned long, targfreq  )
=======
		    __field(unsigned long, curtarg   )
		    __field(unsigned long, curactual )
		    __field(unsigned long, newtarg   )
>>>>>>> upstream/4.3_primoc
	    ),

	    TP_fast_assign(
		    __entry->cpu_id = cpu_id;
		    __entry->load = load;
<<<<<<< HEAD
		    __entry->curfreq = curfreq;
		    __entry->targfreq = targfreq;
	    ),

	    TP_printk("cpu=%lu load=%lu cur=%lu targ=%lu",
		      __entry->cpu_id, __entry->load, __entry->curfreq,
		      __entry->targfreq)
=======
		    __entry->curtarg = curtarg;
		    __entry->curactual = curactual;
		    __entry->newtarg = newtarg;
	    ),

	    TP_printk("cpu=%lu load=%lu cur=%lu actual=%lu targ=%lu",
		      __entry->cpu_id, __entry->load, __entry->curtarg,
		      __entry->curactual, __entry->newtarg)
>>>>>>> upstream/4.3_primoc
);

DEFINE_EVENT(loadeval, cpufreq_interactive_target,
	    TP_PROTO(unsigned long cpu_id, unsigned long load,
<<<<<<< HEAD
		     unsigned long curfreq, unsigned long targfreq),
	    TP_ARGS(cpu_id, load, curfreq, targfreq)
=======
		     unsigned long curtarg, unsigned long curactual,
		     unsigned long newtarg),
	    TP_ARGS(cpu_id, load, curtarg, curactual, newtarg)
>>>>>>> upstream/4.3_primoc
);

DEFINE_EVENT(loadeval, cpufreq_interactive_already,
	    TP_PROTO(unsigned long cpu_id, unsigned long load,
<<<<<<< HEAD
		     unsigned long curfreq, unsigned long targfreq),
	    TP_ARGS(cpu_id, load, curfreq, targfreq)
=======
		     unsigned long curtarg, unsigned long curactual,
		     unsigned long newtarg),
	    TP_ARGS(cpu_id, load, curtarg, curactual, newtarg)
>>>>>>> upstream/4.3_primoc
);

DEFINE_EVENT(loadeval, cpufreq_interactive_notyet,
	    TP_PROTO(unsigned long cpu_id, unsigned long load,
<<<<<<< HEAD
		     unsigned long curfreq, unsigned long targfreq),
	    TP_ARGS(cpu_id, load, curfreq, targfreq)
=======
		     unsigned long curtarg, unsigned long curactual,
		     unsigned long newtarg),
	    TP_ARGS(cpu_id, load, curtarg, curactual, newtarg)
>>>>>>> upstream/4.3_primoc
);

TRACE_EVENT(cpufreq_interactive_boost,
	    TP_PROTO(const char *s),
	    TP_ARGS(s),
	    TP_STRUCT__entry(
		    __string(s, s)
	    ),
	    TP_fast_assign(
		    __assign_str(s, s);
	    ),
	    TP_printk("%s", __get_str(s))
);

TRACE_EVENT(cpufreq_interactive_unboost,
	    TP_PROTO(const char *s),
	    TP_ARGS(s),
	    TP_STRUCT__entry(
		    __string(s, s)
	    ),
	    TP_fast_assign(
		    __assign_str(s, s);
	    ),
	    TP_printk("%s", __get_str(s))
);

#endif /* _TRACE_CPUFREQ_INTERACTIVE_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
