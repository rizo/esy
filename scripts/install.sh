#!/bin/bash
set -e

case $(uname -s) in
Linux) OS=NIX;;
Darwin) OS=NIX;;
*) OS=WIN;
esac

travis_retry() {
    local result=0
    local count=1
    while [ $count -le 3 ]; do
        [ $result -ne 0 ] && {
        echo -e "\n${ANSI_RED}The command \"$@\" failed. Retrying, $count of 3.${ANSI_RESET}\n" >&2
    }
    # ! { } ignores set -e, see https://stackoverflow.com/a/4073372
    ! { "$@"; result=$?; }
    [ $result -eq 0 ] && break
    count=$(($count + 1))
    sleep 1
    done

    [ $count -gt 3 ] && {
    echo -e "\n${ANSI_RED}The command \"$@\" failed 3 times.${ANSI_RESET}\n" >&2
  }

  return $result
}

if [[ "$OS" = "NIX" ]]; then

    # Linux / OSX install
    make bootstrap

else

    # Windows install
    npm install -g jest-cli
    npm install -g rimraf
    powershell.exe echo hi
    powershell.exe which npm
    powershell.exe which rimraf
    # cmd /c where jest
    cp scripts/build/patched-bash-exec.js /c/ProgramData/nvs/node/8.12.0/x64/node_modules/esy/node_modules/esy-bash/bash-exec.js
    /c/ProgramData/nvs/node/8.12.0/x64/node_modules/esy/_build/default/esy/bin/esyCommand.exe --version
    travis_retry /c/ProgramData/nvs/node/8.12.0/x64/node_modules/esy/_build/default/esy/bin/esyCommand.exe install
fi