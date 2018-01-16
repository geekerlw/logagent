pkglib_LTLIBRARIES =	\
	liblogagent-plugin-api.la	\
	liblogagent-aliyun-sdk.la	\
	liblogagent-plugin-filesrc.la	\
	liblogagent-plugin-filesink.la	\
	liblogagent-plugin-aliyun.la	\
	$(NULL)

liblogagent_aliyun_sdk_la_SOURCES = \
	aliyun/src/aos_buf.c	\
	aliyun/src/aos_fstack.c	\
	aliyun/src/aos_http_io.c	\
	aliyun/src/aos_log.c	\
	aliyun/src/aos_status.c	\
	aliyun/src/aos_string.c	\
	aliyun/src/aos_transport.c	\
	aliyun/src/aos_util.c	\
	aliyun/src/cJSON.c	\
	aliyun/src/log_api.c	\
	aliyun/src/log_auth.c	\
	aliyun/src/log_builder.c	\
	aliyun/src/log_define.c	\
	aliyun/src/log_http_cont.c	\
	aliyun/src/log_logstores.pb-c.c	\
	aliyun/src/log_producer_client.c	\
	aliyun/src/log_producer_common.c	\
	aliyun/src/log_producer_config.c	\
	aliyun/src/log_producer_debug_flusher.c	\
	aliyun/src/log_producer_manager.c	\
	aliyun/src/log_producer_sender.c	\
	aliyun/src/log_util.c	\
	aliyun/src/lz4.c	\
	aliyun/src/protobuf-c.c	\
	$(NULL)

liblogagent_aliyun_sdk_la_CFLAGS = \
	-I$(top_srcdir)/plugins/aliyun/src	\
	$(APR_CFLAGS)	\
	$(APR_UTIL_CFLAGS)	\
	$(LIBCURL_CFLAGS)	\
	$(NULL)

liblogagent_aliyun_sdk_la_LIBADD = \
	$(MATH_LIBS) \
	$(APR_LIBS)	\
	$(APR_UTIL_LIBS)	\
	$(LIBCURL_LIBS)	\
	$(NULL)

liblogagent_plugin_api_la_SOURCES = \
	logagent-plugin-api.c

liblogagent_plugin_filesrc_la_SOURCES = \
	logagent-plugin-filesrc.c

liblogagent_plugin_filesrc_la_CFLAGS = \
	$(JSON_C_CFLAGS)

liblogagent_plugin_filesrc_la_LIBADD = \
	$(JSON_C_LIBS)	\
	$(top_srcdir)/plugins/liblogagent-plugin-api.la

liblogagent_plugin_filesink_la_SOURCES = \
	logagent-plugin-filesink.c

liblogagent_plugin_filesink_la_CFLAGS = \
	$(JSON_C_CFLAGS)

liblogagent_plugin_filesink_la_LIBADD = \
	$(JSON_C_LIBS) \
	$(top_srcdir)/plugins/liblogagent-plugin-api.la

liblogagent_plugin_aliyun_la_SOURCES = \
	logagent-plugin-aliyun.c

liblogagent_plugin_aliyun_la_CFLAGS	= \
	$(JSON_C_CFLAGS)	\
	$(APR_CFLAGS)	\
	-I$(top_srcdir)/plugins/aliyun/src

liblogagent_plugin_aliyun_la_LIBADD	= \
	$(JSON_C_LIBS)	\
	$(APR_LIBS)	\
	$(top_srcdir)/plugins/liblogagent-plugin-api.la	\
	$(top_srcdir)/plugins/liblogagent-aliyun-sdk.la
