## Logagent - Multi-channel Log Collector

![build](https://img.shields.io/travis/rust-lang/rust.svg)![license](https://img.shields.io/github/license/mashape/apistatus.svg)

Logagent is designed to collector logs and send to specified places. 


## Framework introduction

Logagent work with extensible plugins. similar to  the [Gstreamer](https://gstreamer.freedesktop.org/), each log stream have a independent

working channel which called `pipeline`,  `pipeline` is make up by mulit `element`, `element` is a copy

of the `plugin` which maintained by the plugin store.

#### pipeline

pipeline is the log channel of the Logagent framework. Each pipeline have a independent thread to run. 

`pipeline` maintained a `element` list, when the element list is prepared, logagent create a thread to run 

pipeline, and call element's work function one by one. Each working period, all elements share a log buffer 

list, so the first element maybe a collector to add log into the log buffer; the second one maybe a log filter 

to remove some logs from the log buffer; the last one maybe a log sender which send logs to the specified 

place.

#### element

element is the smallest component of the pipeline, each element saved the location of it's plugin. Each time

to call the element's function, find the current plugin from the plugin store and call the plugin's thread 

safety work function.

#### plugin

plugin is a dynamic library which provides functions called by logagent. Logagent maintained a plugin store,

after config loading, plugin initialize global environment and save global config to plugin store. Element call 

plugin `init` function and save element's independent config to element's own places.




## Project dependence

Logagent:

​	[libcurl](https://curl.haxx.se/libcurl/): the multiprotocol file transfer library

​	[json-c](https://github.com/json-c/json-c.git) : A JSON implementation in C

Plugins:

​	[inotify](http://www.man7.org/linux/man-pages/man7/inotify.7.html): monitoring filesystem events

​	[aliyun-log-c-sdk](https://github.com/aliyun/aliyun-log-c-sdk.git): Aliyun LOG Producer for C/C++



## Build instruction

install all dependences:

> sudo apt install libjson-c-dev libcurl4-openssl-dev libapr1-dev libaprutil1-dev

clone the repo:

> git clone https://github.com/geekerlw/logagent.git

init and pull submodules:

> git submodule init
>
> git submodule update

build like all project maintained by autotools:

> ./autogen.sh
>
> ./configure  	#  --prefix=/usr
>
> make && make install



## Configuration and usage

### how to write configure json file

Logagent use json to config `pipeline` `element`and `plugins`.

example:

``` json
{
    "pipeline_nums": 2,
    "pipeline@0": {
        "element_nums": 2,
        "element@0": {
            "plugin_name": "filesrc",
            "filesrc_config": "your plugin config"
        },
        "element@1": {
            "plugin_name": "aliyun",
            "aliyun_config": "your plugin config"
        }
    },
	"pipeline@1": {
		"element_nums": 2,
		"element@0": {
			"plugin_name": "filesrc",
			"filesrc_config": "your plugin config"
		},
		"element@1": {
			"plugin_name": "aliyun",
			 "aliyun_config": "your plugin config"
		}
	},
  
  	"plugin_nums": 2,
    "plugin@1": {
          "plugin_name": "filesrc",
          "plugin_path": "/usr/lib/logagent"
    },
    "plugin@2": {
          "plugin_name": "aliyun",
          "plugin_path": "/usr/lib/logagent"
    }
}
```

pipeline:

|      key      |               description                |    value     |
| :-----------: | :--------------------------------------: | :----------: |
| pipeline_nums | pipeline count, each pipeline correspond a thread |     int      |
| `pipeline@x`  | pipeline objects, x start from 0 to pipeline -1 | json objects |

element:

|     key      |               description                |    value    |
| :----------: | :--------------------------------------: | :---------: |
| element_nums | element count, each pipeline contained an elements object |     int     |
| `element@x`  | element objects, x start from 0 to element_nums -1 | json object |
| plugin_name  | under element, given a plugin name match with the plugin objects |   string    |
|     ...      |        element's personal config         |  customiz   |

plugin:

|     key     |               description                |    value    |
| :---------: | :--------------------------------------: | :---------: |
| plugin_nums | plugin count, each plugin contained a plugin |     int     |
| `plugin@x`  | plugin objects, x start from 0 to plugin_nums -1 | json object |
| plugin_name | library load match with liblogagent-plugin-`plugin_name`.so |   string    |
| plugin_path |           plugin library path            |   string    |
|     ...     |          plugin's global config          |  customiz   |

### how to use

try logagent --help for help information

> logagent --help

load json config from local disk:

> logagent -f /etc/logagent/logagent.conf

load json config from network:

> logagent -n `https://your_json_config_url`



## Documentation and development

### plugins description

##### filesrc

use inotify to monitor directory, collect matching log.

how to config: 

`file_path`: the log directory you want to monitor
`file_name`: the log name match rules, regular expression only



##### aliyun

use [aliyun-log-c-sdk](https://github.com/aliyun/aliyun-log-c-sdk.git) to send log to aliyun log server

how to config:

`logstore_config`: global config json file fullpath, see [readme](https://github.com/aliyun/aliyun-log-c-sdk/blob/master/README.md) for more help.

`logstore_name`: set under element's config to match logstore_config.



### how to write a plugin

Each plugin need to provide those functions defined in [logagent plugin api](./plugins/logagent-plugin-api.h)

``` c
extern int logagent_plugin_env_init(void **context);
extern int logagent_plugin_env_destroy(void **context);
extern int logagent_plugin_work(void *gconfig, void *pconfig,
				struct list_head *log_list);
extern int logagent_plugin_init(void *gconfig, void **context);
extern int logagent_plugin_exit(void *gconfig, void **context);
```

see the [full documentation](https://geekerlw.github.io/logagent/) for more help.

## Licence

[MIT Licence](https://github.com/geekerlw/logagent/blob/master/LICENSE)

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
