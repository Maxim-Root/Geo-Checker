const generateEvb = require('generate-evb');
const path = require('path');

const deployDir = path.resolve('deploy');
const evbFile = path.resolve('geochecker.evb');
const inputExe = path.join(deployDir, 'GeoChecker.exe');
const outputExe = path.resolve('GeoChecker_packed.exe');

generateEvb(evbFile, inputExe, outputExe, deployDir, {
    filter: function(fullPath, name, isDir) {
        // Exclude the exe itself from the packed files (it's already the input)
        if (!isDir && name.toLowerCase() === 'geochecker.exe') return false;
        return true;
    },
    evbOptions: {
        deleteExtractedOnExit: true,
        compressFiles: true,
        shareVirtualSystem: false,
        mapExecutableWithTemporaryFile: true,
        allowRunningOfVirtualExeFiles: false
    }
});

console.log('EVB project generated:', evbFile);
console.log('Input exe:', inputExe);
console.log('Output exe:', outputExe);
console.log('Deploy dir:', deployDir);
