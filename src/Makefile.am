bin_PROGRAMS = logagent

logagent_SOURCES = \
	logagent-plugin.c	\
	logagent-element.c	\
	logagent-pipeline.c	\
	logagent-config.c	\
	logagent-log.c	\
	logagent.c	\
	$(NULL)

logagent_CFLAGS = \
	$(JSON_C_CFLAGS)	\
	$(NULL)

logagent_LDADD	= \
	$(PTHREAD_LIBS)	\
	$(JSON_C_LIBS)	\
	$(DLL_LIBS)	\
	$(NULL)

noinst_HEADERS = \
	logagent-plugin.h	\
	logagent-element.h	\
	logagent-pipeline.h	\
	logagent-config.h	\
	logagent-list.h	\
	logagent-log.h	\
	logagent.h	\
	$(NULL)
