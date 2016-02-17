# Roxxy

Roxxy is a rendering proxy that uses Chromium Embedded Framework (CEF) as a headless browser.

## Why?

Imagine you want to write a program that looks at websites and collect some information such as the images or a list of data points on teh page such as the prices... easy right? 

Not quite...

Modern websites are filled with complicated scripts and HTML5 features that dynamically load in data asynchonously or when the user scrolls down the page etc.. 
If you just write a python tool that makes a request to the url and returns the html, you will find that the html you are recieving is missing loads of information for many URLs

Roxxy can be used to execute html pages that require large amounts of javascript processing and then send the rendered and complete html back to the user.

Super simple script (python/ruby/etc..) -> Roxxy -> Complicated website

Roxxy takes away all of the complicated javascript and rendering processes and leaves you with a super simple html page.

## Building

.. coming soon i need to write a script... :(

## Usage

1. start roxxy

```
> ./roxxy
```

2. Send a url to roxxy using it's REST API

```
> curl localhost:8055?url=http://www.google.com
```

## Commercially

If you wish to use this software commercially please contact me.

## Research/Personal projects etc

Go ahead, use this as much as you want.

## Contributing

Because this uses alot of C++ libraries

## Improvements

Generate