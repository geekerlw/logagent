cc = gcc
program = logagent


source = \
	logagent-plugin.c	\
	logagent-pipeline.c \
	logagent-config.c	\
	logagent.c

ldflags = \
	-ljson-c	\
	-lpthread

$(program): $(source)
	$(cc) -g $(source) -o $(program) $(ldflags)

clean:
	rm -rf $(program)
