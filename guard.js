var guard = require('resource-guard'),
    vm = require('vm');

guard.start(500, 5000);

vm.runInNewContext('while(1){}');
