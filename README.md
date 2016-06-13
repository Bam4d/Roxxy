# Roxxy

Roxxy is a rendering proxy that uses Chromium Embedded Framework (CEF) as a headless browser.

Currently building and tested on ubuntu 14.04 x64

## Why?

Imagine you want to write a program that looks at websites and collect some information such as the images or a list of data points on a page such as the prices... easy right? 

Not quite...

Modern websites are filled with complicated scripts and HTML5 features that dynamically load in data asynchonously or when the user scrolls down the page etc.. 
If you just write a python tool that makes a request to the url and returns the html, you will find that the html you are recieving is missing loads of information for many URLs

Roxxy can be used to execute html pages that require large amounts of javascript processing and then send the rendered and complete html back to the user.

Super simple script (python/ruby/etc..) -> Roxxy -> Complicated website

Roxxy takes away all of the complicated javascript and rendering processes and leaves you with a super simple html page.

## How?

Roxxy uses facebook's Proxygen server library to wrap Chromium Embedded Framework as a headless browser.

Concurrency is achieved by starting several browser instances (currently 20 because its hard coded) and putting them into a pool. Each browser is assigned one incoming request at a time, as soon as the HTML has been rendered, this browser is returned back to the pool.

In theory on large servers, the browser pool could consist of several hundred browsers.. meaning very high throughput

## Building

Roxxy uses gradle to build, but we first need to download some dependencies.

Firstly download the dependencies by running:

```
./deps.sh
```

call a tedious relative ... this takes ages...

You can now build using:

```
gradle build
```

The executables will be located in `build/exe/roxxy` and `build/exe/roxxy` for debug and release versions

## Usage

1. Start roxxy

```
> ./roxxy 
```

Send a url to roxxy using it's REST GET API
### Getting rendered HTML

```
> curl localhost:8055/html?url=http://www.google.com
```

### Getting rendered page image PNG

```
> curl localhost:8055/png?url=http://www.google.com
```

### Custom request endpoint:

Get the html and the png in one go:
```
> curl -vvv -XPOST localhost:8055 -d '{"url":"httpbin.org/status/200", "png": True}'
```

.. this endpoint will also serve other options in the future such as scripts etc.

## TODO

* More unit tests
* More Functional testing using python
* Could make this scriptable, so scripts can be run on the browser?
* Transparent proxy mode
* Docker image


## Commercially

If you wish to use this software commercially please contact me.

## Research/Personal projects etc

Go ahead, use this as much as you want.

## Contributing

Because this uses a lot of C++ libraries I'd love some help! Especially from anyone who knows alot about chromium / facebook / google libraries.
I need to set up a github wiki with some examples.

Please create a feature branch i.e `feature/fix-crap-code` from the `master` branch, make sure the tests work after you have made your changes.

When you are sure your branch is ready, please create a pull request.

## Benchmarks

I would love to benchmark this against phantomJS/Splash and any other qtWebKit / pyWebKit server.


