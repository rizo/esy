// @flow

let path = require('path');
let fs = require('fs-extra');
let util = require('util');

let files = [
  '_build/default/esy-build-package/bin/esyRewritePrefixCommand.exe',
  '_build/default/esy-build-package/bin/esyBuildPackageCommand.exe',
  '_build/default/esy/bin/esyCommand.exe'
];

let sourceRoot = process.cwd();
let releaseRoot = path.join(process.cwd(), '_platformrelease');

function copyFileSync(sourcePath, destPath) {
  const data = fs.readFileSync(sourcePath);
  const stat = fs.statSync(sourcePath);
  fs.writeFileSync(destPath, data);
  fs.chmodSync(destPath, stat.mode);
}

function main() {
  fs.removeSync(releaseRoot);
  for (let file of files) {
    let src = path.join(sourceRoot, file);
    let dst = path.join(releaseRoot, file);
    fs.mkdirpSync(path.dirname(dst));
    copyFileSync(src, dst);
    fs.chmodSync(dst, 0o755);
  }
}

main();
