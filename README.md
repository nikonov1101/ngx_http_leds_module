# ngx_http_leds_module - Lights up a LED when http request comes.

The whole idea was just a stupid joke in the private chat, but then something went wrong...

### Definitely Worth reading:

* [Orange Pi's GPIO management code](https://github.com/duxingkei33/orangepi_PC_gpio_pyH3);
* [Orange Pi's GPIO pinout](http://www.orangepi.org/orangepibbsen/data/attachment/forum/201601/24/204946qco5z1icsnigci37.png);
* [Brilliant guide on nginx modules development](https://www.evanmiller.org/nginx-modules-guide.html);
* [the "Hello World" module for nginx](https://github.com/perusio/nginx-hello-world-module);


### Building:

* Obtain a copy of the nginx source code [here](https://nginx.org/en/download.html);
* Clone this repo somewhere, but **not** inside the nginx source tree;
* Configure and build nginx with the module: `./configure --with-debug --add-module=/path/to/ngx_http_leds_module`;


### Running:

I wasn't so smart to define the `--prefix` when `./configure`, so I had to set it up on start:
```
mkdir -p /var/tmp/nginx/logs
./nginx -c /var/tmp/nginx/config -p /var/tmp/nginx
```

Configuration file example:

```
master_process off;
worker_processes  1;
daemon off;

error_log  /var/tmp/nginx/error.log debug;

events {
    worker_connections 32;
    debug_connection localhost;
}

http {
    default_type  application/octet-stream;

    server {
        listen       8092;
        server_name  localhost;

        location / {
            leds;
        }
    }
}
```

