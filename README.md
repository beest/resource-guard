# Resource Guard

Monitor a Node.js process and exit if it consumes too much memory or takes too much time. This can be useful when executing untrusted code in a child process.

## Installation
```
nvm install resource-guard --save
```

## Quick Start
To use in your Node.js code, just require it.

```javascript
var guard = require('resource-guard');
```

To monitor the current Node.js process, call the `start` method. You provide the memory limit in Mb and the time limit in milliseconds. For example, to limit memory usage to 500Mb and time to 2 seconds, use the following:

```javascript
guard.start(500, 2000);
```

If the time limit is exceeded then the process will exit with code -1.

If the memory limit is exceeded then the process will exit with code -2.

Every time you make a call to `start` the clock is reset and the new time limit will apply from the time of the call.

If you provide a value of 0 to either argument then monitoring of that resource is disabled. For example, to limit memory to 100Mb with no time limit, use the following:

```javascript
guard.start(100, 0);
```

To stop monitoring at any time, just call the `stop` method.

```javascript
guard.stop();
```
